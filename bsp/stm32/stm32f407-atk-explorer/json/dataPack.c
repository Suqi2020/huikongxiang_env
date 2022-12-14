#include  "board.h"
#include  "cJSON.h"
const static char sign[]="[dataPack]";
//heartUpStru  heartUp;
extern uint16_t RTU_CRC(uint8_t *puchMsg ,uint16_t usDataLen);
uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];  //与net发送buff大小一致  通过邮箱传递给NetTxBuffer 进行发送出去
mcuParazStru mcu ={0};

//rs485ParaStru devi[MODBUS_NUM];//目前4路485设备
//上行messageID自增  每次打包后自增1
uint32_t upMessIdAdd()
{
	  rt_kprintf("%smessID:%d\r\n",sign,mcu.upMessID);
		return mcu.upMessID++;
}


uint64_t subTimeStamp=0;

//获取到服务器时间戳差值
uint64_t subTimeStampGet()
{
		return subTimeStamp;
}
//存储服务器的时间戳差值  
 void  subTimeStampSet(uint64_t time)
{
	  if(time>=rt_tick_get())
				subTimeStamp=time-rt_tick_get();//服务器rtc值-当前tick值
		else
				subTimeStamp = 0;

}
uint32_t  utcTime()
{
	return rt_tick_get()+subTimeStampGet();
}
//上行数据打包



