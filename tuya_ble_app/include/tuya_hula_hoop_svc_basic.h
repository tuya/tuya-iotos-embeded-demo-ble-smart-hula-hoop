/**
 * @file tuya_hula_hoop_svc_basic.h
 * @author lifan
 * @brief hula hoop basic service processing module header file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_HULA_HOOP_SVC_BASIC_H__
#define __TUYA_HULA_HOOP_SVC_BASIC_H__

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
/* work mode */
typedef BYTE_T MODE_E;
#define MODE_NORMAL         0x00
#define MODE_TARGET         0x01

/* device status */
typedef BYTE_T STAT_E;
#define STAT_USING          0x00
#define STAT_ROTATING       0x01
#define STAT_UNUSED         0x02
#define STAT_SLEEP          0x03
#define STAT_RESET          0x04

/* hula_hoop struct */
typedef struct {
    MODE_E mode;
    MODE_E mode_temp;
    STAT_E stat;
} HULA_HOOP_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
extern HULA_HOOP_T g_hula_hoop;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief switch temporary mode
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_switch_temp_mode(VOID_T);

/**
 * @brief enter mode select
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_enter_mode_select(VOID_T);

/**
 * @brief quit from mode select
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_quit_mode_select(VOID_T);

/**
 * @brief switch to select mode
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_switch_to_select_mode(VOID_T);

/**
 * @brief set work mode
 * @param[in] mode: work mode
 * @return none
 */
VOID_T hula_hoop_set_work_mode(IN CONST MODE_E mode);

/**
 * @brief set device sleep, called by "app_power_management()"
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_set_device_sleep(VOID_T);

/**
 * @brief set device status
 * @param[in] stat: device status
 * @return none
 */
VOID_T hula_hoop_set_device_status(IN CONST STAT_E stat);

/**
 * @brief get device status
 * @param[in] none
 * @return device status
 */
STAT_E hula_hoop_get_device_status(VOID_T);

/**
 * @brief check if the date has changed
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_check_date_change(VOID_T);

/**
 * @brief hula hoop basic service init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_basic_service_init(VOID_T);

/**
 * @brief hula hoop device wakeup handler
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_device_wakeup_handler(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_HULA_HOOP_SVC_BASIC_H__ */
