/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    main.c
 * @brief   This file contains the main application implementation, including
 *          the RTOS task implementations, RTOS hooks and scheduler callback
 *          implementations.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "FreeRTOS.h"
#include "core_stop.h"
#include "error_handler.h"
#include "hardware.h"
#include "rtc.h"
#include "scheduler.h"
#include "task.h"
#include "timers.h"

/* Private variables ---------------------------------------------------------*/
/** RTOS task handle for the blinking LED task */
TaskHandle_t taskHandleLedBlink = NULL;

/** RTOS task handle for the steady LED task */
TaskHandle_t taskHandleLedSteady = NULL;

/* Private function prototypes -----------------------------------------------*/
void TaskLedBlink(void* arg);
void TaskLedSteady(void* arg);

void JobShortPeriodCallback(void);
void JobLongPeriodCallback(void);

/* External functions --------------------------------------------------------*/
extern TickType_t  GetExpectedIdleTime(void);
extern UBaseType_t IsDelayedTaskListEmpty(void);

/**
 * @brief   The main function of the application.
 *
 * @return  The termination value of the application. This function must never
 *          return.
 */
int main(void)
{
    HAL_Init();
    SystemClockConfig();
    GpioInit();
    RtcInit();
    SchedulerInit();

    SchedulerAddJob(5U, JobShortPeriodCallback);
    SchedulerAddJob(10U, JobLongPeriodCallback);

    if(xTaskCreate(TaskLedBlink,             /* Task function */
                   "task_led_blink",         /* Task name */
                   configMINIMAL_STACK_SIZE, /* Stack size */
                   NULL,                     /* Arguments */
                   2,                        /* Task priority */
                   &taskHandleLedBlink) != pdPASS)
    {
        ErrorHandler();
    }

    if(xTaskCreate(TaskLedSteady,            /* Task function */
                   "task_led_steady",        /* Task name */
                   configMINIMAL_STACK_SIZE, /* Stack size */
                   NULL,                     /* Arguments */
                   1,                        /* Task priority */
                   &taskHandleLedSteady) != pdPASS)
    {
        ErrorHandler();
    }

    /* RTOS Kernel Start */
    vTaskStartScheduler();

    /* This line must not be reached */
    ErrorHandler();
}

/**
 * @brief The callback function for the short period job.
 *
 * This function is called when the job with short period is pending. The
 * function unblocks the blinking LED RTOS task.
 *
 * @warning  The function is executed within an interrupt context. Therefore,
 *           this function must not block, furthermore only interrupt-safe RTOS
 *           API calls are allowed.
 */
void JobShortPeriodCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Unblock the Led Blink Task */
    vTaskNotifyGiveFromISR(taskHandleLedBlink, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief The callback function for the long period job.
 *
 * This function is called when the job with long period is pending. The
 * function unblocks the steady LED RTOS task.
 *
 * @warning  The function is executed within an interrupt context. Therefore,
 *           this function must not block, furthermore only interrupt-safe RTOS
 *           API calls are allowed.
 */
void JobLongPeriodCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Unblock the Led Steady Task */
    vTaskNotifyGiveFromISR(taskHandleLedSteady, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief  This function implements the blinking LED RTOS task.
 *
 * This task waits for an RTOS task notification to unblock. The notification is
 * given in the callback function of the short period job. The task blinks the
 * LD3 LED twice, then waits until the task is unblocked again.
 *
 * @param arg  The RTOS task argument.
 */
void TaskLedBlink(void* arg)
{
    UNUSED(arg);
    uint32_t isTaskUnblocked;

    for(;;)
    {
        isTaskUnblocked = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(isTaskUnblocked != 0U)
        {
            for(uint8_t i = 0U; i < 4U; ++i)
            {
                LedLd3Toggle();
                vTaskDelay(pdMS_TO_TICKS(250));
            }
        }
    }
}

/**
 * @brief  This function implements the steady LED RTOS task.
 *
 * This task waits for an RTOS task notification to unblock. The notification is
 * given in the callback function of the long period job. The task turns on the
 * LD2 LED, waits 1 second, turns off the LD2 LED, then waits until the task is
 * unblocked again.
 *
 * @param arg  The RTOS task argument.
 */
void TaskLedSteady(void* arg)
{
    UNUSED(arg);
    uint32_t isTaskUnblocked;

    for(;;)
    {
        isTaskUnblocked = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(isTaskUnblocked != 0U)
        {
            LedLd2On();
            vTaskDelay(pdMS_TO_TICKS(1000));
            LedLd2Off();
        }
    }
}

/**
 * @brief  RTOS daemon startup callback.
 *
 * This function is called only once after starting the kernel. The function
 * runs in the context of the built-in RTOS timer/daemon task.
 */
void vApplicationDaemonTaskStartupHook(void)
{
    SchedulerProcess();
}

/**
 * @brief  RTOS idle task hook.
 *
 * This function is called once per idle task execution. During each function
 * call, the function asks the RTOS kernel what is the expected idle time and
 * whether the delayed task list is empty. If the expected idle time is greater
 * than a specified duration and the delayed task list is empty, the function
 * enters into STOP2 mode.
 */
void vApplicationIdleHook(void)
{
    const uint32_t expectedIdleTime = GetExpectedIdleTime();

    if((expectedIdleTime > pdMS_TO_TICKS(IDLE_TIME_BEFORE_SLEEP)) &&
       (IsDelayedTaskListEmpty() == pdTRUE))
    {
        EnterStop2Mode();
        ResumeFromStop2Mode();
    }
}

/**
 * @brief  RTOS task stack overflow hook.
 *
 * This callback is function is called upon detecting a stack overflow during
 * task execution.
 *
 * @param xTask       Handle to the task where stack overflow is detected.
 * @param pcTaskName  Name of task where stack overflow is detected.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
    UNUSED(xTask);
    UNUSED(pcTaskName);

    ErrorHandler();
}

/**
 * @brief  RTOS malloc failed hook.
 *
 * This callback function is called upon memory allocation failure.
 */
void vApplicationMallocFailedHook(void)
{
    ErrorHandler();
}
