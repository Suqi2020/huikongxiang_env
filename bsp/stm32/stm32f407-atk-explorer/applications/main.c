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
//			task1  w5500����״̬��ά�����Լ���������               w5500Task
//			task2  �������ݷ��͵�ά���������������� ע�� ���ݴ�� ��
//       		�Լ��ط� ����task3                              upKeepStateTask
//			task3  net����˵ķ���                                 netDataSednTask                      
//			task4  ���������ݵĽ����봦��                          netDataRecTask
//			task1  w5500����״̬��ά�����Լ���������               w5500Task
//			task2  �������ݷ��͵�ά���������������� ע�� ���ݴ�� ��
//       				�Լ��ط� ����task3                              upKeepStateTask
//			task3  net����˵ķ���                                 netDataSednTask                      
//			task4  ���������ݵĽ����봦��                          netDataRecTask
//			#define RT_CONSOLEBUF_SIZE 2048  //������Ҫ��С 512
//V0.12  ��������json�ͻظ�OK    																	20221008
//V0.13   ����dataup ��devreg easy timer													 20221009
//V0.14   �ֶ�����3����������OK  																		20221010
//        ����2346 modbus ����1 debug ����5 ������


//V0.15		rs485_���ڻ��� modbus ������ʽ��ȡ���ݳɹ�������ʵ���ϴ� 20221011
//        rt_sprintf float����������  ʹ��sprintf���� 
//V0.16   READ WRITE modbus OK   																			20221012
//V0.17   //Ѹ���л�����485�ӿ���ʹ�� ������ֻ��Ҫ�޸Ĵ��ڷ��ͽӿ� ���жϽ��սӿڼ���
//         rs485Circula.c-cirCurrUartSend(uint8_t *buf,int len) 
//         ��drv_uart.c-USART2_IRQHandler�� -rt_mq_send(&cirCurrmque, &Res, 1);    20221012
//V0.18    modbus�������������0       2022012
//V0.19    �ַŶ�ȡ����  ����modbus��ȡ����  ÿ�η��ͺ��ٶ�ȡ             
//         ���������ʱ�� ���Ծ߱���ʼ�� ��ʼ ֹͣ���� ��Ҫ1��Ļ�׼ 
//         ����modbus�豸��������ֵĸ��Ź��ˣ���modbus��ȡ������ ������
//         �߱���ʱ��ͬʱ��������´����͹���                          20221013
//V0.20    ����16K�������  MAX_SOCK_NUM Ϊ1ʱ�� �����16k��������  
//         TX_RX_MAX_BUF_SIZEΪʵ�ʷ��ͻ���buf��С(��MAX_SOCK_NUM����)                       20221017
//V0.21    ����ͳ�ƴ��룬���Լ�����ߴ����͵���ʱ�� ���� offline                    20221018
//V0.22    ���ӳ����Ǵ��� Ĭ�ϲ�����9600 �ĵ�����      20221018
//V0.23    ����������� Ĭ�ϲ�����9600 ��Ҫ�޸�����ĵ�ַΪ01  ͬ������  20221019
//V0.24 	 ����֧��modbus�豸ѡ�񴮿� UART2(1) UART3(2) UART6(3) UART1(debug) UART4(4)  δ����  20221020
//V0.25    ����4·485���������log��ӡ���                                     20221021
//V0.26    ����ip_from==IP_FROM_DHCP �����Զ�����ʵ������ip���� ���������Զ���ȡipʧ�ܣ� 20221025
//V0.27    1������utf8��ʽ���json��ʽ  nodepad++����Ϊutf8 �Լ� c++������ --no-multibyte-chars
//            ����json��ʽ�����ַ�Ҫ��dataPack.c������ �Ѿ����ļ���nodepad++����Ϊutf8��ʽ
//         2���޸����������������� ������json����û���ͷŵ�                      20221026
//         3������json��ʽ���devRegJsonPack  ��json�Դ�����sprintf �������ݰ� 465Byte���ӵ�586Byte ����ʹ��
//V0.28    �������ö��modbus��ͬһ��������  ��Ҫͬ���͵ķŵ�һ��  ���� �ַźͻ���  ����ͳ�����   20221027
//V0.29    �������ö���ļ��е�keil��      20221028
//V0.30    ����flash�洢  ���4K           20221028
//V0.31    ����atָ��������modbus  example[modbus �ӵػ��� uart5 12]
//				 mobus+�豸����+�˿�+�豸��ַ(0-�ر��豸)+�ɼ�ʱ��(��)           20221029
//V0.32    ����mdbus����ʱ�� ��鲻ͬ�����豸��ͬһ���˿�
//         ���ͬ�����豸��ͬһ���豸��ַ��ͬһ���˿�
//         ʵ��ip��ַ�����úʹ洢��flash��            20221031
//V0.33//4��������һ���ȡ ����ǰ������ʹ�� ֻ��CO��ʱ���ط���ȡ���������  �ر�ʱ��ֻ��Ҫ�ر�CO�Ϳ��԰���������ȫ���ر�
//         COĬ��������4  Ϊ�˷��������������ݴ����һ��    20221101
//V0.34    ������ʪ�Ⱥ�ˮλ��ȡ ÿ��������ֻ֧��һ��       20221102  
//V0.35    ����flash��д ����drv_falsh_f4.c�⺯��          20221103
//V0.36    ����modbus��ȡΪͨ��ɵ��ʽ��ȡ mcu������������ֺ���  20221103
//V0.37    ��������� devid�ظ� ������ͬһ�������±ߵ�modbus�豸��ͬ�����豸��ַ  
//         �ϵ��ӡmodbus���ù����豸      
//          ����128kflash�ռ����洢����modbus�豸(������С��λ��128K) Ŀǰ���֧��200��       20221104
//V0.38    �ַ�GY-JF100-C01�ͱ���豸������ͬһ�����ڵ������������ȡ�豸�ᵼ�¾ַŶ�ȡʧ�� 
//         ��������ȡ�豸�������Ҫ��ʱ2�������ȡ                   20221105
//V0.39    �޸Ĵ洢��������ͳһΪ flash save                      
//         ����ģ������                                            20221107
//         ����bug  uart ��MX_USART2_UART_Init(packFLash.port[i].bps); ��
//V0.40    �޸�ɵ��ʽΪָ��������       20221108
//V0.41    ���õ�������json��ʽ�Ѿ�������   20221109
//V0.42    ���봫�������� �޸�bug    ����ģ��������Ŀǰ֧����ʪ��δ��� 20221110
//V0.43    ���봫������Ӧ״̬�ϴ�  1-��������Ӧ  0-����������Ӧ  20221111
//V0.44    �޸�log��ʾ���� �Լ���ʾ������      20221117
//V0.45    ����7�紥����ʾ�����ַ����         20221201
//V0.46    ����7�紥�����ֽӿں��� ���Ӵ���5���� ����lcdtask ����       20221205
#define APP_VER       ((0<<8)+46)//0x0105 ��ʾ1.5�汾
const char date[]="20221205";

