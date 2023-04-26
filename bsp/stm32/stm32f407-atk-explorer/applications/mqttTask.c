#if 1
#include "board.h"
#include "mqttTask.h"
#include "transport.h"
/*
{"clientId":"a1S9kgxkc8w.humi_protect1|securemode=2,signmethod=hmacsha256,timestamp=1656814731352|",
 "username":"humi_protect1&a1S9kgxkc8w",
 "mqttHostUrl":"a1S9kgxkc8w.iot-as-mqtt.cn-shanghai.aliyuncs.com",
 "passwd":"652b0dc2b786a08a7ab11612246731f87f50e32b78cc6bf9ed3a3a5041c7635e",
 "port":1883}
*/
static char task[]="[mqtt]";
extern uint8_t  NetRxBuffer[TX_RX_MAX_BUF_SIZE];
static char sendBuf[1024];//用于mqtt状态维护发送  不要破坏NetTxBuffer的数据
static char sendBufLen;
extern uint16_t netRxBufLen;
extern rt_sem_t      MQTTDiscon_semp;
extern rt_sem_t      wifiTcpOK_semp;
MQTTDataPacktStru  MQTTDataPackt[MQTT_PACK_CNT]={NULL};
char deviceName[]="temp_0714_001";					
MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;


static bool MQTTState=false;// true con   false nocon

#define  BROKER_RESP_TIMES    300

void offLineGetTimeStart(void);
int gu32PingFailTimes=0;
bool mqttState(void)
{
		return MQTTState;
}

//不够严谨 需要加上信号量保护
void mqttStateSet(bool state)
{
	  MQTTState=state;

}
//返回注册的topic编号
static int  mqttFindTopic(char *topic)
{
	    int i;
	    if(topic==NULL)
					return -1;
	    for( i=0;i<MQTT_PACK_CNT;i++){
						if(MQTTDataPackt[i].topic!=NULL){
							  if(strncmp(MQTTDataPackt[i].topic,topic,strlen(MQTTDataPackt[i].topic))==0){
										rt_kprintf("%stopic has regist %d\r\n",task,i);
										return i;
								}
						}
			}
			return -2;
}
//注册订阅的topic 以及回调函数
static void  mqttRegstTopicCallbak(char *topic,int (*fun)(char *buf))
{
			int i;
	    rt_kprintf("%smqttRegstTopic\r\n",task);
	    if(topic==0)
					return;
			if(mqttFindTopic(topic)>=0){//过滤掉已经注册过的
				  rt_kprintf("%stopic have been regist\r\n",task);
					return;
			}
	    for( i=0;i<MQTT_PACK_CNT;i++){
						if(MQTTDataPackt[i].topic==NULL){
								MQTTDataPackt[i].topic=rt_malloc(strlen(topic)+1);
								MQTTDataPackt[i].fun=fun;
							  if(fun!=NULL){
										rt_kprintf("%sregist callback succ\r\n",task);
								}
								strcpy(MQTTDataPackt[i].topic,topic);
							  rt_kprintf("%stopic  regist %d len=%d\r\n",task,i,strlen(topic));
								break;
						}
			}
}
//清除掉topic释放内存
static void  mqttCleanTopic()
{
	    int i;
	    rt_kprintf("%smqttCleanTopic begin\r\n",task);
	    for( i=0;i<MQTT_PACK_CNT;i++){
						if(MQTTDataPackt[i].topic!=NULL){
								rt_kprintf("%sfree %d st topic\r\n",task, i);
								rt_free(MQTTDataPackt[i].topic);
							  MQTTDataPackt[i].topic=NULL;
						}
			}
			rt_kprintf("%smqttCleanTopic end\n\r",task);
}




//解析设置的参数并显示  /sys/a1X8eraA4gE/%s/thing/service/property/set
//{"method":"thing.service.property.set","id":"1475268842","params":{"CurrentHumidity":80.8,"CurrentTemperature":81.8},"version":"1.0.0"}
static int getSetJsoin(char *buf)
{
		rt_kprintf("%sgetsetJson\n\r",task);				
		cJSON  *Json=NULL;
		Json = cJSON_Parse(buf);
		cJSON  *params   =cJSON_GetObjectItem(Json,"params");
		char *out =cJSON_Print(params);
		  if(out!=NULL){
				rt_free(out);
		}
		rt_kprintf("%sJson %s\n\r",task,out);
		double  humi     = cJSON_GetObjectItem(params,"CurrentHumidity")->valuedouble;
		double  temp     = cJSON_GetObjectItem(params,"CurrentTemperature")->valuedouble;
	  if(Json!=RT_NULL){
				cJSON_Delete(Json);
		}
		rt_kprintf("%ssethumi[%lf] settemp[%lf]\n\r",task,humi,temp);	
		return 1;
}




