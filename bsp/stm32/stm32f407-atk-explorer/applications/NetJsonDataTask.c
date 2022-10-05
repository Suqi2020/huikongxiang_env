#include   "NetJsonDataTask.h"
#include   "board.h"

extern struct rt_mailbox mbNetSendData;
extern uint8_t   packBuf[NET_LEN];
//上行数据的维护以及重发
void   upKeepStateTask(void *para)
{
	 
	  extern  void deviceIDRead();
    deviceIDRead();
	  uint32_t count=0;
		while(1){
			  if(count%30==0){
						heartUpPack();
					  rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
				}
				rt_thread_mdelay(2000);

		}
}

