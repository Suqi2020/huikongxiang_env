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
static char sendBuf[1024];//����mqtt״̬ά������  ��Ҫ�ƻ�packBuf������
//static char recBuf[1024];
static char sendBufLen;
extern uint16_t netRxBufLen;
//static char recBufLen;
extern rt_sem_t      MQTTDiscon_semp;
extern rt_sem_t      wifiTcpOK_semp;
//extern rt_mutex_t 		MQTTState_mutex ;
MQTTDataPacktStru  MQTTDataPackt[MQTT_PACK_CNT]={NULL};
extern rt_mutex_t netRec_mutex;
char deviceName[]="temp_0714_001";
//static  char payload[]="{{\"sys\":{\"ack\":1},\"params\":{\"CurrentHumidity\":{\"value\":44},\"CurrentTemperature\":{\"value\":44.4}},\"method\":\"thing.event.property.post\"}";
					
MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;


static bool MQTTState=false;// true con   false nocon

#define  BROKER_RESP_TIMES    300

void offLineGetTimeStart(void);
int gu32OffLineTimes=0;
int gu32PingFailTimes=0;
bool mqttState(void)
{
		return MQTTState;
}

//�����Ͻ� ��Ҫ�����ź�������
void mqttStateSet(bool state)
{
	  MQTTState=state;

}
//����ע���topic���
static int  mqttFindTopic(char *topic)
{
	    int i;
	    if(topic==NULL)
					return -1;
	    for( i=0;i<MQTT_PACK_CNT;i++){
						if(MQTTDataPackt[i].topic!=NULL){
							  //rt_kprintf("%s%s  %s\r\n",MQTTDataPackt[i].topic,topic);
							  if(strncmp(MQTTDataPackt[i].topic,topic,strlen(MQTTDataPackt[i].topic))==0){
										rt_kprintf("%stopic has regist %d\r\n",task,i);
										return i;
								}
						}
			}
			return -2;
}
//ע�ᶩ�ĵ�topic �Լ��ص�����
static void  mqttRegstTopicCallbak(char *topic,int (*fun)(char *buf))
{
			int i;
	    rt_kprintf("%smqttRegstTopic\r\n",task);
	    if(topic==0)
					return;
			if(mqttFindTopic(topic)>=0){//���˵��Ѿ�ע�����
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
//�����topic�ͷ��ڴ�
static void  mqttCleanTopic()
{
	    int i;
	    rt_kprintf("%smqttCleanTopic begin\r\n",task);
	    for( i=0;i<MQTT_PACK_CNT;i++){
						if(MQTTDataPackt[i].topic!=NULL){
								rt_kprintf("%sfree %d st topic\r\n",task, i);
								rt_free(MQTTDataPackt[i].topic);

							  //rt_kprintf("%sfree %d r\n", rt_freetimes);
							  MQTTDataPackt[i].topic=NULL;
						}
			}
			rt_kprintf("%smqttCleanTopic end\n\r",task);
}




//�������õĲ�������ʾ  /sys/a1X8eraA4gE/%s/thing/service/property/set
//{"method":"thing.service.property.set","id":"1475268842","params":{"CurrentHumidity":80.8,"CurrentTemperature":81.8},"version":"1.0.0"}
static int getSetJsoin(char *buf)
{
	  //char send[17];

	
		rt_kprintf("%sgetsetJson\n\r",task);				
		cJSON  *Json=NULL;
		Json = cJSON_Parse(buf);
//		char *out =cJSON_Print(Json);
//		rt_kprintf("%sJson %s\n\r",out);

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
				//rt_kprintf("sethumi[%lf] settemp[%lf]\n\r",humi,temp);	
		return 1;
}


//����mqtt���ص���Ϣͷ������
int  netPhraseHead()
{
	  char headBuf[10];	//�����ֲ��֪ʣ�೤�ȴӵڶ���ֱ�ӿ�ʼ����ֶ� 4���ֽ�  5���ֽ�buf�㹻
	   rt_memcpy(headBuf,NetRxBuffer,sizeof(headBuf));
	//�����ֲ��֪ʣ�೤�ȴӵڶ���ֱ�ӿ�ʼ����ֶ� 4���ֽ�
   
	    int ret =MQTTPacket_read((uint8_t *)headBuf, sizeof(headBuf), transport_getdata);//����ȷ��ͷ���Լ�ʣ���������
	    return ret;
}






int  dataPhrase(char *json)
{
	extern void AllDownPhraseP(char *data);
	AllDownPhraseP(json);
	return 1;
}
//int mqttRet=-1;
//mqtt��ʵ�ֶ��Ĺ���
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
//	  mqttRegstTopicCallbak(topic,NULL);//ע�� topic
	  rt_free(topic);
	  topic=NULL;
				
	  for(int i=0;i<MQTT_PACK_CNT;i++){
			  if(MQTTDataPackt[i].topic!=NULL){
						rt_thread_delay(BROKER_RESP_TIMES);
						topicString.cstring = MQTTDataPackt[i].topic;
					  rt_kprintf("%smqttSub %s  %s\n\r",task,topicString.cstring,MQTTDataPackt[i].topic);
					
					
				
						rt_kprintf("%smqttSub %d topic\n\r",task,i);
						sendBufLen = MQTTSerialize_subscribe((unsigned char*)sendBuf, sizeof(sendBuf), 0, 0, 1, &topicString, &req_qos);
//						wifiUartSend( (uint8_t *)sendBuf,sendBufLen);
//					  for(int k=0;k<sendBufLen;k++)
//								packBuf[k]=sendBuf[k];
					  transport_sendPacketBuffer((unsigned char*)sendBuf,sendBufLen);
					  rt_thread_delay(BROKER_RESP_TIMES);//dalay for ack
						rt_mutex_take(netRec_mutex,RT_WAITING_FOREVER);
						if (netPhraseHead() == SUBACK) 	/* wait for suback */
						{
								unsigned short submsgid;
								int subcount;
								int granted_qos;
							  rt_kprintf("%smqttSub ack\r\n",task);
								MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos,(uint8_t *)NetRxBuffer, netRxBufLen);
								if (granted_qos != 0)
								{
										rt_kprintf("%sERROR:granted qos != 0, 0x%02x\r\n", task,granted_qos);
								}
						}
						memset(NetRxBuffer,0,sizeof(NetRxBuffer));
						rt_mutex_release(netRec_mutex);
				}
//				else{
//						rt_kprintf("%stopic is NULL %d \n\r",i);
//				}
		}
//		mqttRet=-1;

}



