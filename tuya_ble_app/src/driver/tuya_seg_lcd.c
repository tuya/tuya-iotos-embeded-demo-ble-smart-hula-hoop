/**
 * @file tuya_seg_lcd.c
 * @author lifan
 * @brief segment lcd driver source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_seg_lcd.h"
#include "tuya_ble_mem.h"
#include "tuya_timer.h"

/***********************************************************
************************micro define************************
***********************************************************/
#define SEG_LCD_DISP_DIGIT          3
#define SEG_LCD_DISP_MAX_NUM        999
#define SEG_LCD_COM_SCAN_CYCLE_MS   3
#define SEG_LCD_FLASH_PROC_CYCLE_MS 10

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct {
    UCHAR_T digit;                  /* flash digit */
    SEG_LCD_FLASH_TYPE_E type;      /* flash type */
    USHORT_T intv;                  /* flash interval */
    USHORT_T count;                 /* flash count */
    SEG_LCD_CALLBACK end_cb;        /* flash end callback function */
    UINT_T work_timer;              /* flash work timer */
} SEG_LCD_FLASH_T;

typedef struct {
    SEG_LCD_PIN_T pin;              /* pin */
    UCHAR_T seg_pin_code[COM_NUM];  /* output level code of SEG pin */
    BOOL_T light;                   /* light status */
    UCHAR_T scan_com_num;           /* scan com number */
    SEG_LCD_STEP_E scan_step;       /* scan step */
    SEG_LCD_FLASH_T *flash;         /* flash management */
    BOOL_T stop_flash_req;          /* stop flashing request */
    BOOL_T stop_flash_light;        /* light status after stopping flashing */
} SEG_LCD_MANAGE_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
/* bit7~bit0: fagbecd- */
CONST UCHAR_T ch_com_code[COM_NUM] = {
    0x03,       /* COM1: (bit0)-, (bit1)d */
    0x0c,       /* COM2: (bit2)c, (bit3)e */
    0x30,       /* COM3: (bit4)b, (bit5)g */
    0xc0        /* COM4: (bit6)a, (bit7)f */
};

/* Segment code: "bit7~bit0: fagbecd-" */
CONST UCHAR_T ch_seg_code[] = {
    0xde,       /* 0 */
    0x14,       /* 1 */
    0x7a,       /* 2 */
    0x76,       /* 3 */
    0xb4,       /* 4 */
    0xe6,       /* 5 */
    0xee,       /* 6 */
    0x54,       /* 7 */
    0xfe,       /* 8 */
    0xf6,       /* 9 */
    0x20,       /* - */
    0x00        /*   */
};

/* Character that can be displayed */
CONST CHAR_T lcd_str_tbl[] = "0123456789- ";

/* Segment lcd management */
STATIC SEG_LCD_MANAGE_T sg_seg_lcd_mag;

/***********************************************************
***********************function define**********************
***********************************************************/
STATIC INT_T __seg_lcd_timeout_handler(VOID_T);
STATIC INT_T __seg_lcd_output_ctrl(VOID_T);

/**
 * @brief gpio init
 * @param[in] port: gpio number
 * @return none
 */
STATIC VOID_T __seg_lcd_gpio_init(IN CONST TY_GPIO_PORT_E port)
{
    tuya_gpio_input_init(port, TY_GPIO_FLOATING);
}

/**
 * @brief set gpio shutdown
 * @param[in] port: gpio number
 * @return none
 */
STATIC VOID_T __seg_lcd_gpio_shutdown(IN CONST TY_GPIO_PORT_E port)
{
    tuya_gpio_set_inout(port, TRUE);
}

/**
 * @brief gpio write
 * @param[in] port: gpio number
 * @param[in] level: output level (TRUE-high, FALSE-low)
 * @return none
 */
STATIC VOID_T __seg_lcd_gpio_write(IN CONST TY_GPIO_PORT_E port, IN CONST UINT_T level)
{
    tuya_gpio_set_inout(port, FALSE);
    tuya_gpio_write(port, level);
}

