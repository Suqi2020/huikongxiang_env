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
#define APP_VER     1  //0x0105 表示1.5版本
//初始化  没有加入版本管理 0V1 20220919
/* defined the LED0 pin: PE15 */
#define LED0_PIN    GET_PIN(E, 15)

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_kprintf("20220919  %d.%d\n",(uint8_t)(APP_VER>>8),(uint8_t)APP_VER);
    while (1)
    {
			  //rt_kprintf("test\n");
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
