#include "board.h"
//#include "stmflash.h"
//����Ĭ�ϵ�ַ03  ������9600
//03 03 00 02 00 02 64 29
//04 03 04 00 00 00 00 AF 33 
#ifdef USE_4GAS
const static char sign[]="[����]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //���������õ���������buf����

 float h2s[H2S_485_NUM];
static uint8_t respStat[H2S_485_NUM];
extern bool gasAlarmFlag;
//������ڷ��� 
static void h2sUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.h2s[num].useUartNum,buf, len);
}
//����h2s��ͨѶ״̬ true--ͨѶ�ɹ� false--ͨѶʧ��
int h2sState(int i)
{
		return respStat[i];
}

//h2s�Ƚ���ֵ��������Ӧ��flag���
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

//�� 1A 04 00 01 00 02 23 E0
//�� 1A 04 04 0B 1B 00 1C 23 6F
//��ȡh2sֵ
void readH2S(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.h2s[num].slaveAddr,0X0002,READ_03,2,buf);

	  //485����buf  len  �ȴ�modbus��Ӧ
		h2sUartSend(num,buf,len);
	  rt_kprintf("%sh2s send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);

		while(rt_mq_recv(&uartmque[sheet.h2s[num].useUartNum], buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
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
		int ret2=modbusRespCheck(sheet.h2s[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//ˢ�¶�ȡ����ֵ
        int val	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;

        h2s[num]	=(float)((float)val	/1000);
			  respStat[num]=1;
				h2sCheckSetFlag(num);
			  rt_kprintf("%sŨ��ֵ:%0.2fmol/Lread ok\n",sign,h2s[num]);  
		} 
		else{//��������0
				if(ret2==2){
						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
				}
				respStat[num]=0;
			  h2s[num]	=0;
			  rt_kprintf("%s read fail\n",sign);
		}

		rt_free(buf);
	  buf=RT_NULL;

}

//h2s�Ķ�ȡ�ķ�װ ����ĺ�������
void h2sRead2Send()
{

	 for(int i=0;i<H2S_485_NUM;i++){
			if(sheet.h2s[i].workFlag==RT_TRUE){
						readH2S(i);

				}
		}

}
#endif