//mqtt�յ����ĵ���Ϣ����
bool  mqttpubRead()
{

			rt_mutex_take(netRec_mutex,RT_WAITING_FOREVER);
			if (netPhraseHead()== PUBLISH)
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
						  rt_kprintf("%stopicNum >=0\r\n",task);
							MQTTDataPackt[topicNum].msglen  = payloadlen_in;
							MQTTDataPackt[topicNum].msg			= rt_malloc(payloadlen_in+1);//�����1   ���ַ���

							strcpy(MQTTDataPackt[topicNum].msg,(char *)payload_in);
						  rt_kprintf("%stopicNum %d\r\n",task,topicNum);	
						
						  if(MQTTDataPackt[topicNum].fun!=NULL){
								  
									MQTTDataPackt[topicNum].fun(MQTTDataPackt[topicNum].msg);//ִ�лص�����
							}
							else{
									rt_kprintf("%stopic fun is NULL\r\n",task);
							}
							rt_kprintf("%srec message: %.*s \r\n",task, MQTTDataPackt[topicNum].msglen, MQTTDataPackt[topicNum].msg);
						
						  rt_free(MQTTDataPackt[topicNum].msg);

						  MQTTDataPackt[topicNum].msg=NULL;
						  MQTTDataPackt[topicNum].msglen=0;
							//return true;
					}
					else
							rt_kprintf("%snot find topic message arrived %.*s   topic:%s  %.*s [%d]\r\n",task, payloadlen_in, payload_in,receivedTopic.cstring,receivedTopic.lenstring.len,receivedTopic.lenstring.data,receivedTopic.lenstring.len);
					//return false;
			}
			rt_memset(NetRxBuffer,0,sizeof(NetRxBuffer));
			rt_mutex_release(netRec_mutex);
			return false;
}