/**
 * @brief segment lcd init
 * @param[in] pin_def: pin define
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_init(SEG_LCD_PIN_T pin_def)
{
    UCHAR_T i;

    memset(&sg_seg_lcd_mag, 0, sizeof(SEG_LCD_MANAGE_T));
    sg_seg_lcd_mag.pin = pin_def;

    /* COM pin init */
    for (i = 0; i < COM_NUM; i++) {
        __seg_lcd_gpio_init(pin_def.com[i]);
    }
    /* SEG pin init */
    for (i = 0; i < SEG_NUM; i++) {
        __seg_lcd_gpio_init(pin_def.seg[i]);
    }
    /* timer init */
    tuya_software_timer_create(SEG_LCD_FLASH_PROC_CYCLE_MS*1000, __seg_lcd_timeout_handler);
    tuya_hardware_timer_create(TY_TIMER_0, SEG_LCD_COM_SCAN_CYCLE_MS*1000, __seg_lcd_output_ctrl, TY_TIMER_REPEAT);

    return SEG_LCD_OK;
}

/**
 * @brief segment lcd reset
 * @param[in] none
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_reset(VOID_T)
{
    UCHAR_T i;
    /* COM pin init */
    for (i = 0; i < COM_NUM; i++) {
        __seg_lcd_gpio_init(sg_seg_lcd_mag.pin.com[i]);
    }
    /* SEG pin init */
    for (i = 0; i < SEG_NUM; i++) {
        __seg_lcd_gpio_init(sg_seg_lcd_mag.pin.seg[i]);
    }
    /* timer init */
    tuya_software_timer_delete(__seg_lcd_timeout_handler);
    tuya_software_timer_create(SEG_LCD_FLASH_PROC_CYCLE_MS*1000, __seg_lcd_timeout_handler);
    tuya_hardware_timer_delete(TY_TIMER_0);
    tuya_hardware_timer_create(TY_TIMER_0, SEG_LCD_COM_SCAN_CYCLE_MS*1000, __seg_lcd_output_ctrl, TY_TIMER_REPEAT);
    return SEG_LCD_OK;
}

/**
 * @brief get the actual output code
 * @param[in] seg_pin_code: display seg pin code
 * @return none
 */
STATIC UCHAR_T __get_actual_output_code(UCHAR_T seg_pin_code)
{
    UCHAR_T code = seg_pin_code;

    if (!sg_seg_lcd_mag.light) {
        if (sg_seg_lcd_mag.flash == NULL) {
            code =  0x00;
        } else {
            if (sg_seg_lcd_mag.flash->digit == SEG_LCD_FLASH_DIGIT_ALL) {
                code = 0x00;
            } else {
                code &= ~(ch_com_code[sg_seg_lcd_mag.flash->digit]);
            }
        }
    }
    return code;
}

/**
 * @brief segment lcd output control
 * @param[in] none
 * @return none
 */
