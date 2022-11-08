#include "rs485Gas.h"
//#include "stmflash.h"
//����Ĭ�ϵ�ַ02  ������9600
//02 03 00 02 00 02 65 F8
//02 03 04 00 00 52 1C F5 9A 00 

const static char sign[]="[����]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //���������õ���������buf����


float o2[O2_485_NUM];
//������ڷ��� 
static void o2UartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.o2[num].useUartNum,buf, len);
}



//�� 1A 04 00 01 00 02 23 E0
//�� 1A 04 04 0B 1B 00 1C 23 6F
void readO2(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.o2[num].slaveAddr,0X0002,READ_03,2,buf);
		rt_mutex_take(uartDev[sheet.o2[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		o2UartSend(num,buf,len);
	  rt_kprintf("%so2 send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		
		if(rt_mq_recv(uartDev[sheet.o2[num].useUartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//��һ�ν���ʱ��ų���  ��Ӧʱ���п��ܱȽϾ�
				len++;
		}
		while(rt_mq_recv(uartDev[sheet.o2[num].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 ������1ms 10������
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
//		uartDev[modbusFlash[O2].useUartNum].offline=RT_FALSE;
		int ret2=modbusRespCheck(sheet.o2[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//ˢ�¶�ȡ����ֵ
        int val	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;

        o2[num]=(float)((float)val	/1000);
			  rt_kprintf("%sŨ��ֵ:%0.2fmol/Lread ok\n",sign,o2[num]);  
		} 
		else{//��������0
				if(ret2==2){
						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
//					  uartDev[modbusFlash[O2].useUartNum].offline=RT_TRUE;
				}
			  o2[num]	=0;
			  rt_kprintf("%s read fail\n",sign);
		}
	  rt_mutex_release(uartDev[sheet.o2[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}


