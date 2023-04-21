#include   "dataPhrase.h"
#include   "cJSON.h"
//https://blog.csdn.net/woody218/article/details/119634171  json解析参考
extern uint16_t RTU_CRC(uint8_t *puchMsg , uint16_t usDataLen);
void  readModbusDataResp(char *monitor);
void  readAnaDataResp(char *monitor);
void  senseTHGetJsonResp(cJSON   *Json,bool modbusFlag);
void senseTimeReadJsonResp(char *string,bool  modbusFlag);
void senseTHSetJsonResp(cJSON   *Json,bool  modbusFlag);
void senseTimeJsonSet(cJSON   *Json,bool  modbusFlag);
extern uint16_t digitalInputGetResp(cJSON *Json);
uint16_t digitalOutputSetResp(cJSON *Json,char *identify);
uint16_t digitalInputGetResp(cJSON *Json);
uint16_t digitalOutputGetResp(cJSON *Json,char *identify);
uint16_t resetDeviceResp(cJSON *Json,char *identify);
uint16_t resetMcuResp(cJSON *Json);
uint16_t saveMcuResp(void);
uint16_t logCrtlReadResp(cJSON *Json);
uint16_t logCrtlAddResp(cJSON *Json);
uint16_t logCtrlDel(cJSON *Json);
extern void packMqttSend(void);
const static char sign[]="[dataPhrs]";
uint32_t  respMid=0;

//分别找出下行数据的类型并分类    
packTypeEnum  downLinkPackTpyeGet(cJSON  *TYPE)
{
	  int size =sizeof(typeHeadDown)/sizeof(typeHeadDown[0]);
	
	  for(int i=0;i<size;i++){
				if(rt_strcmp(TYPE->valuestring,typeHeadDown[i])==0){
						return (packTypeEnum)i;
				}
		}
		rt_kprintf("%serr:type head [%s] listsize=%d\n",sign,TYPE->valuestring,size);	
		return ERRRESP;
}



//RTC时钟校时

rt_bool_t timeSetFun(cJSON  *Json)
{

		cJSON  *time =cJSON_GetObjectItem(Json,"timestamp");
	  rt_kprintf("%stime:%s\n\r",sign,time->valuestring);


			
		static uint64_t u64getTick_p;

		u64getTick_p =atoll(time->valuestring);
		rt_kprintf("%stime:[%lu]s \r\n",sign, (uint32_t)((u64getTick_p)/1000));

		rt_kprintf("%stime:[%lu]ms\r\n",sign, (uint32_t)(u64getTick_p)%1000);
	  extern void  subTimeStampSet(uint64_t time);
	  if(utcTime()-u64getTick_p>=3000){
        subTimeStampSet(u64getTick_p);
			  rt_kprintf("%stime:RTC 误差大于3秒 校时\r\n",sign);
		}
	
		cJSON  *mid =cJSON_GetObjectItem(Json,"mid");
    if(mcu.upHeartMessID != mid->valueint){
				rt_kprintf("%sheart resp messID err %d %d\r\n",sign,mcu.upHeartMessID,mid->valueint);
			  return RT_FALSE;
			
		}
		cJSON  *code =cJSON_GetObjectItem(Json,"code");
		if(code->valueint!=0){
			  rt_kprintf("%sheart code err %d\r\n",sign,code->valueint);
				return RT_FALSE;
		}

		cJSON  *devid =cJSON_GetObjectItem(Json,"acuId");
		if(strcmp(packFlash.acuId,devid->valuestring)!=0){
				rt_kprintf("%sheart resp acuId err %s\r\n",sign,devid->valuestring);
			  return RT_FALSE;
		}

		return RT_TRUE;
}


