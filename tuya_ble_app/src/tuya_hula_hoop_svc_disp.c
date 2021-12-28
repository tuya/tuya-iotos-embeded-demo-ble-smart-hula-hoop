/**
 * @file tuya_hula_hoop_svc_disp.c
 * @author lifan
 * @brief hula hoop display service process module source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_hula_hoop_svc_disp.h"
#include "tuya_hula_hoop_svc_basic.h"
#include "tuya_hula_hoop_svc_data.h"
#include "tuya_led.h"
#include "tuya_seg_lcd.h"
#include "tuya_ble_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
#define LED_FLASH_INTV_MS       300
#define SEG_LCD_FLASH_INTV_MS   500
#define SEG_LCD_FLASH_COUNT     3

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct {
    DISP_MODE_E mode;
    DISP_DATA_E data;
    LED_FUNC_E led_func;
    SEG_LCD_STAT_E seg_lcd_stat;
} HULA_HOOP_DISP_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
/* LED user define */
STATIC TY_GPIO_PORT_E sg_user_led_pin[] = {
    TY_GPIOD_4,   /* time/net */
    TY_GPIOB_6,   /* count */
    TY_GPIOD_7    /* calories */
};
LED_HANDLE g_user_led_handle[(SIZEOF(sg_user_led_pin) / SIZEOF(sg_user_led_pin[0]))];

/* Segment LCD user define */
SEG_LCD_PIN_T seg_lcd_pin_s = {
    .com = {TY_GPIOA_1, TY_GPIOC_2, TY_GPIOC_3, TY_GPIOB_4},
    .seg = {TY_GPIOA_0, TY_GPIOC_0, TY_GPIOD_3, TY_GPIOC_1, TY_GPIOC_4, TY_GPIOB_5}
};

/* Display define */
STATIC HULA_HOOP_DISP_T sg_disp;

/***********************************************************
***********************function define**********************
***********************************************************/
STATIC VOID_T __disp_target_remind_end_cb();
STATIC VOID_T __disp_reset_remind_end_cb();

/**
 * @brief dispaly process module init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_proc_init(VOID_T)
{
    UCHAR_T i;
    LED_RET ret;
    /* led init */
    for (i = 0; i < (SIZEOF(sg_user_led_pin) / SIZEOF(sg_user_led_pin[0])); i++) {
        ret = tuya_create_led_handle(sg_user_led_pin[i], FALSE, &g_user_led_handle[i]);
        if (ret != LED_OK) {
            TUYA_APP_LOG_ERROR("led init err:%d", ret);
        }
    }
    /* segment lcd init */
    tuya_seg_lcd_init(seg_lcd_pin_s);
    /* variable init */
    memset(&sg_disp, 0, SIZEOF(HULA_HOOP_DISP_T));
}

/**
 * @brief dispaly process module init on deep retention wakeup
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_proc_init_deepRetn(VOID_T)
{
    tuya_led_reset();
    tuya_seg_lcd_reset();
    memset(&sg_disp, 0, SIZEOF(HULA_HOOP_DISP_T));
}

/**
 * @brief set net led status
 * @param[in] none
 * @return none
 */
STATIC VOID_T __set_net_led_status(VOID_T)
{
    tuya_set_led_flash(g_user_led_handle[DISP_DATA_TIME], LFM_FOREVER, LFT_STA_ON_END_ON, LED_FLASH_INTV_MS, LED_FLASH_INTV_MS, 0, NULL);
    tuya_set_led_light(g_user_led_handle[DISP_DATA_COUNT], FALSE);
    tuya_set_led_light(g_user_led_handle[DISP_DATA_CALORIES], FALSE);
}

/**
 * @brief set data led status
 * @param[in] data: display data
 * @return none
 */
STATIC VOID_T __set_data_led_status(IN CONST DISP_DATA_E data)
{
    UCHAR_T i;

    if (sg_disp.led_func == LED_FUNC_BIND) {
        return;
    }
    for (i = 0; i < (SIZEOF(sg_user_led_pin) / SIZEOF(sg_user_led_pin[0])); i++) {
        if (i == data) {
            tuya_set_led_light(g_user_led_handle[i], TRUE);
        } else {
            tuya_set_led_light(g_user_led_handle[i], FALSE);
        }
    }
}

