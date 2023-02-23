#include "board.h"
//#include "stmflash.h"
//<<Ñ¹²îÊ½³Á½µÒÇ GY-STC-2000>> Ä¬ÈÏ²¨ÌØÂÊ9600  modbusµØÖ· 0xb1(ÓÐÎó) ¼û»úÉí±êÇ©ºó2Î»   Ð­ÒéÎÄµµÓÐÎó
/*
ÈýÖáÑéÖ¤¶ÌµØÖ·£«04 00 01 00 04

Ñ¹²îÑéÖ¤¶ÌµØÖ·£«04 00 01 00 02
²¨ÌØÂÊ¸ÄÎª9600£¬Ñ¹²îºÍÈýÖá¸Ã¶ÌµØÖ·ÃüÁîÂë¶¼ÊÇÒ»ÑùµÄ£¬FF FF 03 0A£«Éè±¸ÍêÕûµÄ³¤µØÖ·£«01£«¶ÌµØÖ·
*/
//ÐÞ¸ÄÉè±¸µØÖ· FF FF 03 0A£«Éè±¸ÍêÕûµÄ³¤µØÖ·£«01£«¶ÌµØÖ· 
//   FF FF 03 0A 6E 01 20 21 09 08 00 B1 01 01 
//  24+ºìÉ«£¬24-ºÚÉ«£¬A+À¶É«£¬B-ÂÌÉ«
// ·¢ 01 04 00 01 00 04 A0 09 
// ÊÕ 01 04 08 0B CA FE 8D 00 03 03 80 C7 23 
// ·¢                   01 04 00 01 00 04 A0 09 
//[·ÀÍâÆÆ]threeAxis send:9  4  0  1  0  4 a1 41 
// ÊÕ         01 04 08 0B CA FE 8D 00 03 03 80 C7 23 
//[·ÀÍâÆÆ]rec: 9  4  6  b 73  0 9a  0 17 fc 32 69 b4
typedef struct{
		float temp;
	  uint16_t acclrationX;
		uint16_t acclrationY;
		uint16_t acclrationZ;
	  uint8_t  respStat;
}threeAxisStru;
const static char sign[]="[·ÀÍâÆÆ]";
threeAxisStru threeAxisp[THREEAXIS_485_NUM];
//·µ»ØÈýÖáµÄÍ¨Ñ¶×´Ì¬ true--Í¨Ñ¶³É¹¦ false--Í¨Ñ¶Ê§°Ü
int threeAxisState(int i)
{
		return threeAxisp[i].respStat;
}
//#define   SLAVE_ADDR     0X02 
#define   LENTH          50  //¹¤×÷»·Á÷ÓÃµ½µÄ×î´ó½ÓÊÕbuf³¤¶È


//´ò°ü´®¿Ú·¢ËÍ 
static void threeAxisUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.threeAxiss[num].useUartNum,buf, len);
}

///////////////////////////////////////¶ÁÐ´¼Ä´æÆ÷Ïà¹Ø²Ù×÷////////////////////////////////////////

//ÓëÑ¹²îÊ½´«¸ÐÆ÷¹²ÓÃÒ»¸ö¶ÁÃüÁî
extern uint8_t psReadReg(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out);

