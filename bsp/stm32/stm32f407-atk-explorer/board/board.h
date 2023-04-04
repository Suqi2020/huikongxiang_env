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

//#define   rt_log(...)  rt_kprintf(const char *fmt, ...)

#define 	ANALOG_NUM   	        8
#define   UART_NUM        4//一共4路串口
#define   ACUID_LEN       18
#define 	DEVID_LEN       20
#define   MODEL_LEN       8
#define   NAME_LEN        20
#define   ANA_SUBNAME_LEN  2
#define   DI_NUM          8
#define   DO_NUM          8
#define   V33O_NUM        2
#define   V5O_NUM         2
#define   V12O_NUM        4
#define  	MODBID_LEN  		20
#define  	INNAME_NUM    	3
#define  	OUTNAME_NUM   	4
#define   USE_4GAS

#define   ANA_MASK       //屏蔽ana


//#incude  <iconv.h>
#include <rtthread.h>
#include "stdbool.h"
#include "stdlib.h"
#include <stm32f4xx.h>
#include "drv_common.h"
#include "drv_gpio.h"
#include "drv_flash.h"
#include "test_hardware.h"
#include "main.h"
//#include "W5500p.h"	
#include "W5500Task.h"
#include "w5500_conf.h"
#include "w5500.h"
#include "socket.h"
#include "tcp_demo.h"
#include "dhcp.h"
#include "loopback.h"
#include "NetJsonDataTask.h"
#include "dataPack.h"
#include "dataPhrase.h"
#include "crc.h"
#include "modbus.h"
#include "utility.h"
#include "uartReconf.h"
#include "stmflash.h"
#include "7inchRegist.h"
#include "ctrlConfig.h"
//#include "rs485ThreeAxis.h"
//#include "rs485Circula.h"
//#include "rs485PartDischag.h"
//#include "rs485PressSettl.h"
//#include "rs485Gas.h"
//#include "rs485waterDepth.h"
//#include "rs485TempHum.h"
#include "cJSON.h"
#include "analogConfig.h"
#include "modbusConfig.h"
//#include "utf_8.h"

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;

extern  void MX_UART4_Init(int bps);
extern  void MX_USART2_UART_Init(int bps);
extern  void MX_USART3_UART_Init(int bps);
extern  void MX_USART6_UART_Init(int bps);
extern struct rt_mailbox mbNetSendData;
//extern rt_mutex_t cirCurrMutex ;
//队列的定义
//extern struct  rt_messagequeue cirCurrmque;

//#define  kt_printfP(const char *str,const char *fmt, ...)  rt_kprintf(str);rt_kprintf(fmt, ...)

/* Private defines -----------------------------------------------------------*/
/*
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

#define SENSER_IN8_Pin GPIO_PIN_0
#define SENSER_IN8_GPIO_Port GPIOC
#define SENSER_IN7_Pin GPIO_PIN_1
#define SENSER_IN7_GPIO_Port GPIOC
#define SENSER_IN6_Pin GPIO_PIN_2
#define SENSER_IN6_GPIO_Port GPIOC
#define SENSER_IN5_Pin GPIO_PIN_3
#define SENSER_IN5_GPIO_Port GPIOC

#define SENSER_IN4_Pin GPIO_PIN_4
#define SENSER_IN4_GPIO_Port GPIOC
#define SENSER_IN3_Pin GPIO_PIN_5
#define SENSER_IN3_GPIO_Port GPIOC
#define SENSER_IN2_Pin GPIO_PIN_0
#define SENSER_IN2_GPIO_Port GPIOB
#define SENSER_IN1_Pin GPIO_PIN_1
#define SENSER_IN1_GPIO_Port GPIOB


#define IO_OUT8_Pin GPIO_PIN_12
#define IO_OUT8_GPIO_Port GPIOA
#define IO_OUT7_Pin GPIO_PIN_15
#define IO_OUT7_GPIO_Port GPIOA
#define IO_OUT6_Pin GPIO_PIN_3
#define IO_OUT6_GPIO_Port GPIOB
#define IO_OUT5_Pin GPIO_PIN_4
#define IO_OUT5_GPIO_Port GPIOB
#define IO_OUT4_Pin GPIO_PIN_5
#define IO_OUT4_GPIO_Port GPIOB
#define IO_OUT3_Pin GPIO_PIN_6
#define IO_OUT3_GPIO_Port GPIOB
#define IO_OUT2_Pin GPIO_PIN_7
#define IO_OUT2_GPIO_Port GPIOB
#define IO_OUT1_Pin GPIO_PIN_8
#define IO_OUT1_GPIO_Port GPIOB


#define IO_IN8_Pin GPIO_PIN_9
#define IO_IN8_GPIO_Port GPIOB
#define IO_IN7_Pin GPIO_PIN_0
#define IO_IN7_GPIO_Port GPIOE
#define IO_IN6_Pin GPIO_PIN_1
#define IO_IN6_GPIO_Port GPIOE

#define IO_IN5_Pin GPIO_PIN_2
#define IO_IN5_GPIO_Port GPIOE
#define IO_IN4_Pin GPIO_PIN_3
#define IO_IN4_GPIO_Port GPIOE
#define IO_IN3_Pin GPIO_PIN_4
#define IO_IN3_GPIO_Port GPIOE
#define IO_IN2_Pin GPIO_PIN_5
#define IO_IN2_GPIO_Port GPIOE
#define IO_IN1_Pin GPIO_PIN_6
#define IO_IN1_GPIO_Port GPIOE


//增加rt配置IO使用
#define LED0_PIN    GET_PIN(E, 15)
*/
//#ddfine  rt_kprintf_deb rt_kprintf(const char *fmt, ...)

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

