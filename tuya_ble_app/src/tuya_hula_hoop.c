/**
 * @file tuya_hula_hoop.c
 * @author lifan
 * @brief hula hoop main source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_hula_hoop.h"
#include "tuya_hula_hoop_evt_user.h"
#include "tuya_hula_hoop_evt_timer.h"
#include "tuya_hula_hoop_svc_basic.h"
#include "tuya_hula_hoop_svc_data.h"
#include "tuya_hula_hoop_svc_disp.h"
#include "tuya_hula_hoop_ble_proc.h"

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief smart hula hoop init
 * @param[in] none
 * @return none
 */
VOID_T tuya_hula_hoop_init(VOID_T)
{
    hula_hoop_timer_init();
    hula_hoop_key_hall_init();
    hula_hoop_disp_proc_init();
    hula_hoop_data_proc_init();
    hula_hoop_ble_proc_init();
    hula_hoop_basic_service_init();
}

/**
 * @brief smart hula hoop init on deep retention wakeup
 * @param[in] none
 * @return none
 */
VOID_T tuya_hula_hoop_init_deepRetn(VOID_T)
{
    hula_hoop_device_wakeup_handler();
    if (hula_hoop_get_device_status() >= STAT_UNUSED) {
        return;
    }
    hula_hoop_key_hall_init_deepRetn();
    hula_hoop_disp_proc_init_deepRetn();
    hula_hoop_ble_proc_init_deepRetn();
    hula_hoop_timer_reset();
}

/**
 * @brief main loop of smart hula hoop
 * @param[in] none
 * @return none
 */
VOID_T tuya_hula_hoop_loop(VOID_T)
{
    if (hula_hoop_get_device_status() >= STAT_UNUSED) {
        return;
    }
    hula_hoop_disp_proc_loop();
}
