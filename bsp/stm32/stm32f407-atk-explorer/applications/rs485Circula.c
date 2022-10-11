
#include "rs485Circula.h"
//调整RS485串口需要调整 UART5_IRQHandler以及HAL_UART_Transmit发送串口  还需要更换对应串口的队列
//<<公众环流>>  目前测试只采集环流值
//  1、读取环流值   
//  2、读取报警标志 
//  3、配置报警阈值 
//  4、读取报警阈值
//  5、设置采集间隔
//  6、读取采集间隔
// 9600波特率 注意修改此处 rt_mq_recv(&rs485_2mque, &buf, sizeof(buf), 2)
#define   SLAVE_ADDR     0X01
#define   LENTH          50  //工作环流用到的最大接收buf长度
#define   LARGE_TIMES    100 //放大倍数  后期如果需要 读取寄存器0x000b 有可能放大10倍
extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];

CIRCURStru  cirCurStru_p;
//读取环流值和报警信息
void readCirCurrAndWaring()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  //uint8_t   buf[100]
	  uint16_t len = modbusReadReg(SLAVE_ADDR,0x0023,12,buf);
	  //485发送buf  len  等待modbus回应
		UART2_485_SEND;
	  HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,1000);
		UART2_485_REC;
	  rt_kprintf("modbus2 send:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
	  rt_mutex_take(rs485_2Mutex,RT_WAITING_FOREVER);
    len=0;
		while(rt_mq_recv(&rs485_2mque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		rt_kprintf("modbus2 rec:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//提取环流值 第一步判断crc 第二部提取
		
		if(RT_TRUE ==  modbusReadRespCheck(SLAVE_ADDR,buf,len)){//刷新读取到的值
				cirCurStru_p.circlCurA=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				cirCurStru_p.circlCurB=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				cirCurStru_p.circlCurC=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				cirCurStru_p.circlCurD=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  cirCurStru_p.warningA	=	buf[offset];	offset+=1;
			  cirCurStru_p.warningB	=	buf[offset];	offset+=1;
			  cirCurStru_p.warningC	=	buf[offset];	offset+=1;
			  cirCurStru_p.warningD	=	buf[offset];	offset+=1;
			  rt_kprintf("提取电流、报警值成功\r\n");
		} 

	  rt_mutex_release(rs485_2Mutex);
		rt_free(buf);
	  buf=RT_NULL;
}


rt_bool_t waringcheck()
{
		if((cirCurStru_p.warningA)||(cirCurStru_p.warningA)||(cirCurStru_p.warningA)|(cirCurStru_p.warningA)){
			 rt_kprintf("ERR:环流值过高 触发报警 \n\r");
				return RT_TRUE;
		}
		return RT_FALSE;
}
//数据主动定时上行的打包  打包前需要调用 readCirCurrAndWaring()
/*
{
     "mid":1234,
     "packetType":"CMD_REPORTDATA",
     "param":
     {
          "identifier":"grounding_current_monitor",
          "acuId":"100000000000001",
          "deviceId":"1000000000003",?
          "data":
          {
               "earthCurA":"22.2",
               "runCurA":"",
               "loadRatioA":"",
               "earthCurB":"22.2",
               "runCurB":"",
               "loadRatioB":"",
               "earthCurC":"22.2",
               "runCurC":"",
               "loadRatioC":"",
               "monitoringTime":"1655172531937"
          }
     },
     "timestamp":"1655172531937"
}
*/
//电流值打包
uint16_t 	cirCulaDataPack()
{
	  char num=0;//第1路485
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
		
		rt_strcpy(str,"\"packetType\":\"CMD_REPORTDATA\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"param\":{");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);

		rt_strcpy(str,"\"identifier\":\"grounding_current_monitor\",");
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
		sprintf(str,"\"earthCurA\":\"%0.2f\"}},",(float)(cirCurStru_p.circlCurA/LARGE_TIMES));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"runCurA\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"loadRatioA\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		sprintf(str,"\"earthCurB\":\"%0.2f\"}},",(float)(cirCurStru_p.circlCurB/LARGE_TIMES));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"runCurB\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"loadRatioB\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		sprintf(str,"\"earthCurC\":\"%0.2f\"}},",(float)(cirCurStru_p.circlCurC/LARGE_TIMES));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"runCurC\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"loadRatioC\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_sprintf(str,"\"monitoringTime\":\"%lu\"}},",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
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
		
		mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("reg len:%d\r\n",len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\nlen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;
}
//告警信息的打包  readCirCurrAndWaring()
void  waringEventPack()
{
		rt_kprintf("后期加入 \n\r");
		
}