//需要判断devid 和消息ID一致才认为注册成功
rt_bool_t comRespFun(cJSON  *Json,uint32_t mesgID)
{

		cJSON  *msg =cJSON_GetObjectItem(Json,"msg");
		rt_kprintf("%sheart msg %s\r\n",sign,msg->valuestring);
		timeSetFun(Json);

		cJSON  *mid =cJSON_GetObjectItem(Json,"mid");
    if(mesgID!= mid->valueint){
				rt_kprintf("%sreg resp messID err %d %d\r\n",sign,mid->valueint,mesgID);
			  return RT_FALSE;
			
		}
		cJSON  *code =cJSON_GetObjectItem(Json,"code");
		rt_kprintf("%sreg code  %d\r\n",sign,code->valueint);
		if(code->valueint==1){
			  rt_kprintf("%sreg code err\r\n",sign);
				return RT_FALSE;
		}

		return RT_TRUE;
}





//下行数据解析
void AllDownPhraseP(char *data)
{

		char *Buffer=data;		
		cJSON  *Json=NULL;
		Json = cJSON_Parse(Buffer);

		if(Json!=RT_NULL){//解析json数据
				cJSON  *pkType = cJSON_GetObjectItem(Json,"packetType");
			  cJSON  *pkIdentf = cJSON_GetObjectItem(Json,"identifier");
			
				cJSON  *mid =cJSON_GetObjectItem(Json,"mid");
			  respMid = mid->valueint;
		 
			  switch(downLinkPackTpyeGet(pkType)){

					case	PROPERTIES_REG_RESP:
						if(RT_TRUE==comRespFun(Json,mcu.devRegMessID)){//收到注册回应 怎么通知发送层
								rt_kprintf("%sreg dev succ\r\n",sign);
							  extern rt_bool_t gbRegFlag;
							  gbRegFlag = RT_TRUE;
						}
						break;
					case	PROPERTIES_485DATA_REP_RESP:
						if(RT_TRUE==comRespFun(Json,mcu.repDataMessID)){//收到数据包回应 怎么通知发送层
								rt_kprintf("%sreport data succ\r\n",sign);
						}
						break;
					case	PROPERTIES_485DATA_GET:
					  readModbusDataResp(pkIdentf->valuestring);
						break;

					case	PROPERTIES_ANADATA_REP_RESP:
						break;
					case	PROPERTIES_ANADATA_GET:
#ifndef     ANA_MASK
					  readAnaDataResp(pkIdentf->valuestring);
#endif
						break;
					case	PROPERTIES_485TIM_GET:
						senseTimeReadJsonResp(pkIdentf->valuestring,true);
					  packMqttSend();
						break;
					case	PROPERTIES_485TIM_SET:
						senseTimeJsonSet(Json,true);
						packMqttSend(); 
						break;
					case	PROPERTIES_ANATIM_GET:
						senseTimeReadJsonResp(pkIdentf->valuestring,false);
						packMqttSend(); 
						break;
					case	PROPERTIES_ANATIM_SET:
						senseTimeJsonSet(Json,false);
						packMqttSend(); 
						break;
					case	PROPERTIES_485TH_GET:
						senseTHGetJsonResp(Json,true);
						packMqttSend(); 
						break;
					case	PROPERTIES_485TH_SET:
						senseTHSetJsonResp(Json,true);
					  packMqttSend(); 
						break;
					case	PROPERTIES_ANATH_GET:
						senseTHGetJsonResp(Json,false);
						packMqttSend(); 
						break;
					case	PROPERTIES_ANATH_SET:
						senseTHSetJsonResp(Json,false);
					  packMqttSend(); 
						break;
					case	PROPERTIES_INPUT_REP_RESP:
						rt_kprintf("%sdi rep response\r\n",sign);
						break;
					case	PROPERTIES_INPUT_GET:
					  digitalInputGetResp(Json);
						packMqttSend(); 
						break;
					case	PROPERTIES_OUTPUT_REP_RESP:
						rt_kprintf("%sdo rep response\r\n",sign);
						break;
					case	PROPERTIES_OUTPUT_GET:
						
						digitalOutputGetResp(Json,pkIdentf->valuestring);
						packMqttSend(); 
						break;
					case	PROPERTIES_OUTPUT_SET:
					  digitalOutputSetResp(Json,pkIdentf->valuestring);
					  packMqttSend(); 
						break;
					case	SERVICES_CTRLCFG_READ:
						logCrtlReadResp(Json);
						packMqttSend(); 
						break;
					case	SERVICES_CTRLCFG_ADD:
						logCrtlAddResp(Json);
						packMqttSend(); 
						break;
					case	SERVICES_CTRLCFG_DEL:
					  logCtrlDel(Json);
					 	packMqttSend(); 
						break;
					case	SERVICES_DEV_REBOOT:
					  resetDeviceResp(Json,pkIdentf->valuestring);
						packMqttSend(); 
						break;
					case	SERVICES_ACU_REBOOT:
						resetMcuResp(Json);
						packMqttSend(); 
						rt_thread_delay(500);//1秒后重启
						rt_hw_cpu_reset();
	
						break;
					case	SERVICES_HISTORY_READ:
						break;
					case	SERVICES_OTA_WRITE:
						break;
					case	SERVICES_SAVE:
					  saveMcuResp();
//						stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFlash));//每次擦除128k字节数据 存储时候需要一起存储
//						stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFlash,sizeof(packFlash));
//						stm32_flash_write(FLASH_MODBUS_SAVE_ADDR,(uint8_t*)&sheet,sizeof(sheet));
					  packMqttSend(); 
						rt_kprintf("%sflash save success\n",sign);
						break;
					case	EVENTS_485_ALARM_RESP:
						break;
					case	EVENTS_ANA_ALARM_RESP:
						break;
					case  ERRRESP:
						break;
				}
		}
		else{
			rt_kprintf("%serr:json cannot phrase\r\n",sign);	
		}
		cJSON_Delete(Json);
}

