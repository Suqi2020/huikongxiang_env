#include "uartReconf.h"

//����ÿ�����ڶ�Ӧ�Ĳ����� �Լ�modbus������
//�����ڶ����������� UART2(1) UART3(2) UART6(3) UART1(debug) UART4(4)
//Ŀǰ��������       ����  �ַ�  ������             ����

const static char sign[]="[uartRecfg]";
uartConfStru  uartDev[UART_NUM];
//uartChanlStru  chanl={USE_DIS_UART,USE_DIS_UART,USE_DIS_UART,USE_DIS_UART};
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
	
		return rt_mq_send(uartDev[uartNum].uartMessque, &dat, 1);  //�յ����ݺ���������ﶪ
}


//������������ ѡ���Ӧ��modbus������
void uartReconfig()
{
		for(int i=0;i<UART_NUM;i++){
				if(uartDev[i].bps==0){//û�õ��Ĵ��� ����Ĭ�ϲ����� 
					 // rt_kprintf("%sport[%d] no use\n",sign,i+1);
					  uartDev[i].bps=4800;
				}
		}
		for(int i=0;i<UART_NUM;i++){
				rt_kprintf("%sport%d bps[%d]\n",sign,i+1,uartDev[i].bps);
		}
		MX_UART4_Init(uartDev[USE_UART4].bps	);
		MX_USART2_UART_Init(uartDev[USE_UART2].bps	);
		MX_USART3_UART_Init(uartDev[USE_UART3].bps	);
		MX_USART6_UART_Init(uartDev[USE_UART6].bps	);
	  rt_kprintf("%sUART re config\n",sign);

}

