#include   "NetJsonDataTask.h"
#include   "board.h"

const static char task[]="[dataSend]";

//struct rt_mailbox mbNetSendData;

extern uint16_t netSend(uint8_t *data,int len);
extern rt_bool_t gbNetState;
//uint16_t netDataSendCheck(uint8_t *str)
//{
//	  int lenth=0;
//		if((str[0]==(uint8_t)((uint16_t)HEAD>>8))&&(str[1]==(uint8_t)(HEAD))){
//			  lenth= (str[2]<<8)+str[3]+HEAD_LEN+LENTH_LEN+CRC_LEN+TAIL_LEN;  
//				if(lenth<=2048){
//				}
//				else{
//					rt_kprintf("%slenth  err：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",lenth,str[0],str[1],str[2],str[3],task);
//						//rt_kprintf("lenth  err %d str[2]:%d  str[3]:%d\r\n",lenth,str[2],str[3]);
//				}
//		}
//		else{
//				 rt_kprintf("%shead  err\r\n",task);
//		}
//		return lenth;
//}
//上行网络数据的发送  mqtt状态维护发包不调用此处
//char mqttPayload[2048];//=rt_malloc(strlen(topic.cstring)+lenth+1);
//void   netDataSendTask(void *para)
//{
//		uint8_t *str=RT_NULL;
//	  static MQTTString topic= MQTTString_initializer;
//	  int sendBufLen=0;
//		while(1){
//			  if (rt_mb_recv(&mbNetSendData, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
//        { 
//					
//						int lenth = strlen((char *)str);
//						topic.cstring = rt_malloc(100);
//					  rt_kprintf("%srec net data\r\n",task);
//						rt_sprintf(topic.cstring,"/acu/%s/up",packFlash.acuId);
////					char *mqttPayload=rt_malloc(strlen(topic.cstring)+lenth+1);
//					
//						char payload2[]="{\"deviceSendTime\":\"1571724098000\"}";
//						if((sendBufLen=MQTTSerialize_publish((unsigned char *)mqttPayload, sizeof(mqttPayload),0, 0, 0,0,topic, (unsigned char *)str,lenth))>0){ //qos=1????packetid
//								rt_kprintf("%stry to pub [%d]  %s\r\n",task,sendBufLen,mqttPayload);
//								if(gbNetState==RT_TRUE)
//										transport_sendPacketBuffer( mqttPayload, sendBufLen);
//						}
//						else
//							rt_kprintf("%serr:sendNetBufLen<=0\n",task);
//						rt_free(topic.cstring);
//						//rt_free(mqttPayload);
//						topic.cstring=NULL;
//						//mqttPayload=NULL;

//			}
//				
//		}
//}
char mqttPayload[2048];//=rt_malloc(strlen(topic.cstring)+lenth+1);
int lenth;
//void   netDataSendTask(void *para)
//{
//		uint8_t *str=RT_NULL;

//	  int sendBufLen=0;
//		while(1){
//			  if (rt_mb_recv(&mbNetSendData, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
//        { 
//					
//						 lenth = strlen((char *)str);
//		

//						rt_kprintf("%stry to pub [%s]\r\n",task,str);
////						if(gbNetState==RT_TRUE)
////										transport_sendPacketBuffer( str, lenth);

//						rt_thread_delay(1000);
//			}
//				
//		}
//}


		
//void netMqttSend()
//{

//	 packMqttSend();
// }
