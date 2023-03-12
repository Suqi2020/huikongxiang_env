/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <rtthread.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
//void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
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
#define SENSER_IN8_Pin GPIO_PIN_0
#define SENSER_IN8_GPIO_Port GPIOC
#define SENSER_IN7_Pin GPIO_PIN_1
#define SENSER_IN7_GPIO_Port GPIOC
#define SENSER_IN6_Pin GPIO_PIN_2
#define SENSER_IN6_GPIO_Port GPIOC
#define SENSER_IN5_Pin GPIO_PIN_3
#define SENSER_IN5_GPIO_Port GPIOC
#define UART2_485_Pin GPIO_PIN_1
#define UART2_485_GPIO_Port GPIOA
#define W5500_CS_Pin GPIO_PIN_4
#define W5500_CS_GPIO_Port GPIOA
#define SENSER_IN4_Pin GPIO_PIN_4
#define SENSER_IN4_GPIO_Port GPIOC
#define SENSER_IN3_Pin GPIO_PIN_5
#define SENSER_IN3_GPIO_Port GPIOC
#define SENSER_IN2_Pin GPIO_PIN_0
#define SENSER_IN2_GPIO_Port GPIOB
#define SENSER_IN1_Pin GPIO_PIN_1
#define SENSER_IN1_GPIO_Port GPIOB
#define W5500_RST_Pin GPIO_PIN_2
#define W5500_RST_GPIO_Port GPIOB
#define REALAY_CTRL2_Pin GPIO_PIN_11
#define REALAY_CTRL2_GPIO_Port GPIOE
#define REALAY_CTRL1_Pin GPIO_PIN_12
#define REALAY_CTRL1_GPIO_Port GPIOE
#define REALAY_CTRL3_Pin GPIO_PIN_13
#define REALAY_CTRL3_GPIO_Port GPIOE
#define REALAY_CTRL4_Pin GPIO_PIN_14
#define REALAY_CTRL4_GPIO_Port GPIOE
#define LED_Pin GPIO_PIN_15
#define LED_GPIO_Port GPIOE
#define W5500_IRQ_Pin GPIO_PIN_11
#define W5500_IRQ_GPIO_Port GPIOB
#define W5500_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define SPAKER1_Pin GPIO_PIN_12
#define SPAKER1_GPIO_Port GPIOB
#define SPAKER3_Pin GPIO_PIN_13
#define SPAKER3_GPIO_Port GPIOB
#define SPAKER4_Pin GPIO_PIN_14
#define SPAKER4_GPIO_Port GPIOB
#define SPAKER2_Pin GPIO_PIN_15
#define SPAKER2_GPIO_Port GPIOB
#define UART3_485_Pin GPIO_PIN_10
#define UART3_485_GPIO_Port GPIOD
#define UART6_485_Pin GPIO_PIN_8
#define UART6_485_GPIO_Port GPIOC
#define UART1_485_Pin GPIO_PIN_9
#define UART1_485_GPIO_Port GPIOC
#define UART4_485_Pin GPIO_PIN_11
#define UART4_485_GPIO_Port GPIOA
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
/* USER CODE BEGIN Private defines */
//UART1´®¿ÚdebugÓÃ
//#define RS485_CTRL1_PIN    GET_PIN(C, 9)
//#define RS485_1_SEND   HAL_GPIO_WritePin(UART1_485_GPIO_Port, UART1_485_Pin, GPIO_PIN_SET)
//#define RS485_1_REC    HAL_GPIO_WritePin(UART1_485_GPIO_Port, UART1_485_Pin, GPIO_PIN_RESET)


#define UART4_485_SEND   HAL_GPIO_WritePin(UART4_485_GPIO_Port, UART4_485_Pin, GPIO_PIN_SET)
#define UART4_485_REC    HAL_GPIO_WritePin(UART4_485_GPIO_Port, UART4_485_Pin, GPIO_PIN_RESET)

#define UART6_485_SEND   HAL_GPIO_WritePin(UART6_485_GPIO_Port, UART6_485_Pin, GPIO_PIN_SET)
#define UART6_485_REC    HAL_GPIO_WritePin(UART6_485_GPIO_Port, UART6_485_Pin, GPIO_PIN_RESET)


#define UART3_485_SEND   HAL_GPIO_WritePin(UART3_485_GPIO_Port, UART3_485_Pin, GPIO_PIN_SET)
#define UART3_485_REC    HAL_GPIO_WritePin(UART3_485_GPIO_Port, UART3_485_Pin, GPIO_PIN_RESET)


#define UART2_485_SEND   HAL_GPIO_WritePin(UART2_485_GPIO_Port, UART2_485_Pin, GPIO_PIN_SET)
#define UART2_485_REC    HAL_GPIO_WritePin(UART2_485_GPIO_Port, UART2_485_Pin, GPIO_PIN_RESET)


//#define UART4_CTRL_PIN    GET_PIN(C, 9)
//#define UART4_485_SEND   rt_pin_write(UART4_CTRL_PIN, PIN_HIGH);
//#define UART4_485_REC    rt_pin_write(UART4_CTRL_PIN, PIN_LOW);

