#include "uartReconf.h"

//配置每个串口对应的波特率 以及modbus传感器
//从网口端数来依次是 UART2(1) UART3(2) UART6(3) UART1(debug) UART4(4)
//目前的配置是       环流  局放  沉降仪             三轴

const static char sign[]="[uartRecfg]";
uartConfStru  uartDev[UART_NUM];
//uartChanlStru  chanl={USE_DIS_UART,USE_DIS_UART,USE_DIS_UART,USE_DIS_UART};
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
				if(packFLash.port[i].bps==0){//没用到的串口 给个默认波特率 
					 // rt_kprintf("%sport[%d] no use\n",sign,i+1);
					  packFLash.port[i].bps=4800;
				}
		}
		for(int i=0;i<UART_NUM;i++){
				rt_kprintf("%sport%d bps[%d] calTime[%d]\n",sign,i+1,packFLash.port[i].bps,packFLash.port[i].calTime);
		}
		
		MX_USART2_UART_Init(packFLash.port[0].bps);
		MX_USART3_UART_Init(packFLash.port[1].bps);
		MX_USART6_UART_Init(packFLash.port[2].bps);
		MX_UART4_Init(packFLash.port[3].bps);	
	  rt_kprintf("%sUART re config\n",sign);

}

//void uartSingConf(int num)
//{
//	//rt_kprintf("%sUART single conf\n");
//	switch(num){
//		case USE_UART2:
//			MX_USART2_UART_Init(uartDev[USE_UART2].bps	);
//		  rt_kprintf("%sUART conf uart2 bps=%d\n",sign,uartDev[USE_UART2].bps);
//			break;
//		case USE_UART3:
//			MX_USART3_UART_Init(uartDev[USE_UART3].bps	);
//		  rt_kprintf("%sUART conf uart3 bps=%d\n",sign,uartDev[USE_UART3].bps);
//			break;
//	  case USE_UART4:
//			MX_UART4_Init(uartDev[USE_UART4].bps	);
//		  rt_kprintf("%sUART conf uart4 bps=%d\n",sign,uartDev[USE_UART4].bps);
//			break;
//		case USE_UART6:
//			MX_USART6_UART_Init(uartDev[USE_UART6].bps	);
//		  rt_kprintf("%sUART conf uart6 bps=%d\n",sign,uartDev[USE_UART6].bps);
//			break;
//	}
//}
//note：需要加入检测同一个串口波特率不同的情况
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

//滤除没有正式接收数据前  杂乱数据的干扰
void clearUartData()
{
	  uint8_t dat;
	  for(int i=0;i<UART_NUM;i++){
			while(rt_mq_recv(uartDev[i].uartMessque,&dat, 1, 1000) == RT_EOK){//115200 波特率1ms 10个数据
			}
		}
}
