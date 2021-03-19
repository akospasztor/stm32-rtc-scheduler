/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    core_stop.h
 * @brief   This file contains the function prototypes for entering and leaving
 *          low power modes.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

#ifndef CORE_STOP_H
#define CORE_STOP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Functions -----------------------------------------------------------------*/
void EnterStop2Mode(void);
void ResumeFromStop2Mode(void);

#ifdef __cplusplus
}
#endif

#endif /* CORE_STOP_H */
