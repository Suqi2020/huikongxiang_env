#include "rs485Gas.h"
//一氧化碳默认地址04 波特率9600
//04 03 00 02 00 02 65 9E
//04 03 04 00 00 00 00 AF 33 
//每次利用co读取定时器来读取4种气体并实现上传 
//4种气体在一起读取 所以前三个不使用 只在CO定时器地方读取并打包发送  关闭时候只需要关闭CO就可以把所有气体全部关闭

const static char sign[]="[一氧化碳]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //工作环流用到的最大接收buf长度




//打包串口发送 
static void coUartSend(uint8_t *buf,int len)
{
		rs485UartSend(modbusFlash[CO].useUartNum,buf, len);
}



//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
void readCO()
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(modbusFlash[CO].slaveAddr,0X0002,2,buf);
		rt_mutex_take(uartDev[modbusFlash[CO].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		coUartSend(buf,len);
	  rt_kprintf("%sCO send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		
		if(rt_mq_recv(uartDev[modbusFlash[CO].useUartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//第一次接收时间放长点  相应时间有可能比较久
				len++;
		}
		while(rt_mq_recv(uartDev[modbusFlash[CO].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 波特率1ms 10个数据
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
		uartDev[modbusFlash[CO].useUartNum].offline=RT_FALSE;
		int ret2=modbusRespCheck(modbusFlash[CO].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值
        gas.co	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
        float cont=(float)((float)gas.co	/1000);
			  rt_kprintf("%s浓度值:%0.2fmol/Lread ok\n",sign,cont);  
		} 
		else{//读不到给0
				if(ret2==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
					  uartDev[modbusFlash[CO].useUartNum].offline=RT_TRUE;
				}
			  gas.co	=0;
			  rt_kprintf("%s read fail\n",sign);
		}
	  rt_mutex_release(uartDev[modbusFlash[CO].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}


