#include "rs485Gas.h"
//����Ĭ�ϵ�ַ03  ������9600
//03 03 00 02 00 02 64 29
//04 03 04 00 00 00 00 AF 33 

const static char sign[]="[����]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //���������õ���������buf����


//������ڷ��� 
static void h2sUartSend(uint8_t *buf,int len)
{
		rs485UartSend(modbusFlash[H2S].useUartNum,buf, len);
}



//�� 1A 04 00 01 00 02 23 E0
//�� 1A 04 04 0B 1B 00 1C 23 6F
void readH2S()
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(modbusFlash[H2S].slaveAddr,0X0002,READ_03,2,buf);
		rt_mutex_take(uartDev[modbusFlash[H2S].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		h2sUartSend(buf,len);
	  rt_kprintf("%sh2s send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		
		if(rt_mq_recv(uartDev[modbusFlash[H2S].useUartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//��һ�ν���ʱ��ų���  ��Ӧʱ���п��ܱȽϾ�
				len++;
		}
		while(rt_mq_recv(uartDev[modbusFlash[H2S].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 ������1ms 10������
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
		uartDev[modbusFlash[H2S].useUartNum].offline=RT_FALSE;
		int ret2=modbusRespCheck(modbusFlash[H2S].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//ˢ�¶�ȡ����ֵ
        gas.h2s	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;

        float cont=(float)((float)gas.h2s	/1000);
			  rt_kprintf("%sŨ��ֵ:%0.2fmol/Lread ok\n",sign,cont);  
		} 
		else{//��������0
				if(ret2==2){
						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
					  uartDev[modbusFlash[H2S].useUartNum].offline=RT_TRUE;
				}
			  gas.h2s	=0;
			  rt_kprintf("%s read fail\n",sign);
		}
	  rt_mutex_release(uartDev[modbusFlash[H2S].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}


