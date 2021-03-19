/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    scheduler.c
 * @brief   This file contains the RTC-based scheduler function implementations.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "scheduler.h"
#include "rtc.h"

/* Private variables ---------------------------------------------------------*/
/** The scheduler object */
Scheduler_t scheduler;

/* Private function prototypes -----------------------------------------------*/
void Scheduler_ProcessRemainingTime(const uint32_t elapsedTime);

/**
 * @brief  Initialize the scheduler by setting its structure values to zero.
 */
void SchedulerInit(void)
{
    scheduler.startTime = 0U;
    scheduler.isRunning = 0U;
    scheduler.numOfJobs = 0U;
}

/**
 * @brief  Add a new job to the scheduler.
 *
 * @param period    The period in [s] which the job needs to be executed.
 * @param callback  The callback function that is called upon job execution.
 * @return  A non-zero value if the job has been successfully added; othwerwise
 *          zero.
 */
uint8_t SchedulerAddJob(const uint32_t period, const Callback_t callback)
{
    uint8_t result = 0U;

    assert_param(period > 0U);
    assert_param(callback != NULL);

    if(scheduler.isRunning == 0U)
    {
        if(scheduler.numOfJobs < MAX_NUM_OF_JOBS)
        {
            scheduler.jobs[scheduler.numOfJobs].period        = period;
            scheduler.jobs[scheduler.numOfJobs].remainingTime = period;
            scheduler.jobs[scheduler.numOfJobs].isPending     = 0U;
            scheduler.jobs[scheduler.numOfJobs].callback      = callback;
            ++scheduler.numOfJobs;
        }
        else
        {
            result = 0U;
        }
    }
    else
    {
        result = 0U;
    }

    return result;
}

/**
 * @brief  Process the scheduler.
 *
 * This function needs to be called each time upon an RTC alarm interrupt. The
 * function checks all jobs whether they need to be executed and sets the
 * appropriate pending flags.
 *
 * @note  The function does not automatically execute the callbacks of the
 *        pending jobs to provide more flexibility for the application.
 *        The pending jobs can be executed by calling the
 *        ::SchedulerExecutePendingJobs() function.
 */
void SchedulerProcess(void)
{
    uint8_t isScheduleNextJob = 0U;

    if(scheduler.isRunning != 0U)
    {
        const uint32_t elapsedTime = RtcGetEpoch() - scheduler.startTime;
        if(elapsedTime > 0U)
        {
            /* Process the remaining time of the jobs */
            Scheduler_ProcessRemainingTime(elapsedTime);

            /* Schedule next job */
            isScheduleNextJob = 1U;
        }
        else
        {
            /* Elapsed time is zero: no need to process and schedule jobs */
            isScheduleNextJob = 0U;
        }
    }
    else
    {
        /* Scheduler is not running: start the scheduler */
        isScheduleNextJob = 1U;
    }

    /* Schedule next job */
    if(isScheduleNextJob != 0U)
    {
        /* Search for the next job with the lowest remaining time */
        uint8_t indexOfNextJob = 0;
        for(uint_fast8_t i = 0U; i < scheduler.numOfJobs; ++i)
        {
            if(scheduler.jobs[i].remainingTime <
               scheduler.jobs[indexOfNextJob].remainingTime)
            {
                indexOfNextJob = i;
            }
        }

        /* Set RTC alarm for next job */
        if(scheduler.jobs[indexOfNextJob].remainingTime > 0U)
        {
            scheduler.startTime = RtcGetEpoch();
            if(RtcSetAlarmFromEpoch(
                   scheduler.startTime +
                   scheduler.jobs[indexOfNextJob].remainingTime) != 0U)
            {
                scheduler.isRunning = 1U;
            }
        }
    }
}

/**
 * @brief  Execute the pending jobs.
 *
 * This function checks the current state of each job. If a job is pending, its
 * callback is executed.
 *
 * @note  The processing function of the scheduler does not automatically
 *        execute the callbacks of the pending jobs. Therefore, this function
 *        must be called after calling the processing function of the scheduler.
 *
 * @warning  Depending on the implementation, this function may be called from
 *           an interrupt context, or a task context (or both). Consequently,
 *           the callback functions of the jobs are executed from the same
 *           context. This must be taken into account, i.e. care must be taken
 *           upon writing the callbacks of the jobs. For instance, calling this
 *           function from an ISR means that the jobs are also executed within
 *           the context of the ISR, thus the callbacks must not block and they
 *           can only call interrupt-safe RTOS API functions.
 */
void SchedulerExecutePendingJobs(void)
{
    for(uint_fast8_t i = 0U; i < scheduler.numOfJobs; ++i)
    {
        if(scheduler.jobs[i].isPending != 0U)
        {
            /* Execute job callback */
            if(scheduler.jobs[i].callback != 0U)
            {
                scheduler.jobs[i].callback();
            }

            /* Reset pending flag */
            scheduler.jobs[i].isPending = 0U;
        }
    }
}

/**
 * @brief  Stop the scheduler.
 *
 * This function stops the running scheduler and deactivates the RTC alarm. The
 * function also processes the jobs.
 *
 * @note  The function does not automatically execute the callbacks of the
 *        pending jobs to provide more flexibility for the application.
 *        The pending jobs can be executed by calling the
 *        ::SchedulerExecutePendingJobs() function.
 */
void SchedulerStop(void)
{
    if(scheduler.isRunning != 0U)
    {
        /* Deactivate RTC alarm */
        RtcDeactivateAlarm();

        const uint32_t elapsedTime = RtcGetEpoch() - scheduler.startTime;
        if(elapsedTime > 0U)
        {
            /* Process the remaining time of the jobs */
            Scheduler_ProcessRemainingTime(elapsedTime);
        }
        else
        {
            /* Elapsed time is zero: no need to process and schedule jobs */
        }

        /* Stop the scheduler */
        scheduler.isRunning = 0U;
    }
    else
    {
        /* Scheduler is already stopped: do nothing */
    }
}

/**
 * @brief  This function calculates the remaining time for each job.
 *
 * @param elapsedTime  The elapsed time since the launch of the scheduler.
 */
void Scheduler_ProcessRemainingTime(const uint32_t elapsedTime)
{
    for(uint_fast8_t i = 0U; i < scheduler.numOfJobs; ++i)
    {
        if(elapsedTime >= scheduler.jobs[i].remainingTime)
        {
            /* Job is ready: reset remaining time and set pending flag */
            scheduler.jobs[i].remainingTime = scheduler.jobs[i].period;
            scheduler.jobs[i].isPending     = 1U;
        }
        else
        {
            /* Decrease remaining time */
            scheduler.jobs[i].remainingTime -= elapsedTime;
        }
    }
}
