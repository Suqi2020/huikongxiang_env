#include "board.h"
//#include "stmflash.h"
//<<Ñ¹²îÊ½³Á½µÒÇ GY-STC-2000>> Ä¬ÈÏ²¨ÌØÂÊ9600  modbusµØÖ· 0xb1(ÓĞÎó) ¼û»úÉí±êÇ©ºó2Î»   Ğ­ÒéÎÄµµÓĞÎó
/*
ÈıÖáÑéÖ¤¶ÌµØÖ·£«04 00 01 00 04

Ñ¹²îÑéÖ¤¶ÌµØÖ·£«04 00 01 00 02
²¨ÌØÂÊ¸ÄÎª9600£¬Ñ¹²îºÍÈıÖá¸Ã¶ÌµØÖ·ÃüÁîÂë¶¼ÊÇÒ»ÑùµÄ£¬FF FF 03 0A£«Éè±¸ÍêÕûµÄ³¤µØÖ·£«01£«¶ÌµØÖ·
*/
//ĞŞ¸ÄÉè±¸µØÖ· FF FF 03 0A£«Éè±¸ÍêÕûµÄ³¤µØÖ·£«01£«¶ÌµØÖ· 
//   FF FF 03 0A 6E 01 20 21 09 08 00 B1 01 01 
//  24+ºìÉ«£¬24-ºÚÉ«£¬A+À¶É«£¬B-ÂÌÉ«
// ·¢ 01 04 00 01 00 04 A0 09 
// ÊÕ 01 04 08 0B CA FE 8D 00 03 03 80 C7 23 
const static char sign[]="[·ÀÍâÆÆ]";
static threeAxisStru threeAxisp[THREEAXIS_485_NUM];

//#define   SLAVE_ADDR     0X02 
#define   LENTH          50  //¹¤×÷»·Á÷ÓÃµ½µÄ×î´ó½ÓÊÕbuf³¤¶È


//´ò°ü´®¿Ú·¢ËÍ 
static void threeAxisUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.threeAxiss[num].useUartNum,buf, len);
}

///////////////////////////////////////¶ÁĞ´¼Ä´æÆ÷Ïà¹Ø²Ù×÷////////////////////////////////////////

//ÓëÑ¹²îÊ½´«¸ĞÆ÷¹²ÓÃÒ»¸ö¶ÁÃüÁî
extern uint8_t psReadReg(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out);



