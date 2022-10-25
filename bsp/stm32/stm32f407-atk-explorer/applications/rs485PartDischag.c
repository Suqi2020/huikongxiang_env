#include <board.h>
#include "rs485PartDischag.h"
//<<¾Ö·ÅÔÚÏß¼ì²â --GY-JF100-C01>>  GZPD-1000µçÀÂ¾Ö·Å
//ÏìÓ¦Ê±¼ä²»È·¶¨ ×î³¤1.7Ãë ÓĞÊ±ºò³¤ ÓĞÊ±ºò¶Ì
//           Ä¬ÈÏ²¨ÌØÂÊ115200


const static char sign[]="[¾Ö·Å]";
partDischargeStru partDiscStru_p;
#define  MSGPOOL_LEN   1024 //485Êı¾İ×î´óÁ¿  ´óÓÚ1kĞèÒªĞŞ¸Ä´Ë´¦
static rt_mutex_t partDischagMutex = RT_NULL;  //¾Ö·Å»¥³âÁ¿ 

//¶ÓÁĞµÄ¶¨Òå

static uint8_t partDischagQuePool[MSGPOOL_LEN];  //¾Ö·Å¶ÔÁĞ³Ø
struct  rt_messagequeue partDischagmque;//¾Ö·Å¶ÓÁĞ

#define   SLAVE_ADDR     0X01
#define   LENTH          1024  //¹¤×÷»·Á÷ÓÃµ½µÄ×î´ó½ÓÊÕbuf³¤¶È
//#define   LARGE_TIMES    100 //·Å´ó±¶Êı  ºóÆÚÈç¹ûĞèÒª ¶ÁÈ¡¼Ä´æÆ÷0x000b ÓĞ¿ÉÄÜ·Å´ó10±¶
extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];
static rt_bool_t  recFlag = RT_FALSE; //Ã¿¸öÑ­»··¢ËÍÒ»´Î ·¢Íê RT_TRUE ½ÓÊÕÍê³É»òÕß½ÓÊÕ³¬Ê±ÖÃÎª RT_FALSE


 void partDischagUartSend(uint8_t *buf,int len)
{
		rs485UartSend(chanl.partDischag,buf, len);
	
}
//´®¿Ú½ÓÊÕºó¶ªµ½¶ÓÁĞÀï
rt_err_t partDischagUartRec(uint8_t dat)
{
	  if(recFlag==RT_TRUE){
				return rt_mq_send(&partDischagmque, &dat, 1);  //ÊÕµ½Êı¾İºó¾ÍÍù¶ÓÁĞÀï¶ª
		}
		else
			  return RT_FALSE;
}
//´´½¨¾Ö·ÅÓÃµ½µÄ»¥³âÁ¿ºÍÏûÏ¢¶ÓÁĞ
void  partDischagMutexQueueCreat()
{
	  rt_kprintf("%sinit partDischagmque start.\n",sign);
	  partDischagMutex = rt_mutex_create("partDischagMutex", RT_IPC_FLAG_FIFO);
    if (partDischagMutex == RT_NULL)
    {
        rt_kprintf("%screate partDischagMutex failed.\n",sign);
        return ;
    }
		rt_kprintf("%screate partDischagMutex succ.\n",sign);
//////////////////////////////////ÏûÏ¢¶ÓÁĞ//////////////////////////////////
		
		int result = rt_mq_init(&partDischagmque,"partDischagmque",&partDischagQuePool[0],1,sizeof(partDischagQuePool),RT_IPC_FLAG_FIFO);       
		if (result != RT_EOK)
    {
        rt_kprintf("%sinit partDischagmque failed.\n",sign);
        return ;
    }
		rt_kprintf("%sinit partDischagmque succ.\n",sign);
}



//¶ÁÈ¡·ùÖµ ÆµÂÊ ·Åµç×ÜÄÜÁ¿
//01 03 0300 0006 C58C
//01 03 24 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 7B A1 