//static    rt_thread_t tid 	= RT_NULL;
static    rt_thread_t tidW5500 	  = RT_NULL;
static    rt_thread_t tidNetRec 	= RT_NULL;
static    rt_thread_t tidNetSend 	= RT_NULL;
static    rt_thread_t tidUpkeep 	= RT_NULL;
static    rt_thread_t tidLCD      = RT_NULL;
//�ź����Ķ���
extern  rt_sem_t  w5500Iqr_semp ;//w5500������ʱ���ж�����


//����Ķ���
extern struct  rt_mailbox mbNetRecData;
extern struct  rt_mailbox mbNetSendData;
static char	 	 mbRecPool[20];//���ջ���20��
static char 	 mbSendPool[20];//���ͻ���20��


//���еĶ���
struct  rt_messagequeue LCDmque;//= {RT_NULL} ;//����LCD����
uint8_t LCDQuePool[LCD_BUF_LEN];  //����lcd���г�
//����Ķ���
extern  void   netDataRecTask(void *para);//�������ݽ���
extern  void   netDataSendTask(void *para);//�������ݷ���
extern  void   upKeepStateTask(void *para);//��ʱ������� ���ڿ��ܼ��붨ʱ��ȡmodbus
extern  void   w5500Task(void *parameter);//w5500����״̬��ά��
extern  void   hardWareDriverTest(void);
extern  void   LCDTask(void *parameter);

const static char sign[]="[main]";
//const char errStr[]="[ERR]";


/* ��ʱ���Ŀ��ƿ� */
static rt_timer_t timer1;

//static int cnt = 0;
/* ��ʱ��1��ʱ���� */
//10������һ�� uart offline״̬
static void timeout1(void *parameter)
{
		static int count=0;
	  static int alarmTick=10;
		extern rt_bool_t gbNetState;
	  extern void modbusWorkErrCheck(void);
	  count++;
	  
		if(gbNetState==RT_TRUE){
				if(count%10==0){
						HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
				}
		}
		else
				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		if(count%alarmTick==0){
			  alarmTick+=20;
			  if(alarmTick>=100){
						alarmTick=100;// 1 2 3 ����10������һ��
				}
//				modbusWorkErrCheck();//modbus ������״̬��ӡ
//				errConfigCheck();//	modbusWorkErrCheck();//errConfigCheck();
				//modbusPrintRead();
//				if(gbNetState ==RT_FALSE){
//						rt_kprintf("%sERR:�������\n",sign);
//				}
		}
}


