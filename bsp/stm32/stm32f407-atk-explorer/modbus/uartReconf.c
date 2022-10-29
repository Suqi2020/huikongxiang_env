#include "uartReconf.h"

//配置每个串口对应的波特率 以及modbus传感器
//从网口端数来依次是 UART2(1) UART3(2) UART6(3) UART1(debug) UART4(4)
//目前的配置是       环流  局放  沉降仪             三轴

const static char sign[]="[uartRecfg]";
uartConfStru  uartDev[UART_NUM];
uartChanlStru  chanl={USE_DIS_UART,USE_DIS_UART,USE_DIS_UART,USE_DIS_UART};
static rt_mutex_t uartMutex[UART_NUM] ;//= {RT_NULL}; //创建4个串口的互斥量保护
#define  MSGPOOL_LEN   1024 //485数据最大量  大于1k需要修改此处
//队列的定义
static struct  rt_messagequeue uartmque[UART_NUM];//= {RT_NULL} ;//创建4个串口的队列
static uint8_t uartQuePool[UART_NUM][MSGPOOL_LEN];  //创建4个串口的队列池
//

//创建环流用到的互斥量和消息队列
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
//////////////////////////////////消息队列/////////////////////////////////
					sprintf(str,"urt%dMsgque",i);
					int result = rt_mq_init(&uartmque[i],str,uartQuePool+i,1,MSGPOOL_LEN,RT_IPC_FLAG_FIFO);       
					if (result != RT_EOK)
					{
							rt_kprintf("%sinit uart%dmsgque failed.\n",sign,i);
							return ;
					}
			}
}

//串口接收 

rt_err_t uartDataRec(uartEnum uartNum,uint8_t dat)
{
	
		return rt_mq_send(uartDev[uartNum].uartMessque, &dat, 1);  //收到数据后就往队列里丢
}


//串口重新配置 选择对应的modbus传感器
void uartReconfig()
{
		for(int i=0;i<UART_NUM;i++){
				if(uartDev[i].bps==0){//没用到的串口 给个默认波特率 
					  rt_kprintf("%sUART[%d] no use\n",sign,i);
					  uartDev[i].bps=4800;
				}
		}
		for(int i=0;i<UART_NUM;i++){
				rt_kprintf("%sUART%d bps[%d]\n",sign,i,uartDev[i].bps);
		}
		MX_UART4_Init(uartDev[USE_UART4].bps	);
		MX_USART2_UART_Init(uartDev[USE_UART2].bps	);
		MX_USART3_UART_Init(uartDev[USE_UART3].bps	);
		MX_USART6_UART_Init(uartDev[USE_UART6].bps	);
	  rt_kprintf("%sUART re config\n",sign);

}

