/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    main.h
 * @brief   This file contains the application-related definitions.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Defines -------------------------------------------------------------------*/
/** Minimum expected idle time required before entering sleep mode in [ms] */
#define IDLE_TIME_BEFORE_SLEEP 1000U

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