STATIC INT_T __seg_lcd_output_ctrl(VOID_T)
{
    UCHAR_T i, active_pin, actl_code;
    BOOL_T seg_pin_level;

    active_pin = (sg_seg_lcd_mag.scan_com_num == 0) ? 0x2a : 0x3f;
    actl_code = __get_actual_output_code(sg_seg_lcd_mag.seg_pin_code[sg_seg_lcd_mag.scan_com_num]);

    switch (sg_seg_lcd_mag.scan_step) {
    case STEP_COM_HIGH:
        __seg_lcd_gpio_write(sg_seg_lcd_mag.pin.com[sg_seg_lcd_mag.scan_com_num], TRUE);
        for (i = 0; i < SEG_NUM; i++) {
            if (active_pin & (1 << i)) {
                seg_pin_level = ((actl_code & (1 << i)) > 0) ? FALSE : TRUE;
                __seg_lcd_gpio_write(sg_seg_lcd_mag.pin.seg[i], seg_pin_level);
            }
        }
        sg_seg_lcd_mag.scan_step = STEP_COM_LOW;
        break;
    case STEP_COM_LOW:
        __seg_lcd_gpio_write(sg_seg_lcd_mag.pin.com[sg_seg_lcd_mag.scan_com_num], FALSE);
        for (i = 0; i < SEG_NUM; i++) {
            if (active_pin & (1 << i)) {
                seg_pin_level = ((actl_code & (1 << i)) > 0) ? TRUE : FALSE;
                __seg_lcd_gpio_write(sg_seg_lcd_mag.pin.seg[i], seg_pin_level);
            }
        }
        sg_seg_lcd_mag.scan_step = STEP_COM_HI_Z;
        break;
    case STEP_COM_HI_Z:
        __seg_lcd_gpio_shutdown(sg_seg_lcd_mag.pin.com[sg_seg_lcd_mag.scan_com_num]);
        for (i = 0; i < SEG_NUM; i++) {
            if (active_pin & (1 << i)) {
                __seg_lcd_gpio_shutdown(sg_seg_lcd_mag.pin.seg[i]);
            }
        }
        sg_seg_lcd_mag.scan_com_num++;
        if (sg_seg_lcd_mag.scan_com_num >= COM_NUM) {
            sg_seg_lcd_mag.scan_com_num = 0;
        }
        sg_seg_lcd_mag.scan_step = STEP_COM_HIGH;
        break;
    default:
        break;
    }
    return 0;
}

/**
 * @brief generate SEG pin output code
 * @param[in] seg_code: segment code composed in the order of "bit7~bit0: fagbecd-"
 * @param[in] digit: 0 means the lowest digit
 * @return none
 */
STATIC VOID_T __generate_seg_pin_output_code(IN CONST UCHAR_T seg_code, IN CONST UCHAR_T digit)
{
    UCHAR_T i, tmp;
    for (i = 0; i < COM_NUM; i++) {
        sg_seg_lcd_mag.seg_pin_code[i] &= ~(ch_com_code[digit]);
        tmp = (seg_code & ch_com_code[i]) >> (i*2);
        sg_seg_lcd_mag.seg_pin_code[i] |= tmp << (digit*2);
    }
}

/**
 * @brief display number
 * @param[in] num: number displayed in decimal
 * @param[in] high_zero: whether to display 0 when the high digit is 0
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_disp_num(IN CONST USHORT_T num, IN CONST BOOL_T high_zero)
{
    UCHAR_T i, num_index[SEG_LCD_DISP_DIGIT];
    UCHAR_T disp_digit = SEG_LCD_DISP_DIGIT;
    USHORT_T tmp_num;

    /* check upper limit */
    if (num > SEG_LCD_DISP_MAX_NUM) {
        return SEG_LCD_ERR_INVALID_PARM;
    }
    /* calculate every digit */
    tmp_num = num;
    for (i = 0; i < SEG_LCD_DISP_DIGIT; i++) {
        num_index[i] = tmp_num % 10;
        tmp_num /= 10;
    }
    /* processing when the high digit is set to not display 0 */
    if (!high_zero) {
        for (i = (SEG_LCD_DISP_DIGIT-1); i > 0; i--) {
            if (num_index[i] == 0) {
                disp_digit--;
            } else {
                break;
            }
        }
    }
    /* generate SEG pin output code */
    for (i = 0; i < SEG_LCD_DISP_DIGIT; i++) {
        if (i < disp_digit) {
            __generate_seg_pin_output_code(ch_seg_code[num_index[i]], i);
        } else {
            __generate_seg_pin_output_code(0x00, i);
        }
    }
    return SEG_LCD_OK;
}

/**
 * @brief display string
 * @param[in] str: the string whose charactes are all defined in the <lcd_str_tbl>
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_disp_str(IN CONST CHAR_T *str)
{
    UINT_T i, len;
    UCHAR_T ch_index[SEG_LCD_DISP_DIGIT];

    /* get string length */
    len = strlen(str);
    if (len > SEG_LCD_DISP_DIGIT) {
        len = SEG_LCD_DISP_DIGIT;
    }
    /* find the character's position and generate SEG pin output code */
    for (i = 0; i < len; i++) {
        ch_index[i] = strchr(lcd_str_tbl, *(str++)) - lcd_str_tbl;
        __generate_seg_pin_output_code(ch_seg_code[ch_index[i]], (SEG_LCD_DISP_DIGIT-1-i));
    }
    return SEG_LCD_OK;
}

