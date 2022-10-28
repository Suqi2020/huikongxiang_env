#include "uartReconf.h"

//����ÿ�����ڶ�Ӧ�Ĳ����� �Լ�modbus������
//�����ڶ����������� UART2(1) UART3(2) UART6(3) UART1(debug) UART4(4)
//Ŀǰ��������       ����  �ַ�  ������             ����

const static char sign[]="[uartRecfg]";
modbusConfStru  modDev[UART_NUM];
uartChanlStru  chanl={USE_DIS_UART,USE_DIS_UART,USE_DIS_UART,USE_DIS_UART};
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
void uartReconfig()
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

void uartSingConf(int num)
{
	switch(num){
		case USE_UART2:
			MX_USART2_UART_Init(modDev[USE_UART2].bps	);
		  rt_kprintf("%sUART conf uart2 bps%d\n",sign,modDev[USE_UART2].bps);
			break;
		case USE_UART3:
			MX_USART3_UART_Init(modDev[USE_UART3].bps	);
		  rt_kprintf("%sUART conf uart3 bps%d\n",sign,modDev[USE_UART3].bps);
			break;
	  case USE_UART4:
			MX_UART4_Init(modDev[USE_UART4].bps	);
		  rt_kprintf("%sUART conf uart4 bps%d\n",sign,modDev[USE_UART4].bps);
			break;
		case USE_UART6:
			MX_USART6_UART_Init(modDev[USE_UART6].bps	);
		  rt_kprintf("%sUART conf uart6 bps%d\n",sign,modDev[USE_UART6].bps);
			break;
	}
}
//note����Ҫ������ͬһ�����ڲ����ʲ�ͬ�����
void uartConfFlashRead()
{
	
	
//		modDev[USE_UART2].bps	=115200;
//		modDev[USE_UART3].bps	=115200;
//		modDev[USE_UART6].bps =9600;
//		modDev[USE_UART4].bps	=9600;
//	
//	  chanl.cirCula		  =USE_UART3;//ʹ�ô���2
//	  chanl.partDischag =USE_UART3;//ʹ�ô���3
//	  chanl.pressSettl	=USE_UART4;//ʹ�ô���6
//	  chanl.threeAxis   =USE_UART4;//ʹ�ô���4
	
		modDev[USE_UART2].uartMutex	=uartMutex[USE_UART2];
		modDev[USE_UART3].uartMutex	=uartMutex[USE_UART3];
		modDev[USE_UART6].uartMutex	=uartMutex[USE_UART6];
		modDev[USE_UART4].uartMutex	=uartMutex[USE_UART4];

		modDev[USE_UART2].uartMessque	=&uartmque[USE_UART2];
		modDev[USE_UART3].uartMessque	=&uartmque[USE_UART3];
		modDev[USE_UART6].uartMessque	=&uartmque[USE_UART6];
		modDev[USE_UART4].uartMessque	=&uartmque[USE_UART4];
	
	  
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
//////////////////////////////////////////////////////////////////////////////
//modbusName ��Ҫ��uartBpsһһ��Ӧ
static char modbusName[UART_NUM][20] ={"�ӵػ���","�ַ�","������","���������"};
static int  modbusBps[UART_NUM]        ={115200,   115200  ,9600,   9600};
static uartEnum *modbusChanl[UART_NUM] ={&chanl.cirCula,&chanl.partDischag,&chanl.pressSettl,&chanl.threeAxis};
//////////////////////////////////////////////////////////////////////////////
//ͬ�� �±߶�����Ҫһһ��Ӧ����
static char     UartName[UART_NUM][6]	   ={"uart1","uart2","uart3","uart4"};
static uartEnum UartNum[UART_NUM]={USE_UART2,USE_UART3,USE_UART6,USE_UART4};
//////////////////////////////////////////////////////////////////////////////
//		modDev[USE_UART2].bps	=115200;
//		modDev[USE_UART3].bps	=115200;
//		modDev[USE_UART6].bps =9600;
//		modDev[USE_UART4].bps	=9600;
//	
//	  chanl.cirCula		  =USE_UART3;//ʹ�ô���2
//	  chanl.partDischag =USE_UART3;//ʹ�ô���3
//	  chanl.pressSettl	=USE_UART4;//ʹ�ô���6
//	  chanl.threeAxis   =USE_UART4;//ʹ�ô���4

static void modbus(int argc, char *argv[])
{
	  int i;
		if (argc != 4)
		{
				rt_kprintf("ERR input argc\n");
				goto ERR;
		}
		for( i=0;i<UART_NUM;i++){
				if(0==rt_strcmp((char *)modbusName[i], argv[1])){
						break;
				}
		}
		if(i==UART_NUM){
				rt_kprintf("err:argv[1]\n");
				goto ERR;
		}
		for( i=0;i<UART_NUM;i++){
				if(0==rt_strcmp((char *)UartName[i], argv[2])){
						break;
				}
		}
		if(i==UART_NUM){
				rt_kprintf("err:argv[2]\n");
				goto ERR;
		}
		int reslt=atoi16(argv[3],10);
		if((reslt==0)||(reslt>255)){
				rt_kprintf("err:argv[3] between 0 and 255 %d\n",argv[3]);
				goto ERR;
		}
		

		for(i=0;i<UART_NUM;i++){
				if(0==rt_strcmp((char *)modbusName[0], argv[1])){
						for(int j=0;j<UART_NUM;j++){
								if(0==rt_strcmp((char *)UartName[i], argv[2])){
										*modbusChanl[j]= UartNum[j];
										modDev[chanl.cirCula].bps =modbusBps[i];
									  uartSingConf(UartNum[j]);
										break;
								}
						}
				}
		}
    uartReconfig();
		//uartConfFlashRead();//��������   flash�洢�������ô���
		return;//��ȷ����
		ERR:
		rt_kprintf("for example mobus+�豸����+����+�豸��ַ\n");
		for( i=0;i<UART_NUM;i++){
				rt_kprintf("[modbus %10s %s %d]\n",modbusName[i],UartNum[i],i+1);
		}
}
//FINSH_FUNCTION_EXPORT(modbus, offline finsh);//FINSH_FUNCTION_EXPORT_CMD
MSH_CMD_EXPORT(modbus,offline stamp);//FINSH_FUNCTION_EXPORT_CMD
