#include   "NetJsonDataTask.h"
#include   "board.h"


rt_bool_t gbRegFlag = RT_FALSE;
extern struct rt_mailbox mbNetSendData;
extern uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];
extern rt_bool_t gbNetState;

const uint16_t  heart_timTick=120; //120秒发一次心跳
const uint16_t  reg_timTick=50;  //5秒注册一次
uint16_t  uart6_timTick=60;//默认值给个60
uint16_t  uart2_timTick=60;//默认值给个60
uint16_t  uart3_timTick=60;//默认值给个60
uint16_t  uart4_timTick=60;//默认值给个60
//利用upKeepStateTask中1秒的间隔做一个简易定时器
//来执行对应的上行包的发送
//掉线情况下先不执行count
static void easyUpTimer()
{
	  
	  static uint32_t count = 0;
	  count++;
		if((count%heart_timTick)==0){
//				heartUpPack();
//				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
			  rt_kprintf("heart timer out\r\n");
		}
		if(gbRegFlag==RT_FALSE){
				if(((count+1)%reg_timTick)==0){//不能用else 有可能多个定时任务同时到
//						rt_kprintf("reg timer out\r\n");
//					  devRegPack();
				}
		}
		if(((count+3)%uart6_timTick)==0){
				rt_kprintf("485_1 timer out\r\n");

		}
		if(((count+5)%uart2_timTick)==0){
			  readCirCurrAndWaring();
			  if(waringcheck()==RT_TRUE){//先发报警状态 再发数据
						waringEventPack();
					  rt_thread_mdelay(1000);//延时1秒再发下一包
				}
				cirCulaDataPack();
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("485_2 timer out\r\n");
		}
		if(((count+7)%uart3_timTick)==0){
				rt_kprintf("485_3 timer out\r\n");
		}
		if(((count+9)%uart4_timTick)==0){
				rt_kprintf("485_4 timer out\r\n");
				rs485DataPack();
		}

}
//上行数据的维护以及重发
void   upKeepStateTask(void *para)
{
	 
	  extern  void devIDRead();
    devIDRead();
	  uint32_t count=20;
	  extern void cirCurrConf();
	  cirCurrConf();//公众电流初始化
	  extern void  partDischagMutexQueueCreat();
	  partDischagMutexQueueCreat();
		while(1){
			  if(gbNetState ==RT_TRUE){
						easyUpTimer();
				}
//				if((count--)==0){
//					count=30;
//				readCirCurrAndWaring();
//				}
				rt_thread_mdelay(1000);
		}
}

