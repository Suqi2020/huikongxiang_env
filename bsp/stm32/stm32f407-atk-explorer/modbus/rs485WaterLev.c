#include "board.h"
//ˮλĬ�ϵ�ַ02  ������9600  ����0~49��

const static char sign[]="[ˮλ]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //���������õ���������buf����

float waterLev=0;
//������ڷ��� 
static void waterLevUartSend(uint8_t *buf,int len)
{
		rs485UartSend(modbusFlash[WATERLEVEL].useUartNum,buf, len);
}



//�� 1A 04 00 01 00 02 23 E0
//�� 1A 04 04 0B 1B 00 1C 23 6F
void readWaterLevel()
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(modbusFlash[WATERLEVEL].slaveAddr,0X0002,READ_04,2,buf);
		rt_mutex_take(uartDev[modbusFlash[WATERLEVEL].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		waterLevUartSend(buf,len);
	  rt_kprintf("%swater send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		
		if(rt_mq_recv(uartDev[modbusFlash[WATERLEVEL].useUartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//��һ�ν���ʱ��ų���  ��Ӧʱ���п��ܱȽϾ�
				len++;
		}
		while(rt_mq_recv(uartDev[modbusFlash[WATERLEVEL].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
		uartDev[modbusFlash[WATERLEVEL].useUartNum].offline=RT_FALSE;
		int ret2=modbusRespCheck(modbusFlash[WATERLEVEL].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//ˢ�¶�ȡ����ֵ
        uint32_t read	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
        extern float write_hex_to_float(uint32_t number);
			  float waterLev= write_hex_to_float(read);
         waterLev=(float)(waterLev	/9.8);
			  rt_kprintf("%sˮ��:%0.4f��\n",sign,waterLev);  
		} 
		else{//��������0
				if(ret2==2){
						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
					  uartDev[modbusFlash[WATERLEVEL].useUartNum].offline=RT_TRUE;
				}
			  gas.o2	=0;
			  rt_kprintf("%s read fail\n",sign);
		}
	  rt_mutex_release(uartDev[modbusFlash[WATERLEVEL].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}


