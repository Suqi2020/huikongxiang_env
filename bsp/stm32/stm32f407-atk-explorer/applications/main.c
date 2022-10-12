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
#define APP_VER       ((0<<8)+16)//0x0105 表示1.5版本
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
//V0.14   手动测试3条上行数据OK  20221010
//        串口2346 modbus 串口1 debug 串口5 串口屏
//V0.15		rs485_公众环流 modbus 初步调式读取数据成功，并能实现上传 20221011
//        rt_sprintf float类型有问题  使用sprintf代替 
//V0.16   READ WRITE modbus OK   20221012
static    rt_thread_t tid 	= RT_NULL;

//信号量的定义
extern  rt_sem_t  w5500Iqr_semp ;//w5500有数据时候中断来临
rt_mutex_t rs485_2Mutex = RT_NULL;
#define  MSGPOOL_LEN   1024 //485数据最大量  大于1k需要修改此处
//队列的定义
struct  rt_messagequeue rs485_2mque;
uint8_t rs485_1quePool[MSGPOOL_LEN];  //
//邮箱的定义
extern struct  rt_mailbox mbNetRecData;
extern struct  rt_mailbox mbNetSendData;
static char	 	 mbRecPool[20];//接收缓存20条
static char 	 mbSendPool[20];//发送缓存20条

//任务的定义
extern  void   netDataRecTask(void *para);//网络数据接收
extern  void   netDataSendTask(void *para);//网络数据发送
extern  void   upKeepStateTask(void *para);//定时打包数据 后期可能加入定时读取modbus
extern  void   w5500Task(void *parameter);//w5500网络状态的维护
extern  void   hardWareDriverTest(void);



int main(void)
{

    rt_kprintf("\n20221012  ver=%02d.%02d\n",(uint8_t)(APP_VER>>8),(uint8_t)APP_VER);
	  rt_err_t result;
//////////////////////////////////////信号量//////////////////////////////
	  w5500Iqr_semp = rt_sem_create("w5500Iqr_semp",0, RT_IPC_FLAG_FIFO);
		if (w5500Iqr_semp == RT_NULL)
    {
        rt_kprintf("create w5500Iqr_semp failed\n");
    }
    rs485_2Mutex = rt_mutex_create("rs485_1Mutex", RT_IPC_FLAG_FIFO);
    if (rs485_2Mutex == RT_NULL)
    {
        rt_kprintf("create rs485_1Mutex failed.\n");
        return -1;
    }
		
//////////////////////////////////消息队列//////////////////////////////////
		
		result = rt_mq_init(&rs485_2mque,"rs485_1mque",&rs485_1quePool[0],1,sizeof(rs485_1quePool),RT_IPC_FLAG_FIFO);       
		if (result != RT_EOK)
    {
        rt_kprintf("init rs485_1mque failed.\n");
        return -1;
    }
		 __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);//队列初始化之后再开启串口中断接收
////////////////////////////////////邮箱//////////////////////////////////
		

    result = rt_mb_init(&mbNetRecData,"mbRec",&mbRecPool[0],sizeof(mbRecPool)/4,RT_IPC_FLAG_FIFO);         
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox NetRecData failed.\n");
        return -1;
    }
    result = rt_mb_init(&mbNetSendData,"mbSend",&mbSendPool[0],sizeof(mbSendPool)/4,RT_IPC_FLAG_FIFO);         
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox NetSend failed.\n");
        return -1;
    }
////////////////////////////////任务////////////////////////////////////
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
		extern void uartIrqEnaAfterQueue();
		uartIrqEnaAfterQueue();//串口中断中用到了队列  开启中断需要放到后边
//////////////////////////////结束//////////////////////////////////////
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


