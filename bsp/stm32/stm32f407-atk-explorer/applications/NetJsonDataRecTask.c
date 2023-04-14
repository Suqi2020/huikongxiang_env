#include   "NetJsonDataTask.h"
#include   "board.h"
#include   "transport.h"
#include "mqttTask.h"
const static char task[]="[dataRec]";
extern MQTTDataPacktStru  MQTTDataPackt[MQTT_PACK_CNT];
//struct rt_mailbox mbNetRecData;
extern void AllDownPhrase(char *data,int lenth);
//备用 解决拼包  错包的问题


//网络数据接收的处理 
//void   netDataRecTask(void *para)
//{
//	
//	  uint8_t *str=RT_NULL;
//		while(1){
//			 if (rt_mb_recv(&mbNetRecData, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
//			 {
//		#if 1
//					 uint16_t  lenth= netDataCheck(str);
//					 if(lenth>0){
//								AllDownPhrase((char *)str,lenth);
//					 }
//		#else
//					 netDataCheckp(str);
//		#endif	
//			 }

//		}
//}


