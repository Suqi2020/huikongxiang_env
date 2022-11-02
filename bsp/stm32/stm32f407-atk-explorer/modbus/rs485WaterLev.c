#include "board.h"
//水位默认地址02  波特率9600  量程0~49米

const static char sign[]="[水位]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //工作环流用到的最大接收buf长度

float waterLev=0;
//打包串口发送 
static void waterLevUartSend(uint8_t *buf,int len)
{
		rs485UartSend(modbusFlash[WATERLEVEL].useUartNum,buf, len);
}



//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
void readWaterLevel()
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(modbusFlash[WATERLEVEL].slaveAddr,0X0002,READ_04,2,buf);
		rt_mutex_take(uartDev[modbusFlash[WATERLEVEL].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		waterLevUartSend(buf,len);
	  rt_kprintf("%swater send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		
		if(rt_mq_recv(uartDev[modbusFlash[WATERLEVEL].useUartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//第一次接收时间放长点  相应时间有可能比较久
				len++;
		}
		while(rt_mq_recv(uartDev[modbusFlash[WATERLEVEL].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 波特率1ms 10个数据
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
		uartDev[modbusFlash[WATERLEVEL].useUartNum].offline=RT_FALSE;
		int ret2=modbusRespCheck(modbusFlash[WATERLEVEL].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值
        uint32_t read	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
        extern float write_hex_to_float(uint32_t number);
			  float waterLev= write_hex_to_float(read);
         waterLev=(float)(waterLev	/9.8);
			  rt_kprintf("%s水深:%0.4f米\n",sign,waterLev);  
		} 
		else{//读不到给0
				if(ret2==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
					  uartDev[modbusFlash[WATERLEVEL].useUartNum].offline=RT_TRUE;
				}
			  gas.o2	=0;
			  rt_kprintf("%s read fail\n",sign);
		}
	  rt_mutex_release(uartDev[modbusFlash[WATERLEVEL].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}