static MQTTEnum MQTTstep=conMQTT_enum; 
//��λmqtt��step
//void rstMqttStep()
//{
//	 MQTTstep=resetMQTT_enum; 
//	 
//}
static uint32_t sendTime ;
void reFlashSendTime()
{
	sendTime = rt_tick_get();
}
//mqtt״̬��ά�� �����շ�
int ping2Times=0;
int pingRspTimes=0;
int pingUnrspTimes=0;
int  mqttLoopData(void)
{
  	

//		static MQTTString topic= MQTTString_initializer;
	  static int ret=0;
	  static int conTimes=0;
	  static uint8_t pingTimes=0;
	  int  tep=0;


//    char send[17];
//	  int getUTCTimes;
		switch(MQTTstep){
			case  conMQTT_enum:
				    //�豸  humi_protect1

#if USE_ALIYUN
			 //������ip 47.103.184.125 port 1883
						connectData.clientID.lenstring.data	=	"a1S9kgxkc8w.humi_protect1|securemode=2,signmethod=hmacsha256,timestamp=1656924907170|";
						connectData.clientID.lenstring.len	=	strlen(connectData.clientID.lenstring.data);
						connectData.username.lenstring.data	=	"humi_protect1&a1S9kgxkc8w";
						connectData.username.lenstring.len	=	strlen(connectData.username.lenstring.data);
						connectData.password.lenstring.data	=	"d7810ceaaa1370606f58d64d443182af6de70c6daff8297b34bf08081e0fdde2";
						connectData.password.lenstring.len	=	strlen(connectData.password.lenstring.data);
			      //�豸 temp_test001
#else  //������ ������͸  115.236.153.174  15260
			
			      //rt_memcpy(connectData.clientID.lenstring.data,packFlash.acuId,strlen(packFlash.acuId));
						connectData.clientID.lenstring.data	=	"fa19d97c0364436bb166e92979ce5408suqi";//���ڲ��ø�ʽ�������ַ�����  ��ֹ���ֶ��clientid��ͬ��� error
						connectData.clientID.lenstring.len	=	strlen(connectData.clientID.lenstring.data);
						connectData.username.lenstring.data	=	"";
						connectData.username.lenstring.len	=	strlen(connectData.username.lenstring.data);
						connectData.password.lenstring.data	=	"";
						connectData.password.lenstring.len	=	strlen(connectData.password.lenstring.data);
#endif
						connectData.keepAliveInterval = MQTTKEEPALIVE_TIME_S;
						if((sendBufLen=MQTTSerialize_connect((unsigned char *)sendBuf, sizeof(sendBuf), &connectData))!=0){
								rt_kprintf("%stry to con [%d]\r\n",task,sendBufLen);
								transport_sendPacketBuffer((unsigned char*)sendBuf,sendBufLen);
						}
						rt_thread_delay(500);//dalay for ack
						rt_mutex_take(netRec_mutex,RT_WAITING_FOREVER);
						if(netPhraseHead()==CONNACK){//qos=0 
								rt_kprintf("%sPUB reading CONNACK\r\n",task);//����3 7 6 6�İ�
							  MQTTstep	=	subMQTT_enum;
							  mqttStateSet(true);
							  conTimes=0;
						}
						else{
							  rt_kprintf("%sno connack\r\n",task);//����3 7 6 6�İ�
							  if(++conTimes>=3){//  dis conn ��  con��3�β�����
										MQTTstep	=	resetMQTT_enum;
										tep= -1;
									  conTimes=0;
								}

								rt_thread_delay(1000);
						}
						memset(NetRxBuffer,0,sizeof(NetRxBuffer));
						rt_mutex_release(netRec_mutex);
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
						sendBufLen=MQTTSerialize_pingreq((uint8_t *)sendBuf, sizeof(sendBuf));			
						rt_kprintf("%stry to ping [%d]\r\n",task,sendBufLen);			
					  transport_sendPacketBuffer((unsigned char*)sendBuf,sendBufLen);
						rt_thread_delay(BROKER_RESP_TIMES);
						//reFlashSendTime();
						rt_mutex_take(netRec_mutex,RT_WAITING_FOREVER);
						if(netPhraseHead()== PINGRESP){
							pingRspTimes++;
								rt_kprintf("%sPong\r\n",task);
		
							  pingTimes=0;
								MQTTstep=dealwithMQTT_enum;
							  
						}
						else {
							pingUnrspTimes++;
								rt_kprintf("%sOOPS %d\r\n",task,ret);
								gu32PingFailTimes++;
							  if(pingTimes++>=2){//���жϺ�++
									  MQTTstep	=	resetMQTT_enum;
									  gu32OffLineTimes++;
//										mqttStateSet(false);;
//										offLineGetTimeStart();
								}
								//rt_thread_delay(10000);
						}
						memset(NetRxBuffer,0,sizeof(NetRxBuffer));
						rt_mutex_release(netRec_mutex);
						rt_kprintf("ping[%d],pingrsp[%d],pingUnrsp[%d]\n",ping2Times,pingRspTimes,pingUnrspTimes);

				break;
			case  dealwithMQTT_enum://
						if((rt_tick_get()-sendTime)>(MQTTKEEPALIVE_TIME_S)*1000){
								MQTTstep=pingMQTT_enum;
						}
						mqttpubRead();
				break;
			case  resetMQTT_enum:
				    mqttCleanTopic();
						rt_kprintf("%sresetMQTT_enum 1\r\n",task);		
				    sendBufLen = (unsigned int)MQTTSerialize_disconnect((unsigned char *)(sendBuf),sizeof(sendBuf));
						rt_kprintf("%sresetMQTT_enum 2\r\n",task);	
						if(sendBufLen !=0){
								transport_sendPacketBuffer((unsigned char*)sendBuf,sendBufLen);
							  rt_kprintf("%sdis conn\r\n",task);	
						}
						else
							  rt_kprintf("%sdis conn err\r\n");
						MQTTstep=conMQTT_enum;
						
						mqttStateSet(false);;
//						offLineGetTimeStart();
						rt_thread_delay(1000);
				break;
		}
		//mqttRet=-1;
		return tep;
}	

