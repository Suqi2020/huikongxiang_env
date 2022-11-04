#include    "modbus.h"

const static char sign[]="[modbus]";
/*
1、什么时候读取数据 
1.1 定时读取  队列发送 485 延时500ms 等待队列接收 接受完打包发送  加入1个 互斥保护
1.2 服务器读取 设置 队列发送 485 延时500ms 等待队列接收 接受完打包发送  加入互斥保护
    每个485 1个接收队列 发送直接调用串口发送接口
要求：能实现迅速切换其它485接口来使用 方法：只需要修改串口发送接口 和中断接收接口即可
      rs485Circula.c-cirCurrUartSend(uint8_t *buf,int len) 和drv_uart.c-USART2_IRQHandler中
串口2346 modbus 串口1 debug 串口5 串口屏
//modbus接口通用read  03 06 10 
*/
//读一个或多个寄存器
//MQTTLenString test;
//// out --输出数据
uint8_t modbusReadReg(uint16_t slavAddr,uint16_t regAddr,uint8_t cmd,uint16_t len,uint8_t * out)
{
		int i=0;
	  out[i]=slavAddr;					 			i++;
	  out[i]=cmd;      					 		i++;
	  out[i]=(uint8_t)(regAddr>>8);   i++;
	  out[i]=(uint8_t) regAddr;       i++;
		out[i]=(uint8_t)(len>>8);       i++;
	  out[i]=(uint8_t) len;       		i++;
	  uint16_t crcRet=RTU_CRC(out ,i);
	  out[i]=(uint8_t)(crcRet>>8);    i++;
	  out[i]=crcRet;       						i++;
		return i;
}


//modbus 读bit位寄存器
uint8_t modbusReadBitReg(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out)
{
		int i=0;
	  out[i]=slavAddr;					 			i++;
	  out[i]=READM_BIT;      					i++;
	  out[i]=(uint8_t)(regAddr>>8);   i++;
	  out[i]=(uint8_t) regAddr;       i++;
		out[i]=(uint8_t)(len>>8);       i++;
	  out[i]=(uint8_t) len;       		i++;
	  uint16_t crcRet=RTU_CRC(out ,i);
	  out[i]=(uint8_t)(crcRet>>8);    i++;
	  out[i]=crcRet;       						i++;
		return i;
}
//写一个寄存器
uint8_t modbusWriteOneReg(uint16_t slavAddr,uint16_t regAddr,uint16_t value,uint8_t *out)
{
		int i=0;
	  out[i]=slavAddr;					 			i++;
	  out[i]=WRITE;      					 		i++;
	  out[i]=(uint8_t)(regAddr>>8);   i++;
	  out[i]=(uint8_t) regAddr;       i++;
		out[i]=(uint8_t)(value>>8);   	i++;
	  out[i]=(uint8_t) value;     		i++;
	  uint16_t crcRet=RTU_CRC(out ,i);
	  out[i]=(uint8_t)(crcRet>>8);    i++;
	  out[i]=crcRet;       						i++;	
    return i;	
}
//写多个寄存器  len  数据长度 len/2寄存器个数  OUT-输出数据
uint8_t modbusWriteMultReg(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t *in,uint8_t *out)
{
		int i=0;
	  out[i]=slavAddr;					 			i++;
	  out[i]=WRITE_MUL;      					i++;
	  out[i]=(uint8_t)(regAddr>>8);   i++;
	  out[i]=(uint8_t) regAddr;       i++;
		out[i]=(uint8_t)((len/2)>>8);   i++;
	  out[i]=(uint8_t) len/2;       	i++; //寄存器个数
	  out[i]=(uint8_t) len;       		i++;//数据长度
	  for(int j=0;j<len;j++,i++){
				out[i]=in[j];
		}
	  uint16_t crcRet=RTU_CRC(out ,i);
	  out[i]=(uint8_t)(crcRet>>8);    i++;
	  out[i]=crcRet;       						i++;	
    return i;			
}
//modbus回复数据校验   readFLAG TRUE-读     FALSE-写
//0 succ  1 slave addr or bsp err    2 modbus uart err
int  modbusRespCheck(uint16_t slavAddr,uint8_t *buf,uint16_t len,rt_bool_t readFlag)
{
	  if(len<2){
				rt_kprintf("%sERR:no resp\r\n",sign);
				return 2;
		}
		if(buf[0]!=slavAddr){
				rt_kprintf("%sERR:slaveADDR\r\n",sign);
				return 1;
		}
		if(readFlag==RT_TRUE){
				if((buf[2]+2+1+2)!=len){
								rt_kprintf("%sERR:可能连包\r\n",sign);
						}
						len =buf[2]+2+1+2;//重新刷新长度
		}
		else{
			#define  WR_RESP_LEN  8
				len =WR_RESP_LEN;//重新刷新长度
		}
		uint16_t respCrc=(buf[len-2]<<8)+buf[len-1];
	  uint16_t checkCrc= RTU_CRC(buf,len-2);
		if(respCrc!=checkCrc){
				rt_kprintf("%sCRC check err 0x%04x  0x%04x\r\n",sign,respCrc,checkCrc);
				return 0;
		}
		return 0;
}


#define   LENTH          200
extern uartConfStru  uartDev[UART_NUM];
rt_bool_t modbusCommRead(uartEnum uartNum,uint8_t cmd,uint16_t slavAddr,uint16_t regAddr,uint16_t reglen)
{
	  rt_bool_t ret=RT_TRUE;
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  //uint16_t len = psReadReg(modbusFlash[THREEAXIS].slaveAddr,0X0001,4,buf);
	  uint16_t len=modbusReadReg( slavAddr, regAddr, cmd, reglen,buf);
		rt_mutex_take(uartDev[uartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
	
	  rs485UartSend(uartNum,buf,len);
	  rt_kprintf("%s send[port%d]:",sign,uartNum+1);
		for(int j=0;j<len;j++){
				rt_kprintf("%02x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		if(rt_mq_recv(uartDev[uartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//第一次接收时间放长点  相应时间有可能比较久
				len++;
		}
		while(rt_mq_recv(uartDev[uartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%02x ",buf[j]);
				}
				rt_kprintf("\n");
		}
//		uartDev[uartNum].offline=RT_FALSE;
		//提取环流值 第一步判断crc 第二部提取
		int ret2=modbusRespCheck(slavAddr,buf,len,RT_TRUE);
		if(0 ==  ret2){//刷新读取到的值
			rt_kprintf("%sread ok valid data:",sign);//
			
			   for(int k=0;k<reglen*2;k++)//偏移3 有效数据长度为 reglen*2
						rt_kprintf("%02x ",buf[k+3]);
			   rt_kprintf("\n");
		} 
		else{//读不到给0
				if(ret2==2){
//					  uartDev[uartNumm].offline=RT_TRUE;
				}
        ret=RT_FALSE;
			  rt_kprintf("%sread fail\n",sign);
		}
	  rt_mutex_release(uartDev[uartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;
    return ret;
}



