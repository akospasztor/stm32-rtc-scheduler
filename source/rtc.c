/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    rtc.c
 * @brief   This file contains the RTC-specific function implementations.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
#include "error_handler.h"
#include "hardware.h"
#include <time.h>

/* Private variables ---------------------------------------------------------*/
/** RTC peripheral handle */
RTC_HandleTypeDef hrtc;

/**
 * @brief  RTC initialization function.
 *
 * This function initializes the RTC peripheral and sets the current date and
 * time to a predefined value.
 */
void RtcInit(void)
{
    RTC_TimeTypeDef sTime = {0U};
    RTC_DateTypeDef sDate = {0U};

    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 4U, 0U);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

    hrtc.Instance            = RTC;
    hrtc.Init.HourFormat     = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv   = 127U;
    hrtc.Init.SynchPrediv    = 249U;
    hrtc.Init.OutPut         = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        ErrorHandler();
    }

    sTime.Hours          = 11U;
    sTime.Minutes        = 0U;
    sTime.Seconds        = 0U;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if(HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
        ErrorHandler();
    }

    sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
    sDate.Month   = RTC_MONTH_FEBRUARY;
    sDate.Date    = 24U;
    sDate.Year    = 21U;
    if(HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
        ErrorHandler();
    }
}

/**
 * @brief  Get the current epoch.
 *
 * This function retrieves the date and time from the RTC registers and converts
 * the values into Unix epoch.
 *
 * @see     https://www.epochconverter.com/programming/c
 * @return  The current epoch in [s].
 */
uint32_t RtcGetEpoch(void)
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    struct tm       dateTime;
    static time_t   epoch;

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    dateTime.tm_year  = date.Year + 100U;
    dateTime.tm_mon   = date.Month - 1U;
    dateTime.tm_mday  = date.Date;
    dateTime.tm_hour  = time.Hours;
    dateTime.tm_min   = time.Minutes;
    dateTime.tm_sec   = time.Seconds;
    dateTime.tm_isdst = 0U;

    epoch = mktime(&dateTime);

    return (uint32_t)epoch;
}

/**
 * @brief  Convert an epoch to human-readable date and time format.
 *
 * This function converts an Unix epoch into human-readable date and time
 * format.
 *
   @see  https://www.epochconverter.com/programming/c
 * @param epoch  The epoch to be converted.
 * @param date   Pointer to a RTC date structure where the converted values are
 *               written.
 * @param time   Pointer to a RTC time structure where the converted values are
 *               written.
 */
void RtcConvertEpochToDatetime(uint32_t         epoch,
                               RTC_DateTypeDef* date,
                               RTC_TimeTypeDef* time)
{
    assert_param(date != NULL);
    assert_param(time != NULL);

    time_t    rawTime  = (time_t)epoch;
    struct tm dateTime = *localtime(&rawTime);

    date->Year  = dateTime.tm_year - 100U;
    date->Month = dateTime.tm_mon + 1U;
    date->Date  = dateTime.tm_mday;
    date->WeekDay =
        (dateTime.tm_wday == 0U) ? RTC_WEEKDAY_SUNDAY : dateTime.tm_wday;

    time->Hours      = dateTime.tm_hour;
    time->Minutes    = dateTime.tm_min;
    time->Seconds    = dateTime.tm_sec;
    time->SubSeconds = 0U;
}

/**
 * @brief  Set an RTC alarm at a given time specified by an epoch.
 *
 * @param epoch  The epoch when an alarm should be set.
 * @return       A non-zero number if the alarm has been successfully set;
 *               otherwise 0.
 */
uint8_t RtcSetAlarmFromEpoch(const uint32_t epoch)
{
    static RTC_DateTypeDef  date   = {0U};
    static RTC_TimeTypeDef  time   = {0U};
    static RTC_AlarmTypeDef sAlarm = {0U};
    uint8_t                 result = 0U;

    /* Allow alarms to be set only in the future */
    if(epoch > RtcGetEpoch())
    {
        RtcConvertEpochToDatetime(epoch, &date, &time);

        sAlarm.Alarm                    = RTC_ALARM_A;
        sAlarm.AlarmDateWeekDay         = date.Date;
        sAlarm.AlarmDateWeekDaySel      = RTC_ALARMDATEWEEKDAYSEL_DATE;
        sAlarm.AlarmTime.Hours          = time.Hours;
        sAlarm.AlarmTime.Minutes        = time.Minutes;
        sAlarm.AlarmTime.Seconds        = time.Seconds;
        sAlarm.AlarmTime.SubSeconds     = time.SubSeconds;
        sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
        sAlarm.AlarmMask                = RTC_ALARMMASK_NONE;
        sAlarm.AlarmSubSecondMask       = RTC_ALARMSUBSECONDMASK_ALL;

        if(HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) == HAL_OK)
        {
            result = 1U;
        }
    }
    else
    {
        result = 0U;
    }

    return result;
}

/**
 * @brief  Deactivate a previously set RTC alarm.
 */
void RtcDeactivateAlarm(void)
{
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
}

/**
 * @brief  This function waits until the RTC time and date registers are
 *         synchronized with RTC APB clock. This function needs to be called
 *         after resuming execution from a sleep mode.
 */
void RtcWaitForClockSynchronization(void)
{
    HAL_RTC_WaitForSynchro(&hrtc);
}
