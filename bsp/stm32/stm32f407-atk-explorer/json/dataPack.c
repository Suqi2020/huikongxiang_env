#include  "dataPack.h"
#include  "cJSON.h"
const static char sign[]="[dataPack]";
//heartUpStru  heartUp;
extern uint16_t RTU_CRC(uint8_t *puchMsg ,uint16_t usDataLen);
uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];  //与net发送buff大小一致  通过邮箱传递给NetTxBuffer 进行发送出去
mcuParazStru mcu ={0};

//rs485ParaStru devi[UART_NUM];//目前4路485设备
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
	  subTimeStamp=time-rt_tick_get();//服务器rtc值-当前tick值

}
uint32_t  utcTime()
{
	return rt_tick_get()+subTimeStampGet();
}
//上行数据打包
uint16_t heartUpPack()
{
	  
	  memset(packBuf,0,sizeof(packBuf));
		int len=0;
    //head+lenth
	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
	  packBuf[len]= (uint8_t)(HEAD);    len++;
	  len+=LENTH_LEN;//json长度最后再填写
	  //json
	  char str[50]={0};//临时使用的数组
		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"packetType\":\"CMD_HEARTBEAT\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_sprintf(str,"\"timestamp\":\"%lu\",",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
			
		rt_strcpy(str,"\"param\":{\"identifier\":\"area_control_unit\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
	
		rt_sprintf(str,"\"id\":\"%s\"}}",mcu.devID);
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新json长度
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]= (uint8_t)(TAIL>>8); len++;
		packBuf[len]= (uint8_t)(TAIL);    len++;
		packBuf[len] =0;//len++;//结尾 补0
		
		mcu.upHeartMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%sheart len:%d\r\n",sign,len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;
}
//上行心跳包 注册信息建立一个task  用来维护
//1、主动发送上行数据每次发送后启动定时器 如果收不到回应就一直重发  间隔5秒
//2、发送用发送邮箱 
//3、发送分2种 主动发送的  和回复的（发一次）
//  1个task

uint16_t devRegJsonPack()
{
	char* out = NULL;
	//创建数组
	cJSON* Array = NULL;
	// 创建JSON Object  
	cJSON* root = NULL;
	cJSON* nodeobj = NULL;
	root = cJSON_CreateObject();
	if (root == NULL) return 0;
	// 加入节点（键值对）
	cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
	cJSON_AddStringToObject(root, "packetType","CMD_DEVICE_REGISTER");
	{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < DEV_NUM; i++)
		{
			if(modbusCheckWork(modbusDevSave[i].port)==RT_TRUE){//端口合法
					nodeobj = cJSON_CreateObject();
					cJSON_AddItemToArray(Array, nodeobj);
					cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(modbusDevSave[i].model));
					cJSON_AddItemToObject(nodeobj,"name", cJSON_CreateString(modbusDevSave[i].name));
					cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(modbusDevSave[i].devID));
			}
//			cJSON_AddItemToObject(nodeobj,"ip", cJSON_CreateString(devi[i].ip));
//			cJSON_AddItemToObject(nodeobj,"port", cJSON_CreateString(devi[i].port));
//			cJSON_AddItemToObject(nodeobj,"type", cJSON_CreateString(devi[i].type));	

		}
	}
	// 打印JSON数据包  
	out = cJSON_Print(root);
	cJSON_Delete(root);
	rt_kprintf("\n %s \n", out);
//			for(int i=0;i<rt_strlen(out);i++)
//				rt_kprintf("%02x",out[i]);
//		rt_kprintf("\n");
	//打包
	int len=0;
	packBuf[len]= (uint8_t)(HEAD>>8); len++;
	packBuf[len]= (uint8_t)(HEAD);    len++;
	len+=LENTH_LEN;//json长度最后再填写
	
	// 释放内存  
	
	
	rt_strcpy((char *)packBuf+len,out);
    len+=rt_strlen(out);
	

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
		rt_kprintf("%s len:%d\r\n",sign,len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;
}



//uint16_t devRegPack()
//{
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
//		rt_strcpy(str,"\"packetType\":\"CMD_DEVICE_REGISTER\",");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"params\":[");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//	
//		for(int i=0;i<UART_NUM;i++){
//				rt_sprintf(str,"{\"model\":\"%s\",",devi[i].model);
//				rt_strcpy((char *)packBuf+len,str);
//				len+=rt_strlen(str);
//			
//			  	
//				sprintf(str,"\"name\":\"%s\",",devi[i].name);
//			  
//				rt_strcpy((char *)packBuf+len,str);
//			
//			
//			