void uartSingConf(int num)
{
	//rt_kprintf("%sUART single conf\n");
	switch(num){
		case USE_UART2:
			MX_USART2_UART_Init(uartDev[USE_UART2].bps	);
		  rt_kprintf("%sUART conf uart2 bps=%d\n",sign,uartDev[USE_UART2].bps);
			break;
		case USE_UART3:
			MX_USART3_UART_Init(uartDev[USE_UART3].bps	);
		  rt_kprintf("%sUART conf uart3 bps=%d\n",sign,uartDev[USE_UART3].bps);
			break;
	  case USE_UART4:
			MX_UART4_Init(uartDev[USE_UART4].bps	);
		  rt_kprintf("%sUART conf uart4 bps=%d\n",sign,uartDev[USE_UART4].bps);
			break;
		case USE_UART6:
			MX_USART6_UART_Init(uartDev[USE_UART6].bps	);
		  rt_kprintf("%sUART conf uart6 bps=%d\n",sign,uartDev[USE_UART6].bps);
			break;
	}
}
//note����Ҫ������ͬһ�����ڲ����ʲ�ͬ�����
void uartMutexQueueCfg()
{
	
		uartDev[USE_UART2].uartMutex	=uartMutex[USE_UART2];
		uartDev[USE_UART3].uartMutex	=uartMutex[USE_UART3];
		uartDev[USE_UART6].uartMutex	=uartMutex[USE_UART6];
		uartDev[USE_UART4].uartMutex	=uartMutex[USE_UART4];

		uartDev[USE_UART2].uartMessque	=&uartmque[USE_UART2];
		uartDev[USE_UART3].uartMessque	=&uartmque[USE_UART3];
		uartDev[USE_UART6].uartMessque	=&uartmque[USE_UART6];
		uartDev[USE_UART4].uartMessque	=&uartmque[USE_UART4];
	
	  
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
//modbusName ��Ҫ��uartBps modbusChanlһһ��Ӧ

//const static char  modbusName[MODBUS_NUM][20] ={"�ӵػ���","�ַ�","������","���������","����"};
//const static int   modbusBps[MODBUS_NUM]      ={115200,  115200  ,9600,9600,4800};
//static uartEnum    *modbusChanl[MODBUS_NUM]   ={&modbusFlash[CIRCULA].useUartNum,&modbusFlash[PARTDISCHAG].useUartNum,&modbusFlash[PRESSSETTL].useUartNum,&modbusFlash[THREEAXIS].useUartNum};
//////////////////////////////////////////////////////////////////////////////
//ͬ�� �±߶�����Ҫһһ��Ӧ����
//const static char     UartName[UART_NUM][6] ={"port1", "port2",  "port3",  "port4"};//��ӳ��һ������
//const static uartEnum UartNum[UART_NUM]     ={USE_UART2,USE_UART3,USE_UART6,USE_UART4};
//////////////////////////////////////////////////////////////////////////////
modbusFlashStru  modbusFlash[MODBUS_NUM]  __attribute__ ((aligned (4)));
//		uartDev[USE_UART2].bps	=115200;
//		uartDev[USE_UART3].bps	=115200;
//		uartDev[USE_UART6].bps =9600;
//		uartDev[USE_UART4].bps	=9600;
//	
//	  modbusFlash[CIRCULA].useUartNum		  =USE_UART3;//ʹ�ô���2
//	  modbusFlash[PARTDISCHAG].useUartNum =USE_UART3;//ʹ�ô���3
//	  modbusFlash[PRESSSETTL].useUartNum	=USE_UART4;//ʹ�ô���6
//	  modbusFlash[THREEAXIS].useUartNum   =USE_UART4;//ʹ�ô���4

//���ͬ�����豸��ͬһ���豸��ַ��ͬһ���˿�
static rt_bool_t modbusSameTypeUseSameAddr()
{
//		if((modbusFlash[a].workFlag==RT_TRUE)&&(modbusFlash[b].workFlag==RT_TRUE)){
//				if(modbusFlash[a].useUartNum  ==modbusFlash[b].useUartNum){
//						if(modbusFlash[a].slaveAddr  ==modbusFlash[b].slaveAddr){
//							 rt_kprintf("%sERR:%s %s �豸��ַ�ظ�\n",sign,modbusName[a],modbusName[b]);
//							 return RT_TRUE;
//						}
//				}
//		}
//		return  RT_FALSE;
		
		
		
		for(int i=0;i<MODBUS_NUM;i++){
				for(int j=i+1;j<MODBUS_NUM;j++){
						if(modbusFlash[i].useUartNum==modbusFlash[j].useUartNum){
								if(modbusFlash[i].workFlag==RT_TRUE){
										if(modbusFlash[j].workFlag==RT_TRUE){
												if(modbusType[i]==modbusType[j]){
														if(modbusFlash[i].slaveAddr  ==modbusFlash[j].slaveAddr){
																rt_kprintf("%sERR:%s %s ͬ����ͬ�˿��豸ʹ������ͬ��ַ\n",sign,modbusName[i],modbusName[j]);
																return RT_TRUE;
														}
												}
										}
								}
						}
				}
		}
		return  RT_FALSE;
}
//��鲻ͬ�����豸��ͬһ���˿�
//modbusType
static rt_bool_t modbusDifTypeUseSamePort(void)
{
		for(int i=0;i<MODBUS_NUM;i++){
				for(int j=i+1;j<MODBUS_NUM;j++){
						if(modbusFlash[i].useUartNum==modbusFlash[j].useUartNum){
								if(modbusFlash[i].workFlag==RT_TRUE){
										if(modbusFlash[j].workFlag==RT_TRUE){
												if(modbusType[i]!=modbusType[j]){
														rt_kprintf("%sERR:%s %s ��ͬ�����豸ʹ����ͬһ���˿�\n",sign,modbusName[i],modbusName[j]);
														return RT_TRUE;
												}
										}
								}
						}
				}
		}
		return  RT_FALSE;
}

//��ӡ����
void  modbusPrintRead()
{
		for(int i=0;i<MODBUS_NUM;i++){
				if(modbusFlash[i].workFlag	==RT_TRUE){//
						uartDev[modbusFlash[i].useUartNum].bps =modbusBps[i];
					  rt_kprintf("%s���� %-10s ������%6d %s  slavAddr=%2d �ɼ����%d\n",sign,modbusName[i],modbusBps[i],UartName[modbusFlash[i].useUartNum],modbusFlash[i].slaveAddr,modbusFlash[i].colTime);
				}
				else
					 rt_kprintf("%sֹͣ %s\n",sign,modbusName[i]);
		}
}
//�������ô���-����true ������-����false
//����Ŀ�� ��Ҫ�ظ����ô��� �п��ܰ��������豸���óɴ���Ĳ����� �����豸������
 rt_bool_t errConfigCheck()
{
		rt_bool_t a=RT_FALSE,c=RT_FALSE;
		//���ͬ�����豸��ͬһ���豸��ַ
		a	=	modbusSameTypeUseSameAddr();
		//��鲻ͬ�����豸��ͬһ���˿�
		c	=	modbusDifTypeUseSamePort();

		if((a==RT_TRUE)||(c==RT_TRUE)){
				for(int i=0;i<MODBUS_NUM;i++){
						rt_kprintf("%s ����[%d],%-10s,port[%-03d],addr[%d]\n",sign,modbusType[i],modbusName[i],modbusFlash[i].useUartNum,modbusFlash[i].slaveAddr);
				}
			  return RT_TRUE;
		}
		return RT_FALSE;
}
//modbus�����쳣���
void modbusWorkErrCheck()
{
		for(int i=0;i<MODBUS_NUM;i++){
				if(modbusFlash[i].workFlag	==RT_TRUE){  
						if(uartDev[modbusFlash[i].useUartNum].offline==RT_TRUE){
								rt_kprintf("%sERR:����<<%s>>%s 485���߻��Դ\n",sign,modbusName[i],UartName[modbusFlash[i].useUartNum]);
						}
				}
		}
}


//modbus   �ַ� port2 2 60
//ʹ��ǰ��������������upKeepStateTask�в����Լ��ص�������ʼ������ʹ��
static void modbus(int argc, char *argv[])
{
	  extern void timeStop(upDataTimEnum num);
	  extern void timeInit(upDataTimEnum num,int value,int firstCnt);
	  int i,j;
		if (argc != 5)
		{
				rt_kprintf("%sERR input argc\n",sign);
				goto ERR;
		}
		int reslt=atoi16(argv[3],10);
    int setTime =atoi16(argv[4],10);
		if(setTime<60){
				rt_kprintf("%serr:argv[4] �ɼ�ʱ��>60 now is%d\n",sign,time);
				goto ERR;
		}
		if(reslt>255){
				rt_kprintf("%serr:argv[3] between 0 and 255 %d\n",sign,argv[3]);
				goto ERR;
		}
	  for(i=0;i<MODBUS_NUM;i++){
				if(0==rt_strcmp((char *)modbusName[i], argv[1])){
					  //rt_kprintf("%sget modbusName \n",sign);
						for(j=0;j<UART_NUM;j++){
								if(0==rt_strcmp((char *)UartName[j], argv[2])){

									  if(reslt==0){
												modbusFlash[i].workFlag	  =RT_FALSE;//ͣ�õ�ǰ�豸
												rt_kprintf("%sͣ��%s\n",sign,modbusName[i]);
											  timeStop(i);
										}
										else{
												uartDev[UartNum[j]].bps =modbusBps[i];
												modbusFlash[i].useUartNum =UartNum[j];
												modbusFlash[i].slaveAddr	=reslt;
												modbusFlash[i].colTime=setTime;
											  
											  modbusFlash[i].workFlag	  =RT_TRUE;//���õ�ǰ�豸 Ϊ��errCheck
											  if(errConfigCheck()==RT_FALSE){//�ظ�����û�д��� �������ô��� ������
	
														uartSingConf(UartNum[j]);
														timeInit(i,modbusFlash[i].colTime,10+5*i);
														modbusFlash[i].modbusRead();//�ص�����
													  rt_kprintf("%s����%s\n",sign,modbusName[i]);
												}
												else
													 	modbusFlash[i].workFlag	  =RT_FALSE;//ͣ�õ�ǰ�豸
										}
										STMFLASH_Write(FLASH_SAVE_ADDR,(uint32_t*)modbusFlash,sizeof(modbusFlash));
										//д��flash��
										modbusWorkErrCheck();//errConfigCheck();
										modbusPrintRead();
										return;
								}
						}
				}
		}
		if(i==MODBUS_NUM){
				rt_kprintf("%serr:argv[1]\n",sign);
				goto ERR;
		}
		if(j==UART_NUM){
				rt_kprintf("%serr:argv[2]\n",sign);
				goto ERR;
		}

		return;//��ȷ����
		ERR:
		rt_kprintf("%sfor example:modbus+�豸����(������)+�˿�(port1-port4)+�豸��ַ(0-�ر��豸)+�ɼ����(��)\n",sign);
		rt_kprintf("%sNOTE:�����ڶԲ������н���,����Ҫ����\n",sign);
		for( i=0;i<MODBUS_NUM;i++){
				rt_kprintf("%son  [modbus %-10s(%6d) %s %d 120]\n",sign,modbusName[i],modbusBps[i],UartName[0],i+1);
		}
		for( i=0;i<MODBUS_NUM;i++){
				rt_kprintf("%soff [modbus %-10s(%6d) %s 0 120]\n",sign,modbusName[i],modbusBps[i],UartName[3]);
		}
}
//FINSH_FUNCTION_EXPORT(modbus, offline finsh);//FINSH_FUNCTION_EXPORT_CMD
MSH_CMD_EXPORT(modbus,port slaveaddr config);//FINSH_FUNCTION_EXPORT_CMD

