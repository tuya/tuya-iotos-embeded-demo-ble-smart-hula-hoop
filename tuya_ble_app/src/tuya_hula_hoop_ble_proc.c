/**
 * @file tuya_hula_hoop_ble_proc.c
 * @author lifan
 * @brief ble process module source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_hula_hoop_ble_proc.h"
#include "tuya_hula_hoop_svc_basic.h"
#include "tuya_hula_hoop_svc_disp.h"
#include "tuya_hula_hoop_svc_data.h"
#include "tuya_hula_hoop_evt_timer.h"
#include "tuya_local_time.h"
#include "tuya_ble_common.h"
#include "tuya_ble_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* Cloud time type */
#define BLE_TIME_TYPE_STAMP         0
#define BLE_TIME_TYPE_NORMAL        1

/* DP ID */
#define DP_ID_MODE                  101
#define DP_ID_TIME_REALTIME         102
#define DP_ID_COUNT_REALTIME        103
#define DP_ID_CALORIES_REALTIME     104
#define DP_ID_TIME_TOTAL_TODAY      105
#define DP_ID_COUNT_TOTAL_TODAY     106
#define DP_ID_CALORIES_TOTAL_TODAY  107
#define DP_ID_TIME_TOTAL_30DAYS     108
#define DP_ID_COUNT_TOTAL_30DAYS    109
#define DP_ID_CALORIES_TOTAL_30DAYS 110
#define DP_ID_TIME_TARGET_TODAY     111
#define DP_ID_TIME_TARGET_MONTH     112
#define DP_ID_TIME_REMAIN_TODAY     113
#define DP_ID_TIME_REMAIN_MONTH     114

#define DP_DATA_INDEX_OFFSET_ID     0
#define DP_DATA_INDEX_OFFSET_TYPE   1
#define DP_DATA_INDEX_OFFSET_LEN    2
#define DP_DATA_INDEX_OFFSET_DATA   3

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
FLAG_BIT g_ble_proc_flag;
STATIC UCHAR_T sg_repo_array[255+3];

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief ble process init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_ble_proc_init(VOID_T)
{
    tuya_ble_connect_status_t ble_conn_sta;
    ble_conn_sta = tuya_ble_connect_status_get();
    TUYA_APP_LOG_DEBUG("BLE connect status: %d", ble_conn_sta);

    if ((ble_conn_sta == BONDING_UNCONN) ||
        (ble_conn_sta == BONDING_CONN)   ||
        (ble_conn_sta == BONDING_UNAUTH_CONN)) {
        F_BLE_BOUND = SET;
        F_WAIT_BINDING = CLR;
    } else {
        F_BLE_BOUND = CLR;
        F_WAIT_BINDING = SET;
        hula_hoop_disp_set_led_func(LED_FUNC_BIND);
    }
}

/**
 * @brief ble process init on deep retention wakeup
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_ble_proc_init_deepRetn(VOID_T)
{
    if (F_BLE_BOUND) {
        bls_ll_setAdvEnable(1);
    }
}

/**
 * @brief whether the device needs to disconnect
 * @param[in] none
 * @return TRUE - need, FALSE - no need
 */