//			
//			
//				len+=rt_strlen(str);
//			
//				rt_sprintf(str,"\"deviceId\":\"%s\",",devi[i].ID);
//				rt_strcpy((char *)packBuf+len,str);
//				len+=rt_strlen(str);
//			
//				rt_sprintf(str,"\"ip\":\"%s\",",devi[i].ip);
//				rt_strcpy((char *)packBuf+len,str);
//				len+=rt_strlen(str);
//			
//				rt_sprintf(str,"\"port\":\"%s\",",devi[i].port);
//				rt_strcpy((char *)packBuf+len,str);
//				len+=rt_strlen(str);
//			
//				rt_sprintf(str,"\"type\":\"%s\"}",devi[i].type);
//				rt_strcpy((char *)packBuf+len,str);
//				len+=rt_strlen(str);
//				if(i+1==UART_NUM){//拷贝 ] 号
//						rt_strcpy((char *)packBuf+len,"],");
//						len+=2;
//				}
//				else{//拷贝 , 号
//						rt_strcpy((char *)packBuf+len,",");
//						len++;
//				}
//		}
//		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		rt_kprintf("[%s]\n",packBuf+4);
//		//lenth
//	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新json长度
//	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
//	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
//	  //crc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

//		//tail
//		packBuf[len]=(uint8_t)(TAIL>>8); len++;
//		packBuf[len]=(uint8_t)(TAIL);    len++;
//		packBuf[len]=0;//len++;//结尾 补0
//		
//		mcu.devRegMessID =mcu.upMessID;
//		upMessIdAdd();
//		rt_kprintf("%sreg len:%d\r\n",sign,len);
//		
//		for(int i=0;i<len;i++)
//				rt_kprintf("%02x",packBuf[i]);
//		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
//		//rt_kprintf("heart:%s \n",packBuf);
//		return len;
//}

//上行注册数据打包
/*
环境监测
{
    "mid":1234,
    "packetType":"CMD_REPORTDATA",
    "param":
    {
        "identifier":"environment_monitor",
        "acuId":"100000000000001",
        "deviceId":"1000000000002",
        "data":
        {
            "temperature":"22.2",
            "humidity":"80.1",
            "waterLevel":"0.1",
            "oxygen":"20.1",
            "carbonMonoxide":"0",
            "hydrogenSulfide":"0",
            "methane":"0",
            "fanState":"0",
            "pumpState":"0",
            "monitoringTime":"1655172531937"
        }
    },
    "timestamp":"1655172531937"
}
*/


//test only

//uint16_t rs485DataPack()
//{
//	  char num=0;//第1路485
//	  memset(packBuf,0,sizeof(packBuf));
//		int len=0;
//    //head+lenth
////	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
////	  packBuf[len]= (uint8_t)(HEAD);    len++;
////	  len+=LENTH_LEN;//json长度最后再填写
//	  //json
//	  char *str=RT_NULL;//临时使用的数组
//	  str= rt_malloc(1024);
//		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"packetType\":\"CMD_REPORTDATA12345678901http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmso1111111111111111122222222222222222222222233333333333\",");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"param\":{");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);

//		rt_strcpy(str,"\"identifier\":\"http:http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600//http://www.cmsoft.cn QQ:10865600www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600environment_monitor11111111111111111111111111111111333333333333333333333http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:1086560033http://www.cmsoft.cn QQ:1086560033\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"deviceId\":\"%s\",",devi[num].ID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_strcpy(str,"\"data\":{");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		
//		rt_strcpy(str,"\"temperature\":\"http://www.cmsoft.cnhttp://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http:http://www.cmsoft.cn QQ:10865600//www.cmsoft.cn QQ:10http://www.cmsoft.cn QQ:10865600865600 QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:108656001111111111111111111111111111111111111111111122.24444444444444444444444444444444444\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"humidity\":\"http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10http://www.cmsoft.cn QQ:10865600865600http://www.cmsoft.cn QQ:108656001112222http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:108656002222222222222222222222222222222222222222280.1444444444444444444444444444444444444http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:1086560044\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_sprintf(str,"\"monitoringTime\":\"%lu\"}},",utcTime());
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		//lenth
////	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新json长度
////	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
////	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
////	  //crc
////	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
////	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

////		//tail
////		packBuf[len]= (uint8_t)(TAIL>>8); len++;
////		packBuf[len]= (uint8_t)(TAIL);    len++;
//		packBuf[len] =0;//len++;//结尾 补0
//		
//		mcu.devRegMessID =mcu.upMessID;
//		upMessIdAdd();
//		rt_kprintf("reg len:%d\r\n",len);
//		
//		for(int i=0;i<len;i++)
//				rt_kprintf("%02x",packBuf[i]);
//		//rt_kprintf("\r\nlen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
//		//rt_kprintf("heart:%s \n",packBuf);
//		rt_free(str);
//		str=RT_NULL;
//		return len;
//}
//uint16_t rs485DataPack()
//{
//	  char num=0;//第1路485
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
//		rt_strcpy(str,"\"packetType\":\"CMD_REPORTDATA\",");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"param\":{");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);

