#include "board.h"
//#include "stmflash.h"
//ˮλĬ�ϵ�ַ02  ������9600  ����0~49��

const static char sign[]="[��ʪ��]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //���������õ���������buf����

typedef struct{
	float temp;
	float hum; 
}thStru;
thStru thum[TEMPHUM_485_NUM];
//float temp[TEMPHUM_485_NUM];
//float hum[TEMPHUM_485_NUM]; 
//������ڷ��� 
static void tempHumUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.tempHum[num].useUartNum,buf, len);
}



//�� 1A 04 00 01 00 02 23 E0
//�� 1A 04 04 0B 1B 00 1C 23 6F
void readTempHum(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.tempHum[num].slaveAddr,0X0000,READ_04,2,buf);
		rt_mutex_take(uartDev[sheet.tempHum[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		tempHumUartSend(num,(uint8_t *)buf,len);
	  rt_kprintf("%stemphum send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		
		if(rt_mq_recv(uartDev[sheet.tempHum[num].useUartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//��һ�ν���ʱ��ų���  ��Ӧʱ���п��ܱȽϾ�
				len++;
		}
		while(rt_mq_recv(uartDev[sheet.tempHum[num].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 ������1ms 10������
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
		//uartDev[modbusFlash[TEMPHUM].useUartNum].offline=RT_FALSE;
		int ret2=modbusRespCheck(sheet.tempHum[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//ˢ�¶�ȡ����ֵ
        uint16_t read_temp	=(buf[offset+2]<<8)+buf[offset+3];offset+=2;
			  uint16_t read_hum	=(buf[offset+2]<<8)+buf[offset+3];offset+=2;
			  rt_bool_t sig=RT_FALSE;
        if(read_temp&0x8000){//����
						read_temp=read_temp&0x8000;
						read_temp=0xffff-read_temp+1;
					  sig=RT_TRUE;
				}
				thum[num].temp=(float)read_temp/10;
				thum[num].hum=(float)read_hum/10;
				if(sig==RT_TRUE)
					rt_kprintf("%s�¶�:-%0.1fC ʪ��:%0.1f\n",sign,thum[num].temp,thum[num].hum); 
				else
					rt_kprintf("%s�¶�:%0.1fC ʪ��:%0.1f\n",sign,thum[num].temp,thum[num].hum);   
		} 
		else{//��������0
				if(ret2==2){
						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
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