/**
 * @brief display a character at the specified digit
 * @param[in] ch: character defined in the <lcd_str_tbl>
 * @param[in] digit: 0 means the lowest digit
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_disp_ch(IN CONST CHAR_T ch, IN CONST UCHAR_T digit)
{
    UCHAR_T ch_index;

    /* find the character's position and generate SEG pin output code */
    ch_index = strchr(lcd_str_tbl, ch) - lcd_str_tbl;
    __generate_seg_pin_output_code(ch_seg_code[ch_index], digit);
    return SEG_LCD_OK;
}

/**
 * @brief display a custom character at the specified digit
 * @param[in] cus_ch: custom character
 * @param[in] digit: 0 means the lowest digit
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_disp_custom_ch(IN CONST SEG_LCD_CH_T cus_ch, IN CONST UCHAR_T digit)
{
    UCHAR_T i, cus_seg_code, ch_index, tmp;
    UCHAR_T seg_code = 0x00;
    CHAR_T cus_seg_seq[8] = "abcdefg-";
    CHAR_T *seg_seq = "-dcebgaf";

    /* Convert segment codes */
    memcpy(&cus_seg_code, &cus_ch, 1);
    for (i = 0; i < 8; i++) {
        tmp = cus_seg_code & 0x01;
        ch_index = strchr(seg_seq, cus_seg_seq[i]) - seg_seq;
        seg_code |= (tmp << ch_index);
        cus_seg_code >>= 1;
    }
    /* generate SEG pin output code */
    __generate_seg_pin_output_code(seg_code, digit);
    return SEG_LCD_OK;
}

/**
 * @brief set light on or off
 * @param[in] on_off: true-on, false-off
 * @return none
 */
VOID_T __set_seg_lcd_light(IN CONST BOOL_T on_off)
{
    sg_seg_lcd_mag.light = on_off;
}

/**
 * @brief set light on or off
 * @param[in] on_off: true-on, false-off
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_set_light(IN CONST BOOL_T on_off)
{
    if (sg_seg_lcd_mag.flash != NULL) {
        sg_seg_lcd_mag.stop_flash_req = TRUE;
        sg_seg_lcd_mag.stop_flash_light = on_off;
    } else {
        __set_seg_lcd_light(on_off);
    }
    return SEG_LCD_OK;
}

/**
 * @brief get segment lcd flash start light
 * @param[in] type: segment lcd flash type
 * @return TRUE - light on, FALSE - light off
 */
STATIC BOOL_T __get_seg_lcd_flash_sta_light(IN CONST SEG_LCD_FLASH_TYPE_E type)
{
    BOOL_T ret = TRUE;
    switch (type) {
    case SLFT_STA_ON_END_ON:
    case SLFT_STA_ON_END_OFF:
        ret = TRUE;
        break;
    case SLFT_STA_OFF_END_ON:
    case SLFT_STA_OFF_END_OFF:
        ret = FALSE;
        break;
    default:
        break;
    }
    return ret;
}

/**
 * @brief get segment lcd flash end light
 * @param[in] type: segment lcd flash type
 * @return TRUE - light on, FALSE - light off
 */
STATIC BOOL_T __get_seg_lcd_flash_end_light(IN CONST SEG_LCD_FLASH_TYPE_E type)
{
    BOOL_T ret = TRUE;
    switch (type) {
    case SLFT_STA_ON_END_ON:
    case SLFT_STA_OFF_END_ON:
        ret = TRUE;
        break;
    case SLFT_STA_ON_END_OFF:
    case SLFT_STA_OFF_END_OFF:
        ret = FALSE;
        break;
    default:
        break;
    }
    return ret;
}

