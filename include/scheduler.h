/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    scheduler.h
 * @brief   This file contains the RTC-based scheduler definitions, structures
 *          and function prototypes.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Defines -------------------------------------------------------------------*/
/** Maximum number of jobs that are allowed to be configured */
#define MAX_NUM_OF_JOBS 10U

/* Typedefs ------------------------------------------------------------------*/
/** Shorthand type for callback functions */
typedef void (*Callback_t)(void);

/* Structures ----------------------------------------------------------------*/
/** Structure of a single job */
typedef struct
{
    /** The period of the job in [s] */
    uint32_t period;
    /** The current remaining time in [s] until the next execution of job */
    uint32_t remainingTime;
    /** Flag to indicate whether the job is pending for execution */
    uint8_t isPending;
    /** Callback that is called when the job is pending for execution */
    Callback_t callback;
} Job_t;

/** Structure of the scheduler */
typedef struct
{
    /** The starting time (Unix epoch) denoting when the scheduler was launched
     * or processed. */
    uint32_t startTime;
    /** Flag to indicate whether the scheduler is running */
    uint8_t isRunning;
    /** The actual number of jobs that the scheduler is scheduling */
    uint8_t numOfJobs;
    /** Array containing the jobs */
    Job_t jobs[MAX_NUM_OF_JOBS];
} Scheduler_t;

/* Functions -----------------------------------------------------------------*/
void SchedulerInit(void);
uint8_t SchedulerAddJob(const uint32_t period, const Callback_t callback);
void SchedulerProcess(void);
void SchedulerExecutePendingJobs(void);
void SchedulerStop(void);

#ifdef __cplusplus
}
#endif

#endif /* SCHEDULER_H */
