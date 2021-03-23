/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    rtc.h
 * @brief   This file contains the RTC-specific function prototypes.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

#ifndef RTC_H
#define RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Functions -----------------------------------------------------------------*/
void     RtcInit(void);
uint32_t RtcGetEpoch(void);
void     RtcConvertEpochToDatetime(uint32_t         epoch,
                                   RTC_DateTypeDef* date,
                                   RTC_TimeTypeDef* time);
uint8_t  RtcSetAlarmFromEpoch(const uint32_t epoch);
void     RtcDeactivateAlarm(void);
void     RtcWaitForClockSynchronization(void);

#ifdef __cplusplus
}
#endif

#endif /* RTC_H */
