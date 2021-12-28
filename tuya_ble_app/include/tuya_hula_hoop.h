/**
 * @file tuya_hula_hoop.h
 * @author lifan
 * @brief hula hoop main header file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_HULA_HOOP_H__
#define __TUYA_HULA_HOOP_H__

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
 * @brief main loop of smart hula hoop
 * @param[in] none
 * @return none
 */
VOID_T tuya_hula_hoop_loop(VOID_T);

/**
 * @brief smart hula hoop init on deep retention wakeup
 * @param[in] none
 * @return none
 */
VOID_T tuya_hula_hoop_init_deepRetn(VOID_T);

/**
 * @brief smart hula_hoop init
 * @param[in] none
 * @return none
 */
VOID_T tuya_hula_hoop_init(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_HULA_HOOP_H__ */
