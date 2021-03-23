/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    stm32l4xx_it.c
 * @brief   This file contains the implementations of interrupt handlers.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "core_stop.h"
#include "error_handler.h"
#include "scheduler.h"

/* External variables --------------------------------------------------------*/
extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim17;

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief  This function handles the non-maskable interrupts.
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles the hard-fault interrupts.
 */
void HardFault_Handler(void)
{
    ErrorHandler();
}

/**
 * @brief  This function handles the memory management faults.
 */
void MemManage_Handler(void)
{
    ErrorHandler();
}

/**
 * @brief  This function handles the prefetch faults and memory access faults.
 */
void BusFault_Handler(void)
{
    ErrorHandler();
}

/**
 * @brief  This function handles the faults of undefined instructions or
 *         illegal state.
 */
void UsageFault_Handler(void)
{
    ErrorHandler();
}

/**
 * @brief This function handles the debug monitor.
 */
void DebugMon_Handler(void)
{
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
 * @brief  This function handles the TIM17 global interrupts.
 */
void TIM1_TRG_COM_TIM17_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim17);
    HAL_IncTick();
}

/**
 * @brief  This function handles the RTC alarm interrupts.
 */
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&hrtc);

    /* Resume operation from STOP2 mode */
    ResumeFromStop2Mode();

    /* Process the scheduler */
    SchedulerProcess();

    /* Execute the pending jobs */
    SchedulerExecutePendingJobs();
}
