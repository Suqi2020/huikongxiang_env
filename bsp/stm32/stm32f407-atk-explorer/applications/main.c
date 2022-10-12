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
#define APP_VER       ((0<<8)+16)//0x0105 ��ʾ1.5�汾
//0V1   20220919
//��ʼ��  û�м���汾���� 
//0V3   20220920
// ���˴���4�������Ĵ����շ�������  
//0V4   20220921
// ����io��������� adc uart�շ��Ѿ�����  
//0V5   20220921
//ȥ��menuconfig ����IO����   
//1��spiʱ��42MHZ �Ѿ�ȷ��
//2��w5500�ڲ�������32K�Ľ��ջ���
//0V6  W5500�ο�Ұ������ͨ�ųɹ�  20220923
//0V7  w5500 �жϽ�������OK  20220927
//0V9  shell������OK        20220927
//V0.11  ����4����Ҫtask    20220930
	//task1  w5500����״̬��ά�����Լ���������               w5500Task
	//task2  �������ݷ��͵�ά���������������� ע�� ���ݴ�� ��
	//       �Լ��ط� ����task3                              upKeepStateTask
	//task3  net����˵ķ���                                 netDataSednTask                      
	//task4  ���������ݵĽ����봦��                          netDataRecTask
//task1  w5500����״̬��ά�����Լ���������               w5500Task
//task2  �������ݷ��͵�ά���������������� ע�� ���ݴ�� ��
//       �Լ��ط� ����task3                              upKeepStateTask
//task3  net����˵ķ���                                 netDataSednTask                      
//task4  ���������ݵĽ����봦��                          netDataRecTask
//#define RT_CONSOLEBUF_SIZE 2048  //������Ҫ��С 512
//V0.12  ��������json�ͻظ�OK    20221008
//V0.13   ����dataup ��devreg easy timer 20221009
//V0.14   �ֶ�����3����������OK  20221010
//        ����2346 modbus ����1 debug ����5 ������
//V0.15		rs485_���ڻ��� modbus ������ʽ��ȡ���ݳɹ�������ʵ���ϴ� 20221011
//        rt_sprintf float����������  ʹ��sprintf���� 
//V0.16   READ WRITE modbus OK   20221012
static    rt_thread_t tid 	= RT_NULL;

//�ź����Ķ���
extern  rt_sem_t  w5500Iqr_semp ;//w5500������ʱ���ж�����
rt_mutex_t rs485_2Mutex = RT_NULL;
#define  MSGPOOL_LEN   1024 //485���������  ����1k��Ҫ�޸Ĵ˴�
//���еĶ���
struct  rt_messagequeue rs485_2mque;
uint8_t rs485_1quePool[MSGPOOL_LEN];  //
//����Ķ���
extern struct  rt_mailbox mbNetRecData;
extern struct  rt_mailbox mbNetSendData;
static char	 	 mbRecPool[20];//���ջ���20��
static char 	 mbSendPool[20];//���ͻ���20��

//����Ķ���
extern  void   netDataRecTask(void *para);//�������ݽ���
extern  void   netDataSendTask(void *para);//�������ݷ���
extern  void   upKeepStateTask(void *para);//��ʱ������� ���ڿ��ܼ��붨ʱ��ȡmodbus
extern  void   w5500Task(void *parameter);//w5500����״̬��ά��
extern  void   hardWareDriverTest(void);



int main(void)
{

    rt_kprintf("\n20221012  ver=%02d.%02d\n",(uint8_t)(APP_VER>>8),(uint8_t)APP_VER);
	  rt_err_t result;
//////////////////////////////////////�ź���//////////////////////////////
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
		
//////////////////////////////////��Ϣ����//////////////////////////////////
		
		result = rt_mq_init(&rs485_2mque,"rs485_1mque",&rs485_1quePool[0],1,sizeof(rs485_1quePool),RT_IPC_FLAG_FIFO);       
		if (result != RT_EOK)
    {
        rt_kprintf("init rs485_1mque failed.\n");
        return -1;
    }
		 __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);//���г�ʼ��֮���ٿ��������жϽ���
////////////////////////////////////����//////////////////////////////////
		

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
////////////////////////////////����////////////////////////////////////
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
		uartIrqEnaAfterQueue();//�����ж����õ��˶���  �����ж���Ҫ�ŵ����
//////////////////////////////����//////////////////////////////////////
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


