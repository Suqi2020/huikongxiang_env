#include    "modbus.h"

const static char sign[]="[modbus]";
/*
1��ʲôʱ���ȡ���� 
1.1 ��ʱ��ȡ  ���з��� 485 ��ʱ500ms �ȴ����н��� ������������  ����1�� ���Ᵽ��
1.2 ��������ȡ ���� ���з��� 485 ��ʱ500ms �ȴ����н��� ������������  ���뻥�Ᵽ��
    ÿ��485 1�����ն��� ����ֱ�ӵ��ô��ڷ��ͽӿ�
Ҫ����ʵ��Ѹ���л�����485�ӿ���ʹ�� ������ֻ��Ҫ�޸Ĵ��ڷ��ͽӿ� ���жϽ��սӿڼ���
      rs485Circula.c-cirCurrUartSend(uint8_t *buf,int len) ��drv_uart.c-USART2_IRQHandler��
����2346 modbus ����1 debug ����5 ������
//modbus�ӿ�ͨ��read  03 06 10 
*/
//��һ�������Ĵ���
//MQTTLenString test;
//// out --�������
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


//modbus ��bitλ�Ĵ���
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
//дһ���Ĵ���
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
//д����Ĵ���  len  ���ݳ��� len/2�Ĵ�������  OUT-�������
uint8_t modbusWriteMultReg(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t *in,uint8_t *out)
{
		int i=0;
	  out[i]=slavAddr;					 			i++;
	  out[i]=WRITE_MUL;      					i++;
	  out[i]=(uint8_t)(regAddr>>8);   i++;
	  out[i]=(uint8_t) regAddr;       i++;
		out[i]=(uint8_t)((len/2)>>8);   i++;
	  out[i]=(uint8_t) len/2;       	i++; //�Ĵ�������
	  out[i]=(uint8_t) len;       		i++;//���ݳ���
	  for(int j=0;j<len;j++,i++){
				out[i]=in[j];
		}
	  uint16_t crcRet=RTU_CRC(out ,i);
	  out[i]=(uint8_t)(crcRet>>8);    i++;
	  out[i]=crcRet;       						i++;	
    return i;			
}
//modbus�ظ�����У��   readFLAG TRUE-��     FALSE-д
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
								rt_kprintf("%sERR:��������\r\n",sign);
						}
						len =buf[2]+2+1+2;//����ˢ�³���
		}
		else{
			#define  WR_RESP_LEN  8
				len =WR_RESP_LEN;//����ˢ�³���
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
	  //485����buf  len  �ȴ�modbus��Ӧ
	
	  rs485UartSend(uartNum,buf,len);
	  rt_kprintf("%s send[port%d]:",sign,uartNum+1);
		for(int j=0;j<len;j++){
				rt_kprintf("%02x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		if(rt_mq_recv(uartDev[uartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//��һ�ν���ʱ��ų���  ��Ӧʱ���п��ܱȽϾ�
				len++;
		}
		while(rt_mq_recv(uartDev[uartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 ������1ms 10������
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
		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
		int ret2=modbusRespCheck(slavAddr,buf,len,RT_TRUE);
		if(0 ==  ret2){//ˢ�¶�ȡ����ֵ
			rt_kprintf("%sread ok valid data:",sign);//
			
			   for(int k=0;k<reglen*2;k++)//ƫ��3 ��Ч���ݳ���Ϊ reglen*2
						rt_kprintf("%02x ",buf[k+3]);
			   rt_kprintf("\n");
		} 
		else{//��������0
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



