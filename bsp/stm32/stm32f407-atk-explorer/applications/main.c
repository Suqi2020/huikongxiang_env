/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2018-11-19     flybreak     add stm32f407-atk-explorer bsp
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>
#define APP_VER     3  //0x0105 表示1.5版本
//初始化  没有加入版本管理 0V1 20220919
// 处了串口4外其他的串口收发都可以 0V3 20220920
/* defined the LED0 pin: PE15 */
#define LED0_PIN    GET_PIN(E, 15)
char str[]="hello\n";
extern UART_HandleTypeDef huart4;
int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_kprintf("\n20220920  ver=%02d.%02d\n",(uint8_t)(APP_VER>>8),(uint8_t)APP_VER);
	  int i=0;
    while (1)
    {
			  if(i++%3==0)
					rt_kprintf("test\n");
				UART4_485_SEND;
				HAL_UART_Transmit(&huart4,(uint8_t *)str,strlen(str),1000);
				UART4_485_REC;
				rt_pin_write(LED0_PIN, PIN_HIGH);
				rt_thread_mdelay(500);
				rt_pin_write(LED0_PIN, PIN_LOW);
				rt_thread_mdelay(500);
			  
    }
}
