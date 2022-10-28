#include "uartReconf.h"

//����ÿ�����ڶ�Ӧ�Ĳ����� �Լ�modbus������
//�����ڶ����������� UART2(1) UART3(2) UART6(3) UART1(debug) UART4(4)
//Ŀǰ��������       ����  �ַ�  ������             ����

const static char sign[]="[uartRecfg]";
modbusConfStru  modDev[UART_NUM];
uartChanlStru  chanl;
static rt_mutex_t uartMutex[UART_NUM] ;//= {RT_NULL}; //����4�����ڵĻ���������
#define  MSGPOOL_LEN   1024 //485���������  ����1k��Ҫ�޸Ĵ˴�
//���еĶ���
static struct  rt_messagequeue uartmque[UART_NUM];//= {RT_NULL} ;//����4�����ڵĶ���
static uint8_t uartQuePool[UART_NUM][MSGPOOL_LEN];  //����4�����ڵĶ��г�
//
//���������õ��Ļ���������Ϣ����
void  uartMutexQueueCreate()
{
		 for(int i=0;i<UART_NUM;i++){
					char str[20]="";
					sprintf(str,"urt%dMutex",i);
					uartMutex[i] = rt_mutex_create(str, RT_IPC_FLAG_FIFO);
					if (uartMutex[i] == RT_NULL)
					{
							rt_kprintf("%screate uart%dMutex failed.\n",sign,i);
							return ;
					}
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
	
		return rt_mq_send(modDev[uartNum].uartMessque, &dat, 1);  //�յ����ݺ���������ﶪ
}


//������������ ѡ���Ӧ��modbus������
static void uartReconfig()
{
		for(int i=0;i<UART_NUM;i++){
				if(modDev[i].bps==0){//û�õ��Ĵ��� ����Ĭ�ϲ����� 
					  rt_kprintf("%sUART[%d] no use\n",sign,i);
				}
		}
		for(int i=0;i<UART_NUM;i++){
				rt_kprintf("%sUART%d bps[%d]\n",sign,i,modDev[i].bps);
		}
		MX_UART4_Init(modDev[USE_UART4].bps	);
		MX_USART2_UART_Init(modDev[USE_UART2].bps	);
		MX_USART3_UART_Init(modDev[USE_UART3].bps	);
		MX_USART6_UART_Init(modDev[USE_UART6].bps	);
	  rt_kprintf("%sUART re config\n",sign);

}


//note����Ҫ������ͬһ�����ڲ����ʲ�ͬ�����
void uartConfFlashRead()
{
	
	
		modDev[USE_UART2].bps	=115200;
		modDev[USE_UART3].bps	=115200;
		modDev[USE_UART6].bps =9600;
		modDev[USE_UART4].bps	=9600;
	
	  chanl.cirCula		  =USE_UART3;//ʹ�ô���2
	  chanl.partDischag =USE_UART3;//ʹ�ô���3
	  chanl.pressSettl	=USE_UART4;//ʹ�ô���6
	  chanl.threeAxis   =USE_UART4;//ʹ�ô���4
	
		modDev[USE_UART2].uartMutex	=uartMutex[USE_UART2];
		modDev[USE_UART3].uartMutex	=uartMutex[USE_UART3];
		modDev[USE_UART6].uartMutex	=uartMutex[USE_UART6];
		modDev[USE_UART4].uartMutex	=uartMutex[USE_UART4];

		modDev[USE_UART2].uartMessque	=&uartmque[USE_UART2];
		modDev[USE_UART3].uartMessque	=&uartmque[USE_UART3];
		modDev[USE_UART6].uartMessque	=&uartmque[USE_UART6];
		modDev[USE_UART4].uartMessque	=&uartmque[USE_UART4];
	
	  uartReconfig();
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


