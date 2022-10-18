#include "rs485PressSettl.h"

//<<—π≤Ó Ω≥¡Ωµ“« GY-STC-2000>> ƒ¨»œ≤®Ãÿ¬ 9600  modbusµÿ÷∑ 0xb1(”–ŒÛ) º˚ª˙…Ì±Í«©∫Û2Œª
//–ﬁ∏ƒ…Ë±∏µÿ÷∑ FF FF 03 0A£´…Ë±∏ÕÍ’˚µƒ≥§µÿ÷∑£´01£´∂Ãµÿ÷∑ 
//   FF FF 03 0A 6E 01 20 21 09 08 00 B1 01 01


static rt_mutex_t pressSettlMutex = RT_NULL;
#define  MSGPOOL_LEN   200 //485 ˝æ›◊Ó¥Û¡ø  ¥Û”⁄1k–Ë“™–ﬁ∏ƒ¥À¥¶
//∂”¡–µƒ∂®“Â
static struct  rt_messagequeue pressSettlmque;
static uint8_t pressSettlQuePool[MSGPOOL_LEN];  //
static rt_bool_t  recFlag = RT_FALSE; //√ø∏ˆ—≠ª∑∑¢ÀÕ“ª¥Œ ∑¢ÕÍ RT_TRUE Ω” ’ÕÍ≥…ªÚ’ﬂΩ” ’≥¨ ±÷√Œ™ RT_FALSE


#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //π§◊˜ª∑¡˜”√µΩµƒ◊Ó¥ÛΩ” ’buf≥§∂»
//#define   LARGE_TIMES    100 //∑≈¥Û±∂ ˝  ∫Û∆⁄»Áπ˚–Ë“™ ∂¡»°ºƒ¥Ê∆˜0x000b ”–ø…ƒ‹∑≈¥Û10±∂
extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];

pressSettlStru pressSettle;

static char num=2;//µ⁄3¬∑485
//¥Ú∞¸¥Æø⁄∑¢ÀÕ 
static void pressSettlUartSend(uint8_t *buf,int len)
{

		UART6_485_SEND;
	  HAL_UART_Transmit(&huart6,(uint8_t *)buf,len,1000);
		UART6_485_REC;

}
//¥Æø⁄Ω” ’∫Û∂™µΩ∂”¡–¿Ô
rt_err_t pressSettlUartRec(uint8_t dat)
{
	
		if(recFlag==RT_TRUE){
				return rt_mq_send(&pressSettlmque, &dat, 1);  // ’µΩ ˝æ›∫ÛæÕÕ˘∂”¡–¿Ô∂™
		}
		else
			  return RT_FALSE;
}

//
//¥¥Ω®ª∑¡˜”√µΩµƒª•≥‚¡ø∫Õœ˚œ¢∂”¡–
void  pressSettlMutexQueueCreat()
{
	  pressSettlMutex = rt_mutex_create("pressSettlMutex", RT_IPC_FLAG_FIFO);
    if (pressSettlMutex == RT_NULL)
    {
        rt_kprintf("create pressSettlMutex failed.\n");
        return ;
    }
		
//////////////////////////////////œ˚œ¢∂”¡–//////////////////////////////////
		
		int result = rt_mq_init(&pressSettlmque,"pressSettlmque",&pressSettlQuePool[0],1,sizeof(pressSettlQuePool),RT_IPC_FLAG_FIFO);       
		if (result != RT_EOK)
    {
        rt_kprintf("init pressSettlmque failed.\n");
        return ;
    }
}

///////////////////////////////////////∂¡–¥ºƒ¥Ê∆˜œ‡πÿ≤Ÿ◊˜////////////////////////////////////////



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



//∑¢ 1A 04 00 01 00 02 23 E0
// ’ 1A 04 04 0B 1B 00 1C 23 6F
void readPSTempHeight()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = psReadReg(SLAVE_ADDR,0X0001,2,buf);
//	  uint16_t ret =0;
	  recFlag = RT_TRUE;
		rt_mutex_take(pressSettlMutex,RT_WAITING_FOREVER);
	  //485∑¢ÀÕbuf  len  µ»¥˝modbusªÿ”¶
		pressSettlUartSend(buf,len);
	  rt_kprintf("pressSettl send:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		if(rt_mq_recv(&pressSettlmque, buf+len, 1, 3000) == RT_EOK){//µ⁄“ª¥ŒΩ” ’ ±º‰∑≈≥§µ„  œ‡”¶ ±º‰”–ø…ƒ‹±»Ωœæ√
				len++;
		}
		while(rt_mq_recv(&pressSettlmque, buf+len, 1, 10) == RT_EOK){//115200 ≤®Ãÿ¬ 1ms 10∏ˆ ˝æ›
				len++;
		}
		rt_kprintf(" rec:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//Ã·»°ª∑¡˜÷µ µ⁄“ª≤Ω≈–∂œcrc µ⁄∂˛≤øÃ·»°
		
		if(0 ==  modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE)){//À¢–¬∂¡»°µΩµƒ÷µ

        pressSettle.temp	=(buf[offset]<<8)+buf[offset+1];offset+=2;
			  pressSettle.height=(buf[offset]<<8)+buf[offset+1];

			  rt_kprintf("ps temp height read ok\n");
		} 
		else{//∂¡≤ªµΩ∏¯0
			  pressSettle.temp	=0;
			  pressSettle.height=0;
			  rt_kprintf("ps temp height read fail\n");
		}
    //
		recFlag = RT_FALSE;
	  rt_mutex_release(pressSettlMutex);
		rt_free(buf);
	  buf=RT_NULL;

}



/////////////////////////////////////////JSON∏Ò Ω¥Ú∞¸//////////////////////////////////////////
//Œ¬∂»∏ﬂ∂»÷µ¥Ú∞¸



//{
//        "mid":1234,
//        "packetType†":"CMD_REPORTDATA",    
//        "param":
//        {
//                "identifier":"pressure_settler_monitor",
//                "acuId":"100000000000001",
//                "deviceId":"1000000000004",†
//                "data":
//                {
//                        "temp":"22.75", //?
//                        "height":"0.8",//mm
//                        "monitoringTime":"1655172531937"
//                }
//        },
//        "timestamp":"1655172531937"
//}


void PSTempHeightPack()
{
 memset(packBuf,0,sizeof(packBuf));
		int len=0;
    //head+lenth
	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
	  packBuf[len]= (uint8_t)(HEAD);    len++;
	  len+=LENTH_LEN;//json≥§∂»◊Ó∫Û‘ŸÃÓ–¥
	  //json
	  char str[50]={0};//¡Ÿ ± π”√µƒ ˝◊È
		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"packetType\":\"CMD_REPORTDATA\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"param\":{");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);

		rt_strcpy(str,"\"identifier\":\"pressure_settler_monitor\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_sprintf(str,"\"deviceId\":\"%s\",",dev[num].ID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_strcpy(str,"\"data\":{");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		
	 	//sprintf(str,"test:%0.2f",(float)121/100);				 
		sprintf(str,"\"temp\":\"%02f\",",(float)(pressSettle.temp/100));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"height\":\"%01f\",",(float)(pressSettle.height/10));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

	
		
		rt_sprintf(str,"\"monitoringTime\":\"%lu\"}},",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//∏¸–¬json≥§∂»
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//∏¸–¬crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]= (uint8_t)(TAIL>>8); len++;
		packBuf[len]= (uint8_t)(TAIL);    len++;
		packBuf[len] =0;//len++;//Ω·Œ≤ ≤π0
		
		mcu.repDataMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("reg len:%d\r\n",len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\nlen£∫%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
}