void uartSingConf(int num)
{
	rt_kprintf("%sUART single conf\n");
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
//note：需要加入检测同一个串口波特率不同的情况
void uartConfFlashRead()
{
	
	
//		uartDev[USE_UART2].bps	=115200;
//		uartDev[USE_UART3].bps	=115200;
//		uartDev[USE_UART6].bps =9600;
//		uartDev[USE_UART4].bps	=9600;
//	
//	  chanl.cirCula		  =USE_UART3;//使用串口2
//	  chanl.partDischag =USE_UART3;//使用串口3
//	  chanl.pressSettl	=USE_UART4;//使用串口6
//	  chanl.threeAxis   =USE_UART4;//使用串口4
	
		uartDev[USE_UART2].uartMutex	=uartMutex[USE_UART2];
		uartDev[USE_UART3].uartMutex	=uartMutex[USE_UART3];
		uartDev[USE_UART6].uartMutex	=uartMutex[USE_UART6];
		uartDev[USE_UART4].uartMutex	=uartMutex[USE_UART4];

		uartDev[USE_UART2].uartMessque	=&uartmque[USE_UART2];
		uartDev[USE_UART3].uartMessque	=&uartmque[USE_UART3];
		uartDev[USE_UART6].uartMessque	=&uartmque[USE_UART6];
		uartDev[USE_UART4].uartMessque	=&uartmque[USE_UART4];
	
	  
}



//485根据串口发送
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


//modbus+huanliu（设备名称）+1(通道)+1（设备地址）
uint16 atoi16(char* str,uint16 base); 
//example--[modbus 环流 uart1 1]
//////////////////////////////////////////////////////////////////////////////
//modbusName 需要跟uartBps modbusChanl一一对应

const static char  modbusName[MODBUS_NUM][20] ={"接地环流",  "局放",             "沉降仪",          "三轴测振仪"};
const static int   modbusBps[MODBUS_NUM]      ={115200,       115200  ,           9600,             9600};
static uartEnum    *modbusChanl[MODBUS_NUM]   ={&chanl.cirCula,&chanl.partDischag,&chanl.pressSettl,&chanl.threeAxis};
//////////////////////////////////////////////////////////////////////////////
//同样 下边定义需要一一对应起来
const static char     UartName[UART_NUM][6] ={"uart1", "uart2",  "uart3",  "uart4"};
const static uartEnum UartNum[UART_NUM]     ={USE_UART2,USE_UART3,USE_UART6,USE_UART4};
//////////////////////////////////////////////////////////////////////////////
modbusFlashStru  modbusFlash[MODBUS_NUM]  __attribute__ ((aligned (4)));
//		uartDev[USE_UART2].bps	=115200;
//		uartDev[USE_UART3].bps	=115200;
//		uartDev[USE_UART6].bps =9600;
//		uartDev[USE_UART4].bps	=9600;
//	
//	  chanl.cirCula		  =USE_UART3;//使用串口2
//	  chanl.partDischag =USE_UART3;//使用串口3
//	  chanl.pressSettl	=USE_UART4;//使用串口6
//	  chanl.threeAxis   =USE_UART4;//使用串口4

static void modbus(int argc, char *argv[])
{
	  int i,j;
		if (argc != 4)
		{
				rt_kprintf("ERR input argc\n");
				goto ERR;
		}
		int reslt=atoi16(argv[3],10);
		if((reslt==0)||(reslt>255)){
				rt_kprintf("err:argv[3] between 0 and 255 %d\n",argv[3]);
				goto ERR;
		}
	 for(i=0;i<MODBUS_NUM;i++){
				if(0==rt_strcmp((char *)modbusName[i], argv[1])){
					  rt_kprintf("get modbusName \n");
						for(j=0;j<UART_NUM;j++){
								if(0==rt_strcmp((char *)UartName[j], argv[2])){
									  rt_kprintf("get UartName \n");
										*modbusChanl[j]= UartNum[j];
										uartDev[UartNum[j]].bps =modbusBps[i];
									  uartSingConf(UartNum[j]);
									  modbusFlash[i].workFlag	  =RT_TRUE;//启用当前设备
									  modbusFlash[i].useUartNum =UartNum[j];
									  modbusFlash[i].slaveAddr	=reslt;
										//写入flash中
										break;
								}
						}
				}
		}
		if(i==MODBUS_NUM){
				rt_kprintf("err:argv[1]\n");
				goto ERR;
		}
		if(j==UART_NUM){
				rt_kprintf("err:argv[2]\n");
				goto ERR;
		}

		


   // uartReconfig();
		//uartConfFlashRead();//根据配置   flash存储重新配置串口
		return;//正确跳出
		ERR:
		rt_kprintf("for example mobus+设备名称+串口+设备地址\n");
		for( i=0;i<UART_NUM;i++){
				rt_kprintf("[modbus %10s %s %d]\n",modbusName[i],UartName[i],i+1);
		}
}
//FINSH_FUNCTION_EXPORT(modbus, offline finsh);//FINSH_FUNCTION_EXPORT_CMD
MSH_CMD_EXPORT(modbus,offline stamp);//FINSH_FUNCTION_EXPORT_CMD

