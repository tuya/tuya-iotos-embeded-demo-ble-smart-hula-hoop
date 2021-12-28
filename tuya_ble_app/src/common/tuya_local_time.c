/**
 * @file tuya_local_time.c
 * @author lifan
 * @brief loacl time module source file
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_local_time.h"
#include <string.h>

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/* Local time define */
LOCAL_TIME_T g_local_time = {
    .year = 1,
    .month = 1,
    .day = 1,
    .hour = 0,
    .minute = 0,
    .second = 0,
};
/* Table for days per month  */
STATIC UCHAR_T sg_day_tbl[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief leap year judgment and update February's day number
 * @param[in] year: year for judgment
 * @return day number
 */
STATIC UCHAR_T __local_time_leap_year_judgment(USHORT_T year)
{
    if ((year % 400 == 0) ||
        ((year % 4 == 0) && (year / 100 != 0))) {
        return 29;
    } else {
        return 28;
    }
}

/**
 * @brief update per year
 * @param[in] none
 * @return none
 */
STATIC VOID_T __local_time_update_per_year(VOID_T)
{
    g_local_time.year++;
    sg_day_tbl[1] = __local_time_leap_year_judgment(g_local_time.year);
}

/**
 * @brief update per month
 * @param[in] none
 * @return none
 */
STATIC VOID_T __local_time_update_per_month(VOID_T)
{
    g_local_time.month++;
    if (g_local_time.month > 12) {
        g_local_time.month = 1;
        __local_time_update_per_year();
    }
}

/**
 * @brief update per day
 * @param[in] none
 * @return none
 */
STATIC VOID_T __local_time_update_per_day(VOID_T)
{
    g_local_time.day++;
    if (g_local_time.day > sg_day_tbl[g_local_time.month-1]) {
        g_local_time.day = 1;
        __local_time_update_per_month();
    }
}

/**
 * @brief update per hour
 * @param[in] none
 * @return none
 */
STATIC VOID_T __local_time_update_per_hour(VOID_T)
{
    g_local_time.hour++;
    if (g_local_time.hour >= 24) {
        g_local_time.hour = 0;
        __local_time_update_per_day();
    }
}

/**
 * @brief update per minute
 * @param[in] none
 * @return none
 */
STATIC VOID_T __local_time_update_per_minute(VOID_T)
{
    g_local_time.minute++;
    if (g_local_time.minute >= 60) {
        g_local_time.minute = 0;
        __local_time_update_per_hour();
    }
}

/**
 * @brief update per second
 * @param[in] none
 * @return none
 */
STATIC VOID_T __local_time_update_per_second(VOID_T)
{
    g_local_time.second++;
    if (g_local_time.second >= 60) {
        g_local_time.second = 0;
        __local_time_update_per_minute();
    }
}

/**
 * @brief update interval
 * @param[in] none
 * @return none
 */
VOID_T tuya_local_time_update(VOID_T)
{
    __local_time_update_per_second();
}

/**
 * @brief get local time
 * @param[in] none
 * @return local time now
 */
LOCAL_TIME_T tuya_get_local_time(VOID_T)
{
    return g_local_time;
}

/**
 * @brief set local time
 * @param[in] time: time set
 * @param[in] time_zone: time zone
 * @return none
 */
VOID_T tuya_set_local_time(IN CONST LOCAL_TIME_T time, IN CONST INT_T time_zone)
{
    INT_T tmp_hour = time.hour;
    tmp_hour += time_zone;
    g_local_time = time;

    if (tmp_hour >= 24) {
        g_local_time.hour = 0;
        __local_time_update_per_day();
        return;
    }
    if (tmp_hour <= 0) {
        g_local_time.hour = tmp_hour + 24;
        if (g_local_time.day > 1) {
            g_local_time.day--;
            return;
        }
        if (g_local_time.month > 1) {
            g_local_time.month--;
            sg_day_tbl[1] = __local_time_leap_year_judgment(g_local_time.year);
            g_local_time.day = sg_day_tbl[g_local_time.month-1];
        } else {
            g_local_time.year--;
            g_local_time.month = 12;
            g_local_time.day = sg_day_tbl[g_local_time.month-1];
        }
        return;
    }
    g_local_time.hour = tmp_hour;
}
