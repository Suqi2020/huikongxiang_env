#include <board.h>
//#include "rs485PartDischag.h"
//#include "stmflash.h"
//<<¾Ö·ÅÔÚÏß¼ì²â --GY-JF100-C01>>  GZPD-1000µçÀÂ¾Ö·Å
//ÏìÓ¦Ê±¼ä²»È·¶¨ ×î³¤1.7Ãë ÓÐÊ±ºò³¤ ÓÐÊ±ºò¶Ì
//           Ä¬ÈÏ²¨ÌØÂÊ115200


const static char sign[]="[¾Ö·Å]";
static partDischargeStru partDiscStru_p[PARTDISCHAG_485_NUM];

//#define   SLAVE_ADDR     0X01
#define   LENTH          1024  //¹¤×÷»·Á÷ÓÃµ½µÄ×î´ó½ÓÊÕbuf³¤¶È


static void partDischagUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.partDischag[num].useUartNum,buf, len);
	
}

//¶ÁÈ¡·ùÖµ ÆµÂÊ ·Åµç×ÜÄÜÁ¿
//01 03 0300 0006 C58C
//01 03 24 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 7B A1 

void readPdFreqDischarge(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.partDischag[num].slaveAddr,0x0300,READ_03,18,buf);
		rt_mutex_take(uartDev[sheet.partDischag[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485·¢ËÍbuf  len  µÈ´ýmodbus»ØÓ¦
		partDischagUartSend(num,buf,len);
	  rt_kprintf("%sPdFreqDiach send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		memset(buf,0,LENTH);
    len=0;
		if(rt_mq_recv(uartDev[sheet.partDischag[num].useUartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//µÚÒ»´Î½ÓÊÕÊ±¼ä·Å³¤µã  ÏàÓ¦Ê±¼äÓÐ¿ÉÄÜ±È½Ï¾Ã
				len++;
		}
		while(rt_mq_recv(uartDev[sheet.partDischag[num].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 ²¨ÌØÂÊ1ms 10¸öÊý¾Ý
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
		//ÌáÈ¡»·Á÷Öµ µÚÒ»²½ÅÐ¶Ïcrc µÚ¶þ²¿ÌáÈ¡
//		uartDev[modbusFlash[PARTDISCHAG].useUartNum].offline=RT_FALSE;
		int ret2= modbusRespCheck(sheet.partDischag[num].slaveAddr,buf,len,RT_TRUE);
		if(0 ==  ret2){//Ë¢ÐÂ¶ÁÈ¡µ½µÄÖµ
			
				partDiscStru_p[num].amplitudeA=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				partDiscStru_p[num].freqA     =(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  partDiscStru_p[num].dischargeA=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			
				partDiscStru_p[num].amplitudeB=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				partDiscStru_p[num].freqB     =(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  partDiscStru_p[num].dischargeB=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			
				partDiscStru_p[num].amplitudeC=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				partDiscStru_p[num].freqC     =(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  partDiscStru_p[num].dischargeC=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  rt_kprintf("%sPdFreqDiach read ok\n",sign);
		} 
		else{//¶Á²»µ½¸ø0
				if(ret2==2){
						//rt_kprintf("%sERR:Çë¼ì²é485½ÓÏß»òÕß¹©µç\r\n",sign);
//						uartDev[modbusFlash[PARTDISCHAG].useUartNum].offline=RT_TRUE;
				}
				partDiscStru_p[num].amplitudeA=0;
				partDiscStru_p[num].freqA     =0;
			  partDiscStru_p[num].dischargeA=0;
			
				partDiscStru_p[num].amplitudeB=0;
				partDiscStru_p[num].freqB     =0;
			  partDiscStru_p[num].dischargeB=0;
			
				partDiscStru_p[num].amplitudeC=0;
				partDiscStru_p[num].freqC     =0;
			  partDiscStru_p[num].dischargeC=0;
			  rt_kprintf("%sPdFreqDiach read fail\n",sign);
		}

	  rt_mutex_release(uartDev[sheet.partDischag[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}

//¶ÁÈ¡¾Ö·Å±¨¾¯×´Ì¬ 
//·¢ 01 01 0001 0008 6C0C
//ÊÕ 01 01 01 00 51 88 

rt_bool_t readPartDischgWarning(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadBitReg(sheet.partDischag[num].slaveAddr,0x0001,8,buf);//¶ÁÈ¡8¸öbit
		rt_mutex_take(uartDev[sheet.partDischag[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485·¢ËÍbuf  len  µÈ´ýmodbus»ØÓ¦
		partDischagUartSend(num,buf,len);
	  rt_kprintf("%sreadPd send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		memset(buf,0,LENTH);
    len=0;
		if(rt_mq_recv(uartDev[sheet.partDischag[num].useUartNum].uartMessque, buf+len, 1, 2000) == RT_EOK){//µÚÒ»´Î½ÓÊÕÊ±¼ä·Å³¤µã  ÏàÓ¦Ê±¼äÓÐ¿ÉÄÜ±È½Ï¾Ã
				len++;
		}
		while(rt_mq_recv(uartDev[sheet.partDischag[num].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 ²¨ÌØÂÊ1ms 10¸öÊý¾Ý
				len++;
		}
		rt_kprintf("%srec:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
//		uartDev[modbusFlash[PARTDISCHAG].useUartNum].offline=RT_FALSE;
		//ÌáÈ¡»·Á÷Öµ µÚÒ»²½ÅÐ¶Ïcrc µÚ¶þ²¿ÌáÈ¡
		int ret2=modbusRespCheck(sheet.partDischag[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//Ë¢ÐÂ¶ÁÈ¡µ½µÄÖµ
     
			  partDiscStru_p[num].alarm.a=(buf[offset]>>0)&0x01;
				partDiscStru_p[num].alarm.b=(buf[offset]>>1)&0x01;
			  partDiscStru_p[num].alarm.c=(buf[offset]>>2)&0x01;
			  rt_kprintf("%sÌáÈ¡alarm OK\r\n",sign);
		} 
		else{
				if(ret2==2){
						//rt_kprintf("%sERR:Çë¼ì²é485½ÓÏß»òÕß¹©µç\r\n",sign);
//					  uartDev[modbusFlash[PARTDISCHAG].useUartNum].offline=RT_TRUE;
				}
				partDiscStru_p[num].alarm.a=0;
				partDiscStru_p[num].alarm.b=0;
				partDiscStru_p[num].alarm.c=0;
			  rt_kprintf("%sÌáÈ¡alarm fail\r\n",sign);
		}
   
	  rt_mutex_release(uartDev[sheet.partDischag[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;
		if(partDiscStru_p[num].alarm.a||partDiscStru_p[num].alarm.b||partDiscStru_p[num].alarm.c)
				return RT_TRUE;
		else 
			  return RT_FALSE;
}

void  partDisWaringEventPack()
{
		rt_kprintf("%slater add \n\r",sign);
		
}
/*
{
    "mid":1234,
    "packetType":"CMD_REPORTDATA",
    "param":
    {
        "identifier":"partial_discharge_monitor",
        "acuId":"100000000000001",
        "deviceId":"1000000000003", 
        "data":
        {
            "pdA":"",
            "freqA":"",
            "dischargeDataA":"",
            "prpdDataA":"",
            "prpsDataA":"",
            "pdB":"",
            "freqB":"",
            "dischargeDataB":"",
            "prpdDataB":"",
            "prpsDataB":"",
            "pdC":"",
            "freqC":"",
            "pdTotalC":"",
            "dischargeDataC":"",
            "prpdDataC":"",

            "monitoringTime":"1655172531937"
        }
    },
    "timestamp":"1655172531937"
}
*/
//void  partDisDataPack()
//{
// 
//	  memset(packBuf,0,sizeof(packBuf));
//		int len=0;
//    //head+lenth
//	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
//	  packBuf[len]= (uint8_t)(HEAD);    len++;
//	  len+=LENTH_LEN;//json³¤¶È×îºóÔÙÌîÐ´
//	  //json
//	  char str[50]={0};//ÁÙÊ±Ê¹ÓÃµÄÊý×é
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

//		rt_strcpy(str,"\"identifier\":\"partial_discharge_monitor\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"deviceId\":\"%s\",",devi[PARTDISCHAG].ID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_strcpy(str,"\"data\":{");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		
//	 	//sprintf(str,"test:%0.2f",(float)121/100);				 
//		sprintf(str,"\"pdA\":\"%u\",",partDiscStru_p.amplitudeA);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		sprintf(str,"\"freqA\":\"%u\",",partDiscStru_p.freqA);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		sprintf(str,"\"dischargeDataA\":\"%u\",",partDiscStru_p.dischargeA);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		rt_strcpy(str,"\"prpdDataA\":\"\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		rt_strcpy(str,"\"prpsDataA\":\"\","); 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		sprintf(str,"\"pdB\":\"%u\",",partDiscStru_p.amplitudeA);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		sprintf(str,"\"freqB\":\"%u\",",partDiscStru_p.freqA);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		sprintf(str,"\"dischargeDataB\":\"%u\",",partDiscStru_p.dischargeA);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		rt_strcpy(str,"\"prpdDataB\":\"\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		rt_strcpy(str,"\"prpsDataB\":\"\","); 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);		

//		sprintf(str,"\"pdC\":\"%u\",",partDiscStru_p.amplitudeA);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		sprintf(str,"\"freqC\":\"%u\",",partDiscStru_p.freqA);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		sprintf(str,"\"dischargeDataC\":\"%u\",",partDiscStru_p.dischargeA);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		rt_strcpy(str,"\"prpdDataC\":\"\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		rt_strcpy(str,"\"prpsDataC\":\"\","); 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);		
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
//	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//¸üÐÂjson³¤¶È
//	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
//	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
//	  //crc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//¸üÐÂcrc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

//		//tail
//		packBuf[len]= (uint8_t)(TAIL>>8); len++;
//		packBuf[len]= (uint8_t)(TAIL);    len++;
//		packBuf[len] =0;//len++;//½áÎ² ²¹0
//		
//		mcu.repDataMessID =mcu.upMessID;
//		upMessIdAdd();
//		rt_kprintf("%spartd len:%d\r\n",sign,len);
//		
////		for(int i=0;i<len;i++)
////				rt_kprintf("%02x",packBuf[i]);
//		rt_kprintf("\r\n%slen£º%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
//		//rt_kprintf("heart:%s \n",packBuf);
//	
//}
uint16_t partDischagJsonPack()
{
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20¸ö×Ö·û´®³¤¶È ¹»ÓÃÁË
		char* out = NULL;
		//´´½¨Êý×é
		cJSON* Array = NULL;
		// ´´½¨JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return 0;
		// ¼ÓÈë½Úµã£¨¼üÖµ¶Ô£©
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_REPORTDATA");
		cJSON_AddStringToObject(root, "identifier","partial_discharge_monitor");
		cJSON_AddStringToObject(root, "acuId","100000000000001");
		
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < PARTDISCHAG_485_NUM; i++)
		{		
			rt_thread_mdelay(2000);
			if(sheet.partDischag[i].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.partDischag[i].ID));
				
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				
				sprintf(sprinBuf,"%d",partDiscStru_p[i].amplitudeA);
				cJSON_AddItemToObject(nodeobj_p,"pdA",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].freqA);
				cJSON_AddItemToObject(nodeobj_p,"freqA",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].dischargeA);
				cJSON_AddItemToObject(nodeobj_p,"dischargeDataA",cJSON_CreateString(sprinBuf));
				cJSON_AddItemToObject(nodeobj_p,"prpdDataA",cJSON_CreateString(""));
				cJSON_AddItemToObject(nodeobj_p,"prpsDataA",cJSON_CreateString(""));
				
				
				sprintf(sprinBuf,"%d",partDiscStru_p[i].amplitudeB);
				cJSON_AddItemToObject(nodeobj_p,"pdB",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].freqB);
				cJSON_AddItemToObject(nodeobj_p,"freqB",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].dischargeB);
				cJSON_AddItemToObject(nodeobj_p,"dischargeDataB",cJSON_CreateString(sprinBuf));
				cJSON_AddItemToObject(nodeobj_p,"prpdDataB",cJSON_CreateString(""));
				cJSON_AddItemToObject(nodeobj_p,"prpsDataB",cJSON_CreateString(""));
				
				sprintf(sprinBuf,"%d",partDiscStru_p[i].amplitudeC);
				cJSON_AddItemToObject(nodeobj_p,"pdC",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].freqC);
				cJSON_AddItemToObject(nodeobj_p,"freqC",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].dischargeC);
				cJSON_AddItemToObject(nodeobj_p,"dischargeDataC",cJSON_CreateString(sprinBuf));
				cJSON_AddItemToObject(nodeobj_p,"prpdDataC",cJSON_CreateString(""));
				cJSON_AddItemToObject(nodeobj_p,"prpsDataC",cJSON_CreateString(""));
				
				
				sprintf(sprinBuf,"%d",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		}
		}
	
		sprintf(sprinBuf,"%d",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// ´òÓ¡JSONÊý¾Ý°ü  
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

		//´ò°ü
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json³¤¶È×îºóÔÙÌîÐ´
		
		// ÊÍ·ÅÄÚ´æ  
		
		
		rt_strcpy((char *)packBuf+len,out);
    len+=rt_strlen(out);
	

		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//¸üÐÂjson³¤¶È
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//¸üÐÂcrc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]=(uint8_t)(TAIL>>8); len++;
		packBuf[len]=(uint8_t)(TAIL);    len++;
		packBuf[len]=0;//len++;//½áÎ² ²¹0
		
		mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%s len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen£º%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}
void partDischagRead2Send(rt_bool_t netStat)
{
		int workFlag=RT_FALSE;
		for(int i=0;i<PARTDISCHAG_485_NUM;i++){
			 if(sheet.partDischag[i].workFlag==RT_TRUE){
						readPdFreqDischarge(i);
						workFlag=RT_TRUE;
				}
		}
		if(workFlag==RT_TRUE){
				rt_kprintf("%s´ò°ü²É¼¯µÄPARTDISCHAGÊý¾Ý\r\n",sign);
				partDischagJsonPack();//ºóÆÚ¼ÓÈë
				if(netStat==RT_TRUE)
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
		}
}