int main(void)
{

		RELAY1_ON;
		RELAY2_ON;
		RELAY3_ON;
		RELAY4_ON;//�ϵ���ⲿ485ȫ����

    rt_kprintf("\n%s%s  ver=%02d.%02d\n",sign,date,(uint8_t)(APP_VER>>8),(uint8_t)APP_VER);
	  rt_err_t result;
		stm32_flash_read(FLASH_IP_SAVE_ADDR,    (uint8_t*)&packFLash,sizeof(packFLash));
		stm32_flash_read(FLASH_MODBUS_SAVE_ADDR,(uint8_t*)&sheet,    sizeof(sheet));
//	  extern void  flashTest();
//	  flashTest();
//////////////////////////////////////�ź���//////////////////////////////
	  w5500Iqr_semp = rt_sem_create("w5500Iqr_semp",0, RT_IPC_FLAG_FIFO);
		if (w5500Iqr_semp == RT_NULL)
    {
        rt_kprintf("%screate w5500Iqr_semp failed\n",sign);
    }
		
		
		  /* ������ʱ�� ���ڶ�ʱ�� */
    timer1 = rt_timer_create("timer1", timeout1,
                             RT_NULL, 100,
                             RT_TIMER_FLAG_PERIODIC);
		if (timer1 != RT_NULL)
        rt_timer_start(timer1);
		//����285�豸�õ��Ļ��� ����
		
		int ret = rt_mq_init(&LCDmque,"LCDrecBuf",&LCDQuePool[0],1,LCD_BUF_LEN,RT_IPC_FLAG_FIFO);       
		if (ret != RT_EOK)
		{
				rt_kprintf("%sinit LCD msgque failed.\n",sign);
				return -1;
		}
		extern void 	uartMutexQueueCreate();
		uartMutexQueueCreate();
////////////////////////////////////����//////////////////////////////////
		

    result = rt_mb_init(&mbNetRecData,"mbRec",&mbRecPool[0],sizeof(mbRecPool)/4,RT_IPC_FLAG_FIFO);         
    if (result != RT_EOK)
    {
        rt_kprintf("%sinit mailbox NetRecData failed.\n",sign);
        return -1;
    }
    result = rt_mb_init(&mbNetSendData,"mbSend",&mbSendPool[0],sizeof(mbSendPool)/4,RT_IPC_FLAG_FIFO);         
    if (result != RT_EOK)
    {
        rt_kprintf("%sinit mailbox NetSend failed.\n",sign);
        return -1;
    }
		

////////////////////////////////����////////////////////////////////////
    tidW5500 =  rt_thread_create("w5500",w5500Task,RT_NULL,1024,2, 10 );
		if(tidW5500!=NULL){
				rt_thread_startup(tidW5500);													 
				rt_kprintf("%sRTcreat w5500Task task\r\n",sign);
		}
		tidNetRec =  rt_thread_create("netRec",netDataRecTask,RT_NULL,1024,2, 10 );
		if(tidNetRec!=NULL){
				rt_thread_startup(tidNetRec);													 
				rt_kprintf("%sRTcreat netDataRecTask \r\n",sign);
		}
		tidNetSend =  rt_thread_create("netSend",netDataSendTask,RT_NULL,1024,2, 10 );
		if(tidNetSend!=NULL){
				rt_thread_startup(tidNetSend);													 
				rt_kprintf("%sRTcreat netDataSendTask \r\n",sign);
		}

		
		tidUpkeep =  rt_thread_create("upKeep",upKeepStateTask,RT_NULL,512*3,2, 10 );
		if(tidUpkeep!=NULL){
				rt_thread_startup(tidUpkeep);													 
				rt_kprintf("%sRTcreat upKeepStateTask \r\n",sign);
		}
		tidLCD    =  rt_thread_create("LCD",LCDTask,RT_NULL,512*3,2, 10 );
		if(tidLCD!=NULL){
				rt_thread_startup(tidLCD);													 
				rt_kprintf("%sRTcreat LCDStateTask \r\n",sign);
		}
		//���г�ʼ��֮���ٿ��������жϽ���
//////////////////////////////����//////////////////////////////////////
    while (0)//task���ڲ��� �Լ����Ʋ���
    {
				hardWareDriverTest();
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
				rt_thread_mdelay(250);
			  extern rt_bool_t gbNetState;
			  if(gbNetState==RT_TRUE){
					  rt_thread_mdelay(250);
						
				}
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
				rt_thread_mdelay(250);
				if(gbNetState==RT_TRUE){
					  rt_thread_mdelay(250);	
				}
    }
}
//tasklog delete
void  tasklog(int argc, char *argv[])
{
		if(argc!=2){
				goto ERR;
		}
		if(rt_strcmp("delete",argv[1])==0){
				rt_thread_delete(tidW5500);
			  rt_thread_delete(tidNetRec);
			  rt_thread_delete(tidNetSend);
			  rt_thread_delete(tidUpkeep);
			  rt_kprintf("%s[tasklog delete OK]",sign);
		}
		ERR:
		rt_kprintf("%s[tasklog delete]",sign);
}
MSH_CMD_EXPORT(tasklog,tasklog del);//FINSH_FUNCTION_EXPORT_CMD