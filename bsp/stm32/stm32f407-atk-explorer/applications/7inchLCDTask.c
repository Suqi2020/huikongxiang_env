
#include   "board.h"

extern struct  rt_messagequeue LCDmque;
extern void LCDDispIP(void);
extern void LCDDispUart(void);
extern void LCDDispMCUID(void);
extern void LCDDispModbusGet(void);
extern void LDCDispMosbusInfo(void);
extern void LCDDispConfig(uint8_t *recBuf,int len);
extern void firstNameDispInit(void);
extern uint8_t  recLCDBuf[LCD_BUF_LEN];



void  LCDTask(void *parameter)
{
	  rt_thread_mdelay(1000);//必须加入延时等待串口屏启动
	  LCDDispIP();
	  LCDDispUart();
	  LCDDispMCUID();//LCD send:5a a5 15 82 12 10 ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
	  LCDDispModbusGet();
  	LDCDispMosbusInfo();
	  int revLen=0;
		while(1){
			
			if(rt_mq_recv(&LCDmque, recLCDBuf+revLen, 1, 1000) == RT_EOK){
				  revLen++;
					while(rt_mq_recv(&LCDmque, recLCDBuf+revLen, 1, 2) == RT_EOK){
						 revLen++;
					}
			}
			if(revLen){
					 LCDDispConfig(recLCDBuf,revLen);
					 revLen=0;
			}
#ifdef  USE_WDT
			rt_event_send(&WDTEvent,EVENT_WDT_LCDTASK);
#endif
		}
}

