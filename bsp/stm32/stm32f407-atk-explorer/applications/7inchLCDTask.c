#include   "board.h"

extern struct  rt_messagequeue LCDmque;
extern void LCDDispIP();
extern void LCDDispUart();
extern void LCDDispMCUID();
extern void LCDDispModbusGet();
extern void LDCDispMosbusInfo();
extern void LCDDispModInfoCpy();
extern void LCDDispConfig(uint8_t *recBuf,int len);
extern void firstNameDispInit();
extern uint8_t  recLCDBuf[LCD_BUF_LEN];



//char te1[5]={0xff,0xff,0xff,0xff,0xff};
//char te2[5]={0xff,0xff,0xff,0xff,0};
//char te3[5]={0xff,0xff,0xff,0xff,0};
//char te4[5]={0xff,0xff,0xff,0xff,0};
//char test[20];
//int testfun(void)
//{

//    strcpy(test,te1);
//	rt_kprintf("testfun:");
//		for(int i=0;i<20;i++){

//		rt_kprintf("%02x ",test[i]);
//	}
//		rt_kprintf("\n ");
//}
void  LCDTask(void *parameter)
{
	  rt_thread_mdelay(1000);//必须加入延时等待串口屏启动
	//testfun();
	  LCDDispIP();
	  LCDDispUart();
	  LCDDispMCUID();//LCD send:5a a5 15 82 12 10 ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
	  LCDDispModbusGet();
	  LCDDispModInfoCpy();
	  //firstNameDispInit();
  	LDCDispMosbusInfo();
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