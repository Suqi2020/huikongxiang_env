#include   "dataPhrase.h"
#include   "cJSON.h"
//https://blog.csdn.net/woody218/article/details/119634171  json解析参考
extern uint16_t RTU_CRC(uint8_t *puchMsg , uint16_t usDataLen);

//数据校验 头尾 校验和 是否正确
//rt_TRUE 正确 rt_FALSE 错误
rt_bool_t dataCheck(char *data,int lenth)
{
//	1、解析头尾校验 不对丢弃
//	2、提取packettype,分别校验
		uint16_t jsonBodyCrc=RTU_CRC((uint8_t *)data+HEAD_LEN+LENTH_LEN,lenth-HEAD_LEN-LENTH_LEN);
	  uint16_t dataCrc=(uint16_t)(data[lenth-2]<<8)+data[lenth-1];
	  if(((data[0]<<8)+data[1])!=HEAD){
				rt_kprintf("head err\r\n");
				return RT_FALSE;		
		}
		if(((data[lenth-2]<<8)+data[lenth-1])!=TAIL){
				rt_kprintf("tail err\r\n");
				return RT_FALSE;		
		}
	  if(jsonBodyCrc!=dataCrc){
				rt_kprintf("crc err\r\n");
				return RT_FALSE;
		}
		return RT_TRUE;
}
//分别找出下行数据的类型并分类    
packTypeEnum  downLinkPackTpyeGet(cJSON  *TYPE)
{
	  if(rt_strcmp(TYPE->string,"CMD_HEARTBEAT_RESPONSE")==0){
				return heartResp;
		}
		else if(rt_strcmp(TYPE->string,"CMD_DEVICE_REGISTE_RESPONSE")==0){
				return devRegResp;
		}
		else if(rt_strcmp(TYPE->string,"CMD_REPORTDATA_RESPONSE")==0){
				return repDataResp;
		}else if(rt_strcmp(TYPE->string,"CMD_REQUESTDATA_RESPONSE")==0){
				return CMDRepDataResp;
		}
		else{
				rt_kprintf("err:packetType\r\n");
		}
		
		return errResp;
}


//需要判断devid 和消息ID一致才认为心跳发送成功
rt_bool_t heartRespFun(cJSON  *Json)
{

		cJSON  *time =cJSON_GetObjectItem(Json,"timestamp");
	  rt_kprintf("time:%s\n\r",time->valuestring);

		static uint64_t u64getTick_p;

		u64getTick_p =atoll(time->valuestring);
		rt_kprintf("time:[%lu]s \r\n", (uint32_t)((u64getTick_p)/1000));

		rt_kprintf("time:[%u]ms\r\n", (u64getTick_p)%1000);
    extern void  subTimeStampSet(uint64_t time);
		subTimeStampSet(u64getTick_p);
	
		cJSON  *mid =cJSON_GetObjectItem(Json,"mid");
    if(device.upHeartMessID != mid->valueint){
				rt_kprintf("heart resp messID err %d\r\n",mid->valueint);
			  return RT_FALSE;
			
		}
    if(device.upHeartMessID != mid->valueint){
				rt_kprintf("heart resp messID err %d\r\n",mid->valueint);
			  return RT_FALSE;
			
		}
		cJSON  *devid =cJSON_GetObjectItem(Json,"id");
		if(strcmp(device.devID,devid->valuestring)!=0){
				rt_kprintf("heart resp devID err %s\r\n",devid->valuestring);
			  return RT_FALSE;
		}

		return RT_TRUE;
}
//下行数据解析
void AllDownPhrase(char *data,int lenth)
{

	  if(dataCheck(data,lenth)==RT_FALSE){
				return;
		}
		char *buf=data+HEAD_LEN+LENTH_LEN;//偏移后是真实的json数据
		int  len=lenth-HEAD_LEN-LENTH_LEN-TAIL_LEN;//获取真实的json数据长度
		
		
		char *Buffer=(char *)rt_malloc(len+1);
		rt_strncpy(Buffer,buf,len);
    Buffer[len]=0;
		//开始解析json
		rt_kprintf("getJson:%s\r\n",buf);			
		cJSON  *Json=NULL;
		Json = cJSON_Parse(Buffer);
		
		
		if(Json!=RT_NULL){//解析json数据
				cJSON  *pkType = cJSON_GetObjectItem(Json,"packetType");
			  switch(downLinkPackTpyeGet(pkType)){
					case 	heartResp:
						if(RT_TRUE==heartRespFun(Json)){//收到心跳回应 怎么通知发送层
							
						}
						break;
					case devRegResp:
						break;
					case repDataResp:
						break;
					case CMDRepDataResp:
						break;
					case CMDRepEvtResp:
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
			rt_kprintf("err:json empty\r\n");	
		}
		
		rt_free(Buffer);
	
		
}
