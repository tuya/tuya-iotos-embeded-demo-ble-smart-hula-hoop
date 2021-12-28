/**
 * @file tuya_hall_sw.c
 * @author lifan
 * @brief hall switch driver source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_hall_sw.h"
#include "tuya_timer.h"
#include "tuya_ble_mem.h"

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct hall_sw_manage_s {
    struct hall_sw_manage_s *next;
    HALL_SW_DEF_T *def;
    UINT_T wk_tm;
} HALL_SW_MANAGE_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC HALL_SW_MANAGE_T *sg_hsw_mag_list = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/
STATIC VOID_T __hall_sw_irq_handler(TY_GPIO_PORT_E port);

/**
 * @brief hall switch gpio init
 * @param[in] pin: pin number
 * @param[in] active_low: TRUE - active low, FALSE - active high
 * @return none
 */
STATIC VOID_T __hall_sw_gpio_init(IN CONST TY_GPIO_PORT_E port, IN CONST BOOL_T active_low)
{
    tuya_gpio_init(port, TRUE, active_low);
    if (active_low) {
        tuya_gpio_irq_init(port, TY_GPIO_IRQ_FALLING, __hall_sw_irq_handler);
    } else {
        tuya_gpio_irq_init(port, TY_GPIO_IRQ_RISING, __hall_sw_irq_handler);
    }
}

/**
 * @brief hall switch register
 * @param[in] hsw_def: user hall switch define
 * @return HSW_RET
 */
HSW_RET tuya_reg_hall_sw(IN HALL_SW_DEF_T *hsw_def)
{
    /* check callback function */
    if (hsw_def->hall_sw_cb == NULL) {
        return HSW_ERR_CB_UNDEFINED;
    }

    /* allocate and clear for hall_sw_mag */
    HALL_SW_MANAGE_T *hall_sw_mag = (HALL_SW_MANAGE_T *)tuya_ble_malloc(SIZEOF(HALL_SW_MANAGE_T));
    if (NULL == hall_sw_mag) {
        return HSW_ERR_MALLOC_FAILED;
    }
    hall_sw_mag->def = hsw_def;

    /* update hall sw manage list */
    if (sg_hsw_mag_list) {
        hall_sw_mag->next = sg_hsw_mag_list;
    }
    sg_hsw_mag_list = hall_sw_mag;

    /* gpio init */
    __hall_sw_gpio_init(hsw_def->port, hsw_def->active_low);

    return HSW_OK;
}

/**
 * @brief hall switch reset
 * @param[in] none
 * @return HSW_RET
 */
HSW_RET tuya_hall_sw_reset(VOID_T)
{
    HALL_SW_MANAGE_T *hsw_mag_tmp = sg_hsw_mag_list;
    if (NULL == hsw_mag_tmp) {
        return HSW_ERR_CB_UNDEFINED;
    }
    while (hsw_mag_tmp) {
        __hall_sw_gpio_init(hsw_mag_tmp->def->port, hsw_mag_tmp->def->active_low);
        hsw_mag_tmp = hsw_mag_tmp->next;
    }
    return HSW_OK;
}

/**
 * @brief hall switch trigger handler
 * @param[in] hsw_mag: hall switch management
 * @return none
 */
STATIC VOID_T __hall_sw_trigger_handler(IN HALL_SW_MANAGE_T *hsw_mag)
{
    /* interval detection between two triggers */
    if (!tuya_is_clock_time_exceed(hsw_mag->wk_tm, hsw_mag->def->invalid_intv)) {
        return;
    }
    hsw_mag->wk_tm = tuya_get_clock_time();
    /* callback */
    hsw_mag->def->hall_sw_cb();
}

/**
 * @brief hall switch irq handler
 * @param[in] port: gpio number
 * @return none
 */
STATIC VOID_T __hall_sw_irq_handler(TY_GPIO_PORT_E port)
{
    HALL_SW_MANAGE_T *hsw_mag_tmp = sg_hsw_mag_list;
    while (hsw_mag_tmp) {
        if (hsw_mag_tmp->def->port == port) {
            __hall_sw_trigger_handler(hsw_mag_tmp);
            break;
        }
        hsw_mag_tmp = hsw_mag_tmp->next;
    }
}
