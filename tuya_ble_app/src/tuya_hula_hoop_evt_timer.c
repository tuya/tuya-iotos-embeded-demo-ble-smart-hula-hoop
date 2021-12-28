/**
 * @file tuya_hula_hoop_evt_timer.c
 * @author lifan
 * @brief hula hoop timer event processing module source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_hula_hoop_evt_timer.h"
#include "tuya_hula_hoop_svc_basic.h"
#include "tuya_hula_hoop_svc_data.h"
#include "tuya_hula_hoop_svc_disp.h"
#include "tuya_hula_hoop_ble_proc.h"
#include "tuya_local_time.h"
#include "tuya_timer.h"
#include "tuya_ble_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
#define TIMER_PERIOD_MS                 (100)       /* 100ms */
#define DISP_SLEEP_CONFIRM_TIME_MS      (6*1000)    /* 6s */
#define DISP_WAKEUP_CONFIRM_TIME_MS     (5*60*1000) /* 5min */
#define STOP_ROTATING_CONFIRM_TIME_MS   (3000)      /* 3s */
#define STOP_USING_CONFIRM_TIME_MS      (30*1000)   /* 30s */
#define DISP_DATA_SWITCH_INTV_MS        (2000)      /* 2s */
#define TIME_DATA_UPDATE_INTV_MS        (1*60*1000) /* 1min */
#define LOCAL_TIME_UPDATE_INTV_MS       (1000)      /* 1s */
#define DP_DATA_REPO_INTV_MS            (5*1000)    /* 5s */
#define WAIT_BIND_END_TIME_MS           (1*60*1000) /* 1min */

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct {
    UINT_T disp_sleep;
    UINT_T disp_wakeup;
    UINT_T stop_rotating;
    UINT_T stop_using;
    UINT_T switch_disp_data;
    UINT_T upd_time_data;
    UINT_T upd_local_time;
    UINT_T repo_dp_data;
    UINT_T wait_bind;
} HULA_HOOP_TIMER_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC HULA_HOOP_TIMER_T sg_timer;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief display screen sleep confirm timer
 * @param[in] time_inc: time increment
 * @return none
 */
STATIC VOID_T __disp_sleep_timer(IN CONST UINT_T time_inc)
{
    if ((FALSE == hula_hoop_disp_is_wakeup()) ||
        (hula_hoop_get_device_status() != STAT_ROTATING)   ||
        (F_WAIT_BINDING == SET)) {
        sg_timer.disp_sleep = 0;
        return;
    }
    sg_timer.disp_sleep += time_inc;
    if (sg_timer.disp_sleep >= DISP_SLEEP_CONFIRM_TIME_MS) {
        if (FALSE == hula_hoop_disp_is_flash()) {
            hula_hoop_disp_sleep();
        }
    }
}

/**
 * @brief display screen wakeup confirm timer
 * @param[in] time_inc: time increment
 * @return none
 */
STATIC VOID_T __disp_wakeup_timer(IN CONST UINT_T time_inc)
{
    if ((TRUE == hula_hoop_disp_is_wakeup()) ||
        (hula_hoop_get_device_status() != STAT_ROTATING)) {
        sg_timer.disp_wakeup = 0;
        return;
    }
    sg_timer.disp_wakeup += time_inc;
    if (sg_timer.disp_wakeup >= DISP_WAKEUP_CONFIRM_TIME_MS) {
        sg_timer.disp_wakeup = 0;
        hula_hoop_disp_wakeup();
    }
}

/**
 * @brief stop rotating confirm timer
 * @param[in] time_inc: time increment
 * @return none
 */
STATIC VOID_T __stop_rotating_timer(IN CONST UINT_T time_inc)
{
    if (hula_hoop_get_device_status() != STAT_ROTATING) {
        sg_timer.stop_rotating = 0;
        return;
    }
    sg_timer.stop_rotating += time_inc;
    if (sg_timer.stop_rotating >= STOP_ROTATING_CONFIRM_TIME_MS) {
        sg_timer.stop_rotating = 0;
        hula_hoop_set_device_status(STAT_USING);
    }
}

/**
 * @brief stop using confirm timer
 * @param[in] time_inc: time increment
 * @return none
 */
STATIC VOID_T __stop_using_timer(IN CONST UINT_T time_inc)
{
    if ((hula_hoop_get_device_status() != STAT_USING) ||
        (F_WAIT_BINDING == SET)) {
        sg_timer.stop_using = 0;
        return;
    }
    sg_timer.stop_using += time_inc;
    if (sg_timer.stop_using >= STOP_USING_CONFIRM_TIME_MS) {
        sg_timer.stop_using = 0;
        hula_hoop_set_device_status(STAT_UNUSED);
        if (FALSE == hula_hoop_is_need_disconnect()) {
            hula_hoop_set_device_status(STAT_SLEEP);
        }
    }
}

