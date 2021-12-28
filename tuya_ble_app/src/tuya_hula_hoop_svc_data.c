/**
 * @file tuya_hula_hoop_svc_data.c
 * @author lifan
 * @brief hula hoop data service processing module source file
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_hula_hoop_svc_data.h"
#include "tuya_hula_hoop_svc_basic.h"
#include "tuya_hula_hoop_svc_disp.h"
#include "tuya_local_time.h"
#include "tuya_ble_log.h"

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
HULA_HOOP_SPORT_DATA_T g_sport_data;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief sport data process module init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_data_proc_init(VOID_T)
{
    memset(&g_sport_data, 0, SIZEOF(HULA_HOOP_SPORT_DATA_T));
}

/**
 * @brief update sport data --timer
 * @param[in] none
 * @return TRUE - target finished, FALSE - target unfinished
 */
BOOL_T hula_hoop_update_sport_data_time(VOID_T)
{
    /* realtime */
    if (g_sport_data.time_realtime < 999) {
        g_sport_data.time_realtime++;
    } else {
        g_sport_data.time_realtime = 0;
    }
    /* total */
    g_sport_data.time_total_today++;
    g_sport_data.time_total_month++;
    g_sport_data.time_total_30days++;
    /* remaining */
    if (g_sport_data.time_remain_today > 0) {
        g_sport_data.time_remain_today--;
        if (g_sport_data.time_remain_today == 0) {
            return TRUE;
        }
    }
    if (g_sport_data.time_remain_month > 0) {
        g_sport_data.time_remain_month--;
    }
    return FALSE;
}

/**
 * @brief update sport data --count
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_update_sport_data_count(VOID_T)
{
    /* realtime */
    if (g_sport_data.count_realtime < 999) {
        g_sport_data.count_realtime++;
    } else {
        g_sport_data.count_realtime = 0;
    }
    /* total */
    g_sport_data.count_total_today++;
    g_sport_data.count_total_30days++;
}

/**
 * @brief update sport data --calories
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_update_sport_data_calories(VOID_T)
{
    /* realtime */
    g_sport_data.calories_realtime = g_sport_data.count_realtime / 10;
    /* total */
    g_sport_data.calories_total_today = g_sport_data.count_total_today / 10;
    g_sport_data.calories_total_30days = g_sport_data.count_total_30days / 10;
}

/**
 * @brief set target time for today
 * @param[in] tar_tm: target time
 * @return none
 */
VOID_T hula_hoop_set_time_target_today(IN CONST UCHAR_T tar_tm)
{
    if (tar_tm == g_sport_data.time_target_today) {
        return;
    }
    g_sport_data.time_target_today = tar_tm;
    if (tar_tm > g_sport_data.time_total_today) {
        g_sport_data.time_remain_today = tar_tm - g_sport_data.time_total_today;
    } else {
        g_sport_data.time_remain_today = 0;
    }
}

/**
 * @brief set target time for the month
 * @param[in] tar_tm: target time
 * @return none
 */
VOID_T hula_hoop_set_time_target_month(IN CONST USHORT_T tar_tm)
{
    if (tar_tm == g_sport_data.time_target_month) {
        return;
    }
    g_sport_data.time_target_month = tar_tm;
    if (tar_tm > g_sport_data.time_total_month) {
        g_sport_data.time_remain_month = tar_tm - g_sport_data.time_total_month;
    } else {
        g_sport_data.time_remain_month = 0;
    }
}

/**
 * @brief update total data of the days
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_update_total_data_day(VOID_T)
{
    UCHAR_T i;

    g_sport_data.time_total_30days -= g_sport_data.time_total_days[0];
    g_sport_data.count_total_30days -= g_sport_data.count_total_days[0];
    g_sport_data.calories_total_30days -= g_sport_data.calories_total_days[0];
    for (i = 0; i < (SPORT_DATA_HISTORY_SIZE-1); i++) {
        g_sport_data.time_total_days[i] = g_sport_data.time_total_days[i+1];
        g_sport_data.count_total_days[i] = g_sport_data.count_total_days[i+1];
        g_sport_data.calories_total_days[i] = g_sport_data.calories_total_days[i+1];
    }
    g_sport_data.time_total_days[i] = 0;
    g_sport_data.count_total_days[i] = 0;
    g_sport_data.calories_total_days[i] = 0;
    g_sport_data.time_total_today = 0;
    g_sport_data.count_total_today = 0;
    g_sport_data.calories_total_today = 0;
}

/**
 * @brief update total data of the month
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_update_total_data_month(VOID_T)
{
    g_sport_data.time_total_month = 0;
}

/**
 * @brief clear realtime data
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_clear_realtime_data(VOID_T)
{
    g_sport_data.time_realtime = 0;
    g_sport_data.count_realtime = 0;
    g_sport_data.calories_realtime = 0;
}

/**
 * @brief clear all sport data
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_clear_sport_data(VOID_T)
{
    memset(&g_sport_data, 0, SIZEOF(g_sport_data));
}
