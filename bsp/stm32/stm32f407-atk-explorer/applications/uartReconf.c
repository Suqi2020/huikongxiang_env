#include "uartReconf.h"

//配置每个串口对应的波特率 以及modbus传感器
//从网口端数来依次是 UART2(1) UART3(2) UART6(3) UART1(debug) UART4(4)
//目前的配置是       环流  局放  沉降仪             三轴


//devIDRead()
const static char sign[]="[uartRecfg]";
modbusConfStru  modDev[UART_NUM];
uartChanlStru  chanl;


extern void cirCurrUartSend(uint8_t *buf,int len);
//串口接收后丢到队列里
extern rt_err_t cirCurrUartRec(uint8_t dat);

extern void partDischagUartSend(uint8_t *buf,int len);
//串口接收后丢到队列里
extern rt_err_t partDischagUartRec(uint8_t dat);

extern void pressSettlUartSend(uint8_t *buf,int len);
//串口接收后丢到队列里
extern rt_err_t pressSettlUartRec(uint8_t dat);

extern void threeAxisUartSend(uint8_t *buf,int len);
//串口接收后丢到队列里
extern rt_err_t threeAxisUartRec(uint8_t dat);
void uartReconfig(void);
void uartConfFlashRead()
{
	
//	  rt_strcpy(modDev[0].name,"circula");
//	  chanl.cirCula		  =USE_UART3;//使用串口2
//	  chanl.partDischag =USE_UART6;//使用串口3
//	  chanl.pressSettl	=USE_UART4;//使用串口6
//	  chanl.threeAxis   =USE_UART2;//使用串口4
	  chanl.cirCula		  =USE_UART2;//使用串口2
	  chanl.partDischag =USE_UART3;//使用串口3
	  chanl.pressSettl	=USE_UART6;//使用串口6
	  chanl.threeAxis   =USE_UART4;//使用串口4
		modDev[chanl.cirCula].bps		 =115200;
		modDev[chanl.partDischag].bps=115200;
		modDev[chanl.pressSettl].bps =9600;
		modDev[chanl.threeAxis].bps	 =9600;
	
		modDev[chanl.cirCula].UartRec    =&cirCurrUartRec;
		modDev[chanl.partDischag].UartRec=&partDischagUartRec;
		modDev[chanl.pressSettl].UartRec =&pressSettlUartRec;
		modDev[chanl.threeAxis].UartRec  =&threeAxisUartRec;

	  uartReconfig();
}


uint8_t  tetStr[]="qwertyuiopASDFGHJKLZXCVBNM";
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

//串口重新配置 选择对应的modbus传感器
void uartReconfig()
{
		
		MX_UART4_Init(modDev[USE_UART4].bps	);
		MX_USART2_UART_Init(modDev[USE_UART2].bps	);
		MX_USART3_UART_Init(modDev[USE_UART3].bps	);
		MX_USART6_UART_Init(modDev[USE_UART6].bps	);
	  rt_kprintf("%sUART re config\n",sign);
//	for(int i=0;i<1000;i++){
//		rs485UartSend(0,tetStr,sizeof(tetStr));
//		rs485UartSend(1,tetStr,sizeof(tetStr));
//		rs485UartSend(2,tetStr,sizeof(tetStr));
//		rs485UartSend(3,tetStr,sizeof(tetStr));
//		rt_thread_mdelay(2000);
//	}
}

