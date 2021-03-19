/**
 *******************************************************************************
 * STM32 RTC Scheduler
 *******************************************************************************
 * @author  Akos Pasztor
 * @file    error_handler.c
 * @brief   This file contains the error handling implementation.
 * @see     Please refer to README for detailed information.
 *******************************************************************************
 * @copyright (c) 2021 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "error_handler.h"
#include <stdint.h>

#ifdef USE_FULL_ASSERT
#include <stdio.h>
#endif /* USE_FULL_ASSERT */

/**
 * @brief  This function is executed in case of an error.
 */
void ErrorHandler(void)
{
    for(;;)
    {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number where
 *         the assert_param error has occurred.
 *
 * @param  file  Pointer to the source file name.
 * @param  line  Line number of where the error occurred within the source file.
 */
void assert_failed(char* file, uint32_t line)
{
    printf("Assert: %s line %lu\r\n", file, line);
    for(;;)
    {
    }
}
#endif /* USE_FULL_ASSERT */
