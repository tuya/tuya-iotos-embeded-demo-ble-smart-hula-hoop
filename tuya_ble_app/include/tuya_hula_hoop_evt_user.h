/**
 * @file tuya_hula_hoop_evt_user.h
 * @author lifan
 * @brief hula hoop user event processing module header file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_HULA_HOOP_EVT_USER_H__
#define __TUYA_HULA_HOOP_EVT_USER_H__

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
 * @brief key and hall sensor init
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_key_hall_init(VOID_T);

/**
 * @brief key and hall sensor init on deep retention wakeup
 * @param[in] none
 * @return none
 */
VOID_T hula_hoop_key_hall_init_deepRetn(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_HULA_HOOP_EVT_USER_H__ */