/**
 * @brief set segment lcd flash
 * @param[in] digit: flash digit, "0xFF" means all digit
 * @param[in] type: flash type
 * @param[in] intv: flash interval (ms)
 * @param[in] count: flash count, "0xFFFF" means flash forever
 * @param[in] end_cb: flash end callback function
 * @return SEG_LCD_RET
 */
SEG_LCD_RET tuya_seg_lcd_set_flash(IN CONST UCHAR_T digit, IN CONST SEG_LCD_FLASH_TYPE_E type, IN CONST USHORT_T intv, IN CONST USHORT_T count, IN CONST SEG_LCD_CALLBACK end_cb)
{
    sg_seg_lcd_mag.stop_flash_req = FALSE;
    if (sg_seg_lcd_mag.flash == NULL) {
        SEG_LCD_FLASH_T *seg_lcd_flash = (SEG_LCD_FLASH_T *)tuya_ble_malloc(SIZEOF(SEG_LCD_FLASH_T));
        if (NULL == seg_lcd_flash) {
            return SEG_LCD_ERR_MALLOC_FAILED;
        }
        sg_seg_lcd_mag.flash = seg_lcd_flash;
    }
    sg_seg_lcd_mag.flash->digit = digit;
    sg_seg_lcd_mag.flash->type = type;
    sg_seg_lcd_mag.flash->intv = intv;
    sg_seg_lcd_mag.flash->count = count;
    sg_seg_lcd_mag.flash->work_timer = 0;
    sg_seg_lcd_mag.flash->end_cb = end_cb;
    __set_seg_lcd_light(__get_seg_lcd_flash_sta_light(type));
    return SEG_LCD_OK;
}

/**
 * @brief segment lcd flash process
 * @param[inout] none
 * @return none
 */
STATIC VOID_T __seg_lcd_flash_proc(VOID_T)
{
    BOOL_T one_cycle_flag = FALSE;
    BOOL_T start_light = __get_seg_lcd_flash_sta_light(sg_seg_lcd_mag.flash->type);

    /* flash cycle process */
    sg_seg_lcd_mag.flash->work_timer += SEG_LCD_FLASH_PROC_CYCLE_MS;
    if (sg_seg_lcd_mag.flash->work_timer >= sg_seg_lcd_mag.flash->intv*2) {
        sg_seg_lcd_mag.flash->work_timer -= sg_seg_lcd_mag.flash->intv*2;
        __set_seg_lcd_light(start_light);
        one_cycle_flag = TRUE;
    } else if (sg_seg_lcd_mag.flash->work_timer >= sg_seg_lcd_mag.flash->intv) {
        __set_seg_lcd_light(!start_light);
    } else {
        ;
    }

    /* flash countdown process */
    if (sg_seg_lcd_mag.flash->count == SEG_LCD_FLASH_FOREVER) {
        return;
    }
    if (one_cycle_flag) {
        if (sg_seg_lcd_mag.flash->count > 0) {
            sg_seg_lcd_mag.flash->count--;
        }
    }

    /* flash end process */
    if (sg_seg_lcd_mag.flash->count == 0) {
        if (sg_seg_lcd_mag.flash->end_cb != NULL) {
            sg_seg_lcd_mag.flash->end_cb();
        }
        sg_seg_lcd_mag.stop_flash_req = TRUE;
        sg_seg_lcd_mag.stop_flash_light = __get_seg_lcd_flash_end_light(sg_seg_lcd_mag.flash->type);
    }
}

/**
 * @brief segment lcd timeout handler
 * @param[in] none
 * @return none
 */
STATIC INT_T __seg_lcd_timeout_handler(VOID_T)
{
    if (sg_seg_lcd_mag.stop_flash_req) {
        __set_seg_lcd_light(sg_seg_lcd_mag.stop_flash_light);
        tuya_ble_free((UCHAR_T *)sg_seg_lcd_mag.flash);
        sg_seg_lcd_mag.flash = NULL;
        sg_seg_lcd_mag.stop_flash_req = FALSE;
    }
    if (sg_seg_lcd_mag.flash != NULL) {
        __seg_lcd_flash_proc();
    }
    return 0;
}
