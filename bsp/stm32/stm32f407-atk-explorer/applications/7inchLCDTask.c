#include   "board.h"

extern struct  rt_messagequeue LCDmque;
extern void LCDDispIP();
extern void LCDDispUart();
extern void LCDDispMCUID();
extern void LCDDispModbusGet();
extern void LDCDispMosbus();
extern void LCDDispModInfoCpy();
extern void LCDDispConfig(uint8_t *recBuf,int len);
extern uint8_t  recLCDBuf[LCD_BUF_LEN];
//uint8_t id[]={0x5A,0xA5 ,0x14 ,0x82 ,0x12 ,0x10 ,0x31 ,0x32 ,0x33 ,0x34 ,0x35 ,0x35 ,0x36 ,0x6A ,0x75 ,0x68 ,0x75 ,0x6A ,0x6A ,0x31 ,0x31 ,0xFF ,0xFF};
//uint8_t name[]={0x5A ,0xA5 ,0x11,0x82 ,0x13,0x60 ,0xC9,0xCF ,0xBA,0xA3 ,0xB9,0xC8 ,0xD4,0xAA ,0xBF,0xC6 ,0xBC,0xBC ,0xFF,0xFF};

void  LCDTask(void *parameter)
{
	  rt_thread_mdelay(1000);//必须加入延时等待串口屏启动
//	 for( int i=0;i<sizeof(id);i++){
//		 HAL_UART_Transmit(&huart5,id+i,1,1000); 
//	 }
//	 
//	  
//	 for( int i=0;i<sizeof(name);i++){
//		 HAL_UART_Transmit(&huart5,name+i,1,1000); 
//	 }	
	
	  LCDDispIP();
	  LCDDispUart();
	  LCDDispMCUID();
	  LCDDispModbusGet();
	  LCDDispModInfoCpy();
  	LDCDispMosbus();
	  int revLen=0;
		while(1){
			
			if(rt_mq_recv(&LCDmque, recLCDBuf+revLen, 1, RT_WAITING_FOREVER) == RT_EOK){
				  revLen++;
					while(rt_mq_recv(&LCDmque, recLCDBuf+revLen, 1, 2) == RT_EOK){
						 revLen++;
					}
			}
			if(revLen){
					 LCDDispConfig(recLCDBuf,revLen);
					 revLen=0;
			}
		}
}