uint16_t heartUpJsonPack()
{
		char* out = NULL;
		//创建数组
		cJSON* Array = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return 0;
		// 加入节点（键值对）
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_HEARTBEAT");
	  cJSON_AddStringToObject(root, "acuId",(char *)packFLash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		
		sprintf(sprinBuf,"%u",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		nodeobj = cJSON_CreateObject();
	  cJSON_AddStringToObject(nodeobj, "identifier","area_control_unit");

		//cJSON_AddItemToObject(nodeobj,"acuId",cJSON_CreateString((char *)"100000000000001"));
		cJSON_AddItemToObject(root, "params", nodeobj);


		//打包
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json长度最后再填写
		
		// 释放内存  
		
		out = cJSON_Print(root);
		rt_strcpy((char *)packBuf+len,out);
		len+=rt_strlen(out);
		if(out!=NULL){
				for(int i=0;i<rt_strlen(out);i++)
						rt_kprintf("%c",out[i]);
				rt_kprintf("\n");
				rt_free(out);
				out=NULL;
		}
		if(root!=NULL){
			cJSON_Delete(root);
			out=NULL;
		}
	

		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新json长度
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]=(uint8_t)(TAIL>>8); len++;
		packBuf[len]=(uint8_t)(TAIL);    len++;
		packBuf[len]=0;//len++;//结尾 补0
		mcu.upHeartMessID =mcu.upMessID;
		//mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%s len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}


//uint16_t heartUpPack()
//{
//	  
//	  memset(packBuf,0,sizeof(packBuf));
//		int len=0;
//    //head+lenth
//	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
//	  packBuf[len]= (uint8_t)(HEAD);    len++;
//	  len+=LENTH_LEN;//json长度最后再填写
//	  //json
//	  char str[50]={0};//临时使用的数组
//		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"packetType\":\"CMD_HEARTBEAT\",");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"timestamp\":\"%lu\",",utcTime());
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//			
//		rt_strcpy(str,"\"param\":{\"identifier\":\"area_control_unit\",");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//	
//		rt_sprintf(str,"\"id\":\"%s\"}}",mcu.devID);
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		//lenth
//	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新json长度
//	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
//	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
//	  //crc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

//		//tail
//		packBuf[len]= (uint8_t)(TAIL>>8); len++;
//		packBuf[len]= (uint8_t)(TAIL);    len++;
//		packBuf[len] =0;//len++;//结尾 补0
//		
//		mcu.upHeartMessID =mcu.upMessID;
//		upMessIdAdd();
//		rt_kprintf("%sheart len:%d\r\n",sign,len);
//		
////		for(int i=0;i<len;i++)
////				rt_kprintf("%02x",packBuf[i]);
//		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
//		//rt_kprintf("heart:%s \n",packBuf);
//		return len;
//}
//上行心跳包 注册信息建立一个task  用来维护
//1、主动发送上行数据每次发送后启动定时器 如果收不到回应就一直重发  间隔5秒
//2、发送用发送邮箱 
//3、发送分2种 主动发送的  和回复的（发一次）
//  1个task


//extern struct rt_mailbox mbNetSendData;
//void netSendQueue()
//{
//	rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
//}

//上行注册数据打包
/*
{    
		"mid": 1234,
	    "packetType": "CMD_DEVICE_REGISTER",
	    "params":     [{        
			"model": "GY280",
			"name": "环境监测",
			"deviceId": "GYNJLXSD000000457",
			"ip": "192.168.1.108",
			"port": "",
			"type": "1"
		},
		{        
			"model": "GY281",
			"name": "接地环流",
			"deviceId": "GYNJLXSD000000458",
			"ip": "192.168.1.108",
			"port": "",
			"type": "2"
		}
		],
		"timestamp": "1655172531937"
}
*/
/*
{
	"mid": 0,
	"packetType": "CMD_DEVICE_REGISTER",
	"params": [{
		"model": "GY280",
		"name": "????",
		"deviceId": "A000000000001",
		"ip": "",
		"port": "",
		"type": "1"
	}, {
		"model": "GY281",
		"name": "????",
		"deviceId": "A000000000002",
		"ip": "",
		"port": "",
		"type": "2"
	}, {
		"model": "GY282",
		"name": "???",
		"deviceId": "A000000000003",
		"ip": "",
		"port": "",
		"type": "3"
	}, {
		"model": "GY283",
		"name": "?????",
		"deviceId": "A000000000004",
		"ip": "",
		"port": "",
		"type": "4"
	}],
	"timestamp": "17152"
}*/
uint16_t devRegJsonPack()
{
	char* out = NULL;
	//创建数组
	cJSON* Array = NULL;
	// 创建JSON Object  
	cJSON* root = NULL;
	cJSON* nodeobj = NULL;
	cJSON* nodeobj_p = NULL;
	root = cJSON_CreateObject();
	if (root == NULL) return 0;
	// 加入节点（键值对）
	cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
	cJSON_AddStringToObject(root, "packetType","CMD_DEVICE_REGISTER");
	cJSON_AddStringToObject(root, "acuId",(char *)packFLash.acuId);
	char *sprinBuf=RT_NULL;
	sprinBuf=rt_malloc(20);//20个字符串长度 够用了
	{
			Array = cJSON_CreateArray();
			if (Array == NULL) return 0;
			cJSON_AddItemToObject(root, "params", Array);
			for(int i=0;i<MODBUS_NUM;i++){
			switch(i)
			{
				case CIRCULA:
					for(int j=0;j<CIRCULA_485_NUM;j++){//核对有没有配置过
							if(sheet.cirCula[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.cirCula[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName_utf8[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.cirCula[j].ID));
							}
					}
				break;
				case PARTDISCHAG:
					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//核对有没有配置过
							if(sheet.partDischag[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.partDischag[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName_utf8[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.partDischag[j].ID));
							}
					}
				break;
				case PRESSSETTL:
					for(int j=0;j<PRESSSETTL_485_NUM;j++){//核对有没有配置过
							if(sheet.pressSetl[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.pressSetl[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName_utf8[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.pressSetl[j].ID));
							}
					}
				break;
				case THREEAXIS:
					for(int j=0;j<THREEAXIS_485_NUM;j++){//核对有没有配置过
							if(sheet.threeAxiss[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.threeAxiss[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName_utf8[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.threeAxiss[j].ID));
							}
					}
				break;
				case CH4:
					for(int j=0;j<CH4_485_NUM;j++){//核对有没有配置过
							if(sheet.ch4[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.ch4[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName_utf8[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.ch4[j].ID));
							}
					}
				break;
				case O2:
					for(int j=0;j<O2_485_NUM;j++){//核对有没有配置过
							if(sheet.o2[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.o2[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName_utf8[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.o2[j].ID));
							}
					}
				break;
				case H2S:
					for(int j=0;j<H2S_485_NUM;j++){//核对有没有配置过
							if(sheet.h2s[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.h2s[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName_utf8[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.h2s[j].ID));
							}
					}
				break;
				case CO:
					for(int j=0;j<CO_485_NUM;j++){//核对有没有配置过
							if(sheet.co[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.co[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName_utf8[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.co[j].ID));
							}
					}
				break;
				case TEMPHUM:
					for(int j=0;j<TEMPHUM_485_NUM;j++){//核对有没有配置过
							if(sheet.tempHum[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.tempHum[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName_utf8[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.tempHum[j].ID));
							}
					}
				break;
				case WATERDEPTH:
					for(int j=0;j<WATERDEPTH_485_NUM;j++){//核对有没有配置过
							if(sheet.waterDepth[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.waterDepth[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName_utf8[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.waterDepth[j].ID));
							}
					}
				break;
				default:
				break;
			}	
			extern uint8_t analogTemChanl;
			//增加模拟温湿度传感器
			if(sheet.analog[analogTemChanl].workFlag==RT_TRUE){
					nodeobj = cJSON_CreateObject();
					cJSON_AddItemToArray(Array, nodeobj);
					cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.analog[analogTemChanl].model));
					cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(sheet.analog[analogTemChanl].name));
					cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.analog[analogTemChanl].ID));
			}
		}
	}
	sprintf(sprinBuf,"%u",utcTime());
	cJSON_AddStringToObject(root,"timestamp",sprinBuf);
	rt_free(sprinBuf);
	sprinBuf=RT_NULL;
	// 打印JSON数据包  
	int len=0;
	packBuf[len]= (uint8_t)(HEAD>>8); len++;
	packBuf[len]= (uint8_t)(HEAD);    len++;
	len+=LENTH_LEN;//json长度最后再填写
	
	// 释放内存  
	
	out = cJSON_Print(root);
	rt_strcpy((char *)packBuf+len,out);
  len+=rt_strlen(out);
	if(out!=NULL){
			for(int i=0;i<rt_strlen(out);i++)
					rt_kprintf("%c",out[i]);
			rt_kprintf("\n");
		
		
//					for(int i=0;i<rt_strlen(out);i++)
//					rt_kprintf("%02X",out[i]);
//			rt_kprintf("\n");
			rt_free(out);
			out=NULL;
	}
	if(root!=NULL){
		cJSON_Delete(root);
		out=NULL;
	}
		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新json长度
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]=(uint8_t)(TAIL>>8); len++;
		packBuf[len]=(uint8_t)(TAIL);    len++;
		packBuf[len]=0;//len++;//结尾 补0
		
		mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%sreg len:%d\r\n",sign,len);
		
//		for(int i=0;i<len;i++)
//				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		return len;
}




