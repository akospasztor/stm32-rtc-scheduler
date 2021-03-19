/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    core_stop.c
 * @brief   This file contains the implementation for entering and leaving low
 *          power modes.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "core_stop.h"
#include "hardware.h"
#include "rtc.h"

/* Private variables ---------------------------------------------------------*/
/** Variable to track whether the core has stopped and entered into STOP mode */
static volatile uint8_t isCoreStopped = 0U;

/**
 * @brief  Enter into STOP2 mode.
 *
 * This funtion suspends the SysTick, deinitializes all previously initialized
 * peripherals (except the RTC), resets the clock configuration and puts the MCU
 * into STOP2 mode. The RTC remains running in STOP2 mode.
 */
void EnterStop2Mode(void)
{
    /* Mask interrupts */
    __set_BASEPRI((TICK_INT_PRIORITY + 1) << (8 - __NVIC_PRIO_BITS));
    __DSB();
    __ISB();

    /* Set core stop flag */
    isCoreStopped = 1U;

    /* Suspend RTOS Systick */
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);

    /* Reset system clock to MSI */
    HAL_RCC_DeInit();

    /* Suspend HAL tick interrupt */
    HAL_SuspendTick();

    /* Set configured GPIOs to analog mode
     * Note: further reduction in current consumption can be reached by setting
     * all pins of the MCU (including the debugging pins) to analog mode. */
    GpioDeinit();

    /* Disable peripheral clocks */
    __HAL_RCC_FLASH_CLK_DISABLE();
    __HAL_RCC_PWR_CLK_DISABLE();
    __HAL_RCC_SYSCFG_CLK_DISABLE();

    RCC->AHB1SMENR  = 0U;
    RCC->AHB2SMENR  = 0U;
    RCC->AHB3SMENR  = 0U;
    RCC->APB1SMENR1 = 0U;
    RCC->APB1SMENR2 = 0U;
    RCC->APB2SMENR  = 0U;

    /* Ensure that MSI is the wake-up system clock */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_RCCEx_WakeUpStopCLKConfig(RCC_STOP_WAKEUPCLOCK_MSI);

    /* Re-enable interrupts */
    __set_BASEPRI(0);

    /* Enter Stop2 */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
}

/**
 * @brief  Resume from STOP2 mode.
 *
 * This funtion reinitializes the clock configuration, reconfigures the
 * peripherals and resumes the SysTick operation.
 */
void ResumeFromStop2Mode(void)
{
    if(isCoreStopped != 0U)
    {
        /* Mask interrupts */
        __set_BASEPRI((TICK_INT_PRIORITY + 1) << (8 - __NVIC_PRIO_BITS));
        __DSB();
        __ISB();

        /* Restore clock configuration */
        SystemClockConfig();

        /* Wait for RTC sync */
        RtcWaitForClockSynchronization();

        /* Resume HAL tick */
        /* Note: done by HAL_RCC_ClockConfig() in SystemClockConfig() */

        /* Restore GPIO and Power On required peripherals */
        GpioInit();

        /* Resume SysTick */
        SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);

        /* Reset core stop flag */
        isCoreStopped = 0U;

        /* Enable interrupts */
        __set_BASEPRI(0);
    }
    else
    {
        /* Do nothing when core is running */
    }
}
