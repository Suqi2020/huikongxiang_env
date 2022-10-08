#include   "NetJsonDataTask.h"
#include   "board.h"

extern struct rt_mailbox mbNetSendData;
extern uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];
extern rt_bool_t gbNetState;
//上行数据的维护以及重发
void   upKeepStateTask(void *para)
{
	 
	  extern  void deviceIDRead();
    deviceIDRead();
	  uint32_t count=0;
		while(1){
			  if(gbNetState ==RT_TRUE){
						if(count++%30==0){
								heartUpPack();
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
								
						}
				}
				rt_thread_mdelay(2000);

		}
}