/**
 * @brief set segment lcd status
 * @param[in] stat: segment lcd status
 * @return none
 */
STATIC VOID_T __set_seg_lcd_status(IN CONST SEG_LCD_STAT_E stat)
{
    sg_disp.seg_lcd_stat = stat;
    switch (stat) {
    case SEG_LCD_STAT_OFF:
        tuya_seg_lcd_set_light(FALSE);
        break;
    case SEG_LCD_STAT_ON:
        tuya_seg_lcd_set_light(TRUE);
        break;
    case SEG_LCD_STAT_FLASH:
        if (sg_disp.mode == DISP_TARGET_MODE) {
            tuya_seg_lcd_set_flash(SEG_LCD_FLASH_DIGIT_ALL, SLFT_STA_ON_END_ON, SEG_LCD_FLASH_INTV_MS, SEG_LCD_FLASH_COUNT, __disp_target_remind_end_cb);
        }
        if (sg_disp.mode == DISP_RESET_REMIND) {
            tuya_seg_lcd_set_flash(SEG_LCD_FLASH_DIGIT_ALL, SLFT_STA_ON_END_OFF, SEG_LCD_FLASH_INTV_MS, SEG_LCD_FLASH_COUNT, __disp_reset_remind_end_cb);
        }
        break;
    default:
        break;
    }
}

/**
 * @brief set lcd display content of normal mode interface
 * @param[in] data: display data
 * @return none
 */
STATIC VOID_T __set_seg_lcd_disp_normal_mode(IN CONST DISP_DATA_E data)
{
    switch (data) {
    case DISP_DATA_TIME:
        tuya_seg_lcd_disp_num(g_sport_data.time_realtime, 0);
        break;
    case DISP_DATA_COUNT:
        tuya_seg_lcd_disp_num(g_sport_data.count_realtime, 0);
        break;
    case DISP_DATA_CALORIES:
        tuya_seg_lcd_disp_num(g_sport_data.calories_realtime, 0);
        break;
    default:
        break;
    }
}

/**
 * @brief set lcd display content of target mode interface
 * @param[in] none
 * @return none
 */
STATIC VOID_T __set_seg_lcd_disp_target_mode(VOID_T)
{
    if (g_sport_data.time_remain_today == 0) {
        tuya_seg_lcd_disp_str("---");
    } else {
        tuya_seg_lcd_disp_num(g_sport_data.time_remain_today, 0);
    }
}

/**
 * @brief set lcd display content of mode select interface
 * @param[in] none
 * @return none
 */
STATIC VOID_T __set_seg_lcd_disp_mode_select(VOID_T)
{
    switch (g_hula_hoop.mode_temp) {
    case MODE_NORMAL:
        tuya_seg_lcd_disp_str("010");
        break;
    case MODE_TARGET:
        tuya_seg_lcd_disp_str("020");
        break;
    default:
        break;
    }
}

/**
 * @brief set lcd display content of reset remind interface
 * @param[in] none
 * @return none
 */
STATIC VOID_T __set_seg_lcd_disp_reset_remind(VOID_T)
{
    tuya_seg_lcd_disp_str("000");
}

