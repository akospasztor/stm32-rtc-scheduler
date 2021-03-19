/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    FreeRTOSConfig.h
 * @brief   This file contains the FreeRTOS configuration definitions. These
 *          parameters are described in the FreeRTOS API documentation.
 * @see     http://www.freertos.org/a00110.html
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif

#define configENABLE_FPU 1
#define configENABLE_MPU 0

#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_TICKLESS_IDLE                 0

#define configCPU_CLOCK_HZ       (SystemCoreClock)
#define configTICK_RATE_HZ       ((TickType_t)1000U)
#define configMINIMAL_STACK_SIZE ((uint16_t)128U)
#define configMAX_PRIORITIES     (5)
#define configMAX_TASK_NAME_LEN  (16)
#define configIDLE_SHOULD_YIELD  1

#define configUSE_16_BIT_TICKS        0
#define configUSE_COUNTING_SEMAPHORES 0
#define configUSE_NEWLIB_REENTRANT    0
#define configUSE_MUTEXES             0
#define configUSE_RECURSIVE_MUTEXES   0
#define configUSE_QUEUE_SETS          0
#define configUSE_TASK_NOTIFICATIONS  1

#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H 1
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS   0
#define configQUEUE_REGISTRY_SIZE                 8

#define configSTACK_DEPTH_TYPE           uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE size_t

/* Backward compatibility and deprecated parameters */
#define configENABLE_BACKWARD_COMPATIBILITY 0
#define configUSE_ALTERNATIVE_API           0

/* Memory allocation parameters */
#define configTOTAL_HEAP_SIZE            ((size_t)4096)
#define configAPPLICATION_ALLOCATED_HEAP 0
#define configSUPPORT_STATIC_ALLOCATION  0
#define configSUPPORT_DYNAMIC_ALLOCATION 1

/* Hook function parameters */
#define configUSE_TICK_HOOK                0
#define configUSE_IDLE_HOOK                1
#define configUSE_MALLOC_FAILED_HOOK       1
#define configUSE_DAEMON_TASK_STARTUP_HOOK 1
#define configCHECK_FOR_STACK_OVERFLOW     1

/* Runtime and task stats gathering parameters */
#define configGENERATE_RUN_TIME_STATS        0
#define configUSE_TRACE_FACILITY             0
#define configUSE_STATS_FORMATTING_FUNCTIONS 0

/* Co-routine parameters */
#define configUSE_CO_ROUTINES           0
#define configMAX_CO_ROUTINE_PRIORITIES 1

/* Software timer parameters */
#define configUSE_TIMERS             1
#define configTIMER_TASK_PRIORITY    4
#define configTIMER_QUEUE_LENGTH     10
#define configTIMER_TASK_STACK_DEPTH configMINIMAL_STACK_SIZE

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_xEventGroupSetBitFromISR       0
#define INCLUDE_xQueueGetMutexHolder           0
#define INCLUDE_xTaskAbortDelay                0
#define INCLUDE_vTaskCleanUpResources          0
#define INCLUDE_vTaskDelay                     1
#define INCLUDE_vTaskDelayUntil                1
#define INCLUDE_vTaskDelete                    0
#define INCLUDE_xTaskGetCurrentTaskHandle      1
#define INCLUDE_xTaskGetHandle                 0
#define INCLUDE_xTaskGetIdleTaskHandle         1
#define INCLUDE_xTaskGetSchedulerState         1
#define INCLUDE_uxTaskGetStackHighWaterMark    1
#define INCLUDE_eTaskGetState                  1
#define INCLUDE_uxTaskPriorityGet              1
#define INCLUDE_vTaskPrioritySet               0
#define INCLUDE_xTaskResumeFromISR             0
#define INCLUDE_vTaskSuspend                   1
#define INCLUDE_xTimerPendFunctionCall         0
#define INCLUDE_xTimerGetTimerDaemonTaskHandle 1

/* FreeRTOS MPU specific definitions */
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS __NVIC_PRIO_BITS
#else
#define configPRIO_BITS 4
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 4

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT(x)           \
    if((x) == 0)                  \
    {                             \
        taskDISABLE_INTERRUPTS(); \
        for(;;)                   \
            ;                     \
    }

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* Runtime macros */
#if defined(configGENERATE_RUN_TIME_STATS) && (configGENERATE_RUN_TIME_STATS)
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
#include "stm32l4xx_hal.h"
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE() HAL_GetTick()
#endif /* assembler restriction */
#endif /* configGENERATE_RUN_TIME_STATS */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif /* FREERTOS_CONFIG_H */