//·¢ 1A 04 00 01 00 02 23 E0
//ÊÕ 1A 04 04 0B 1B 00 1C 23 6F
void readThreeTempAcc(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = psReadReg(sheet.threeAxiss[num].slaveAddr,0X0001,4,buf);
		rt_mutex_take(uartDev[sheet.threeAxiss[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485·¢ËÍbuf  len  µÈ´ımodbus»ØÓ¦
		threeAxisUartSend(num,buf,len);
	  rt_kprintf("%sthreeAxis send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		while(rt_mq_recv(uartDev[sheet.threeAxiss[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ²¨ÌØÂÊ1ms 10¸öÊı¾İ
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
//		uartDev[modbusFlash[THREEAXIS].useUartNum].offline=RT_FALSE;
		//ÌáÈ¡»·Á÷Öµ µÚÒ»²½ÅĞ¶Ïcrc µÚ¶ş²¿ÌáÈ¡
		int ret2=modbusRespCheck(sheet.threeAxiss[num].slaveAddr,buf,len,RT_TRUE);
		if(0 ==  ret2){//Ë¢ĞÂ¶ÁÈ¡µ½µÄÖµ
        int temp	=(buf[offset]<<8)+buf[offset+1];offset+=2;
			  threeAxisp[num].acclrationX = (buf[offset]<<8)+buf[offset+1];offset+=2;
				threeAxisp[num].acclrationY = (buf[offset]<<8)+buf[offset+1];offset+=2;
				threeAxisp[num].acclrationZ = (buf[offset]<<8)+buf[offset+1];offset+=2;
        threeAxisp[num].temp=(float)temp*0.0625; 
			  rt_kprintf("%stemp:%0.2f*C ACC:X%dmg Y%dmg Z%dmg ok\n",sign,temp,threeAxisp[num].acclrationX,threeAxisp[num].acclrationY,threeAxisp[num].acclrationZ);  
		} 
		else{//¶Á²»µ½¸ø0
				if(ret2==2){
//					  uartDev[modbusFlash[THREEAXIS].useUartNum].offline=RT_TRUE;
				}
				threeAxisp[num].temp=0;
			  threeAxisp[num].acclrationX	= 0;
			  threeAxisp[num].acclrationY = 0;
			  threeAxisp[num].acclrationY = 0;
			  rt_kprintf("%stemp height read fail\n",sign);
		}
	  rt_mutex_release(uartDev[sheet.threeAxiss[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}
//void multReadThreeTempAcc()
//{
//		for(int i=0;i<THREEAXIS_485_NUM;i++){
//				if(sheet.threeAxiss[i].slaveAddr!=0){
//					if(sheet.threeAxiss[i].slaveAddr!=255){
//						readThreeTempAcc(i);
//					}
//				}
//		}
//}

/////////////////////////////////////////JSON¸ñÊ½´ò°ü//////////////////////////////////////////
//ÎÂ¶È¸ß¶ÈÖµ´ò°ü

/*
{
    "mid":1234,
    "packetType ":"CMD_REPORTDATA",  
    "param":
    {
        "identifier":" vibration_meter_monitor",
        "acuId":"100000000000001",
        "deviceId":"1000000000004", 
        "data":
        {
            "temp":"22.75", //?
            "accelerationX":"1234",//mg
"accelerationY":"1234",//mg
"accelerationZ":"1234",//mg
            "monitoringTime":"1655172531937"
        }
    },
    "timestamp":"1655172531937"
}
*/

//void t3AxisTempAccPack()
//{
//		memset(packBuf,0,sizeof(packBuf));
//		int len=0;
//    //head+lenth
//	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
//	  packBuf[len]= (uint8_t)(HEAD);    len++;
//	  len+=LENTH_LEN;//json³¤¶È×îºóÔÙÌîĞ´
//	  //json
//	  char str[50]={0};//ÁÙÊ±Ê¹ÓÃµÄÊı×é
//		sprintf(str,"{\"mid\":%u,",mcu.upMessID);
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
//		rt_sprintf(str,"\"deviceId\":\"%s\",",devi[THREEAXIS].ID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_strcpy(str,"\"data\":{");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		
//	 	//sprintf(str,"test:%0.2f",(float)121/100);				 
//		sprintf(str,"\"temp\":\"%0.2f\",",(float)((float)threeAxis.temp/100));	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		sprintf(str,"\"accelerationX\":\"%d\",",threeAxis.acclrationX);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		sprintf(str,"\"accelerationY\":\"%d\",",threeAxis.acclrationY);	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		sprintf(str,"\"accelerationZ\":\"%d\",",threeAxis.acclrationZ);	 
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
//	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//¸üĞÂjson³¤¶È
//	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
//	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
//	  //crc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//¸üĞÂcrc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

//		//tail
//		packBuf[len]= (uint8_t)(TAIL>>8); len++;
//		packBuf[len]= (uint8_t)(TAIL);    len++;
//		packBuf[len] =0;//len++;//½áÎ² ²¹0
//		
//		mcu.repDataMessID =mcu.upMessID;
//		upMessIdAdd();
//		rt_kprintf("%sThreeAx len:%d\r\n",sign,len);
//		
////		for(int i=0;i<len;i++)
////				rt_kprintf("%02x",packBuf[i]);
//		rt_kprintf("\r\n%slen£º%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
//}

static uint16_t threeAxisJsonPack()
{
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20¸ö×Ö·û´®³¤¶È ¹»ÓÃÁË
		char* out = NULL;
		//´´½¨Êı×é
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
		cJSON_AddStringToObject(root, "identifier","external_breakout_prevention");
		cJSON_AddStringToObject(root, "acuId","100000000000001");
		
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < THREEAXIS_485_NUM; i++)
		{		

			if(sheet.threeAxiss[i].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.threeAxiss[i].ID));
				
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				
				sprintf(sprinBuf,"%02f",threeAxisp[i].temp);
				cJSON_AddItemToObject(nodeobj_p,"temperature",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",threeAxisp[i].acclrationX);
				cJSON_AddItemToObject(nodeobj_p,"accelerationX",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",threeAxisp[i].acclrationY);
				cJSON_AddItemToObject(nodeobj_p,"accelerationY",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",threeAxisp[i].acclrationZ);
				cJSON_AddItemToObject(nodeobj_p,"accelerationZ",cJSON_CreateString(sprinBuf));

				sprintf(sprinBuf,"%d",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		}
		}
	
		sprintf(sprinBuf,"%d",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// ´òÓ¡JSONÊı¾İ°ü  
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
		len+=LENTH_LEN;//json³¤¶È×îºóÔÙÌîĞ´
		
		// ÊÍ·ÅÄÚ´æ  
		
		
		rt_strcpy((char *)packBuf+len,out);
    len+=rt_strlen(out);
	

		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//¸üĞÂjson³¤¶È
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//¸üĞÂcrc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]=(uint8_t)(TAIL>>8); len++;
		packBuf[len]=(uint8_t)(TAIL);    len++;
		packBuf[len]=0;//len++;//½áÎ² ²¹0
		
		mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%sthreeAxis len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen£º%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}


void threeAxisRead2Send(rt_bool_t netStat)
{					
		int workFlag=RT_FALSE;
		for(int i=0;i<THREEAXIS_485_NUM;i++){
				if(sheet.threeAxiss[i].workFlag==RT_TRUE){
							readThreeTempAcc(i);
							workFlag=RT_TRUE;
					}
			}
			if(workFlag==RT_TRUE){
					rt_kprintf("%s´ò°ü²É¼¯µÄTHREEAXISÊı¾İ\r\n",sign);
					threeAxisJsonPack();//circulaJsonPack();
					if(netStat==RT_TRUE)
							rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
			}
}