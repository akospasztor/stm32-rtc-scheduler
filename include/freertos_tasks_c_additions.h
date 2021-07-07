/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    freertos_tasks_c_additions.h
 * @brief   This file contains the additional code that needs to be inserted
 *          into the FreeRTOS tasks.c file.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

#ifndef FREERTOS_TASKS_C_ADDITIONS_H
#define FREERTOS_TASKS_C_ADDITIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Get the expected idle time from RTOS kernel.
 *
 * @return  The expected idle time in RTOS ticks.
 */
TickType_t GetExpectedIdleTime(void)
{
    TickType_t xReturn;
    UBaseType_t uxHigherPriorityReadyTasks = pdFALSE;

    /* uxHigherPriorityReadyTasks takes care of the case where
    configUSE_PREEMPTION is 0, so there may be tasks above the idle priority
    task that are in the Ready state, even though the idle task is running. */
#if(configUSE_PORT_OPTIMISED_TASK_SELECTION == 0)
    {
        if(uxTopReadyPriority > tskIDLE_PRIORITY)
        {
            uxHigherPriorityReadyTasks = pdTRUE;
        }
    }
#else
    {
        const UBaseType_t uxLeastSignificantBit = (UBaseType_t)0x01;

        /* When port optimised task selection is used the uxTopReadyPriority
        variable is used as a bit map.  If bits other than the least
        significant bit are set then there are tasks that have a priority
        above the idle priority that are in the Ready state.  This takes
        care of the case where the co-operative scheduler is in use. */
        if(uxTopReadyPriority > uxLeastSignificantBit)
        {
            uxHigherPriorityReadyTasks = pdTRUE;
        }
    }
#endif

    if(pxCurrentTCB->uxPriority > tskIDLE_PRIORITY)
    {
        xReturn = 0;
    }
    else if(listCURRENT_LIST_LENGTH(&(pxReadyTasksLists[tskIDLE_PRIORITY])) > 1)
    {
        /* There are other idle priority tasks in the ready state.  If
        time slicing is used then the very next tick interrupt must be
        processed. */
        xReturn = 0;
    }
    else if(uxHigherPriorityReadyTasks != pdFALSE)
    {
        /* There are tasks in the Ready state that have a priority above the
        idle priority.  This path can only be reached if
        configUSE_PREEMPTION is 0. */
        xReturn = 0;
    }
    else
    {
        xReturn = xNextTaskUnblockTime - xTickCount;
    }

    return xReturn;
}

/**
 * @brief  Check whether the delayed task list of RTOS is empty.
 *
 * @retval pdTRUE   if the delayed task list is empty.
 * @retval pdFALSE  if there is a task waiting in the delayed task list.
 */
UBaseType_t IsDelayedTaskListEmpty(void)
{
    return (listCURRENT_LIST_LENGTH(pxDelayedTaskList)) ? pdFALSE : pdTRUE;
}

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_TASKS_C_ADDITIONS_H */
