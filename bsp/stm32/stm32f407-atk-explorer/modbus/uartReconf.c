#include "uartReconf.h"

//����ÿ�����ڶ�Ӧ�Ĳ����� �Լ�modbus������
//�����ڶ����������� UART2(1) UART3(2) UART6(3) UART1(debug) UART4(4)
//Ŀǰ��������       ����  �ַ�  ������             ����



const static char sign[]="[uartRecfg]";
//uartConfStru  .uartMessque[UART_NUM];
//struct  rt_messagequeue uartMessque;
//uartChanlStru  chanl={USE_DIS_UART,USE_DIS_UART,USE_DIS_UART,USE_DIS_UART};
//static rt_mutex_t uartMutex[UART_NUM] ;//= {RT_NULL}; //����4�����ڵĻ���������
//#define  MSGPOOL_LEN   1024 //485���������  ����1k��Ҫ�޸Ĵ˴�
//���еĶ���
struct  rt_messagequeue uartmque[UART_NUM];//= {RT_NULL} ;//����4�����ڵĶ���
static uint8_t uartQuePool[UART_NUM][MSGPOOL_LEN];  //����4�����ڵĶ��г�


//���������õ��Ļ���������Ϣ����
void  uartMutexQueueCreate()
{
		 for(int i=0;i<UART_NUM;i++){
					char str[20]="";
//					sprintf(str,"urt%dMutex",i);
//					uartMutex[i] = rt_mutex_create(str, RT_IPC_FLAG_FIFO);
//					if (uartMutex[i] == RT_NULL)
//					{
//							rt_kprintf("%screate uart%dMutex failed.\n",sign,i);
//							return ;
//					}
//////////////////////////////////��Ϣ����/////////////////////////////////
					sprintf(str,"urt%dMsgque",i);
					int result = rt_mq_init(&uartmque[i],str,uartQuePool+i,1,MSGPOOL_LEN,RT_IPC_FLAG_FIFO);       
					if (result != RT_EOK)
					{
							rt_kprintf("%sinit uart%dmsgque failed.\n",sign,i);
							return ;
					}
			}
}

//���ڽ��� 

rt_err_t uartDataRec(uartEnum uartNum,uint8_t dat)
{
	
		return rt_mq_send(&uartmque[uartNum], &dat, 1);  //�յ����ݺ���������ﶪ
}


//������������ ѡ���Ӧ��modbus������
void uartReconfig()
{
		for(int i=0;i<UART_NUM;i++){
				if(packFlash.uartBps[i]==0){//û�õ��Ĵ��� ����Ĭ�ϲ����� 
					 // rt_kprintf("%sport[%d] no use\n",sign,i+1);
					  packFlash.uartBps[i]=4800;
				}
		}
		for(int i=0;i<UART_NUM;i++){
				rt_kprintf("%sport%d bps[%d]\n",sign,i+1,packFlash.uartBps[i]);
		}
	
		MX_USART2_UART_Init(packFlash.uartBps[0]);
		MX_USART3_UART_Init(packFlash.uartBps[1]);
		MX_USART6_UART_Init(packFlash.uartBps[2]);
		MX_UART4_Init(packFlash.uartBps[3]);
	  rt_kprintf("%sUART re config\n",sign);

}

void uartSingConf(int num,int bps)
{
	//rt_kprintf("%sUART single conf\n");
	switch(num){
		case USE_UART2:
			MX_USART2_UART_Init(bps	);
		  rt_kprintf("%sUART conf uart2 bps=%d\n",sign,packFlash.uartBps[0]);
			break;
		case USE_UART3:
			MX_USART3_UART_Init(bps	);
		  rt_kprintf("%sUART conf uart3 bps=%d\n",sign,packFlash.uartBps[1]);
			break;

		case USE_UART6:
			MX_USART6_UART_Init(bps	);
		  rt_kprintf("%sUART conf uart6 bps=%d\n",sign,packFlash.uartBps[2]);
			break;
		case USE_UART4:
			MX_UART4_Init(bps	);
		  rt_kprintf("%sUART conf uart4 bps=%d\n",sign,packFlash.uartBps[3]);
			break;
	}
}
//note����Ҫ������ͬһ�����ڲ����ʲ�ͬ�����
void uartMutexQueueCfg()
{
	
////		.uartMessque[USE_UART2].uartMutex	=uartMutex[USE_UART2];
//		.uartMessque[USE_UART3].uartMutex	=uartMutex[USE_UART3];
//		.uartMessque[USE_UART6].uartMutex	=uartMutex[USE_UART6];
//		.uartMessque[USE_UART4].uartMutex	=uartMutex[USE_UART4];

//		.uartMessque[USE_UART2]	=&uartmque[USE_UART2];
//		.uartMessque[USE_UART3]	=&uartmque[USE_UART3];
//		.uartMessque[USE_UART6]	=&uartmque[USE_UART6];
//		.uartMessque[USE_UART4]	=&uartmque[USE_UART4];
	
	  
}



//485���ݴ��ڷ���
void rs485UartSend(uint8_t chanl,uint8_t *buf,int len)
{
//UART2(1) UART3(2) UART6(3) UART1(debug) UART4(4)
		switch(chanl){
			case 0:
				UART2_485_SEND;
				HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,1000);
				UART2_485_REC;
				break;
			case 1:
				UART3_485_SEND;
				HAL_UART_Transmit(&huart3,(uint8_t *)buf,len,1000);
				UART3_485_REC;
				break;
			case 2:
				UART6_485_SEND;
				HAL_UART_Transmit(&huart6,(uint8_t *)buf,len,1000);
				UART6_485_REC;
				break;
			case 3:
				UART4_485_SEND;
				HAL_UART_Transmit(&huart4,(uint8_t *)buf,len,1000);
				UART4_485_REC;
				break;
			default:
				rt_kprintf("%sERR:UART chanl config\n",sign);
				break;
		}
}


//modbus+huanliu���豸���ƣ�+1(ͨ��)+1���豸��ַ��
uint16 atoi16(char* str,uint16 base); 
//example--[modbus ���� uart1 1]




//�˳�û����ʽ��������ǰ  �������ݵĸ���
void clearUartData()
{
	  uint8_t dat;
	  for(int i=0;i<UART_NUM;i++){
				while(rt_mq_recv(&uartmque[i],&dat, 1, 1000) == RT_EOK){//115200 ������1ms 10������
				}
		}
}

