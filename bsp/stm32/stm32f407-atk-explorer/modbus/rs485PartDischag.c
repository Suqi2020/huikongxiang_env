#include <board.h>
//#include "rs485PartDischag.h"
//#include "stmflash.h"
//<<�ַ����߼�� --GY-JF100-C01>>  GZPD-1000���¾ַ�
//��Ӧʱ�䲻ȷ�� �1.7�� ��ʱ�� ��ʱ���
//           Ĭ�ϲ�����115200

 typedef struct  
 { 
  unsigned a:1; 
  unsigned b:3; 
  unsigned c:4; 
 } bs; 
 
typedef struct{
		uint32_t amplitudeA;
	  uint32_t freqA;
	  uint32_t dischargeA;
	
		uint32_t amplitudeB;
		uint32_t freqB;
		uint32_t dischargeB;

		uint32_t amplitudeC;
		uint32_t freqC;
		uint32_t dischargeC;
	
	  bs alarm;  //bit0 A bit1 B bit2 C
	  uint8_t respStat;
}partDischargeStru;

const static char sign[]="[�ַ�]";
static partDischargeStru partDiscStru_p[PARTDISCHAG_485_NUM];


int partDisState(int i)
{
		return partDiscStru_p[i].respStat;
}
//#define   SLAVE_ADDR     0X01
#define   LENTH          1024  //���������õ���������buf����


static void partDischagUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.partDischag[num].useUartNum,buf, len);
	
}

//��ȡ��ֵ Ƶ�� �ŵ�������
//01 03 0300 0006 C58C
//01 03 24 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 7B A1 

