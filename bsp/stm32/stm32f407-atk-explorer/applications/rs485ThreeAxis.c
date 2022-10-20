#include "rs485ThreeAxis.h"

//<<压差式沉降仪 GY-STC-2000>> 默认波特率9600  modbus地址 0xb1(有误) 见机身标签后2位
/*
三轴验证短地址＋04 00 01 00 04

压差验证短地址＋04 00 01 00 02
波特率改为9600，压差和三轴该短地址命令码都是一样的，FF FF 03 0A＋设备完整的长地址＋01＋短地址
*/
//修改设备地址 FF FF 03 0A＋设备完整的长地址＋01＋短地址 
//   FF FF 03 0A 6E 01 20 21 09 08 00 B1 01 01 
//  24+红色，24-黑色，A+蓝色，B-绿色
// 发 01 04 00 01 00 04 A0 09 
// 收 01 04 08 0B CA FE 8D 00 03 03 80 C7 23 
const static char sign[]="[三轴]";

threeAxisStru threeAxis;
static rt_mutex_t threeAxisMutex = RT_NULL;
#define  MSGPOOL_LEN   200 //485数据最大量  大于1k需要修改此处
//队列的定义
static struct  rt_messagequeue threeAxismque;
static uint8_t threeAxisQuePool[MSGPOOL_LEN];  //
static rt_bool_t  recFlag = RT_FALSE; //每个循环发送一次 发完 RT_TRUE 接收完成或者接收超时置为 RT_FALSE


#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //工作环流用到的最大接收buf长度
//#define   LARGE_TIMES    100 //放大倍数  后期如果需要 读取寄存器0x000b 有可能放大10倍
extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];



//打包串口发送 
 void threeAxisUartSend(uint8_t *buf,int len)
{

		rs485UartSend(chanl.threeAxis,buf, len);

}
//串口接收后丢到队列里
rt_err_t threeAxisUartRec(uint8_t dat)
{
	
		if(recFlag==RT_TRUE){
				return rt_mq_send(&threeAxismque, &dat, 1);  //收到数据后就往队列里丢
		}
		else
			  return RT_FALSE;
}

//
//创建环流用到的互斥量和消息队列  main中调用
void  threeAxisMutexQueueCreat()
{
	  threeAxisMutex = rt_mutex_create("threeAxisMutex", RT_IPC_FLAG_FIFO);
    if (threeAxisMutex == RT_NULL)
    {
        rt_kprintf("%screate threeAxisMutex failed.\n",sign);
        return ;
    }
		
//////////////////////////////////消息队列//////////////////////////////////
		
		int result = rt_mq_init(&threeAxismque,"threeAxismque",&threeAxisQuePool[0],1,sizeof(threeAxisQuePool),RT_IPC_FLAG_FIFO);       
		if (result != RT_EOK)
    {
        rt_kprintf("%sinit threeAxismque failed.\n",sign);
        return ;
    }
}

///////////////////////////////////////读写寄存器相关操作////////////////////////////////////////


//与压差式传感器共用一个读命令
extern uint8_t psReadReg(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out);



//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
void readThreeTempAcc()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = psReadReg(SLAVE_ADDR,0X0001,4,buf);
//	  uint16_t ret =0;
	  recFlag = RT_TRUE;
		rt_mutex_take(threeAxisMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		threeAxisUartSend(buf,len);
	  rt_kprintf("%sthreeAxis send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		if(rt_mq_recv(&threeAxismque, buf+len, 1, 3000) == RT_EOK){//第一次接收时间放长点  相应时间有可能比较久
				len++;
		}
		while(rt_mq_recv(&threeAxismque, buf+len, 1, 10) == RT_EOK){//115200 波特率1ms 10个数据
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
		int ret2=modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE);
		if(0 ==  ret2){//刷新读取到的值

        threeAxis.temp	=(buf[offset]<<8)+buf[offset+1];offset+=2;
			  threeAxis.acclrationX = (buf[offset]<<8)+buf[offset+1];offset+=2;
				threeAxis.acclrationY = (buf[offset]<<8)+buf[offset+1];offset+=2;
				threeAxis.acclrationZ = (buf[offset]<<8)+buf[offset+1];offset+=2;
        float temp=(float)((float)threeAxis.temp/100); 

			  rt_kprintf("%stemp:%0.2f*C ACC:X%dmg Y%dmg Z%dmg ok\n",sign,temp,threeAxis.acclrationX,threeAxis.acclrationY,threeAxis.acclrationZ);  
		} 
		else{//读不到给0
				if(ret2==2){
						rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
			  threeAxis.acclrationX	= 0;
			  threeAxis.acclrationY = 0;
			  threeAxis.acclrationY = 0;
			  rt_kprintf("%stemp height read fail\n",sign);
		}
    //
		recFlag = RT_FALSE;
	  rt_mutex_release(threeAxisMutex);
		rt_free(buf);
	  buf=RT_NULL;

}



/////////////////////////////////////////JSON格式打包//////////////////////////////////////////
//温度高度值打包

/*
{
    "mid":1234,
    "packetType�":"CMD_REPORTDATA",  
    "param":
    {
        "identifier":" vibration_meter_monitor",
        "acuId":"100000000000001",
        "deviceId":"1000000000004",�
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

void t3AxisTempAccPack()
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
		
		rt_sprintf(str,"\"deviceId\":\"%s\",",dev[chanl.threeAxis].ID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_strcpy(str,"\"data\":{");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		
	 	//sprintf(str,"test:%0.2f",(float)121/100);				 
		sprintf(str,"\"temp\":\"%0.2f\",",(float)((float)threeAxis.temp/100));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"accelerationX\":\"%d\",",threeAxis.acclrationX);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		sprintf(str,"\"accelerationY\":\"%d\",",threeAxis.acclrationY);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"accelerationZ\":\"%d\",",threeAxis.acclrationZ);	 
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
		
		mcu.repDataMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%sreg len:%d\r\n",sign,len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
}