uint32_t e;
extern struct rt_event mqttAckEvent;

//注册了回调函数
int  dataPhrase(char *json)
{
	rt_mutex_take(read485_mutex,RT_WAITING_FOREVER);
	extern void AllDownPhraseP(char *data);
	AllDownPhraseP(json);
	rt_mutex_release(read485_mutex);
	return 1;
}
//int mqttRet=-1;
//mqtt来实现订阅功能
static void mqttSubFun()
{
	/* subscribe */
		MQTTString topicString = MQTTString_initializer;
		int req_qos = 0;
   // int i=0;
		char *topic=rt_malloc(100);

		rt_sprintf(topic,"/acu/%s/down",packFlash.acuId);
		mqttRegstTopicCallbak(topic,dataPhrase);
//	  rt_sprintf(topic,"/sys/a1X8eraA4gE/%s/thing/service/property/set",deviceName);
//	  mqttRegstTopicCallbak(topic,getSetJsoin);
//		rt_sprintf(topic,"/ext/ntp/a1X8eraA4gE/%s/response",deviceName);
//	  mqttRegstTopicCallbak(topic,NULL);//注册 topic
	  rt_free(topic);
	  topic=NULL;
				
	  for(int i=0;i<MQTT_PACK_CNT;i++){
			  if(MQTTDataPackt[i].topic!=NULL){
						topicString.cstring = MQTTDataPackt[i].topic;
					  rt_kprintf("%smqttSub %s  %s\n\r",task,topicString.cstring,MQTTDataPackt[i].topic);

						rt_kprintf("%smqttSub %d topic\n\r",task,i);
						sendBufLen = MQTTSerialize_subscribe((unsigned char*)sendBuf+PACK_HEAD_LEN, sizeof(sendBuf), 0, 0, 1, &topicString, &req_qos);
         
						sendBuf[0]=(uint8_t)(sendBufLen>>24);
						sendBuf[1]=(uint8_t)(sendBufLen>>16);
						sendBuf[2]=(uint8_t)(sendBufLen>>8);
						sendBuf[3]=(uint8_t)sendBufLen;
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&sendBuf,RT_WAITING_FOREVER); 
					  if (rt_event_recv(&mqttAckEvent, (EVENT_SUBACK),
                       RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,
                      BROKER_RESP_TIMES, &e) == RT_EOK)
						{
								//rt_kprintf("thread1: OR recv event 0x%x\n", e);
							  rt_kprintf("%smqttSub ack\r\n",task);
						}
				}
		}
}



//mqtt收到订阅的消息过来
bool  mqttpubRead()
{

					unsigned char dup;
					int qos;
					unsigned char retained;
					unsigned short msgid;
					int payloadlen_in;
					unsigned char* payload_in;

					MQTTString receivedTopic;
					int rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
							&payload_in, &payloadlen_in, (uint8_t *)NetRxBuffer,  netRxBufLen);
					rt_kprintf("%srec topic: %.*s [%d]\r\n",task,receivedTopic.lenstring.len,receivedTopic.lenstring.data,receivedTopic.lenstring.len);
					int topicNum = mqttFindTopic(receivedTopic.lenstring.data);
				  if(topicNum>=0){
						  rt_kprintf("%stopicNum =%d  payloadlen=%d\r\n",task,topicNum,payloadlen_in);
							MQTTDataPackt[topicNum].msglen  = payloadlen_in;
							MQTTDataPackt[topicNum].msg			= rt_malloc(payloadlen_in+1);//必须加1   是字符串
							//strcpy(MQTTDataPackt[topicNum].msg,(char *)payload_in);//此句可能导致(MEM_POOL(&small_mem->heap_ptr[mem->next]) == small_mem) assertion failed at function:rt_smem_free, line number:542 
							strncpy(MQTTDataPackt[topicNum].msg,(char *)payload_in,payloadlen_in);
							MQTTDataPackt[topicNum].msg[payloadlen_in]=0;//此句可能导致
						  rt_kprintf("%stopicNum %d\r\n",task,topicNum);	
						  if(MQTTDataPackt[topicNum].fun!=NULL){
									MQTTDataPackt[topicNum].fun(MQTTDataPackt[topicNum].msg);//执行回调函数
							}
							else{
									rt_kprintf("%stopic fun is NULL\r\n",task);
							}
							rt_kprintf("%srec message: %.*s \r\n",task, MQTTDataPackt[topicNum].msglen, MQTTDataPackt[topicNum].msg);//
						
						  rt_free(MQTTDataPackt[topicNum].msg);

						  MQTTDataPackt[topicNum].msg=NULL;
						  MQTTDataPackt[topicNum].msglen=0;

					}
					else
							rt_kprintf("%snot find topic message arrived %.*s   topic:%s  %.*s [%d]\r\n",task, payloadlen_in, payload_in,receivedTopic.cstring,receivedTopic.lenstring.len,receivedTopic.lenstring.data,receivedTopic.lenstring.len);

			return false;
}