void readPdFreqDischarge(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.partDischag[num].slaveAddr,0x0300,READ_03,18,buf);
		rt_mutex_take(uartDev[sheet.partDischag[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		partDischagUartSend(num,buf,len);
	  rt_kprintf("%sPdFreqDiach send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		memset(buf,0,LENTH);
    len=0;
		if(rt_mq_recv(uartDev[sheet.partDischag[num].useUartNum].uartMessque, buf+len, 1, 3000) == RT_EOK){//��һ�ν���ʱ��ų���  ��Ӧʱ���п��ܱȽϾ�
				len++;
		}
		while(rt_mq_recv(uartDev[sheet.partDischag[num].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
		int ret2= modbusRespCheck(sheet.partDischag[num].slaveAddr,buf,len,RT_TRUE);
		if(0 ==  ret2){//ˢ�¶�ȡ����ֵ
			
				partDiscStru_p[num].amplitudeA=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				partDiscStru_p[num].freqA     =(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  partDiscStru_p[num].dischargeA=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			
				partDiscStru_p[num].amplitudeB=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				partDiscStru_p[num].freqB     =(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  partDiscStru_p[num].dischargeB=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			
				partDiscStru_p[num].amplitudeC=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				partDiscStru_p[num].freqC     =(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  partDiscStru_p[num].dischargeC=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  rt_kprintf("%sPdFreqDiach read ok\n",sign);
			  partDiscStru_p[num].respStat=1;
		} 
		else{//��������0
				if(ret2==2){
						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
				}
				partDiscStru_p[num].respStat=0;
				partDiscStru_p[num].amplitudeA=0;
				partDiscStru_p[num].freqA     =0;
			  partDiscStru_p[num].dischargeA=0;
			
				partDiscStru_p[num].amplitudeB=0;
				partDiscStru_p[num].freqB     =0;
			  partDiscStru_p[num].dischargeB=0;
			
				partDiscStru_p[num].amplitudeC=0;
				partDiscStru_p[num].freqC     =0;
			  partDiscStru_p[num].dischargeC=0;
			  rt_kprintf("%sPdFreqDiach read fail\n",sign);
		}

	  rt_mutex_release(uartDev[sheet.partDischag[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}

//��ȡ�ַű���״̬ 
//�� 01 01 0001 0008 6C0C
//�� 01 01 01 00 51 88 

rt_bool_t readPartDischgWarning(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadBitReg(sheet.partDischag[num].slaveAddr,0x0001,8,buf);//��ȡ8��bit
		rt_mutex_take(uartDev[sheet.partDischag[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		partDischagUartSend(num,buf,len);
	  rt_kprintf("%sreadPd send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		memset(buf,0,LENTH);
    len=0;
		if(rt_mq_recv(uartDev[sheet.partDischag[num].useUartNum].uartMessque, buf+len, 1, 2000) == RT_EOK){//��һ�ν���ʱ��ų���  ��Ӧʱ���п��ܱȽϾ�
				len++;
		}
		while(rt_mq_recv(uartDev[sheet.partDischag[num].useUartNum].uartMessque, buf+len, 1, 10) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		rt_kprintf("%srec:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
//		uartDev[modbusFlash[PARTDISCHAG].useUartNum].offline=RT_FALSE;
		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
		int ret2=modbusRespCheck(sheet.partDischag[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//ˢ�¶�ȡ����ֵ
     
			  partDiscStru_p[num].alarm.a=(buf[offset]>>0)&0x01;
				partDiscStru_p[num].alarm.b=(buf[offset]>>1)&0x01;
			  partDiscStru_p[num].alarm.c=(buf[offset]>>2)&0x01;
			  rt_kprintf("%s��ȡalarm OK\r\n",sign);
			  partDiscStru_p[num].respStat=1;
		} 
		else{
				if(ret2==2){
				}
				partDiscStru_p[num].respStat=0;
				partDiscStru_p[num].alarm.a=0;
				partDiscStru_p[num].alarm.b=0;
				partDiscStru_p[num].alarm.c=0;
			  rt_kprintf("%s��ȡalarm fail\r\n",sign);
		}
   
	  rt_mutex_release(uartDev[sheet.partDischag[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;
		if(partDiscStru_p[num].alarm.a||partDiscStru_p[num].alarm.b||partDiscStru_p[num].alarm.c)
				return RT_TRUE;
		else 
			  return RT_FALSE;
}

void  partDisWaringEventPack()
{
		rt_kprintf("%slater add \n\r",sign);
		
}
uint16_t partDischagJsonPack()
{

		char* out = NULL;
		//��������
		cJSON* Array = NULL;
		// ����JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return 0;
		// ����ڵ㣨��ֵ�ԣ�
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_REPORTDATA");
		cJSON_AddStringToObject(root, "identifier","partial_discharge_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFLash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20���ַ������� ������
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < PARTDISCHAG_485_NUM; i++)
		{		
			
			if(sheet.partDischag[i].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.partDischag[i].ID));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].respStat);
				cJSON_AddItemToObject(nodeobj,"responseStatus",cJSON_CreateString(sprinBuf));
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				
				sprintf(sprinBuf,"%d",partDiscStru_p[i].amplitudeA);
				cJSON_AddItemToObject(nodeobj_p,"pdA",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].freqA);
				cJSON_AddItemToObject(nodeobj_p,"freqA",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].dischargeA);
				cJSON_AddItemToObject(nodeobj_p,"dischargeDataA",cJSON_CreateString(sprinBuf));
				cJSON_AddItemToObject(nodeobj_p,"prpdDataA",cJSON_CreateString(""));
				cJSON_AddItemToObject(nodeobj_p,"prpsDataA",cJSON_CreateString(""));
				
				
				sprintf(sprinBuf,"%d",partDiscStru_p[i].amplitudeB);
				cJSON_AddItemToObject(nodeobj_p,"pdB",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].freqB);
				cJSON_AddItemToObject(nodeobj_p,"freqB",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].dischargeB);
				cJSON_AddItemToObject(nodeobj_p,"dischargeDataB",cJSON_CreateString(sprinBuf));
				cJSON_AddItemToObject(nodeobj_p,"prpdDataB",cJSON_CreateString(""));
				cJSON_AddItemToObject(nodeobj_p,"prpsDataB",cJSON_CreateString(""));
				
				sprintf(sprinBuf,"%d",partDiscStru_p[i].amplitudeC);
				cJSON_AddItemToObject(nodeobj_p,"pdC",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].freqC);
				cJSON_AddItemToObject(nodeobj_p,"freqC",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",partDiscStru_p[i].dischargeC);
				cJSON_AddItemToObject(nodeobj_p,"dischargeDataC",cJSON_CreateString(sprinBuf));
				cJSON_AddItemToObject(nodeobj_p,"prpdDataC",cJSON_CreateString(""));
				cJSON_AddItemToObject(nodeobj_p,"prpsDataC",cJSON_CreateString(""));
				
				
				sprintf(sprinBuf,"%u",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		}
		}
	
		sprintf(sprinBuf,"%u",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// ��ӡJSON���ݰ�  
		//���
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json�����������д
		
		// �ͷ��ڴ�  
		
		
		out = cJSON_Print(root);
		rt_strcpy((char *)packBuf+len,out);
		len+=rt_strlen(out);
		if(out!=NULL){
				for(int i=0;i<rt_strlen(out);i++)
						rt_kprintf("%c",out[i]);
				rt_kprintf("\n");
				rt_free(out);
				out=NULL;
		}
		if(root!=NULL){
			cJSON_Delete(root);
			out=NULL;
		}
	

		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//����json����
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//����crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]=(uint8_t)(TAIL>>8); len++;
		packBuf[len]=(uint8_t)(TAIL);    len++;
		packBuf[len]=0;//len++;//��β ��0
		mcu.repDataMessID =mcu.upMessID;
		//mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%s len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen��%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}
void partDischagRead2Send(rt_bool_t netStat)
{
		int workFlag=RT_FALSE;
		for(int i=0;i<PARTDISCHAG_485_NUM;i++){
			 if(sheet.partDischag[i].workFlag==RT_TRUE){
				    rt_thread_mdelay(2000);
						readPdFreqDischarge(i);
						workFlag=RT_TRUE;
				}
		}
		if(workFlag==RT_TRUE){
				rt_kprintf("%s����ɼ���PARTDISCHAG����\r\n",sign);
				partDischagJsonPack();//���ڼ���
				if(netStat==RT_TRUE)
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
		}
}