//#define UART6_CTRL_PIN    GET_PIN(C, 8)
//#define UART6_485_SEND   rt_pin_write(UART6_CTRL_PIN, PIN_HIGH);
//#define UART6_485_REC    rt_pin_write(RS485_CTRL6_PIN, PIN_LOW);

//#define UART3_CTRL_PIN    GET_PIN(D, 10)
//#define UART3_485_SEND   rt_pin_write(UART3_CTRL_PIN, PIN_HIGH);
//#define UART3_485_REC    rt_pin_write(UART3_CTRL_PIN, PIN_LOW);

//#define UART2_CTRL_PIN    GET_PIN(A, 1)
//#define UART2_485_SEND   rt_pin_write(UART2_CTRL_PIN, PIN_HIGH);
//#define UART2_485_REC    rt_pin_write(UART2_CTRL_PIN, PIN_LOW);

#define RELAY1_ON				 HAL_GPIO_WritePin(REALAY_CTRL1_GPIO_Port, REALAY_CTRL1_Pin, GPIO_PIN_SET)
#define RELAY2_ON				 HAL_GPIO_WritePin(REALAY_CTRL2_GPIO_Port, REALAY_CTRL2_Pin, GPIO_PIN_SET)
#define RELAY3_ON				 HAL_GPIO_WritePin(REALAY_CTRL3_GPIO_Port, REALAY_CTRL3_Pin, GPIO_PIN_SET)
#define RELAY4_ON				 HAL_GPIO_WritePin(REALAY_CTRL4_GPIO_Port, REALAY_CTRL4_Pin, GPIO_PIN_SET)

#define RELAY1_OFF			 HAL_GPIO_WritePin(REALAY_CTRL1_GPIO_Port, REALAY_CTRL1_Pin, GPIO_PIN_RESET)
#define RELAY2_OFF			 HAL_GPIO_WritePin(REALAY_CTRL2_GPIO_Port, REALAY_CTRL2_Pin, GPIO_PIN_RESET)
#define RELAY3_OFF			 HAL_GPIO_WritePin(REALAY_CTRL3_GPIO_Port, REALAY_CTRL3_Pin, GPIO_PIN_RESET)
#define RELAY4_OFF			 HAL_GPIO_WritePin(REALAY_CTRL4_GPIO_Port, REALAY_CTRL4_Pin, GPIO_PIN_RESET)

#define RELAY1_READ			 HAL_GPIO_ReadPin(REALAY_CTRL1_GPIO_Port, REALAY_CTRL1_Pin)
#define RELAY2_READ			 HAL_GPIO_ReadPin(REALAY_CTRL2_GPIO_Port, REALAY_CTRL2_Pin)
#define RELAY3_READ			 HAL_GPIO_ReadPin(REALAY_CTRL3_GPIO_Port, REALAY_CTRL3_Pin)
#define RELAY4_READ			 HAL_GPIO_ReadPin(REALAY_CTRL4_GPIO_Port, REALAY_CTRL4_Pin)

#define SPAKER1_ON			 HAL_GPIO_WritePin(SPAKER1_GPIO_Port, SPAKER1_Pin, GPIO_PIN_SET)
#define SPAKER2_ON			 HAL_GPIO_WritePin(SPAKER2_GPIO_Port, SPAKER2_Pin, GPIO_PIN_SET)
#define SPAKER3_ON			 HAL_GPIO_WritePin(SPAKER3_GPIO_Port, SPAKER3_Pin, GPIO_PIN_SET)
#define SPAKER4_ON			 HAL_GPIO_WritePin(SPAKER4_GPIO_Port, SPAKER4_Pin, GPIO_PIN_SET)

#define SPAKER1_OFF			 HAL_GPIO_WritePin(SPAKER1_GPIO_Port, SPAKER1_Pin, GPIO_PIN_RESET)
#define SPAKER2_OFF			 HAL_GPIO_WritePin(SPAKER1_GPIO_Port, SPAKER2_Pin, GPIO_PIN_RESET)
#define SPAKER3_OFF			 HAL_GPIO_WritePin(SPAKER1_GPIO_Port, SPAKER3_Pin, GPIO_PIN_RESET)
#define SPAKER4_OFF			 HAL_GPIO_WritePin(SPAKER1_GPIO_Port, SPAKER4_Pin, GPIO_PIN_RESET)

#define SPAKER1_READ			 HAL_GPIO_ReadPin(SPAKER1_GPIO_Port, SPAKER1_Pin)
#define SPAKER2_READ			 HAL_GPIO_ReadPin(SPAKER1_GPIO_Port, SPAKER2_Pin)
#define SPAKER3_READ			 HAL_GPIO_ReadPin(SPAKER1_GPIO_Port, SPAKER3_Pin)
#define SPAKER4_READ			 HAL_GPIO_ReadPin(SPAKER1_GPIO_Port, SPAKER4_Pin)



