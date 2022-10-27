#include   "dataPhrase.h"
#include   "cJSON.h"
//https://blog.csdn.net/woody218/article/details/119634171  json解析参考
extern uint16_t RTU_CRC(uint8_t *puchMsg , uint16_t usDataLen);
const static char sign[]="[dataPhrs]";
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
	
	
	  if(rt_strcmp(TYPE->valuestring,"CMD_HEARTBEA_RESPONSE")==0){
				return heartResp;
		}
		else if(rt_strcmp(TYPE->valuestring,"CMD_DEVICE_REGISTER_RESPONSE")==0){
				return devRegResp;
		}
		else if(rt_strcmp(TYPE->valuestring,"CMD_REPORTDATA_RESPONSE")==0){
				return repDataResp;
		}else if(rt_strcmp(TYPE->valuestring,"CMD_REQUESTDATA")==0){
				return CMDRepData;
		}
		else{
				rt_kprintf("%serr:packetType  %s %d\r\n",sign,TYPE->valuestring,strlen(TYPE->valuestring));
		}
		
		return errResp;
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
		subTimeStampSet(u64getTick_p);
	
		cJSON  *mid =cJSON_GetObjectItem(Json,"mid");
    if(mcu.upHeartMessID != mid->valueint){
				rt_kprintf("%sheart resp messID err %d\r\n",sign,mid->valueint);
			  return RT_FALSE;
			
		}
		cJSON  *code =cJSON_GetObjectItem(Json,"code");
		if(code->valueint!=0){
			  rt_kprintf("%sheart code err %d\r\n",sign,code->valueint);
				return RT_FALSE;
		}

		cJSON  *devid =cJSON_GetObjectItem(Json,"id");
		if(strcmp(mcu.devID,devid->valuestring)!=0){
				rt_kprintf("%sheart resp devID err %s\r\n",sign,devid->valuestring);
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
		
//		for(int i=0;i<lenth;i++)
//				rt_kprintf("%02x",data[i]);
//		rt_kprintf("\r\n");
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
			  switch(downLinkPackTpyeGet(pkType)){
					case 	heartResp:
						if(RT_TRUE==heartRespFun(Json)){//收到心跳回应 怎么通知发送层
								rt_kprintf("%srec heart resp\r\n",sign);
						}
						break;
					case devRegResp:
						if(RT_TRUE==comRespFun(Json,mcu.devRegMessID)){//收到心跳回应 怎么通知发送层
								rt_kprintf("%sreg dev succ\r\n",sign);
							  extern rt_bool_t gbRegFlag;
							  gbRegFlag = RT_TRUE;
						}
						break;
					case repDataResp:
						if(RT_TRUE==comRespFun(Json,mcu.repDataMessID)){//收到心跳回应 怎么通知发送层
								rt_kprintf("%srep data succ\r\n",sign);
						}
						break;
					case CMDRepData:
						break;
					case CMDRepEvt:
						break;
					case CMDReqData:
						break;
					case CMDReqHis:
						break;
					case errResp:
						break;
					default:
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
