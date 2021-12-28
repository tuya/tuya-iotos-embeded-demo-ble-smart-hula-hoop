/**
 * @file tuya_hula_hoop_svc_basic.c
 * @author lifan
 * @brief hula hoop basic service processing module source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_hula_hoop_svc_basic.h"
#include "tuya_hula_hoop_svc_disp.h"
#include "tuya_hula_hoop_svc_data.h"
#include "tuya_local_time.h"
#include "tuya_ble_log.h"
#include "timer.h"
#include "pm.h"

/***********************************************************
************************micro define************************
***********************************************************/
#define SLEEP_TIME_SEC  10

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
HULA_HOOP_T g_hula_hoop;
STATIC UINT_T sg_sys_time = 0;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief set work mode
 * @param[in] mode: work mode
 * @return none
 */
STATIC VOID_T __set_work_mode(IN CONST MODE_E mode)
{
    g_hula_hoop.mode = mode;
    switch (mode) {
    case MODE_NORMAL:
        TUYA_APP_LOG_INFO("Work mode is normal mode now.");
        hula_hoop_disp_switch_to_normal_mode();
        break;
    case MODE_TARGET:
        TUYA_APP_LOG_INFO("Work mode is target mode now.");
        hula_hoop_disp_switch_to_target_mode();
        break;
    default:
        break;
    }
}

/**
 * @brief switch temporary mode
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_switch_temp_mode(VOID_T)
{
    switch (g_hula_hoop.mode_temp) {
    case MODE_NORMAL:
        g_hula_hoop.mode_temp = MODE_TARGET;
        break;
    case MODE_TARGET:
        g_hula_hoop.mode_temp = MODE_NORMAL;
        break;
    default:
        break;
    }
}

/**
 * @brief enter mode select
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_enter_mode_select(VOID_T)
{
    g_hula_hoop.mode_temp = g_hula_hoop.mode;
    hula_hoop_disp_switch_to_mode_select();
}

/**
 * @brief quit from mode select
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_quit_mode_select(VOID_T)
{
    __set_work_mode(g_hula_hoop.mode);
}

/**
 * @brief switch to select mode
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_switch_to_select_mode(VOID_T)
{
    hula_hoop_clear_realtime_data();
    __set_work_mode(g_hula_hoop.mode_temp);
}

/**
 * @brief set work mode
 * @param[in] mode: work mode
 * @return none
 */
VOID_T hula_hoop_set_work_mode(IN CONST MODE_E mode)
{
    if (mode != g_hula_hoop.mode) {
        __set_work_mode(mode);
    }
}

/**
 * @brief set device sleep, called by "app_power_management()"
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_set_device_sleep(VOID_T)
{
    if (g_hula_hoop.stat != STAT_SLEEP) {
        return;
    }
    /* save current time */
    sg_sys_time = clock_time();
    /* set wakeup pin then sleep */
    GPIO_WAKEUP_MODULE_HIGH;
    cpu_set_gpio_wakeup(GPIO_WAKEUP_MODULE, Level_Low, 1);
    cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_PAD|PM_WAKEUP_TIMER, clock_time()+SLEEP_TIME_SEC*CLOCK_16M_SYS_TIMER_CLK_1S);
}

/**
 * @brief set device status
 * @param[in] stat: device status
 * @return none
 */
STATIC VOID_T __set_device_status(IN CONST STAT_E stat)
{
    g_hula_hoop.stat = stat;
    switch (stat) {
    case STAT_USING:
        TUYA_APP_LOG_INFO("Device status is 'using'.\n");
        hula_hoop_disp_wakeup();
        break;
    case STAT_ROTATING:
        TUYA_APP_LOG_INFO("Device status is 'rotating'.\n");
        hula_hoop_disp_wakeup();
        break;
    case STAT_UNUSED:
        TUYA_APP_LOG_INFO("Device status is 'unused'.\n");
        hula_hoop_clear_realtime_data();
        hula_hoop_disp_sleep();
        break;
    case STAT_SLEEP:
        break;
    case STAT_RESET:
        start_reboot();
        break;
    default:
        break;
    }
}

/**
 * @brief set device status
 * @param[in] stat: device status
 * @return none
 */
VOID_T hula_hoop_set_device_status(IN CONST STAT_E stat)
{
    if (stat != g_hula_hoop.stat) {
        __set_device_status(stat);
    }
}

/**
 * @brief get device status
 * @param[in] none
 * @return device status
 */
STAT_E hula_hoop_get_device_status(VOID_T)
{
    return g_hula_hoop.stat;
}

/**
 * @brief check if the date has changed
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_check_date_change(VOID_T)
{
    STATIC UCHAR_T s_day = 0;
    STATIC UCHAR_T s_month = 0;
    LOCAL_TIME_T local_time;
    /* get local time */
    local_time = tuya_get_local_time();
    /* is day changed? */
    if (local_time.day != s_day) {
        s_day = local_time.day;
        hula_hoop_update_total_data_day();
    }
    /* is month changed? */
    if (local_time.month != s_month) {
        s_month = local_time.month;
        hula_hoop_update_total_data_month();
    }
}

/**
 * @brief hula hoop basic service module init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_basic_service_init(VOID_T)
{
    memset(&g_hula_hoop, 0, SIZEOF(HULA_HOOP_T));
    __set_device_status(STAT_USING);
    __set_work_mode(MODE_NORMAL);
}

/**
 * @brief update local time
 * @param[in] time_diff_sec: time difference in seconds
 * @return none
 */
VOID_T __update_local_time(IN UCHAR_T time_diff_sec)
{
    while(time_diff_sec--) {
        tuya_local_time_update();
    }
    hula_hoop_check_date_change();
}

/**
 * @brief set device work
 * @param[in] none
 * @return none
 */
VOID_T __set_device_work(VOID_T)
{
    /* local time processing */
    UINT_T time_diff = clock_time() - sg_sys_time;
    if ((time_diff % CLOCK_16M_SYS_TIMER_CLK_1S) >= (CLOCK_16M_SYS_TIMER_CLK_1S / 2)) {
        time_diff = time_diff / CLOCK_16M_SYS_TIMER_CLK_1S + 1;
    } else {
        time_diff = time_diff / CLOCK_16M_SYS_TIMER_CLK_1S;
    }
    __update_local_time(time_diff);
    TUYA_APP_LOG_INFO("Time difference: %ds.", time_diff);
    TUYA_APP_LOG_INFO("Local time has been updated to %04d.%02d.%02d %02d:%02d:%02d.\n",
                g_local_time.year, g_local_time.month, g_local_time.day,
                g_local_time.hour, g_local_time.minute, g_local_time.second);
    __set_device_status(STAT_USING);
    __set_work_mode(MODE_NORMAL);
}

/**
 * @brief hula hoop device wakeup handler
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_device_wakeup_handler(VOID_T)
{
    if (pm_get_wakeup_src() == (WAKEUP_STATUS_PAD | WAKEUP_STATUS_CORE)) {
        /* wakeup from gpio */
        __set_device_work();
    } else {
        /* wakeup from timer */
        __update_local_time(SLEEP_TIME_SEC);
    }
}