#define IOOUT1_ON				 HAL_GPIO_WritePin(IO_OUT1_GPIO_Port, IO_OUT1_Pin, GPIO_PIN_SET)
#define IOOUT2_ON				 HAL_GPIO_WritePin(IO_OUT2_GPIO_Port, IO_OUT2_Pin, GPIO_PIN_SET)
#define IOOUT3_ON				 HAL_GPIO_WritePin(IO_OUT3_GPIO_Port, IO_OUT3_Pin, GPIO_PIN_SET)
#define IOOUT4_ON				 HAL_GPIO_WritePin(IO_OUT4_GPIO_Port, IO_OUT4_Pin, GPIO_PIN_SET)
#define IOOUT5_ON			   HAL_GPIO_WritePin(IO_OUT5_GPIO_Port, IO_OUT5_Pin, GPIO_PIN_SET)
#define IOOUT6_ON			   HAL_GPIO_WritePin(IO_OUT6_GPIO_Port, IO_OUT6_Pin, GPIO_PIN_SET)
#define IOOUT7_ON			   HAL_GPIO_WritePin(IO_OUT7_GPIO_Port, IO_OUT7_Pin, GPIO_PIN_SET)
#define IOOUT8_ON			   HAL_GPIO_WritePin(IO_OUT8_GPIO_Port, IO_OUT8_Pin, GPIO_PIN_SET)



#define IOOUT1_OFF			 HAL_GPIO_WritePin(IO_OUT1_GPIO_Port, IO_OUT1_Pin, GPIO_PIN_RESET)
#define IOOUT2_OFF			 HAL_GPIO_WritePin(IO_OUT2_GPIO_Port, IO_OUT2_Pin, GPIO_PIN_RESET)
#define IOOUT3_OFF			 HAL_GPIO_WritePin(IO_OUT3_GPIO_Port, IO_OUT3_Pin, GPIO_PIN_RESET)
#define IOOUT4_OFF			 HAL_GPIO_WritePin(IO_OUT4_GPIO_Port, IO_OUT4_Pin, GPIO_PIN_RESET)
#define IOOUT5_OFF		   HAL_GPIO_WritePin(IO_OUT5_GPIO_Port, IO_OUT5_Pin, GPIO_PIN_RESET)
#define IOOUT6_OFF		   HAL_GPIO_WritePin(IO_OUT6_GPIO_Port, IO_OUT6_Pin, GPIO_PIN_RESET)
#define IOOUT7_OFF		   HAL_GPIO_WritePin(IO_OUT7_GPIO_Port, IO_OUT7_Pin, GPIO_PIN_RESET)
#define IOOUT8_OFF		   HAL_GPIO_WritePin(IO_OUT8_GPIO_Port, IO_OUT8_Pin, GPIO_PIN_RESET)




#define IOOUT1_READ			 HAL_GPIO_ReadPin(IO_OUT1_GPIO_Port, IO_OUT1_Pin)
#define IOOUT2_READ			 HAL_GPIO_ReadPin(IO_OUT2_GPIO_Port, IO_OUT2_Pin)
#define IOOUT3_READ			 HAL_GPIO_ReadPin(IO_OUT3_GPIO_Port, IO_OUT3_Pin)
#define IOOUT4_READ			 HAL_GPIO_ReadPin(IO_OUT4_GPIO_Port, IO_OUT4_Pin)
#define IOOUT5_READ		   HAL_GPIO_ReadPin(IO_OUT5_GPIO_Port, IO_OUT5_Pin)
#define IOOUT6_READ		   HAL_GPIO_ReadPin(IO_OUT6_GPIO_Port, IO_OUT6_Pin)
#define IOOUT7_READ		   HAL_GPIO_ReadPin(IO_OUT7_GPIO_Port, IO_OUT7_Pin)
#define IOOUT8_READ		   HAL_GPIO_ReadPin(IO_OUT8_GPIO_Port, IO_OUT8_Pin)

#define IOIN1_READ       HAL_GPIO_ReadPin(IO_IN1_GPIO_Port, IO_IN1_Pin)
#define IOIN2_READ       HAL_GPIO_ReadPin(IO_IN2_GPIO_Port, IO_IN2_Pin)
#define IOIN3_READ       HAL_GPIO_ReadPin(IO_IN3_GPIO_Port, IO_IN3_Pin)
#define IOIN4_READ       HAL_GPIO_ReadPin(IO_IN4_GPIO_Port, IO_IN4_Pin)
#define IOIN5_READ       HAL_GPIO_ReadPin(IO_IN5_GPIO_Port, IO_IN5_Pin)
#define IOIN6_READ       HAL_GPIO_ReadPin(IO_IN6_GPIO_Port, IO_IN6_Pin)
#define IOIN7_READ       HAL_GPIO_ReadPin(IO_IN7_GPIO_Port, IO_IN7_Pin)
#define IOIN8_READ       HAL_GPIO_ReadPin(IO_IN8_GPIO_Port, IO_IN8_Pin)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
