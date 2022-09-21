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
#define APP_VER     4  //0x0105 ��ʾ1.5�汾
//��ʼ��  û�м���汾���� 0V1 20220919
// ���˴���4�������Ĵ����շ������� 0V3 20220920
// ����io��������� adc uart�շ��Ѿ�����  0V4 20220921
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
