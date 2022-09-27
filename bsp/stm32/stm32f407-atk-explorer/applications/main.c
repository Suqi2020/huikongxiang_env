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
#define APP_VER     9//0x0105 表示1.5版本


static    rt_thread_t tid 	= RT_NULL;
extern  rt_sem_t  w5500Iqr_semp ;//w5500有数据时候中断来临


extern struct rt_mailbox mbNetData;
extern  void   NetJsonTask(void *para);
extern  void   NetJsonTask2(void *para);
extern  void  w5500Task(void *parameter);
extern  void  hardWareDriverTest(void);

static char mb_pool[20];
int main(void)
{

    rt_kprintf("\n20220927  ver=%02d.%02d\n",(uint8_t)(APP_VER>>8),(uint8_t)APP_VER);
	
	
	  w5500Iqr_semp = rt_sem_create("w5500Iqr_semp",0, RT_IPC_FLAG_FIFO);
		if (w5500Iqr_semp == RT_NULL)
    {
        rt_kprintf("create w5500Iqr_semp failed\n");
    }
		
		
		rt_err_t result;

    result = rt_mb_init(&mbNetData,
                        "mbt",                      /* ???mbt */
                        &mb_pool[0],                /* ?????????mb_pool */
                        sizeof(mb_pool) / 4,        /* ????????,???????4?? */
                        RT_IPC_FLAG_FIFO);          /* ??FIFO???????? */
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox failed.\n");
        return -1;
    }
		
		
    tid =  rt_thread_create("w5500Task",w5500Task,RT_NULL,1024,2, 10 );
		if(tid!=NULL){
				rt_thread_startup(tid);													 
				rt_kprintf("RTcreat w5500Task task\r\n");
		}
    
		tid =  rt_thread_create("NetJsonTask",NetJsonTask,RT_NULL,1024,2, 10 );
		if(tid!=NULL){
				rt_thread_startup(tid);													 
				rt_kprintf("RTcreat NetJsonTask task\r\n");
		}
		
				tid =  rt_thread_create("NetJsonTask2",NetJsonTask2,RT_NULL,1024,2, 10 );
		if(tid!=NULL){
				rt_thread_startup(tid);													 
				rt_kprintf("RTcreat NetJsonTask2 task\r\n");
		}
    while (1)
    {
				hardWareDriverTest();
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
				rt_thread_mdelay(500);
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
				rt_thread_mdelay(500);
			  
    }
}



//void  rt_kprintf(char *str,const char *fmt, ...)
//{
//	rt_kprintf(const char *fmt, ...)
//}


