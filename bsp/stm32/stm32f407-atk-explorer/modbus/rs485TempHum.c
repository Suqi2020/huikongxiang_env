#include "board.h"
//#include "stmflash.h"
//水位默认地址02  波特率9600  量程0~49米

const static char sign[]="[温湿度]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //工作环流用到的最大接收buf长度

typedef struct{
	float temp;
	float hum; 
}thStru;
thStru thum[TEMPHUM_485_NUM];
//float temp[TEMPHUM_485_NUM];
//float hum[TEMPHUM_485_NUM]; 
//打包串口发送 
static void tempHumUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.tempHum[num].useUartNum,buf, len);
}



//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
void readTempHum(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.tempHum[num].slaveAddr,0X0000,READ_04,2,buf);
		rt_mutex_take(uartDev[sheet.tempHum[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		tempHumUartSend(num,(uint8_t *)buf,len);
	  rt_kprintf("%stemphum send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		
		if(rt_mq_recv(uartDev[sheet.tempHum[num].useUartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//第一次接收时间放长点  相应时间有可能比较久
				len++;
		}
		while(rt_mq_recv(uartDev[sheet.tempHum[num].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 波特率1ms 10个数据
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
		//uartDev[modbusFlash[TEMPHUM].useUartNum].offline=RT_FALSE;
		int ret2=modbusRespCheck(sheet.tempHum[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值
        uint16_t read_temp	=(buf[offset+2]<<8)+buf[offset+3];offset+=2;
			  uint16_t read_hum	=(buf[offset+2]<<8)+buf[offset+3];offset+=2;
			  rt_bool_t sig=RT_FALSE;
        if(read_temp&0x8000){//负数
						read_temp=read_temp&0x8000;
						read_temp=0xffff-read_temp+1;
					  sig=RT_TRUE;
				}
				thum[num].temp=(float)read_temp/10;
				thum[num].hum=(float)read_hum/10;
				if(sig==RT_TRUE)
					rt_kprintf("%s温度:-%0.1fC 湿度:%0.1f\n",sign,thum[num].temp,thum[num].hum); 
				else
					rt_kprintf("%s温度:%0.1fC 湿度:%0.1f\n",sign,thum[num].temp,thum[num].hum);   
		} 
		else{//读不到给0
				if(ret2==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
//					  uartDev[modbusFlash[TEMPHUM].useUartNum].offline=RT_TRUE;
				}
				thum[num].temp=0;
				thum[num].hum=0;
			  rt_kprintf("%s read fail\n",sign);
		}
	  rt_mutex_release(uartDev[sheet.tempHum[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}


