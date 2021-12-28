/**
 * @file tuya_hula_hoop_evt_user.c
 * @author lifan
 * @brief hula hoop user event processing module source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_hula_hoop_evt_user.h"
#include "tuya_hula_hoop_evt_timer.h"
#include "tuya_hula_hoop_svc_basic.h"
#include "tuya_hula_hoop_svc_disp.h"
#include "tuya_hula_hoop_svc_data.h"
#include "tuya_hula_hoop_ble_proc.h"
#include "tuya_key.h"
#include "tuya_hall_sw.h"
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
/* Key and Hall sensor define */
STATIC VOID_T __mode_key_cb(KEY_PRESS_TYPE_E type);
STATIC VOID_T __reset_key_cb(KEY_PRESS_TYPE_E type);
STATIC VOID_T __hall_sw_cb();
KEY_DEF_T mode_key_def_s = {
    .port = TY_GPIOB_7,
    .active_low = TRUE,
    .long_press_time1 = 2000,
    .long_press_time2 = 5000,
    .key_cb = __mode_key_cb
};
KEY_DEF_T reset_key_def_s = {
    .port = TY_GPIOB_1,
    .active_low = TRUE,
    .long_press_time1 = 2000,
    .long_press_time2 = 0,
    .key_cb = __reset_key_cb
};
HALL_SW_DEF_T hall_sw_def_s = {
    .port = TY_GPIOD_2,
    .active_low = FALSE,
    .hall_sw_cb = __hall_sw_cb,
    .invalid_intv = 200000
};

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief key and hall sensor init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_key_hall_init(VOID_T)
{
    UCHAR_T ret;
    /* register mode key */
    ret = tuya_reg_key(&mode_key_def_s);
    if (KEY_OK != ret) {
        TUYA_APP_LOG_ERROR("mode key init error: %d", ret);
    }
    /* register reset key */
    ret = tuya_reg_key(&reset_key_def_s);
    if (KEY_OK != ret) {
        TUYA_APP_LOG_ERROR("reset key init error: %d", ret);
    }
    /* register hall switch */
    ret = tuya_reg_hall_sw(&hall_sw_def_s);
    if (HSW_OK != ret) {
        TUYA_APP_LOG_ERROR("hall switch init error: %d", ret);
    }
}

/**
 * @brief key and hall sensor init on deep retention wakeup
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_key_hall_init_deepRetn(VOID_T)
{
    tuya_key_reset();
    tuya_hall_sw_reset();
}

/**
 * @brief mode key short press handler
 * @param[in] none
 * @return none
 */
STATIC VOID_T __mode_key_short_press_handler(VOID_T)
{
    if (hula_hoop_get_disp_mode() != DISP_MODE_SELECT) {
        hula_hoop_enter_mode_select();
    } else {
        hula_hoop_switch_temp_mode();
    }
}

/**
 * @brief mode key long press handler
 * @param[in] none
 * @return none
 */
STATIC VOID_T __mode_key_long_press_handler(VOID_T)
{
    if (hula_hoop_get_disp_mode() != DISP_MODE_SELECT) {
        return;
    }
    hula_hoop_switch_to_select_mode();
    hula_hoop_reset_upd_time_data_timer();
    hula_hoop_report_mode();
}

/**
 * @brief mode key longer press handler
 * @param[in] none
 * @return none
 */
STATIC VOID_T __mode_key_longer_press_handler(VOID_T)
{
    hula_hoop_allow_binding();
}

/**
 * @brief reset key short press handler
 * @param[in] none
 * @return none
 */
STATIC VOID_T __reset_key_short_press_handler(VOID_T)
{
    if (hula_hoop_get_disp_mode() != DISP_MODE_SELECT) {
        return;
    }
    hula_hoop_quit_mode_select();
}

/**
 * @brief reset key long press handler
 * @param[in] none
 * @return none
 */
STATIC VOID_T __reset_key_long_press_handler(VOID_T)
{
    if (hula_hoop_get_device_status() != STAT_USING) {
        return;
    }
    hula_hoop_disp_switch_to_reset_remind();
}

/**
 * @brief hall switch handler
 * @param[in] none
 * @return none
 */
STATIC VOID_T __hall_switch_handler(VOID_T)
{
    hula_hoop_update_sport_data_count();
    hula_hoop_update_sport_data_calories();
    hula_hoop_set_device_status(STAT_ROTATING);
    hula_hoop_reset_timer_for_hall_event();
}

/**
 * @brief key event common handler
 * @param[in] none
 * @return TRUE - handle finished, FALSE - handle unfinished
 */
STATIC BOOL_T __key_event_handler(VOID_T)
{
    if (FALSE == hula_hoop_disp_is_wakeup()) {
        hula_hoop_disp_wakeup();
        return TRUE;
    } else {
        hula_hoop_reset_timer_for_key_event();
    }
    return FALSE;
}

/**
 * @brief mode key callback function
 * @param[in] type: key event type
 * @return none
 */
STATIC VOID_T __mode_key_cb(KEY_PRESS_TYPE_E type)
{
    BOOL_T ret = __key_event_handler();
    if ((ret) &&
        (type == SHORT_PRESS)) {
        return;
    }

    switch (type) {
    case SHORT_PRESS:
        TUYA_APP_LOG_INFO("Mode key pressed.");
        __mode_key_short_press_handler();
        break;
    case LONG_PRESS_FOR_TIME1:
        TUYA_APP_LOG_INFO("Mode key long pressed 2s.");
        __mode_key_long_press_handler();
        break;
    case LONG_PRESS_FOR_TIME2:
        TUYA_APP_LOG_INFO("Mode key long pressed 5s.");
        __mode_key_longer_press_handler();
        break;
    default:
        break;
    }
}

/**
 * @brief reset key callback function
 * @param[in] type: key event type
 * @return none
 */
STATIC VOID_T __reset_key_cb(KEY_PRESS_TYPE_E type)
{
    BOOL_T ret = __key_event_handler();
    if ((ret) &&
        (type == SHORT_PRESS)) {
        return;
    }

    switch (type) {
    case SHORT_PRESS:
        TUYA_APP_LOG_INFO("Reset key short pressed.");
        __reset_key_short_press_handler();
        break;
    case LONG_PRESS_FOR_TIME1:
        TUYA_APP_LOG_INFO("Reset key long pressed 2s.");
        __reset_key_long_press_handler();
        break;
    case LONG_PRESS_FOR_TIME2:
        break;
    default:
        break;
    }
}

/**
 * @brief hall switch callback function
 * @param[in] none
 * @return none
 */
STATIC VOID_T __hall_sw_cb()
{
    __hall_switch_handler();
}
