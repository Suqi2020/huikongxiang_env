#include   "NetJsonDataTask.h"
#include   "board.h"
#include   "transport.h"
#include "mqttTask.h"
const static char task[]="[dataRec]";
extern MQTTDataPacktStru  MQTTDataPackt[MQTT_PACK_CNT];
struct rt_mailbox mbNetRecData;
extern void AllDownPhrase(char *data,int lenth);
//备用 解决拼包  错包的问题


//网络数据接收的处理 
void   netDataRecTask(void *para)
{
	
	  uint8_t *str=RT_NULL;
		while(1){
			 if (rt_mb_recv(&mbNetRecData, (rt_ubase_t *)&str, 1000) == RT_EOK)
			 {
		
					 uint32_t  lenth= (str[0]<<24)+(str[1]<<16)+(str[2]<<8)+str[3];
					 if(lenth>0){
							 void netRecSendEvent(uint8_t *recBuf,int len)	;
							 netRecSendEvent((uint8_t *)(str+PACK_HEAD_LEN),lenth);		
					 }
			 }
#ifdef  USE_WDT
			rt_event_send(&WDTEvent,EVENT_WDT_RECTASK);
#endif
		}
}


