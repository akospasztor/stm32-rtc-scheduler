/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    stm32l4xx_hal_timebase.c
 * @brief   This file contains the STM32 HAL timebase implementation.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_tim.h"

/* Private variables ---------------------------------------------------------*/
/** TIM17 peripheral handle */
TIM_HandleTypeDef htim17;

/**
 * @brief  This function configures the TIM17 as the HAL time base source.
 *
 * The timer is configured to provide a 1 ms time base.
 *
 * @note  This function is called automatically by HAL_Init() at the beginning
 *        of the application after reset, or at any time when clock is
 *        configured by HAL_RCC_rccClockConfig().
 * @param  TickPriority  The tick interrupt priority.
 * @retval HAL_OK upon success.
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    HAL_StatusTypeDef result;
    RCC_ClkInitTypeDef rccClockConfig = {0U};
    uint32_t clockFrequency           = 0U;
    uint32_t clockPrescaler           = 0U;
    uint32_t flashLatency;

    /* Set the global tick interrupt priority variable */
    uwTickPrio = TickPriority;

    /* Configure the timer IRQ priority */
    HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, TickPriority, 0U);

    /* Enable the timer global Interrupt */
    HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);

    /* Enable timer clock */
    __HAL_RCC_TIM17_CLK_ENABLE();

    /* Compute the prescaler value to have the timer clock equal to 1MHz */
    HAL_RCC_GetClockConfig(&rccClockConfig, &flashLatency);
    clockFrequency = HAL_RCC_GetPCLK2Freq();
    clockPrescaler = (uint32_t)((clockFrequency / 1000000U) - 1U);

    /* Initialize timer */
    htim17.Instance           = TIM17;
    htim17.Init.CounterMode   = TIM_COUNTERMODE_UP;
    htim17.Init.ClockDivision = 0U;
    htim17.Init.Period        = (1000000U / 1000U) - 1U;
    htim17.Init.Prescaler     = clockPrescaler;

    result = HAL_TIM_Base_Init(&htim17);
    if(result == HAL_OK)
    {
        /* Start the TIM time Base generation in interrupt mode */
        result = HAL_TIM_Base_Start_IT(&htim17);
    }

    return result;
}

/**
 * @brief  Suspend the tick increment.
 *
 * This function disables the tick increment by disabling the timer update
 * interrupt.
 */
void HAL_SuspendTick(void)
{
    __HAL_TIM_DISABLE_IT(&htim17, TIM_IT_UPDATE);
}

/**
 * @brief  Resume the tick increment.
 *
 * This function resumes the tick increment by enabling the timer update
 * interrupt.
 */
void HAL_ResumeTick(void)
{
    __HAL_TIM_ENABLE_IT(&htim17, TIM_IT_UPDATE);
}