//ÈýÖá±È½ÏãÐÖµ²¢ÉèÖÃÏàÓ¦µÄflag±ê¼Ç
static void threeAccCheckSetFlag(int num)
{
	
		if(threeAxisp[num].temp>=sheet.modbusThreAxis[num].tempUpLimit)
				inpoutpFlag.modbusThreAxis[num].tempUpFlag=true;
		else
			  inpoutpFlag.modbusThreAxis[num].tempUpFlag=false;
		if(threeAxisp[num].temp<=sheet.modbusThreAxis[num].tempLowLimit)
				inpoutpFlag.modbusThreAxis[num].tempLowFlag=true;
		else
			  inpoutpFlag.modbusThreAxis[num].tempLowFlag=false;
		
		if(threeAxisp[num].acclrationX>=sheet.modbusThreAxis[num].accXUpLimit)
				inpoutpFlag.modbusThreAxis[num].accXUpFlag=true;
		else
			  inpoutpFlag.modbusThreAxis[num].accXUpFlag=false;
		if(threeAxisp[num].acclrationX<=sheet.modbusThreAxis[num].accXLowLimit)
				inpoutpFlag.modbusThreAxis[num].accXLowFlag=true;
		else
			  inpoutpFlag.modbusThreAxis[num].accXLowFlag=false;

		if(threeAxisp[num].acclrationY>=sheet.modbusThreAxis[num].accYUpLimit)
				inpoutpFlag.modbusThreAxis[num].accYUpFlag=true;
		else
			  inpoutpFlag.modbusThreAxis[num].accYUpFlag=false;
		if(threeAxisp[num].acclrationY<=sheet.modbusThreAxis[num].accYLowLimit)
				inpoutpFlag.modbusThreAxis[num].accYLowFlag=true;
		else
			  inpoutpFlag.modbusThreAxis[num].accYLowFlag=false;

		if(threeAxisp[num].acclrationZ>=sheet.modbusThreAxis[num].accZUpLimit)
				inpoutpFlag.modbusThreAxis[num].accZUpFlag=true;
		else
			  inpoutpFlag.modbusThreAxis[num].accZUpFlag=false;
		if(threeAxisp[num].acclrationZ<=sheet.modbusThreAxis[num].accZLowLimit)
				inpoutpFlag.modbusThreAxis[num].accZLowFlag=true;
		else
			  inpoutpFlag.modbusThreAxis[num].accZLowFlag=false;
}
//·¢ 1A 04 00 01 00 02 23 E0
//ÊÕ 1A 04 04 0B 1B 00 1C 23 6F
//Í¨¹ý485½Ó¿Ú¶ÁÈ¡ÈýÖáµÄÖµ
void readThreeTempAcc(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = psReadReg(sheet.threeAxiss[num].slaveAddr,0X0001,4,buf);
		rt_mutex_take(uartDev[sheet.threeAxiss[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485·¢ËÍbuf  len  µÈ´ýmodbus»ØÓ¦
		threeAxisUartSend(num,buf,len);
	  rt_kprintf("%sthreeAxis send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		while(rt_mq_recv(uartDev[sheet.threeAxiss[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ²¨ÌØÂÊ1ms 10¸öÊý¾Ý
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
		//ÌáÈ¡»·Á÷Öµ µÚÒ»²½ÅÐ¶Ïcrc µÚ¶þ²¿ÌáÈ¡
		int ret2=modbusRespCheck(sheet.threeAxiss[num].slaveAddr,buf,len,RT_TRUE);
		if(0 ==  ret2){//Ë¢ÐÂ¶ÁÈ¡µ½µÄÖµ
        int temp	=(buf[offset]<<8)+buf[offset+1];offset+=2;
			  threeAxisp[num].acclrationX = (buf[offset]<<8)+buf[offset+1];offset+=2;
				threeAxisp[num].acclrationY = (buf[offset]<<8)+buf[offset+1];offset+=2;
				threeAxisp[num].acclrationZ = (buf[offset]<<8)+buf[offset+1];offset+=2;
        threeAxisp[num].temp=(float)temp/100; 
			  threeAxisp[num].respStat=1;
				threeAccCheckSetFlag(num);
			  rt_kprintf("%stemp:%0.2f*C ACC:X%dmg Y%dmg Z%dmg ok\n",sign,threeAxisp[num].temp,threeAxisp[num].acclrationX,threeAxisp[num].acclrationY,threeAxisp[num].acclrationZ);  
		} 
		else{//¶Á²»µ½¸ø0
				if(ret2==2){
//					  uartDev[modbusFlash[THREEAXIS].useUartNum].offline=RT_TRUE;
				}
				threeAxisp[num].respStat=0;
				threeAxisp[num].temp=0;
			  threeAxisp[num].acclrationX	= 0;
			  threeAxisp[num].acclrationY = 0;
			  threeAxisp[num].acclrationY = 0;
			  rt_kprintf("%stemp height read fail\n",sign);
		}
//		threeAccCheckSetFlag(num);
	  rt_mutex_release(uartDev[sheet.threeAxiss[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

		
		

		
}

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
//ÈýÖáÏà¹ØÖµÍ¨¹ýjson´ò°ü
static uint16_t threeAxisJsonPack()
{

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
		cJSON_AddStringToObject(root, "identifier","vibration_monitor");
	  cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20¸ö×Ö·û´®³¤¶È ¹»ÓÃÁË
		
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
				
				sprintf(sprinBuf,"%d",threeAxisp[i].respStat);
				cJSON_AddItemToObject(nodeobj,"responseStatus",cJSON_CreateString(sprinBuf));
				
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

				sprintf(sprinBuf,"%llu",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		}
		}
	
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// ´òÓ¡JSONÊý¾Ý°ü  
//		out = cJSON_Print(root);
//		if(out!=NULL){
//			for(int i=0;i<rt_strlen(out);i++)
//					rt_kprintf("%c",out[i]);
//			rt_kprintf("\n");
//			rt_free(out);
//			out=NULL;
//		}
//		if(root!=NULL){
//			cJSON_Delete(root);
//			out=NULL;
//		}

		//´ò°ü
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json³¤¶È×îºóÔÙÌîÐ´
		
		// ÊÍ·ÅÄÚ´æ  
		

	
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
		mcu.repDataMessID =mcu.upMessID;
		//mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%sthreeAxis len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen£º%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}

//ÈýÖá¶ÁÈ¡modbusÊý¾Ý²¢´ò°ü·¢ËÍ ¸øÆäËüº¯Êýµ÷ÓÃ
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
					rt_kprintf("%s´ò°ü²É¼¯µÄTHREEAXISÊý¾Ý\r\n",sign);
					threeAxisJsonPack();//circulaJsonPack();
					if(netStat==RT_TRUE)
							rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
			}
}
