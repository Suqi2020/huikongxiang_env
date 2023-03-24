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
const static char sign[]="[dataPhrs]";
uint32_t  respMid=0;
//数据校验 头尾 校验和 是否正确
//rt_TRUE 正确 rt_FALSE 错误
rt_bool_t dataCheck(char *data,int lenth)
{
//	1、解析头尾校验 不对丢弃
//	2、提取packettype,分别校验
	  if(lenth<=8)
				return RT_FALSE; //头尾校验至少9个字节
		uint16_t jsonBodyCrc=RTU_CRC((uint8_t *)data+HEAD_LEN+LENTH_LEN,lenth-HEAD_LEN-LENTH_LEN-TAIL_LEN-CRC_LEN);
	  uint16_t dataCrc=(uint16_t)(data[lenth-4]<<8)+data[lenth-3];
	  if(((data[0]<<8)+data[1])!=HEAD){
				rt_kprintf("%shead err  %02x %02x\r\n",sign,data[0],data[1]);
				return RT_FALSE;		
		}
		if(((data[lenth-2]<<8)+data[lenth-1])!=TAIL){
				rt_kprintf("%stail err\r\n",sign);
				return RT_FALSE;		
		}
	  if(lenth!=((data[2]<<8)+data[3]+HEAD_LEN+LENTH_LEN+TAIL_LEN+CRC_LEN)){
				rt_kprintf("%slenth err %d %d\r\n",sign,lenth,((data[2]<<8)+data[3]+HEAD_LEN+LENTH_LEN+TAIL_LEN+CRC_LEN));
				return RT_FALSE;		
		}
	  if(jsonBodyCrc!=dataCrc){
			  rt_kprintf("%scrc err r:0x%04x c:0x%04x\r\n",sign,dataCrc,jsonBodyCrc);
				return RT_FALSE;
		}
		return RT_TRUE;
}
//分别找出下行数据的类型并分类    
packTypeEnum  downLinkPackTpyeGet(cJSON  *TYPE)
{
	  int size =sizeof(typeHeadDown)/sizeof(typeHeadDown[0]);
	
	  for(int i=0;i<size;i++){
				if(rt_strcmp(TYPE->valuestring,typeHeadDown[i])==0){
						return i;
				}
		}
		rt_kprintf("%serr:type head [%s]\n",TYPE->valuestring);	
		return ERRRESP;
}


//需要判断devid 和消息ID一致才认为心跳发送成功
rt_bool_t heartRespFun(cJSON  *Json)
{

		cJSON  *time =cJSON_GetObjectItem(Json,"timestamp");
	  rt_kprintf("%stime:%s\n\r",sign,time->valuestring);

	
		cJSON  *msg =cJSON_GetObjectItem(Json,"msg");
		rt_kprintf("%sheart msg %s\r\n",sign,msg->valuestring);
			
			
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
void AllDownPhrase(char *data,int lenth)
{
		rt_kprintf("%sphrase len:%d\r\n",sign,lenth);
	  if(dataCheck(data,lenth)==RT_FALSE){
				return;
		}
		char *buf=data+HEAD_LEN+LENTH_LEN;//偏移后是真实的json数据
		int  len=lenth-HEAD_LEN-LENTH_LEN-TAIL_LEN-CRC_LEN;//获取真实的json数据长度
		
//		rt_kprintf("Jsonlen: %d\r\n",len);
		
		
		char *Buffer=(char *)rt_malloc(len+1);
		rt_strncpy(Buffer,buf,len);
    Buffer[len]=0;
		
		
//		for(int i=0;i<len;i++)
//		rt_kprintf("%c",Buffer[i]);
//		rt_kprintf("\r\n");

		//开始解析json
		rt_kprintf("%sgetJson:%s  \r\n",sign,Buffer);	
		//rt_kprintf("getJson:%.*s  %d\r\n",len,Buffer,len);			
		cJSON  *Json=NULL;
		Json = cJSON_Parse(Buffer);
		
		
		if(Json!=RT_NULL){//解析json数据
				cJSON  *pkType = cJSON_GetObjectItem(Json,"packetType");
			  cJSON  *pkIdentf = cJSON_GetObjectItem(Json,"identifier");
			
				cJSON  *mid =cJSON_GetObjectItem(Json,"mid");
			  respMid = mid->valueint;
		 
			  switch(downLinkPackTpyeGet(pkType)){

					case	PROPERTIES_HEART_RESP:
						if(RT_TRUE==heartRespFun(Json)){//收到心跳回应 怎么通知发送层
								rt_kprintf("%srec heart resp\r\n",sign);
						}
						break;
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
					  readAnaDataResp(pkIdentf->valuestring);
						break;
					case	PROPERTIES_485TIM_GET:
						senseTimeReadJsonResp(pkIdentf->valuestring,true);
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
						break;
					case	PROPERTIES_485TIM_SET:
						senseTimeJsonSet(Json,true);
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
						break;
					case	PROPERTIES_ANATIM_GET:
						senseTimeReadJsonResp(pkIdentf->valuestring,false);
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
						break;
					case	PROPERTIES_ANATIM_SET:
						senseTimeJsonSet(Json,false);
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
						break;
					case	PROPERTIES_485TH_GET:
						senseTHGetJsonResp(Json,true);
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
						break;
					case	PROPERTIES_485TH_SET:
						senseTHSetJsonResp(Json,true);
					  rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
						break;
					case	PROPERTIES_ANATH_GET:
						senseTHGetJsonResp(Json,false);
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
						break;
					case	PROPERTIES_ANATH_SET:
						senseTHSetJsonResp(Json,false);
					  rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
						break;
					case	PROPERTIES_INPUT_REP_RESP:
						break;
					case	PROPERTIES_INPUT_GET:
						break;
					case	PROPERTIES_OUTPUT_REP_RESP:
						break;
					case	PROPERTIES_OUTPUT_GET:
						break;
					case	PROPERTIES_OUTPUT_SET:
						break;
					case	SERVICES_CTRLCFG_READ:
						break;
					case	SERVICES_CTRLCFG_ADD:
						break;
					case	SERVICES_CTRLCFG_DEL:
						break;
					case	SERVICES_DEV_REBOOT:
						break;
					case	SERVICES_ACU_REBOOT:
						break;
					case	SERVICES_HISTORY_READ:
						break;
					case	SERVICES_OTA_WRITE:
						break;
					case	SERVICES_SAVE:
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
			rt_kprintf("%serr:json empty\r\n",sign);	
		}
		cJSON_Delete(Json);
		rt_free(Buffer);
	  Buffer =RT_NULL;
		
}
