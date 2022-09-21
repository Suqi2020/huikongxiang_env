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
#define APP_VER     4  //0x0105 表示1.5版本
//初始化  没有加入版本管理 0V1 20220919
// 处了串口4外其他的串口收发都可以 0V3 20220920
// 所有io口输入输出 adc uart收发已经测试  0V4 20220921
extern void  hardWareDriverTest(void);


int main(void)
{

    rt_kprintf("\n20220920  ver=%02d.%02d\n",(uint8_t)(APP_VER>>8),(uint8_t)APP_VER);
	  int i=0;
    while (1)
    {
			  if(i++%5==0){
					rt_kprintf("rt_test\n");
				}
				hardWareDriverTest();
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
				rt_thread_mdelay(500);
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
				rt_thread_mdelay(500);
			  
    }
}
