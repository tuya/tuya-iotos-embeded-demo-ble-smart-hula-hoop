/**
 * @file tuya_hula_hoop_evt_timer.h
 * @author lifan
 * @brief hula hoop timer event processing module header file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_HULA_HOOP_EVT_TIMER_H__
#define __TUYA_HULA_HOOP_EVT_TIMER_H__

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

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief timer init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_timer_init(VOID_T);

/**
 * @brief timer reset
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_timer_reset(VOID_T);

/**
 * @brief reset timer when key events are received
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_reset_timer_for_key_event(VOID_T);

/**
 * @brief reset timer when hall events are received
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_reset_timer_for_hall_event(VOID_T);

/**
 * @brief reset upd_time_data_timer
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_reset_upd_time_data_timer(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_HULA_HOOP_EVT_TIMER_H__ */
