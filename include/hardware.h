/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    hardware.h
 * @brief   This file contains the hardware-specific definitions and function
 *          prototypes.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

#ifndef HARDWARE_H
#define HARDWARE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Defines -------------------------------------------------------------------*/
/* LED: LD2 */
#define LED_LD2_Port GPIOB       /*!< Port of LED LD2 */
#define LED_LD2_Pin  GPIO_PIN_13 /*!< Pin of LED LD2 */

/* LED: LD3 */
#define LED_LD3_Port GPIOA      /*!< Port of LED LD3 */
#define LED_LD3_Pin  GPIO_PIN_5 /*!< Pin of LED LD3 */

/* Functions -----------------------------------------------------------------*/
void SystemClockConfig(void);
void GpioInit(void);
void GpioDeinit(void);

void LedLd2On(void);
void LedLd2Off(void);
void LedLd2Toggle(void);
void LedLd3On(void);
void LedLd3Off(void);
void LedLd3Toggle(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_H */