void rstMqttStep()
{
		MQTTstep=resetMQTT_enum;
}

extern rt_bool_t gbNetState;
extern struct rt_mutex  test_mutex;
void  mqttTask(void *parameter)
{
	while(0){
					rt_mutex_take((rt_mutex_t)&test_mutex,0xfffff);
					rt_kprintf("test_mutex 002\n");
					rt_thread_delay(2000);
	}
			while(1){
				  if(gbNetState==RT_TRUE)
						  mqttLoopData();
					rt_thread_delay(1000);
			}
}





//���յ��������ڴ˺������д���  ��net��������ݺ����
//�ǵ�ÿ�δ�������buf
//extern uint32_t gu32RecDataLen;

//int  netPhraseRecData(uint8_t *Buf ,int len)
//{
////			rt_kprintf("rec1:");


//	   rt_memcpy(headBuf,NetRxBuffer,sizeof(headBuf));
//	//�����ֲ��֪ʣ�೤�ȴӵڶ���ֱ�ӿ�ʼ����ֶ� 4���ֽ�
//   
//	    int ret =MQTTPacket_read((uint8_t *)headBuf, sizeof(headBuf), transport_getdata);//����ȷ��ͷ���Լ�ʣ���������
//	
//	    switch(ret)
//			{
//				case SUBACK:
//					break;
//				case PUBLISH:
//				 {
//					unsigned char dup;
//					int qos;
//					unsigned char retained;
//					unsigned short msgid;
//					int payloadlen_in;
//					unsigned char* payload_in;

//					MQTTString receivedTopic;
//					int rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
//							&payload_in, &payloadlen_in, (uint8_t *)Buf,  len);
//					rt_kprintf("%srec topic: %.*s [%d]\r\n",task,receivedTopic.lenstring.len,receivedTopic.lenstring.data,receivedTopic.lenstring.len);
//					int topicNum = mqttFindTopic(receivedTopic.lenstring.data);
//				  if(topicNum>=0){
//						  rt_kprintf("%s,topicNum >=0\r\n",task);	
//							MQTTDataPackt[topicNum].msglen  = payloadlen_in;
//							MQTTDataPackt[topicNum].msg			= rt_malloc(payloadlen_in+1);//�����1   ���ַ���

//							strcpy(MQTTDataPackt[topicNum].msg,(char *)payload_in);
//						  rt_kprintf("%stopicNum %d\r\n",task,topicNum);	
//						
//						  if(MQTTDataPackt[topicNum].fun!=NULL){
//								  
//									MQTTDataPackt[topicNum].fun(MQTTDataPackt[topicNum].msg);//ִ�лص�����
//							}
//							else{
//									rt_kprintf("%stopic fun is NULL\r\n",task);	
//							}
//							rt_kprintf("%srec len=%d message: %.*s \r\n",task, MQTTDataPackt[topicNum].msglen,MQTTDataPackt[topicNum].msglen, MQTTDataPackt[topicNum].msg);
//						
//						  rt_free(MQTTDataPackt[topicNum].msg);

//						  MQTTDataPackt[topicNum].msg=NULL;
//						  MQTTDataPackt[topicNum].msglen=0;
//							return true;
//					}
//					else
//							rt_kprintf("%snot find topic message arrived %.*s   topic:%s  %.*s [%d]\r\n",task, payloadlen_in, payload_in,receivedTopic.cstring,receivedTopic.lenstring.len,receivedTopic.lenstring.data,receivedTopic.lenstring.len);
//				 }
//					break;
//				case CONNACK:
//					{
//					extern void offLineGettimesEnd();
//				
//					offLineGettimesEnd();
//						//rt_kprintf("%sPUB reading CONNACK\r\n",task);//����3 7 6 6�İ�
//					}
//					break;
//				case PINGRESP:
//						//rt_kprintf("%sPong\r\n",task);
//					break;
//				default:
//					rt_kprintf("%s error:netPhraseRecData [%d]\n",task,ret);
//				  break;
//				
//			}
//			return ret;
//}




#endif

