/**
 * @file tuya_hall_sw.h
 * @author lifan
 * @brief hall switch driver header file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_HALL_SW_H__
#define __TUYA_HALL_SW_H__

#include "tuya_common.h"
#include "tuya_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef BYTE_T HSW_RET;
#define HSW_OK                  0x00
#define HSW_ERR_MALLOC_FAILED   0x01
#define HSW_ERR_CB_UNDEFINED    0x02

typedef VOID_T (*HALL_SW_CALLBACK)();
typedef struct {                    /* user define */
    TY_GPIO_PORT_E port;            /* hall switch port */
    BOOL_T active_low;              /* hall switch's active level is low? */
    HALL_SW_CALLBACK hall_sw_cb;    /* hall switch trigger callback function */
    UINT_T invalid_intv;            /* it's invalid when the interval between two triggers is less than this time */
} HALL_SW_DEF_T;

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief hall switch register
 * @param[in] hsw_def: user hall switch define
 * @return HSW_RET
 */
HSW_RET tuya_reg_hall_sw(IN HALL_SW_DEF_T *hsw_def);

/**
 * @brief hall switch reset
 * @param[in] none
 * @return HSW_RET
 */
HSW_RET tuya_hall_sw_reset(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_HALL_SW_H__ */
