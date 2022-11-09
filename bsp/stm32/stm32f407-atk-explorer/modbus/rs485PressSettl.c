#include "board.h"
//<<压差式沉降仪 GY-STC-2000>> 默认波特率9600  modbus地址 0xb1(有误) 见机身标签后2位  协议文档有误
//修改设备地址 FF FF 03 0A＋设备完整的长地址＋01＋短地址 
//   FF FF 03 0A 6E 01 20 21 09 08 00 B1 01 01
//  24+红色，24-黑色，A+蓝色，B-绿色
const static char sign[]="[沉降仪]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //工作环流用到的最大接收buf长度


static pressSettlStru pressSettle[PRESSSETTL_485_NUM];


//打包串口发送 
static void pressSettlUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.pressSetl[num].useUartNum,buf, len);
}

///////////////////////////////////////读写寄存器相关操作////////////////////////////////////////



uint8_t psReadReg(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out)
{
		int i=0;
	  out[i]=slavAddr;					 			i++;
	  out[i]=0x04;      					 		i++;
	  out[i]=(uint8_t)(regAddr>>8);   i++;
	  out[i]=(uint8_t) regAddr;       i++;
		out[i]=(uint8_t)(len>>8);       i++;
	  out[i]=(uint8_t) len;       		i++;
	  uint16_t crcRet=RTU_CRC(out ,i);
	  out[i]=(uint8_t)(crcRet>>8);    i++;
	  out[i]=crcRet;       						i++;
		return i;
}



//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
void readPSTempHeight(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = psReadReg(sheet.pressSetl[num].slaveAddr,0X0001,2,buf);
		rt_mutex_take(uartDev[sheet.pressSetl[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		pressSettlUartSend(num,buf,len);
	  rt_kprintf("%spressSettl send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		
		while(rt_mq_recv(uartDev[sheet.pressSetl[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
		//提取环流值 第一步判断crc 第二部提取
	//	uartDev[modbusFlash[PRESSSETTL].useUartNum].offline=RT_FALSE;
		int ret2=modbusRespCheck(sheet.pressSetl[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值

        int temp	=(buf[offset]<<8)+buf[offset+1];offset+=2;
			  pressSettle[num].height.intVal=(buf[offset]<<8)+buf[offset+1];
			
				pressSettle[num].temp =temp*0.0625;
				

			  rt_kprintf("%stemp:%0.2f*C height:%0.1fmm read ok\n",sign,pressSettle[num].temp,pressSettle[num].height.flotVal);  
		} 
		else{//读不到给0
				if(ret2==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
//					  uartDev[modbusFlash[PRESSSETTL].useUartNum].offline=RT_TRUE;
				}
			  pressSettle[num].temp	=0;
			  pressSettle[num].height.intVal=0;
			  rt_kprintf("%stemp height read fail\n",sign);
		}
	  rt_mutex_release(uartDev[sheet.pressSetl[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}



/////////////////////////////////////////JSON格式打包//////////////////////////////////////////
//温度高度值打包



//{
//        "mid":1234,
//        "packetType?:"CMD_REPORTDATA",    
//        "param":
//        {
//                "identifier":"pressure_settler_monitor",
//                "acuId":"100000000000001",
//                "deviceId":"1000000000004",?
//                "data":
//                {
//                        "temp":"22.75", //?
//                        "height":"0.8",//mm
//                        "monitoringTime":"1655172531937"
//                }
//        },
//        "timestamp":"1655172531937"
//}


//void PSTempHeightPack()
//{
//		memset(packBuf,0,sizeof(packBuf));
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

//		rt_strcpy(str,"\"identifier\":\"pressure_settler_monitor\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"deviceId\":\"%s\",",devi[PRESSSETTL].ID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_strcpy(str,"\"data\":{");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		
//	 	//sprintf(str,"test:%0.2f",(float)121/100);				 
//		sprintf(str,"\"temp\":\"%0.2f\",",(float)((float)pressSettle.temp/100));	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		sprintf(str,"\"height\":\"%0.1f\",",(float)((float)pressSettle.height/10));	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//	
//		
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
//		mcu.repDataMessID =mcu.upMessID;
//		upMessIdAdd();
//		rt_kprintf("%spressSet len:%d\r\n",sign,len);
//		
////		for(int i=0;i<len;i++)
////				rt_kprintf("%02x",packBuf[i]);
//		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
//}
static uint16_t pressSettlJsonPack()
{
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
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
		cJSON_AddStringToObject(root, "packetType","CMD_REPORTDATA");
		cJSON_AddStringToObject(root, "identifier","anti_sedimentation");
		cJSON_AddStringToObject(root, "acuId","100000000000001");
		
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < PRESSSETTL_485_NUM; i++)
		{		
			if(sheet.pressSetl[i].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.pressSetl[i].ID));
				
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				sprintf(sprinBuf,"%02f",pressSettle[i].temp);
				cJSON_AddItemToObject(nodeobj_p,"temperature",cJSON_CreateString(sprinBuf));

				sprintf(sprinBuf,"%02f",pressSettle[i].height.flotVal );
				cJSON_AddItemToObject(nodeobj_p,"height",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		}
		}
	
		sprintf(sprinBuf,"%d",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// 打印JSON数据包  
		out = cJSON_Print(root);
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
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}

void pressSettRead2Send(rt_bool_t netStat)
{
	 int workFlag=RT_FALSE;
		for(int i=0;i<PRESSSETTL_485_NUM;i++){
				if(sheet.pressSetl[i].workFlag==RT_TRUE){
						readPSTempHeight(i);
						workFlag=RT_TRUE;
				}
		}
		if(workFlag==RT_TRUE){
				rt_kprintf("%s打包采集的PRESSSETTL数据\r\n",sign);
				pressSettlJsonPack();
				if(netStat==RT_TRUE)
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
		}
}