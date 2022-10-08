#include  "dataPack.h"


//heartUpStru  heartUp;
extern uint16_t RTU_CRC(uint8_t *puchMsg , uint16_t usDataLen);
uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];  //与net发送buff大小一致  通过邮箱传递给NetTxBuffer 进行发送出去
deviceParazStru  device ={0};
//上行messageID自增  每次打包后自增1
uint32_t upMessIdAdd()
{
	  rt_kprintf("messID:%d\r\n",device.upMessID);
		return device.upMessID++;
}

//后期需要从flash中读取ID
void  deviceIDRead()
{
		rt_strcpy(device.devID,"1000000000001");
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

	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
	  packBuf[len]= (uint8_t)(HEAD);    len++;
	  len+=LENTH_LEN;//json长度最后再填写
	  char str[50]={0};//临时使用的数组
		rt_sprintf(str,"{\"mid\":%lu,",device.upMessID);
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
	
		rt_sprintf(str,"\"id\":\"%s\"}}",device.devID);
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新crc
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		
		packBuf[len]= (uint8_t)(TAIL>>8); len++;
		packBuf[len]= (uint8_t)(TAIL);    len++;
		packBuf[len] =0;//len++;//结尾 补0
		
		device.upHeartMessID =device.upMessID;
		upMessIdAdd();
		rt_kprintf("len:%d\r\n",len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\nlen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;
}
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