void readPdFreqDischarge()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(SLAVE_ADDR,0x0300,18,buf);
//	  uint16_t ret =0;
	  recFlag = RT_TRUE;
		rt_mutex_take(partDischagMutex,RT_WAITING_FOREVER);
	  //485·¢ËÍbuf  len  µÈ´ımodbus»ØÓ¦
		partDischagUartSend(buf,len);
	  rt_kprintf("%sPdFreqDiach send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		if(rt_mq_recv(&partDischagmque, buf+len, 1, 3000) == RT_EOK){//µÚÒ»´Î½ÓÊÕÊ±¼ä·Å³¤µã  ÏàÓ¦Ê±¼äÓĞ¿ÉÄÜ±È½Ï¾Ã
				len++;
		}
		while(rt_mq_recv(&partDischagmque, buf+len, 1, 10) == RT_EOK){//115200 ²¨ÌØÂÊ1ms 10¸öÊı¾İ
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
		//ÌáÈ¡»·Á÷Öµ µÚÒ»²½ÅĞ¶Ïcrc µÚ¶ş²¿ÌáÈ¡
		modDev[chanl.partDischag].offline=RT_FALSE;
		int ret2= modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE);
		if(0 ==  ret2){//Ë¢ĞÂ¶ÁÈ¡µ½µÄÖµ


				partDiscStru_p.amplitudeA=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				partDiscStru_p.freqA     =(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  partDiscStru_p.dischargeA=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			
				partDiscStru_p.amplitudeB=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				partDiscStru_p.freqB     =(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  partDiscStru_p.dischargeB=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			
				partDiscStru_p.amplitudeC=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				partDiscStru_p.freqC     =(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  partDiscStru_p.dischargeC=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  rt_kprintf("%sPdFreqDiach read ok\n",sign);
		} 
		else{//¶Á²»µ½¸ø0
				if(ret2==2){
						//rt_kprintf("%sERR:Çë¼ì²é485½ÓÏß»òÕß¹©µç\r\n",sign);
						modDev[chanl.partDischag].offline=RT_TRUE;
				}
				partDiscStru_p.amplitudeA=0;
				partDiscStru_p.freqA     =0;
			  partDiscStru_p.dischargeA=0;
			
				partDiscStru_p.amplitudeB=0;
				partDiscStru_p.freqB     =0;
			  partDiscStru_p.dischargeB=0;
			
				partDiscStru_p.amplitudeC=0;
				partDiscStru_p.freqC     =0;
			  partDiscStru_p.dischargeC=0;
			  rt_kprintf("%sPdFreqDiach read fail\n",sign);
		}
    //
		recFlag = RT_FALSE;
	  rt_mutex_release(partDischagMutex);
		rt_free(buf);
	  buf=RT_NULL;

}

//¶ÁÈ¡¾Ö·Å±¨¾¯×´Ì¬ 
//·¢ 01 01 0001 0008 6C0C
//ÊÕ 01 01 01 00 51 88 

rt_bool_t readPartDischgWarning()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadBitReg(SLAVE_ADDR,0x0001,8,buf);//¶ÁÈ¡8¸öbit
//	  uint16_t ret =0;
	  recFlag = RT_TRUE;
		rt_mutex_take(partDischagMutex,RT_WAITING_FOREVER);
	  //485·¢ËÍbuf  len  µÈ´ımodbus»ØÓ¦
		partDischagUartSend(buf,len);
	  rt_kprintf("%sreadPd send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		if(rt_mq_recv(&partDischagmque, buf+len, 1, 3000) == RT_EOK){//µÚÒ»´Î½ÓÊÕÊ±¼ä·Å³¤µã  ÏàÓ¦Ê±¼äÓĞ¿ÉÄÜ±È½Ï¾Ã
				len++;
		}
		while(rt_mq_recv(&partDischagmque, buf+len, 1, 10) == RT_EOK){//115200 ²¨ÌØÂÊ1ms 10¸öÊı¾İ
				len++;
		}
		rt_kprintf("%srec:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		modDev[chanl.partDischag].offline=RT_FALSE;
		//ÌáÈ¡»·Á÷Öµ µÚÒ»²½ÅĞ¶Ïcrc µÚ¶ş²¿ÌáÈ¡
		int ret2=modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE);
		if(0 == ret2){//Ë¢ĞÂ¶ÁÈ¡µ½µÄÖµ
     
			  partDiscStru_p.alarm.a=(buf[offset]>>0)&0x01;
				partDiscStru_p.alarm.b=(buf[offset]>>1)&0x01;
			  partDiscStru_p.alarm.c=(buf[offset]>>2)&0x01;
			  rt_kprintf("%sÌáÈ¡alarm OK\r\n",sign);
		} 
		else{
				if(ret2==2){
						//rt_kprintf("%sERR:Çë¼ì²é485½ÓÏß»òÕß¹©µç\r\n",sign);
					  modDev[chanl.partDischag].offline=RT_TRUE;
				}
				partDiscStru_p.alarm.a=0;
				partDiscStru_p.alarm.b=0;
				partDiscStru_p.alarm.c=0;
			  rt_kprintf("%sÌáÈ¡alarm fail\r\n",sign);
		}
    //
	  rt_mutex_release(partDischagMutex);
		rt_free(buf);
	  buf=RT_NULL;
		recFlag = RT_FALSE;
		if(partDiscStru_p.alarm.a||partDiscStru_p.alarm.b||partDiscStru_p.alarm.c)
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
void  partDisDataPack()
{
 
	  memset(packBuf,0,sizeof(packBuf));
		int len=0;
    //head+lenth
	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
	  packBuf[len]= (uint8_t)(HEAD);    len++;
	  len+=LENTH_LEN;//json³¤¶È×îºóÔÙÌîĞ´
	  //json
	  char str[50]={0};//ÁÙÊ±Ê¹ÓÃµÄÊı×é
		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"packetType\":\"CMD_REPORTDATA\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"param\":{");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);

		rt_strcpy(str,"\"identifier\":\"partial_discharge_monitor\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_sprintf(str,"\"deviceId\":\"%s\",",dev[chanl.partDischag].ID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_strcpy(str,"\"data\":{");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		
	 	//sprintf(str,"test:%0.2f",(float)121/100);				 
		sprintf(str,"\"pdA\":\"%u\",",partDiscStru_p.amplitudeA);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"freqA\":\"%u\",",partDiscStru_p.freqA);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"dischargeDataA\":\"%u\",",partDiscStru_p.dischargeA);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"prpdDataA\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"prpsDataA\":\"\","); 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		sprintf(str,"\"pdB\":\"%u\",",partDiscStru_p.amplitudeA);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"freqB\":\"%u\",",partDiscStru_p.freqA);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"dischargeDataB\":\"%u\",",partDiscStru_p.dischargeA);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"prpdDataB\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"prpsDataB\":\"\","); 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);		

		sprintf(str,"\"pdC\":\"%u\",",partDiscStru_p.amplitudeA);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"freqC\":\"%u\",",partDiscStru_p.freqA);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"dischargeDataC\":\"%u\",",partDiscStru_p.dischargeA);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"prpdDataC\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"prpsDataC\":\"\","); 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);		
		
		rt_sprintf(str,"\"monitoringTime\":\"%lu\"}},",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//¸üĞÂjson³¤¶È
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//¸üĞÂcrc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]= (uint8_t)(TAIL>>8); len++;
		packBuf[len]= (uint8_t)(TAIL);    len++;
		packBuf[len] =0;//len++;//½áÎ² ²¹0
		
		mcu.repDataMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%sreg len:%d\r\n",sign,len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\n%slen£º%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
	
}

