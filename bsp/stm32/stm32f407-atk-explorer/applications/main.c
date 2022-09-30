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
#define APP_VER     9//0x0105 ��ʾ1.5�汾
//task1  w5500����״̬��ά�����Լ���������               w5500Task
//task2  �������ݷ��͵�ά���������������� ע�� ���ݴ�� ��
//       �Լ��ط� ����task3                              upKeepStateTask
//task3  net����˵ķ���                                 netDataSednTask                      
//task4  ���������ݵĽ����봦��                          netDataRecTask



static    rt_thread_t tid 	= RT_NULL;
extern  rt_sem_t  w5500Iqr_semp ;//w5500������ʱ���ж�����


extern struct rt_mailbox mbNetRecData;

extern struct rt_mailbox mbNetSendData;

extern  void   netDataRecTask(void *para);
extern  void   upKeepStateTask(void *para);
extern  void  w5500Task(void *parameter);
extern  void  hardWareDriverTest(void);

static char mbRecPool[20];//���ջ���20��
static char mbSendPool[20];//���ͻ���20��

int main(void)
{

    rt_kprintf("\n20220927  ver=%02d.%02d\n",(uint8_t)(APP_VER>>8),(uint8_t)APP_VER);
	
	
	  w5500Iqr_semp = rt_sem_create("w5500Iqr_semp",0, RT_IPC_FLAG_FIFO);
		if (w5500Iqr_semp == RT_NULL)
    {
        rt_kprintf("create w5500Iqr_semp failed\n");
    }
		
		
		rt_err_t result;

    result = rt_mb_init(&mbNetRecData,
                        "mbRec",                      
                        &mbRecPool[0],                
                        sizeof(mbRecPool) / 4,        
                        RT_IPC_FLAG_FIFO);         
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox NetRecData failed.\n");
        return -1;
    }
		
    result = rt_mb_init(&mbNetSendData,
                        "mbSend",                      
                        &mbSendPool[0],                
                        sizeof(mbSendPool) / 4,        
                        RT_IPC_FLAG_FIFO);         
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox NetSend failed.\n");
        return -1;
    }
    tid =  rt_thread_create("w5500",w5500Task,RT_NULL,1024,2, 10 );
		if(tid!=NULL){
				rt_thread_startup(tid);													 
				rt_kprintf("RTcreat w5500Task task\r\n");
		}
    
//		tid =  rt_thread_create("netRec",netDataRecTask,RT_NULL,1024,2, 10 );
//		if(tid!=NULL){
//				rt_thread_startup(tid);													 
//				rt_kprintf("RTcreat netDataRecTask \r\n");
//		}
//		tid =  rt_thread_create("upKeep",upKeepStateTask,RT_NULL,1024,2, 10 );
//		if(tid!=NULL){
//				rt_thread_startup(tid);													 
//				rt_kprintf("RTcreat upKeepStateTask \r\n");
//		}
    while (1)//task���ڲ��� �Լ����Ʋ���
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


