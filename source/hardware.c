/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    hardware.c
 * @brief   This file contains the hardware-specific function implementations.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "hardware.h"
#include "error_handler.h"

/**
 * @brief  This function configures the system and peripheral clocks.
 *
 * The function switches the clock source to HSI16, enables and configures the
 * PLL, configures the peripheral clock sources, enables the RTC clock and sets
 * its source to LSI and sets the core clock to 80 MHz.
 */
void SystemClockConfig(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct   = {0U};
    RCC_ClkInitTypeDef RCC_ClkInitStruct   = {0U};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0U};

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState            = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = 1U;
    RCC_OscInitStruct.PLL.PLLN            = 10U;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        ErrorHandler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        ErrorHandler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection    = RCC_RTCCLKSOURCE_LSI;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        ErrorHandler();
    }

    if(HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        ErrorHandler();
    }
}

/**
 * @brief  This function initializes the GPIOs of the LEDs.
 */
void GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0U};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    LedLd2Off();
    LedLd3Off();

    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin = LED_LD2_Pin;
    HAL_GPIO_Init(LED_LD2_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED_LD3_Pin;
    HAL_GPIO_Init(LED_LD3_Port, &GPIO_InitStruct);
}

/**
 * @brief  This function deinitializes the GPIOs and sets them to analog mode.
 */
void GpioDeinit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0U};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    LedLd2Off();
    LedLd3Off();

    GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin = LED_LD2_Pin;
    HAL_GPIO_Init(LED_LD2_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED_LD3_Pin;
    HAL_GPIO_Init(LED_LD3_Port, &GPIO_InitStruct);

    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
}

/**
 * @brief  Turn on the LD2 LED.
 */
void LedLd2On(void)
{
    HAL_GPIO_WritePin(LED_LD2_Port, LED_LD2_Pin, GPIO_PIN_RESET);
}

/**
 * @brief  Turn off the LD2 LED.
 */
void LedLd2Off(void)
{
    HAL_GPIO_WritePin(LED_LD2_Port, LED_LD2_Pin, GPIO_PIN_SET);
}

/**
 * @brief  Toggle the status of the LD2 LED.
 */
void LedLd2Toggle(void)
{
    HAL_GPIO_TogglePin(LED_LD2_Port, LED_LD2_Pin);
}

/**
 * @brief  Turn on the LD3 LED.
 */
void LedLd3On(void)
{
    HAL_GPIO_WritePin(LED_LD3_Port, LED_LD3_Pin, GPIO_PIN_SET);
}

/**
 * @brief  Turn off the LD3 LED.
 */
void LedLd3Off(void)
{
    HAL_GPIO_WritePin(LED_LD3_Port, LED_LD3_Pin, GPIO_PIN_RESET);
}

/**
 * @brief  Toggle the status of the LD3 LED.
 */
void LedLd3Toggle(void)
{
    HAL_GPIO_TogglePin(LED_LD3_Port, LED_LD3_Pin);
}
