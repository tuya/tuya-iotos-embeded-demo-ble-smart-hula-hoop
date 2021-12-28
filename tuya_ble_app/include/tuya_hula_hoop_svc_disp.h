/**
 * @file tuya_hula_hoop_svc_disp.h
 * @author lifan
 * @brief hula hoop display service processing module header file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_HULA_HOOP_SVC_DISP_H__
#define __TUYA_HULA_HOOP_SVC_DISP_H__

#include "tuya_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef BYTE_T DISP_MODE_E;
#define DISP_NORMAL_MODE    0x00
#define DISP_TARGET_MODE    0x01
#define DISP_MODE_SELECT    0x02
#define DISP_RESET_REMIND   0x03

typedef BYTE_T DISP_DATA_E;
#define DISP_DATA_TIME      0x00
#define DISP_DATA_COUNT     0x01
#define DISP_DATA_CALORIES  0x02
#define DISP_DATA_NONE      0x03

typedef BYTE_T LED_FUNC_E;
#define LED_FUNC_DATA       0x00
#define LED_FUNC_BIND       0x01

typedef BYTE_T SEG_LCD_STAT_E;
#define SEG_LCD_STAT_OFF    0x00
#define SEG_LCD_STAT_ON     0x01
#define SEG_LCD_STAT_FLASH  0x02

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief dispaly process module init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_proc_init(VOID_T);

/**
 * @brief dispaly process module init on deep retention wakeup
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_proc_init_deepRetn(VOID_T);

/**
 * @brief display process module loop
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_proc_loop(VOID_T);

/**
 * @brief switch to normal mode interface
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_switch_to_normal_mode(VOID_T);

/**
 * @brief switch to target mode interface
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_switch_to_target_mode(VOID_T);

/**
 * @brief switch to mode select interface
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_switch_to_mode_select(VOID_T);

/**
 * @brief switch to reset remind interface
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_switch_to_reset_remind(VOID_T);

/**
 * @brief set led function
 * @param[in] func: led function
 * @return none
 */
VOID_T hula_hoop_disp_set_led_func(IN CONST LED_FUNC_E func);

/**
 * @brief display wakeup
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_wakeup(VOID_T);

/**
 * @brief display sleep
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_sleep(VOID_T);

/**
 * @brief check seg lcd status is wakeup or sleep
 * @param[in] none
 * @return TRUE - wakeup, FALSE - sleep
 */
BOOL_T hula_hoop_disp_is_wakeup(VOID_T);

/**
 * @brief check seg lcd status is flash
 * @param[in] none
 * @return TRUE - flash, FALSE - not flash
 */
BOOL_T hula_hoop_disp_is_flash(VOID_T);

/**
 * @brief get display mode
 * @param[in] none
 * @return display mode
 */
DISP_MODE_E hula_hoop_get_disp_mode(VOID_T);

/**
 * @brief switch displayed data
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_switch_disp_data(VOID_T);

/**
 * @brief display handler when target finish
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_disp_target_finish(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_HULA_HOOP_SVC_DISP_H__ */
