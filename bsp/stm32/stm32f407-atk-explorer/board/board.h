/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-5      SummerGift   first version
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <rtthread.h>
#include <stm32f4xx.h>
#include "drv_common.h"
#include "drv_gpio.h"


/* Private defines -----------------------------------------------------------*/
#define UART2_485_Pin GPIO_PIN_1
#define UART2_485_GPIO_Port GPIOA

#define LED_Pin GPIO_PIN_15
#define LED_GPIO_Port GPIOE

#define UART3_485_Pin GPIO_PIN_10
#define UART3_485_GPIO_Port GPIOD

#define UART6_485_Pin GPIO_PIN_8
#define UART6_485_GPIO_Port GPIOC

#define UART1_485_Pin GPIO_PIN_9
#define UART1_485_GPIO_Port GPIOC

#define UART4_485_Pin GPIO_PIN_11
#define UART4_485_GPIO_Port GPIOA


//UART1´®¿ÚdebugÓÃ
//#define RS485_CTRL1_PIN    GET_PIN(C, 9)
//#define RS485_1_SEND   rt_pin_write(RS485_CTRL1_PIN, PIN_HIGH);
//#define RS485_1_REC    rt_pin_write(RS485_CTRL1_PIN, PIN_LOW);

#define UART4_CTRL_PIN    GET_PIN(A, 11)
#define UART4_485_SEND   rt_pin_write(UART4_CTRL_PIN, PIN_HIGH);
#define UART4_485_REC    rt_pin_write(UART4_CTRL_PIN, PIN_LOW);

#define UART6_CTRL_PIN    GET_PIN(C, 8)
#define UART6_485_SEND   rt_pin_write(UART6_CTRL_PIN, PIN_HIGH);
#define UART6_485_REC    rt_pin_write(UART6_CTRL_PIN, PIN_LOW);

#define UART3_CTRL_PIN    GET_PIN(D, 10)
#define UART3_485_SEND   rt_pin_write(UART3_CTRL_PIN, PIN_HIGH);
#define UART3_485_REC    rt_pin_write(UART3_CTRL_PIN, PIN_LOW);

#define UART2_CTRL_PIN    GET_PIN(A, 1)
#define UART2_485_SEND   rt_pin_write(UART2_CTRL_PIN, PIN_HIGH);
#define UART2_485_REC    rt_pin_write(UART2_CTRL_PIN, PIN_LOW);

#ifdef __cplusplus
extern "C" {
#endif

#define STM32_SRAM_SIZE        (128)
#define STM32_SRAM_END         (0x20000000 + STM32_SRAM_SIZE * 1024)

#define STM32_FLASH_START_ADRESS     ((uint32_t)0x08000000)
#define STM32_FLASH_SIZE             (1024 * 1024)
#define STM32_FLASH_END_ADDRESS      ((uint32_t)(STM32_FLASH_START_ADRESS + STM32_FLASH_SIZE))

#if defined(__ARMCC_VERSION)
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN      ((void *)&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="CSTACK"
#define HEAP_BEGIN      (__segment_end("CSTACK"))
#else
extern int __bss_end;
#define HEAP_BEGIN      ((void *)&__bss_end)
#endif

#define HEAP_END        STM32_SRAM_END

void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif

