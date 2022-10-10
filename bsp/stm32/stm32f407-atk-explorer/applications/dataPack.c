#include  "dataPack.h"


//heartUpStru  heartUp;
extern uint16_t RTU_CRC(uint8_t *puchMsg ,uint16_t usDataLen);
uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];  //与net发送buff大小一致  通过邮箱传递给NetTxBuffer 进行发送出去
mcuParazStru mcu ={0};
#define  NUM  2
rs485ParaStru dev[NUM];//目前4路485设备
//上行messageID自增  每次打包后自增1
uint32_t upMessIdAdd()
{
	  rt_kprintf("messID:%d\r\n",mcu.upMessID);
		return mcu.upMessID++;
}

//后期需要从flash中读取ID
void  devIDRead()
{
		rt_strcpy(mcu.devID  ,"1000000000001");
		rt_strcpy(dev[0].ID,"A000000000001");
		rt_strcpy(dev[0].model,"GY280");
	  rt_strcpy(dev[0].type,"1");
	  rt_strcpy(dev[0].name,"环境监测");
		rt_strcpy(dev[1].ID,"A000000000002");
		rt_strcpy(dev[1].model,"GY281");
	  rt_strcpy(dev[1].type,"2");
	  rt_strcpy(dev[1].name,"接地环流");
//		rt_strcpy(dev[2].ID,"A000000000003");
//		rt_strcpy(dev[3].ID,"A000000000004");
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
		rt_kprintf("heart len:%d\r\n",len);
		
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
uint16_t devRegPack()
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
		
		rt_strcpy(str,"\"packetType\":\"CMD_DEVICE_REGISTER\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"params\":[");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
	
		for(int i=0;i<NUM;i++){
				rt_sprintf(str,"{\"model\":\"%s\",",dev[i].model);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
			
				rt_sprintf(str,"\"name\":\"%s\",",dev[i].name);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
			
				rt_sprintf(str,"\"deviceId\":\"%s\",",dev[i].ID);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
			
				rt_sprintf(str,"\"ip\":\"%s\",",dev[i].ip);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
			
				rt_sprintf(str,"\"port\":\"%s\",",dev[i].port);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
			
				rt_sprintf(str,"\"type\":\"%s\"}",dev[i].type);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
				if(i+1==NUM){//拷贝 ] 号
						rt_strcpy((char *)packBuf+len,"],");
						len+=2;
				}
				else{//拷贝 , 号
						rt_strcpy((char *)packBuf+len,",");
						len++;
				}
		}
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


//环流 公众公司
uint16_t rs485_1DataPack()
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
		
		rt_strcpy(str,"\"packetType\":\"CMD_DEVICE_REGISTER\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"param\":{");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);

		rt_strcpy(str,"\"identifier\":\"environment_monitor\",");
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
		
		
		rt_strcpy(str,"\"temperature\":\"22.2\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"humidity\":\"80.1\",");
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

