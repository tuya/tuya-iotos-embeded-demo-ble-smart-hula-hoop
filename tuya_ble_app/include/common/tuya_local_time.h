/**
 * @file tuya_local_time.h
 * @author lifan
 * @brief loacl time module header file
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_LOCAL_TIME_H__
#define __TUYA_LOCAL_TIME_H__

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
typedef struct {
    USHORT_T year;
    UCHAR_T month;
    UCHAR_T day;
    UCHAR_T hour;
    UCHAR_T minute;
    UCHAR_T second;
} LOCAL_TIME_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
extern LOCAL_TIME_T g_local_time;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief update interval
 * @param[in] none
 * @return none
 */
VOID_T tuya_local_time_update(VOID_T);

/**
 * @brief get local time
 * @param[in] none
 * @return local time now
 */
LOCAL_TIME_T tuya_get_local_time(VOID_T);

/**
 * @brief set local time
 * @param[in] time: time set
 * @param[in] time_zone: time zone
 * @return none
 */
VOID_T tuya_set_local_time(IN CONST LOCAL_TIME_T time, IN CONST INT_T time_zone);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_LOCAL_TIME_H__ */