/**
 * @brief switch display data timer
 * @param[in] time_inc: time increment
 * @return none
 */
STATIC VOID_T __switch_disp_data_timer(IN CONST UINT_T time_inc)
{
    if ((hula_hoop_get_disp_mode() != DISP_NORMAL_MODE) ||
        (FALSE == hula_hoop_disp_is_wakeup())) {
        sg_timer.switch_disp_data = 0;
        return;
    }
    sg_timer.switch_disp_data += time_inc;
    if (sg_timer.switch_disp_data >= DISP_DATA_SWITCH_INTV_MS) {
        sg_timer.switch_disp_data -= DISP_DATA_SWITCH_INTV_MS;
        hula_hoop_switch_disp_data();
    }
}

/**
 * @brief update time data timer
 * @param[in] time_inc: time increment
 * @return none
 */
STATIC VOID_T __upd_time_data_timer(IN CONST UINT_T time_inc)
{
    if (hula_hoop_get_device_status() != STAT_ROTATING) {
        return;
    }
    sg_timer.upd_time_data += time_inc;
    if (sg_timer.upd_time_data >= TIME_DATA_UPDATE_INTV_MS) {
        sg_timer.upd_time_data -= TIME_DATA_UPDATE_INTV_MS;
        if (hula_hoop_update_sport_data_time()) {
            hula_hoop_disp_target_finish();
        }
        hula_hoop_report_sport_data1();
    }
}

/**
 * @brief update local time timer
 * @param[in] time_inc: time increment
 * @return none
 */
STATIC VOID_T __upd_local_time_timer(IN CONST UINT_T time_inc)
{
    sg_timer.upd_local_time += time_inc;
    if (sg_timer.upd_local_time >= LOCAL_TIME_UPDATE_INTV_MS) {
        sg_timer.upd_local_time -= LOCAL_TIME_UPDATE_INTV_MS;
        tuya_local_time_update();
        hula_hoop_check_date_change();
    }
}

/**
 * @brief report dp data timer
 * @param[in] time_inc: time increment
 * @return none
 */
STATIC VOID_T __repo_dp_data_timer(IN CONST UINT_T time_inc)
{
    sg_timer.repo_dp_data += time_inc;
    if (sg_timer.repo_dp_data >= DP_DATA_REPO_INTV_MS) {
        sg_timer.repo_dp_data -= DP_DATA_REPO_INTV_MS;
        hula_hoop_report_sport_data2();
    }
}

/**
 * @brief wait for binding timer
 * @param[in] time_inc: time increment
 * @return none
 */
STATIC VOID_T __wait_bind_timer(IN CONST UINT_T time_inc)
{
    if ((F_BLE_BOUND == SET) || (F_WAIT_BINDING == CLR)) {
        sg_timer.wait_bind = 0;
        return;
    }
    sg_timer.wait_bind += time_inc;
    if (sg_timer.wait_bind >= WAIT_BIND_END_TIME_MS) {
        sg_timer.wait_bind = 0;
        hula_hoop_prohibit_binding();
    }
}

/**
 * @brief timer timeout handler
 * @param[in] none
 * @return none
 */
STATIC INT_T __timer_timeout_handler(VOID_T)
{
    __disp_sleep_timer(TIMER_PERIOD_MS);
    __disp_wakeup_timer(TIMER_PERIOD_MS);
    __stop_rotating_timer(TIMER_PERIOD_MS);
    __stop_using_timer(TIMER_PERIOD_MS);
    __switch_disp_data_timer(TIMER_PERIOD_MS);
    __upd_time_data_timer(TIMER_PERIOD_MS);
    __upd_local_time_timer(TIMER_PERIOD_MS);
    __repo_dp_data_timer(TIMER_PERIOD_MS);
    __wait_bind_timer(TIMER_PERIOD_MS);
    return 0;
}

/**
 * @brief timer init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_timer_init(VOID_T)
{
    memset(&sg_timer, 0, SIZEOF(HULA_HOOP_TIMER_T));
    tuya_software_timer_create(TIMER_PERIOD_MS*1000, __timer_timeout_handler);
}

/**
 * @brief timer reset
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_timer_reset(VOID_T)
{
    memset(&sg_timer, 0, SIZEOF(HULA_HOOP_TIMER_T));
    tuya_software_timer_delete(__timer_timeout_handler);
    tuya_software_timer_create(TIMER_PERIOD_MS*1000, __timer_timeout_handler);
}

/**
 * @brief reset timer when key events are received
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_reset_timer_for_key_event(VOID_T)
{
    sg_timer.disp_sleep = 0;
    sg_timer.stop_using = 0;
}

/**
 * @brief reset timer when hall events are received
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_reset_timer_for_hall_event(VOID_T)
{
    sg_timer.stop_rotating = 0;
}

/**
 * @brief reset upd_time_data_timer
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_reset_upd_time_data_timer(VOID_T)
{
    sg_timer.upd_time_data = 0;
}
