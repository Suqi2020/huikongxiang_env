#include "board.h"
//#include "stmflash.h"
//硫化氢默认地址03  波特率9600
//03 03 00 02 00 02 64 29
//04 03 04 00 00 00 00 AF 33 
#ifdef USE_4GAS
const static char sign[]="[硫化氢]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //工作环流用到的最大接收buf长度

 float h2s[H2S_485_NUM];
static uint8_t respStat[H2S_485_NUM];
extern bool gasAlarmFlag;
//打包串口发送 
static void h2sUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.h2s[num].useUartNum,buf, len);
}
//返回h2s的通讯状态 true--通讯成功 false--通讯失败
int h2sState(int i)
{
		return respStat[i];
}

//h2s比较阈值并设置相应的flag标记
static void h2sCheckSetFlag(int num)
{
	  gasAlarmFlag=false;
		if(sheet.modbusH2s[num].h2sUpLimit!=0){
			if(h2s[num]>=sheet.modbusH2s[num].h2sUpLimit){
					inpoutpFlag.modbusH2s[num].h2sUpFlag=true;
					gasAlarmFlag=true;
			}
		}
		if(sheet.modbusH2s[num].h2sLowLimit!=0){
			if(h2s[num]<=sheet.modbusH2s[num].h2sLowLimit){
					inpoutpFlag.modbusH2s[num].h2sLowFlag=true;
					gasAlarmFlag=true;
			}
		}
}

//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
//读取h2s值
void readH2S(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.h2s[num].slaveAddr,0X0002,READ_03,2,buf);

	  //485发送buf  len  等待modbus回应
		h2sUartSend(num,buf,len);
	  rt_kprintf("%sh2s send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);

		while(rt_mq_recv(&uartmque[sheet.h2s[num].useUartNum], buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
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
		int ret2=modbusRespCheck(sheet.h2s[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值
        int val	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;

        h2s[num]	=(float)((float)val	/1000);
			  respStat[num]=1;
				h2sCheckSetFlag(num);
			  rt_kprintf("%s浓度值:%0.2fmol/Lread ok\n",sign,h2s[num]);  
		} 
		else{//读不到给0
				if(ret2==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
				respStat[num]=0;
			  h2s[num]	=0;
			  rt_kprintf("%s read fail\n",sign);
		}

		rt_free(buf);
	  buf=RT_NULL;

}

//h2s的读取的封装 供别的函数调用
void h2sRead2Send()
{

	 for(int i=0;i<H2S_485_NUM;i++){
			if(sheet.h2s[i].workFlag==RT_TRUE){
						readH2S(i);

				}
		}

}
#endif

