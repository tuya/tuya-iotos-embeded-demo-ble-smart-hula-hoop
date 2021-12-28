/**
 * @file tuya_seg_lcd.h
 * @author lifan
 * @brief segment lcd driver header file
 * @version 1.0
 * @date 2021-09-18
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_SEG_LCD_H__
#define __TUYA_SEG_LCD_H__

#include "tuya_common.h"
#include "tuya_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/
#define COM_NUM                     4
#define SEG_NUM                     6

#define SEG_LCD_FLASH_DIGIT_ALL     0xFF
#define SEG_LCD_FLASH_FOREVER       0xFFFF

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef BYTE_T SEG_LCD_RET;
#define SEG_LCD_OK                  0x00
#define SEG_LCD_ERR_INVALID_PARM    0x01
#define SEG_LCD_ERR_MALLOC_FAILED   0x02

typedef BYTE_T SEG_LCD_STEP_E;
#define STEP_COM_HIGH               0x00
#define STEP_COM_LOW                0x01
#define STEP_COM_HI_Z               0x02

typedef BYTE_T SEG_LCD_FLASH_TYPE_E;
#define SLFT_STA_ON_END_ON          0x00    /* start at light on and end at light on */
#define SLFT_STA_ON_END_OFF         0x01    /* start at light on and end at light off */
#define SLFT_STA_OFF_END_ON         0x02    /* start at light off and end at light on */
#define SLFT_STA_OFF_END_OFF        0x03    /* start at light off and end at light off */

typedef VOID_T (*SEG_LCD_CALLBACK)();

/* Character define */
typedef struct {
    UCHAR_T a : 1;
    UCHAR_T b : 1;
    UCHAR_T c : 1;
    UCHAR_T d : 1;
    UCHAR_T e : 1;
    UCHAR_T f : 1;
    UCHAR_T g : 1;
    UCHAR_T dp : 1;
} SEG_LCD_CH_T;

/* Pin define */
typedef struct {
    TY_GPIO_PORT_E com[COM_NUM];    /* COM pin: COM1-COM4 */
                                    /* COM1: -, d */
                                    /* COM2: c, e */
                                    /* COM3: b, g */
                                    /* COM4: a, f */
    TY_GPIO_PORT_E seg[SEG_NUM];    /* SEG pin: SEG1-SEG6 */
                                    /* SEG1: --, 3c, 3b, 3a */
                                    /* SEG2: 3d, 3e, 3g, 3f */
                                    /* SEG3: --, 2c, 2b, 2a */
                                    /* SEG4: 2d, 2e, 2g, 2f */
                                    /* SEG5: --, 1c, 1b, 1a */
                                    /* SEG6: 1d, 1e, 1g, 1f */
} SEG_LCD_PIN_T;

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief segment lcd init
 * @param[in] pin_def: pin define
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_init(SEG_LCD_PIN_T pin_def);

/**
 * @brief segment lcd reset
 * @param[in] none
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_reset(VOID_T);

/**
 * @brief display number
 * @param[in] num: number displayed in decimal
 * @param[in] high_zero: whether to display 0 when the high digit is 0
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_disp_num(IN CONST USHORT_T num, IN CONST BOOL_T high_zero);

/**
 * @brief display string
 * @param[in] str: the string whose charactes are all defined in the "0123456789- "
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_disp_str(IN CONST CHAR_T *str);

/**
 * @brief display a character at the specified digit
 * @param[in] ch: character defined in the "0123456789- "
 * @param[in] digit: 0 means the lowest digit
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_disp_ch(IN CONST CHAR_T ch, IN CONST UCHAR_T digit);

/**
 * @brief display a custom character at the specified digit
 * @param[in] cus_ch: custom character
 * @param[in] digit: 0 means the lowest digit
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_disp_custom_ch(IN CONST SEG_LCD_CH_T cus_ch, IN CONST UCHAR_T digit);

/**
 * @brief set light on or off
 * @param[in] on_off: true-on, false-off
 * @return none
 */
SEG_LCD_RET tuya_seg_lcd_set_light(BOOL_T on_off);

/**
 * @brief set segment lcd flash
 * @param[in] digit: flash digit, "0xFF" means all digit
 * @param[in] type: flash type
 * @param[in] intv: flash interval (ms)
 * @param[in] count: flash count, "0xFFFF" means flash forever
 * @param[in] end_cb: flash end callback function
 * @return SEG_LCD_RET
 */
SEG_LCD_RET tuya_seg_lcd_set_flash(IN CONST UCHAR_T digit, IN CONST SEG_LCD_FLASH_TYPE_E type, IN CONST USHORT_T intv, IN CONST USHORT_T count, IN CONST SEG_LCD_CALLBACK end_cb);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_SEG_LCD_H__ */
