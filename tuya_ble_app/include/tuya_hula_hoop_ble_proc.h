/**
 * @file tuya_hula_hoop_ble_proc.h
 * @author lifan
 * @brief hula hoop ble process module header file
 * @version 1.0
 * @date 2021-09-15
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_HULA_HOOP_BLE_PROC_H__
#define __TUYA_HULA_HOOP_BLE_PROC_H__

#include "tuya_common.h"
#include "tuya_ble_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/
#define F_BLE_BOUND     g_ble_proc_flag.bit0
#define F_WAIT_BINDING  g_ble_proc_flag.bit1

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
extern FLAG_BIT g_ble_proc_flag;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief ble process init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_ble_proc_init(VOID_T);

/**
 * @brief ble process init on deep retention wakeup
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_ble_proc_init_deepRetn(VOID_T);

/**
 * @brief whether the device needs to disconnect
 * @param[in] none
 * @return TRUE - need, FALSE - no need
 */
BOOL_T hula_hoop_is_need_disconnect(VOID_T);

/**
 * @brief allow users to bind
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_allow_binding(VOID_T);

/**
 * @brief prohibit users to bind
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_prohibit_binding(VOID_T);

/**
 * @brief report dp data of work mode
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_report_mode(VOID_T);

/**
 * @brief report dp data of sport data --time
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_report_sport_data1(VOID_T);

/**
 * @brief report dp data of sport data --count&calories
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_report_sport_data2(VOID_T);

/**
 * @brief dp data handler
 * @param[in] dp_data: dp data array
 * @return none
 */
VOID_T hula_hoop_ble_dp_write_handler(IN UCHAR_T *dp_data);

/**
 * @brief ble connection status handler
 * @param[in] status: ble connect status
 * @return none
 */
VOID_T hula_hoop_ble_conn_stat_handler(IN CONST tuya_ble_connect_status_t status);

/**
 * @brief ble unbound handler
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_ble_unbound_handler(VOID_T);

/**
 * @brief ble time normal handler
 * @param[in] time_normal: ble time data in normal format
 * @return none
 */
VOID_T hula_hoop_ble_time_normal_handler(IN CONST tuya_ble_time_noraml_data_t time_normal);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_HULA_HOOP_BLE_PROC_H__ */
