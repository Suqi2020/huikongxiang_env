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
#define APP_VER     13//0x0105 表示1.5版本
//0V1   20220919
//初始化  没有加入版本管理 
//0V3   20220920
// 处了串口4外其他的串口收发都可以  
//0V4   20220921
// 所有io口输入输出 adc uart收发已经测试  
//0V5   20220921
//去掉menuconfig 配置IO驱动   
//1、spi时钟42MHZ 已经确认
//2、w5500内部缓存多大32K的接收缓存
//0V6  W5500参考野火网络通信成功  20220923
//0V7  w5500 中断接收数据OK  20220927
//0V9  shell命令行OK        20220927
//V0.11  增加4个主要task    20220930
	//task1  w5500网络状态的维护，以及接收数据               w5500Task
	//task2  上行数据发送的维护（包括上行心跳 注册 数据打包 ）
	//       以及重发 丢给task3                              upKeepStateTask
	//task3  net网络端的发送                                 netDataSednTask                      
	//task4  接收完数据的解析与处理                          netDataRecTask
//task1  w5500网络状态的维护，以及接收数据               w5500Task
//task2  上行数据发送的维护（包括上行心跳 注册 数据打包 ）
//       以及重发 丢给task3                              upKeepStateTask
//task3  net网络端的发送                                 netDataSednTask                      
//task4  接收完数据的解析与处理                          netDataRecTask
//#define RT_CONSOLEBUF_SIZE 2048  //后期需要该小 512
//V0.12  上行心跳json和回复OK    20221008
//V0.13   加入dataup 和devreg easy timer 20221009
 
static    rt_thread_t tid 	= RT_NULL;
extern  rt_sem_t  w5500Iqr_semp ;//w5500有数据时候中断来临


extern struct rt_mailbox mbNetRecData;

extern struct rt_mailbox mbNetSendData;

extern  void   netDataRecTask(void *para);
extern  void   netDataSendTask(void *para);
extern  void   upKeepStateTask(void *para);
extern  void  w5500Task(void *parameter);
extern  void  hardWareDriverTest(void);

static char mbRecPool[20];//接收缓存20条
static char mbSendPool[20];//发送缓存20条

int main(void)
{

    rt_kprintf("\n20221009  ver=%02d.%02d\n",(uint8_t)(APP_VER>>8),(uint8_t)APP_VER);
	
	
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
    
		tid =  rt_thread_create("netRec",netDataRecTask,RT_NULL,1024,2, 10 );
		if(tid!=NULL){
				rt_thread_startup(tid);													 
				rt_kprintf("RTcreat netDataRecTask \r\n");
		}
		tid =  rt_thread_create("netSend",netDataSendTask,RT_NULL,1024,2, 10 );
		if(tid!=NULL){
				rt_thread_startup(tid);													 
				rt_kprintf("RTcreat netDataSendTask \r\n");
		}
		tid =  rt_thread_create("upKeep",upKeepStateTask,RT_NULL,1024,2, 10 );
		if(tid!=NULL){
				rt_thread_startup(tid);													 
				rt_kprintf("RTcreat upKeepStateTask \r\n");
		}
		
    while (1)//task用于测试 以及闪灯操作
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


