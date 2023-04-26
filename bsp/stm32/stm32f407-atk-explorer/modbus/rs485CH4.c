#include "board.h"
//#include "stmflash.h"
//一氧化碳默认地址04 波特率9600
//04 03 00 02 00 02 65 9E
//04 03 04 00 00 00 00 AF 33 
#ifdef USE_4GAS
const static char sign[]="[甲烷]";

//#define   SLAVE_ADDR     0X01 
//#define   LENTH          50  //工作环流用到的最大接收buf长度


//concentrationStru gas;
//#define CH4_485_NUM   				10

 float ch4[CH4_485_NUM];
static uint8_t respStat[CH4_485_NUM];
extern bool gasAlarmFlag;
//打包串口发送 
static void ch4UartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.ch4[num].useUartNum,buf, len);
}
//返回ch4的通讯状态 true--通讯成功 false--通讯失败
int ch4State(int i)
{
		return respStat[i];
}
//ch4比较阈值并设置相应的flag标记
static void ch4CheckSetFlag(int num)
{
		gasAlarmFlag=false;
		if(sheet.modbusCh4[num].ch4UpLimit!=0){
			if(ch4[num]>=sheet.modbusCh4[num].ch4UpLimit){
					inpoutpFlag.modbusCh4[num].ch4UpFlag=true;gasAlarmFlag=true;
			}
		}
		if(sheet.modbusCh4[num].ch4LowLimit!=0){
			if(ch4[num]<=sheet.modbusCh4[num].ch4LowLimit){
					inpoutpFlag.modbusCh4[num].ch4LowFlag=true;gasAlarmFlag=true;
			}
		}

}
//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
//读取ch4的浓度值
void readCH4(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.ch4[num].slaveAddr,0X0002,READ_03,2,buf);
	 //rt_kprintf("%sCH4 take %d\n",modbusFlash[CH4].useUartNum);
//		rt_mutex_take(.uartMessque[sheet.ch4[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		ch4UartSend(num,buf,len);
	  rt_kprintf("%sCH4 send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		
		while(rt_mq_recv(&uartmque[sheet.ch4[num].useUartNum], buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
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
		int ret2=modbusRespCheck(sheet.ch4[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值
        int val	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;

        ch4[num]=(float)((float)val	/1000);
			  respStat[num]=1;
			  ch4CheckSetFlag(num);
			  rt_kprintf("%s浓度值:%0.2fmol/Lread ok\n",sign,ch4[num]);  
		} 
		else{//读不到给0
				if(ret2==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
				respStat[num]=0;
			  ch4[num]=0;
			  rt_kprintf("%s read fail\n",sign);
		}
		//ch4CheckSetFlag(num);
//	  rt_mutex_release(.uartMessque[sheet.ch4[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}

//ch4读取再次封装 供别处函数来调用
void ch4Read2Send()
{
	 //int workFlag=RT_FALSE;
	 for(int i=0;i<CH4_485_NUM;i++){
			if(sheet.ch4[i].workFlag==RT_TRUE){
						readCH4(i);
					//	workFlag=RT_TRUE;
				}
		}

}
#endif