//		rt_strcpy(str,"\"identifier\":\"environment_monitor\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"deviceId\":\"%s\",",devi[num].ID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_strcpy(str,"\"data\":{");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		
//		rt_strcpy(str,"\"temperature\":\"22.2\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"humidity\":\"80.1\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_sprintf(str,"\"monitoringTime\":\"%lu\"}},",utcTime());
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
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
//		mcu.devRegMessID =mcu.upMessID;
//		upMessIdAdd();
//		rt_kprintf("%sreg len:%d\r\n",sign,len);
//		
//		for(int i=0;i<len;i++)
//				rt_kprintf("%02x",packBuf[i]);
//		//rt_kprintf("\r\nlen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
//		//rt_kprintf("heart:%s \n",packBuf);
//		return len;
//}

extern rt_bool_t modbusCommRead(modbusDevSaveStru modbus,uint8_t *out);
extern rt_bool_t errNumFlag[DEV_NUM];
//打包的时候读取modbus  NUM 第几路
uint16_t uartModbusJsonPack(int num)
{
		uint8_t *readBuf=rt_malloc(250);
		
		char* out = NULL;
		//创建数组
		cJSON* Array = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return 0;
		// 加入节点（键值对）
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_REPORTDATA");
		cJSON_AddStringToObject(root, "acuID","100000000000001");
		{
			Array = cJSON_CreateArray();
			if (Array == NULL) return 0;
			cJSON_AddItemToObject(root, "params", Array);
			for (int i = 0; i < DEV_NUM; i++)
			{
								 
				if(modbusDevSave[i].port==num+1){//端口合法
					
						 rt_kprintf("%s read%s %s\n",sign,modbusDevSave[i].devID,modbusDevSave[i].name);
						 if(RT_FALSE== modbusCommRead(modbusDevSave[i],readBuf))//执行到此处打包param和data
								errNumFlag[i]=RT_TRUE;//modbus 不回应
						 else
								errNumFlag[i]=RT_FALSE;
		
						 uint8_t *sprBuf=rt_malloc(modbusDevSave[i].regLen*4+1);//二进制数据的2倍 +1
						 for(int k=0;k<modbusDevSave[i].regLen*2;k++)
								sprintf((char*)sprBuf+k*2,"%02x",readBuf[k]);//16进制数据格式化字符串
						 sprBuf[modbusDevSave[i].regLen*4+1]=0;//末尾补0		
						 
						 

						nodeobj = cJSON_CreateObject();
						cJSON_AddItemToArray(Array, nodeobj);
						cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(modbusDevSave[i].devID));
						cJSON_AddItemToObject(nodeobj,"port",cJSON_CreateNumber(modbusDevSave[i].port));
						cJSON_AddItemToObject(nodeobj,"devaddr",cJSON_CreateNumber(modbusDevSave[i].devAddr));
						cJSON_AddItemToObject(nodeobj,"regcmd",cJSON_CreateNumber(modbusDevSave[i].regCmd));
						cJSON_AddItemToObject(nodeobj,"regaddr",cJSON_CreateNumber(modbusDevSave[i].regAddr));
						cJSON_AddItemToObject(nodeobj,"reglen",cJSON_CreateNumber(modbusDevSave[i].regLen));
					
						
						cJSON_AddItemToObject(nodeobj,"regdata",cJSON_CreateString((char*)sprBuf));
						rt_free(sprBuf);
						sprBuf=RT_NULL;
						rt_thread_mdelay(packFLash.port[num].delayTime*100);// 局放GY-JF100-C01和别的设备挂载在同一个串口的情况下连续读取设备会导致局放读取失败 此处需要延时2秒
					 
				}
			}
		}
		rt_free(readBuf);
		readBuf=RT_NULL;

		char time[20];
		sprintf(time,"%d",utcTime());
		cJSON_AddStringToObject(root, "timestamp",time);
		// 打印JSON数据包  
		out = cJSON_Print(root);
		cJSON_Delete(root);
		rt_kprintf("\n %s \n", out);

	//打包
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json长度最后再填写
		
		// 释放内存  
		
		
		rt_strcpy((char *)packBuf+len,out);
    len+=rt_strlen(out);
	

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
		rt_kprintf("%s len:%d\r\n",sign,len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;

}