BOOL_T hula_hoop_is_need_disconnect(VOID_T)
{
    bls_ll_setAdvEnable(0);
    if (tuya_ble_connect_status_get() == BONDING_CONN) {
        tuya_ble_gap_disconnect();
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief allow users to bind
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_allow_binding(VOID_T)
{
    if (F_BLE_BOUND) {
        tuya_ble_device_factory_reset();
    }
    F_WAIT_BINDING = SET;
    bls_ll_setAdvEnable(1);
    hula_hoop_disp_set_led_func(LED_FUNC_BIND);
}

/**
 * @brief prohibit users to bind
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_prohibit_binding(VOID_T)
{
    F_WAIT_BINDING = CLR;
    bls_ll_setAdvEnable(0);
    hula_hoop_disp_set_led_func(LED_FUNC_DATA);
}

/**
 * @brief report one dp data
 * @param[in] dp_id: DP ID
 * @param[in] dp_type: DP type
 * @param[in] dp_len: DP length
 * @param[in] dp_data: DP data address
 * @return none
 */
STATIC VOID_T __report_one_dp_data(IN CONST UCHAR_T dp_id, IN CONST UCHAR_T dp_type, IN CONST UCHAR_T dp_len, IN CONST UCHAR_T *dp_data)
{
    UCHAR_T i;
    sg_repo_array[DP_DATA_INDEX_OFFSET_ID] = dp_id;
    sg_repo_array[DP_DATA_INDEX_OFFSET_TYPE] = dp_type;
    sg_repo_array[DP_DATA_INDEX_OFFSET_LEN] = dp_len;
    for (i = 0; i < dp_len; i++) {
        sg_repo_array[DP_DATA_INDEX_OFFSET_DATA + i] = *(dp_data + (dp_len-i-1));
    }
    tuya_ble_dp_data_report(sg_repo_array, dp_len + 3);
}

/**
 * @brief add one dp data
 * @param[in] dp_id: DP ID
 * @param[in] dp_type: DP type
 * @param[in] dp_len: DP length
 * @param[in] dp_data: DP data address
 * @param[in] addr: DP report address
 * @return total length
 */
STATIC UCHAR_T __add_one_dp_data(IN CONST UCHAR_T dp_id, IN CONST UCHAR_T dp_type, IN CONST UCHAR_T dp_len, IN CONST UCHAR_T *dp_data, IN UCHAR_T *addr)
{
    UCHAR_T i;
    *(addr + DP_DATA_INDEX_OFFSET_ID) = dp_id;
    *(addr + DP_DATA_INDEX_OFFSET_TYPE) = dp_type;
    *(addr + DP_DATA_INDEX_OFFSET_LEN) = dp_len;
    for (i = 0; i < dp_len; i++) {
        *(addr + DP_DATA_INDEX_OFFSET_DATA + i) = *(dp_data + (dp_len-i-1));
    }
    return (dp_len + 3);
}

/**
 * @brief report dp data of realtime sport data
 * @param[in] none
 * @return none
 */
VOID_T __report_sport_data_realtime(VOID_T)
{
    UCHAR_T total_len = 0;
    total_len += __add_one_dp_data(DP_ID_TIME_REALTIME, DT_VALUE, SIZEOF(g_sport_data.time_realtime), (UCHAR_T *)&g_sport_data.time_realtime, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_COUNT_REALTIME, DT_VALUE, SIZEOF(g_sport_data.count_realtime), (UCHAR_T *)&g_sport_data.count_realtime, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_CALORIES_REALTIME, DT_VALUE, SIZEOF(g_sport_data.calories_realtime), (UCHAR_T *)&g_sport_data.calories_realtime, (sg_repo_array + total_len));
    tuya_ble_dp_data_report(sg_repo_array, total_len);
}

/**
 * @brief report dp data of work mode
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_report_mode(VOID_T)
{
    __report_one_dp_data(DP_ID_MODE, DT_ENUM, SIZEOF(g_hula_hoop.mode), &g_hula_hoop.mode);
    __report_sport_data_realtime();
}

/**
 * @brief report dp data of sport data --time
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_report_sport_data1(VOID_T)
{
    UCHAR_T total_len = 0;
    total_len += __add_one_dp_data(DP_ID_TIME_REALTIME, DT_VALUE, SIZEOF(g_sport_data.time_realtime), (UCHAR_T *)&g_sport_data.time_realtime, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_TIME_TOTAL_TODAY, DT_VALUE, SIZEOF(g_sport_data.time_total_today), (UCHAR_T *)&g_sport_data.time_total_today, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_TIME_TOTAL_30DAYS, DT_VALUE, SIZEOF(g_sport_data.time_total_30days), (UCHAR_T *)&g_sport_data.time_total_30days, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_TIME_REMAIN_TODAY, DT_VALUE, SIZEOF(g_sport_data.time_remain_today), (UCHAR_T *)&g_sport_data.time_remain_today, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_TIME_REMAIN_MONTH, DT_VALUE, SIZEOF(g_sport_data.time_remain_month), (UCHAR_T *)&g_sport_data.time_remain_month, (sg_repo_array + total_len));
    tuya_ble_dp_data_report(sg_repo_array, total_len);
}

/**
 * @brief report dp data of sport data --count&calories
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_report_sport_data2(VOID_T)
{
    UCHAR_T total_len = 0;
    total_len += __add_one_dp_data(DP_ID_COUNT_REALTIME, DT_VALUE, SIZEOF(g_sport_data.count_realtime), (UCHAR_T *)&g_sport_data.count_realtime, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_CALORIES_REALTIME, DT_VALUE, SIZEOF(g_sport_data.calories_realtime), (UCHAR_T *)&g_sport_data.calories_realtime, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_COUNT_TOTAL_TODAY, DT_VALUE, SIZEOF(g_sport_data.count_total_today), (UCHAR_T *)&g_sport_data.count_total_today, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_CALORIES_TOTAL_TODAY, DT_VALUE, SIZEOF(g_sport_data.calories_total_today), (UCHAR_T *)&g_sport_data.calories_total_today, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_COUNT_TOTAL_30DAYS, DT_VALUE, SIZEOF(g_sport_data.count_total_30days), (UCHAR_T *)&g_sport_data.count_total_30days, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_CALORIES_TOTAL_30DAYS, DT_VALUE, SIZEOF(g_sport_data.calories_total_30days), (UCHAR_T *)&g_sport_data.calories_total_30days, (sg_repo_array + total_len));
    tuya_ble_dp_data_report(sg_repo_array, total_len);
}

/**
 * @brief report all dp data
 * @param[in] none
 * @return none
 */
STATIC VOID_T __report_all_dp_data(VOID_T)
{
    UCHAR_T total_len = 0;
    total_len += __add_one_dp_data(DP_ID_MODE, DT_ENUM, SIZEOF(g_hula_hoop.mode), &g_hula_hoop.mode, sg_repo_array);
    total_len += __add_one_dp_data(DP_ID_TIME_REALTIME, DT_VALUE, SIZEOF(g_sport_data.time_realtime), (UCHAR_T *)&g_sport_data.time_realtime, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_COUNT_REALTIME, DT_VALUE, SIZEOF(g_sport_data.count_realtime), (UCHAR_T *)&g_sport_data.count_realtime, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_CALORIES_REALTIME, DT_VALUE, SIZEOF(g_sport_data.calories_realtime), (UCHAR_T *)&g_sport_data.calories_realtime, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_TIME_TOTAL_TODAY, DT_VALUE, SIZEOF(g_sport_data.time_total_today), (UCHAR_T *)&g_sport_data.time_total_today, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_COUNT_TOTAL_TODAY, DT_VALUE, SIZEOF(g_sport_data.count_total_today), (UCHAR_T *)&g_sport_data.count_total_today, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_CALORIES_TOTAL_TODAY, DT_VALUE, SIZEOF(g_sport_data.calories_total_today), (UCHAR_T *)&g_sport_data.calories_total_today, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_TIME_TOTAL_30DAYS, DT_VALUE, SIZEOF(g_sport_data.time_total_30days), (UCHAR_T *)&g_sport_data.time_total_30days, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_COUNT_TOTAL_30DAYS, DT_VALUE, SIZEOF(g_sport_data.count_total_30days), (UCHAR_T *)&g_sport_data.count_total_30days, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_CALORIES_TOTAL_30DAYS, DT_VALUE, SIZEOF(g_sport_data.calories_total_30days), (UCHAR_T *)&g_sport_data.calories_total_30days, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_TIME_REMAIN_TODAY, DT_VALUE, SIZEOF(g_sport_data.time_remain_today), (UCHAR_T *)&g_sport_data.time_remain_today, (sg_repo_array + total_len));
    total_len += __add_one_dp_data(DP_ID_TIME_REMAIN_MONTH, DT_VALUE, SIZEOF(g_sport_data.time_remain_month), (UCHAR_T *)&g_sport_data.time_remain_month, (sg_repo_array + total_len));
    tuya_ble_dp_data_report(sg_repo_array, total_len);
}

/**
 * @brief dp data handler
 * @param[in] dp_data: dp data array
 * @return none
 */
VOID_T hula_hoop_ble_dp_write_handler(IN UCHAR_T *dp_data)
{
    if (hula_hoop_get_device_status() == STAT_USING) {
        hula_hoop_reset_timer_for_key_event();
    }
    switch (dp_data[0]) {
    case DP_ID_MODE:
        {
            hula_hoop_set_work_mode(dp_data[3]);
            __report_sport_data_realtime();
            TUYA_APP_LOG_INFO("APP set the mode : %d.", dp_data[3]);
        }
        break;
    case DP_ID_TIME_TARGET_TODAY:
        {
            hula_hoop_set_time_target_today(dp_data[6]);
            __report_one_dp_data(DP_ID_TIME_REMAIN_TODAY, DT_VALUE, SIZEOF(g_sport_data.time_remain_today), (UCHAR_T *)&g_sport_data.time_remain_today);
            TUYA_APP_LOG_INFO("APP set today's target : %dmin.", dp_data[6]);
        }
        break;
    case DP_ID_TIME_TARGET_MONTH:
        {
            USHORT_T tar_tm = (((USHORT_T)dp_data[5]) << 8) | ((USHORT_T)dp_data[6]);
            hula_hoop_set_time_target_month(tar_tm);
            __report_one_dp_data(DP_ID_TIME_REMAIN_MONTH, DT_VALUE, SIZEOF(g_sport_data.time_remain_month), (UCHAR_T *)&g_sport_data.time_remain_month);
            TUYA_APP_LOG_INFO("APP set this month's target : %dmin.", tar_tm);
        }
        break;
    default:
        break;
    }
}

/**
 * @brief ble connection status handler
 * @param[in] status: ble connect status
 * @return none
 */
VOID_T hula_hoop_ble_conn_stat_handler(IN CONST tuya_ble_connect_status_t status)
{
    if (hula_hoop_get_device_status() == STAT_UNUSED) {
        if (status == BONDING_UNCONN){
            hula_hoop_set_device_status(STAT_SLEEP);
        }
        return;
    }
    if (status == BONDING_CONN) {
        __report_all_dp_data();
        tuya_ble_time_req(BLE_TIME_TYPE_NORMAL);
        if (F_WAIT_BINDING == SET) {
            F_BLE_BOUND = SET;
            F_WAIT_BINDING = CLR;
            hula_hoop_disp_set_led_func(LED_FUNC_DATA);
        }
        if (hula_hoop_get_device_status() == STAT_USING) {
            hula_hoop_reset_timer_for_key_event();
        }
    }
}

/**
 * @brief ble unbound handler
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_ble_unbound_handler(VOID_T)
{
    F_BLE_BOUND = CLR;
    bls_ll_setAdvEnable(0);
}

/**
 * @brief ble time normal handler
 * @param[in] time_normal: ble time data in normal format
 * @return none
 */
VOID_T hula_hoop_ble_time_normal_handler(IN CONST tuya_ble_time_noraml_data_t time_normal)
{
    LOCAL_TIME_T time_now = {
        .year = time_normal.nYear + 2000,
        .month = time_normal.nMonth,
        .day = time_normal.nDay,
        .hour = time_normal.nHour,
        .minute = time_normal.nMin,
        .second = time_normal.nSec
    };
    tuya_set_local_time(time_now, (time_normal.time_zone / 100));
    hula_hoop_check_date_change();
    TUYA_APP_LOG_INFO("Local time has been updated to %04d.%02d.%02d %02d:%02d:%02d.\n",
                      g_local_time.year, g_local_time.month, g_local_time.day,
                      g_local_time.hour, g_local_time.minute, g_local_time.second);
}