/**
 * @brief display process module loop
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_proc_loop(VOID_T)
{
    switch (sg_disp.mode) {
    case DISP_NORMAL_MODE:
        __set_seg_lcd_disp_normal_mode(sg_disp.data);
        break;
    case DISP_TARGET_MODE:
        __set_seg_lcd_disp_target_mode();
        break;
    case DISP_MODE_SELECT:
        __set_seg_lcd_disp_mode_select();
        break;
    case DISP_RESET_REMIND:
        __set_seg_lcd_disp_reset_remind();
        break;
    default:
        break;
    }
}

/**
 * @brief switch to normal mode interface
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_switch_to_normal_mode(VOID_T)
{
    sg_disp.mode = DISP_NORMAL_MODE;
    sg_disp.data = DISP_DATA_TIME;
    __set_data_led_status(sg_disp.data);
    __set_seg_lcd_status(SEG_LCD_STAT_ON);
    __set_seg_lcd_disp_normal_mode(DISP_DATA_TIME);
}

/**
 * @brief switch to target mode interface
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_switch_to_target_mode(VOID_T)
{
    sg_disp.mode = DISP_TARGET_MODE;
    sg_disp.data = DISP_DATA_TIME;
    __set_data_led_status(sg_disp.data);
    __set_seg_lcd_status(SEG_LCD_STAT_ON);
    __set_seg_lcd_disp_target_mode();
}

/**
 * @brief switch to mode select interface
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_switch_to_mode_select(VOID_T)
{
    sg_disp.mode = DISP_MODE_SELECT;
    sg_disp.data = DISP_DATA_NONE;
    __set_data_led_status(sg_disp.data);
    __set_seg_lcd_status(SEG_LCD_STAT_ON);
    __set_seg_lcd_disp_mode_select();
}

/**
 * @brief switch to reset remind interface
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_switch_to_reset_remind(VOID_T)
{
    sg_disp.mode = DISP_RESET_REMIND;
    sg_disp.data = DISP_DATA_NONE;
    __set_data_led_status(sg_disp.data);
    __set_seg_lcd_status(SEG_LCD_STAT_FLASH);
    __set_seg_lcd_disp_reset_remind();
}

/**
 * @brief set led function
 * @param[in] func: led function
 * @return none
 */
VOID_T hula_hoop_disp_set_led_func(IN CONST LED_FUNC_E func)
{
    if (func == sg_disp.led_func) {
        return;
    }
    sg_disp.led_func = func;
    if (func == LED_FUNC_BIND) {
        __set_net_led_status();
    } else {
        __set_data_led_status(sg_disp.data);
    }
}

/**
 * @brief display wakeup
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_wakeup(VOID_T)
{
    if (sg_disp.mode <= DISP_TARGET_MODE) {
        sg_disp.data = DISP_DATA_TIME;
    }
    __set_data_led_status(sg_disp.data);
    __set_seg_lcd_status(SEG_LCD_STAT_ON);
}

/**
 * @brief display sleep
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_sleep(VOID_T)
{
    __set_data_led_status(DISP_DATA_NONE);
    __set_seg_lcd_status(SEG_LCD_STAT_OFF);
}

/**
 * @brief check seg lcd status is wakeup or sleep
 * @param[in] none
 * @return TRUE - wakeup, FALSE - sleep
 */
BOOL_T hula_hoop_disp_is_wakeup(VOID_T)
{
    if (sg_disp.seg_lcd_stat == SEG_LCD_STAT_OFF) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
 * @brief check seg lcd status is flash
 * @param[in] none
 * @return TRUE - flash, FALSE - not flash
 */
BOOL_T hula_hoop_disp_is_flash(VOID_T)
{
    if (sg_disp.seg_lcd_stat == SEG_LCD_STAT_FLASH) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @brief get display mode
 * @param[in] none
 * @return display mode
 */
DISP_MODE_E hula_hoop_get_disp_mode(VOID_T)
{
    return sg_disp.mode;
}

/**
 * @brief switch displayed data
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_switch_disp_data(VOID_T)
{
    if (sg_disp.data == DISP_DATA_CALORIES) {
        if (hula_hoop_get_device_status() != STAT_ROTATING) {
            sg_disp.data = DISP_DATA_TIME;
        }
    } else {
        sg_disp.data++;
    }
    if (sg_disp.seg_lcd_stat != SEG_LCD_STAT_OFF) {
        __set_data_led_status(sg_disp.data);
    }
}

/**
 * @brief display handler when target finish
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_target_finish(VOID_T)
{
    if (sg_disp.mode == DISP_TARGET_MODE) {
        __set_seg_lcd_status(SEG_LCD_STAT_FLASH);
    }
}

/**
 * @brief display target remind end callback
 * @param[in] none
 * @return none
 */
STATIC VOID_T __disp_target_remind_end_cb()
{
    __set_seg_lcd_status(SEG_LCD_STAT_ON);
}

/**
 * @brief display reset remind end callback
 * @param[in] none
 * @return none
 */
STATIC VOID_T __disp_reset_remind_end_cb()
{
    hula_hoop_set_device_status(STAT_RESET);
}
