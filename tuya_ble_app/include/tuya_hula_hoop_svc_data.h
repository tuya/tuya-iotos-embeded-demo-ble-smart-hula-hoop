/**
 * @file tuya_hula_hoop_svc_data.h
 * @author lifan
 * @brief hula hoop data service processing module header file
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_HULA_HOOP_SVC_DATA_H__
#define __TUYA_HULA_HOOP_SVC_DATA_H__

#include "tuya_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/
#define SPORT_DATA_HISTORY_SIZE 30

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct {
    USHORT_T time_realtime;
    USHORT_T count_realtime;
    USHORT_T calories_realtime;
    USHORT_T time_total_today;
    UINT_T count_total_today;
    USHORT_T calories_total_today;
    USHORT_T time_total_month;
    USHORT_T time_total_30days;
    UINT_T count_total_30days;
    UINT_T calories_total_30days;
    UCHAR_T time_target_today;
    USHORT_T time_target_month;
    UCHAR_T time_remain_today;
    USHORT_T time_remain_month;
    USHORT_T time_total_days[SPORT_DATA_HISTORY_SIZE];
    UINT_T count_total_days[SPORT_DATA_HISTORY_SIZE];
    USHORT_T calories_total_days[SPORT_DATA_HISTORY_SIZE];
} HULA_HOOP_SPORT_DATA_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
extern HULA_HOOP_SPORT_DATA_T g_sport_data;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief sport data process module init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_data_proc_init(VOID_T);

/**
 * @brief update sport data --timer
 * @param[in] none
 * @return TRUE - target finished, FALSE - target unfinished
 */
BOOL_T hula_hoop_update_sport_data_time(VOID_T);

/**
 * @brief update sport data --count
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_update_sport_data_count(VOID_T);

/**
 * @brief update sport data --calories
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_update_sport_data_calories(VOID_T);

/**
 * @brief set target time for today
 * @param[in] tar_tm: target_time
 * @return none
 */
VOID_T hula_hoop_set_time_target_today(IN CONST UCHAR_T tar_tm);

/**
 * @brief set target time for the month
 * @param[in] tar_tm: target_time
 * @return none
 */
VOID_T hula_hoop_set_time_target_month(IN CONST USHORT_T tar_tm);

/**
 * @brief update total data of the days
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_update_total_data_day(VOID_T);

/**
 * @brief update total data of the month
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_update_total_data_month(VOID_T);

/**
 * @brief clear realtime data
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_clear_realtime_data(VOID_T);

/**
 * @brief clear all sport data
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_clear_sport_data(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_HULA_HOOP_SVC_DATA_H__ */