static MQTTEnum MQTTstep=conMQTT_enum; 

static uint32_t respTime ;
void getMqttRespTime()
{
	respTime = rt_tick_get();
}

extern rt_bool_t gbNetState;
//mqtt状态的维持 数据收发
int ping2Times=0;
int pingRspTimes=0;
int pingUnrspTimes=0;
//bool  mqttConStat=false;
int  mqttLoopData(void)
{

	  static int ret=0;
	  static int conTimes=0;
	  static uint8_t pingTimes=0;
	  int  tep=0;

		switch(MQTTstep){
			case  conMQTT_enum:
				    //设备  humi_protect1

#ifdef USE_ALIYUN
			 //阿里云ip 47.103.184.125 port 1883
						connectData.clientID.lenstring.data	=	"a1S9kgxkc8w.humi_protect1|securemode=2,signmethod=hmacsha256,timestamp=1656924907170|";
						connectData.clientID.lenstring.len	=	strlen(connectData.clientID.lenstring.data);
						connectData.username.lenstring.data	=	"humi_protect1&a1S9kgxkc8w";
						connectData.username.lenstring.len	=	strlen(connectData.username.lenstring.data);
						connectData.password.lenstring.data	=	"d7810ceaaa1370606f58d64d443182af6de70c6daff8297b34bf08081e0fdde2";
						connectData.password.lenstring.len	=	strlen(connectData.password.lenstring.data);
			      //设备 temp_test001
#else  //花生壳 内网穿透  115.236.153.174  15260
			
			      memcpy(connectData.clientID.lenstring.data,packFlash.acuId,strlen(packFlash.acuId));
						connectData.clientID.lenstring.data	=	"fa19d97c0364436bb166e92979ce5408suqi";//后期采用格式化物理地址来填充  防止出现多个clientid相同情况 error
						connectData.clientID.lenstring.len	=	strlen(connectData.clientID.lenstring.data);
						connectData.username.lenstring.data	=	"";
						connectData.username.lenstring.len	=	strlen(connectData.username.lenstring.data);
						connectData.password.lenstring.data	=	"";
						connectData.password.lenstring.len	=	strlen(connectData.password.lenstring.data);
#endif
						connectData.keepAliveInterval = MQTTKEEPALIVE_TIME_S;
						if((sendBufLen=MQTTSerialize_connect((unsigned char *)sendBuf+PACK_HEAD_LEN, sizeof(sendBuf), &connectData))!=0){
								rt_kprintf("%stry to con [%d]\r\n",task,sendBufLen);
								sendBuf[0]=(uint8_t)(sendBufLen>>24);
								sendBuf[1]=(uint8_t)(sendBufLen>>16);
								sendBuf[2]=(uint8_t)(sendBufLen>>8);
								sendBuf[3]=(uint8_t)sendBufLen;
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&sendBuf,RT_WAITING_FOREVER); 
						}
						if (rt_event_recv(&mqttAckEvent,EVENT_CONNACK,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR, BROKER_RESP_TIMES, &e) == RT_EOK)
						{//qos=0 
								rt_kprintf("%sPUB reading CONNACK\r\n",task);//返回3 7 6 6的包
							  MQTTstep	=	subMQTT_enum;
							  mqttStateSet(true);
							  conTimes=0;
								extern void LCDDispNetOffline();
								LCDDispNetOffline();
						}
						else{

							  rt_kprintf("%sno connack\r\n",task);//返回3 7 6 6的包
							  if(++conTimes>=3){//  dis conn 后  con了3次才连上
										MQTTstep	=	resetMQTT_enum;
										tep= -1;
									  conTimes=0;
								}
								rt_thread_delay(1000);
						}
				break;
			case 	subMQTT_enum:
						mqttSubFun();
			      MQTTstep	=	pubGetUtcMqtt_enum;
				break;
			case pubGetUtcMqtt_enum:
						MQTTstep=pubMQTT_enum;
			case  pubMQTT_enum:
						MQTTstep	=	dealwithMQTT_enum;
			case  pingMQTT_enum:
						ping2Times++;
				    rt_kprintf("%sping \r\n",task);
						sendBufLen=MQTTSerialize_pingreq((uint8_t *)sendBuf+PACK_HEAD_LEN, sizeof(sendBuf));			
						rt_kprintf("%stry to ping [%d]\r\n",task,sendBufLen);	
						sendBuf[0]=(uint8_t)(sendBufLen>>24);
						sendBuf[1]=(uint8_t)(sendBufLen>>16);
						sendBuf[2]=(uint8_t)(sendBufLen>>8);
						sendBuf[3]=(uint8_t)sendBufLen;
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&sendBuf,RT_WAITING_FOREVER); 			
						if (rt_event_recv(&mqttAckEvent, (EVENT_PINGRESP),
                       RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,
                      BROKER_RESP_TIMES, &e) == RT_EOK){
								pingRspTimes++;
								rt_kprintf("%sPong\r\n",task);
							  pingTimes=0;
								MQTTstep=dealwithMQTT_enum;
								getMqttRespTime();
//							  respTime=rt_tick_get();
						}
						else {
								pingUnrspTimes++;
								rt_kprintf("%sOOPS %d\r\n",task,ret);
								gu32PingFailTimes++;
							  if(pingTimes++>=2){//先判断后++
									  MQTTstep	=	resetMQTT_enum;
										mqttStateSet(false);
										if(gbNetState==RT_TRUE){
												gbNetState = RT_FALSE;
												offLine.mqttTimes++;
										}
								}
						}
						rt_kprintf("ping[%d],pingrsp[%d],pingUnrsp[%d]\n",ping2Times,pingRspTimes,pingUnrspTimes);
				break;
			case  dealwithMQTT_enum://
						if((rt_tick_get()-respTime)>(MQTTKEEPALIVE_TIME_S)*1000){
								MQTTstep=pingMQTT_enum;
						}
				break;
			case  resetMQTT_enum:
				    mqttCleanTopic();
						rt_kprintf("%sresetMQTT_enum 1\r\n",task);		
				    sendBufLen = (unsigned int)MQTTSerialize_disconnect((unsigned char *)(sendBuf+PACK_HEAD_LEN),sizeof(sendBuf));
						rt_kprintf("%sresetMQTT_enum 2\r\n",task);	
						if(sendBufLen !=0){
								sendBuf[0]=(uint8_t)(sendBufLen>>24);
								sendBuf[1]=(uint8_t)(sendBufLen>>16);
								sendBuf[2]=(uint8_t)(sendBufLen>>8);
								sendBuf[3]=(uint8_t)sendBufLen;
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&sendBuf,RT_WAITING_FOREVER); 
							  rt_kprintf("%sdis conn\r\n",task);	
						}
						else
							  rt_kprintf("%sdis conn err\r\n");
						MQTTstep=conMQTT_enum;
//						rt_event_send(&WDTEvent,EVENT_WDT_MQTTTASK);
						mqttStateSet(false);;
						rt_thread_delay(1000);
				break;
		}
		return tep;
}	

void rstMqttStep()
{
		if(mqttState()==true){
			offLine.mqttTimes++;
			mqttStateSet(false);
		}
	  MQTTstep=resetMQTT_enum;
}


extern struct rt_mutex  test_mutex;
void  mqttTask(void *parameter)
{

			while(1){
				  if(gbNetState==RT_TRUE)
						  mqttLoopData();
#ifdef  USE_WDT
					rt_event_send(&WDTEvent,EVENT_WDT_MQTTTASK);
#endif
					rt_thread_delay(1000);
			}
}








#endif