//接收到的网络数据解析头部 判断类别
void netRecSendEvent()
{
	   extern uint8_t  NetRxBuffer[TX_RX_MAX_BUF_SIZE];
	   extern uint32_t netRxBufLen;
	   extern struct rt_event mqttAckEvent;
	   char headBuf[10]={0};	//根据手册得知剩余长度从第二个直接开始最大字段 4个字节  5个字节buf足够
	   memcpy(headBuf,NetRxBuffer,sizeof(headBuf));
	//根据手册得知剩余长度从第二个直接开始最大字段 4个字节
   
	   int ret =MQTTPacket_read((uint8_t *)headBuf, sizeof(headBuf), transport_getdata);//作用确定头部以及剩余最大数量
	   switch(ret)
		 {
			 case CONNACK:
				 rt_event_send(&mqttAckEvent, EVENT_CONNACK);
			   rt_kprintf("%seVent CONNACK\r\n",sign);
					break;
			 case SUBACK:{
					unsigned short submsgid;
					int subcount;
					int granted_qos;
				//	rt_kprintf("%smqttSub ack\r\n",sign);
					MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos,(uint8_t *)NetRxBuffer, netRxBufLen);
					if (granted_qos != 0)
					{
							rt_kprintf("%sERROR:granted qos != 0, 0x%02x\r\n", sign,granted_qos);
					}
				 rt_event_send(&mqttAckEvent, EVENT_SUBACK);
					rt_kprintf("%seVent SUBACK\r\n",sign);
				}
				 break;
			 case PINGRESP:
				 rt_event_send(&mqttAckEvent, EVENT_PINGRESP);
			   rt_kprintf("%sEVENT PINGRESP\r\n",sign);
				 break;
			 case PUBLISH:{
				 extern bool mqttpubRead();
				 rt_kprintf("%smqttPub ack begin\r\n",sign);
				 mqttpubRead();
				 extern void getMqttRespTime();
			   getMqttRespTime();
				 rt_kprintf("%smqttPub ack end\r\n",sign);
			  }
				 break;
			 default:
				 rt_kprintf("%sphrase mqtthead err %d\n",sign,ret);
				 break;
			 
		 }
		 
}



