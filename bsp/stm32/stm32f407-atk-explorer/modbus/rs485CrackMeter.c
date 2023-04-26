#include "board.h"
//<<TH-DCM����ʽ�ѷ�ƴ���ͨѶЭ��˵��-20210622>>
//  24+��ɫ��24-��ɫ��A+��ɫ��B-��ɫ
// ͬ���޸ĵ�ַ �޸��豸��ַ FF FF 03 0A���豸�����ĳ���ַ��FF(01)���̵�ַ   �����Ǻ��ѷ�����FF  ���������01
const static char sign[]="[�ѷ���]";

//#define   SLAVE_ADDR     0X02 
#define   LENTH          50  //���������õ���������buf����

typedef struct{
	   union {
				float flotVal;
				uint32_t   intVal;
		} distanc;
		 union {
				float flotVal;
				uint32_t   intVal;
		} temp;
		uint8_t respStat;
}crackMeterStru;
crackMeterStru crackMeter[CRACKMETER_485_NUM];
//���س����ǵ�ͨѶ״̬ true--ͨѶ�ɹ� false--ͨѶʧ��
int  crackMeterState(int i)
{
		return crackMeter[i].respStat;
}
//������ڷ��� 
static void  crackMeterUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.crackMeter[num].useUartNum,buf, len);
}

///////////////////////////////////////��д�Ĵ�����ز���////////////////////////////////////////



//�����ǱȽ���ֵ��������Ӧ��flag���
static void pressStlCheckSetFlag(int num)
{

	  if(sheet.modbusCrackMeter[num].tempUpLimit!=0){
			if(crackMeter[num].temp.flotVal>=sheet.modbusCrackMeter[num].tempUpLimit){
					inpoutpFlag.modbusCrackMeter[num].tempUpFlag=true;
			}
		}
		if(sheet.modbusCrackMeter[num].tempLowLimit!=0){
			if(crackMeter[num].temp.flotVal<=sheet.modbusCrackMeter[num].tempLowLimit){
					inpoutpFlag.modbusCrackMeter[num].tempLowFlag=true;
			}
		}
		if(sheet.modbusCrackMeter[num].distancUpLimit!=0){
			if(crackMeter[num].distanc.flotVal>=sheet.modbusCrackMeter[num].distancUpLimit){
					inpoutpFlag.modbusCrackMeter[num].distancUpFlag=true;
			}
		}
		if(sheet.modbusCrackMeter[num].distancLowLimit!=0){
			if(crackMeter[num].distanc.flotVal<=sheet.modbusCrackMeter[num].distancLowLimit){
					inpoutpFlag.modbusCrackMeter[num].distancLowFlag=true;
			}
		}
}

extern uint8_t tongHeModbusRead(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out);
//��ȡ�����ǵ��¶Ⱥ͸߶�
void readCrackMeter(int num)
{
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = tongHeModbusRead(sheet.crackMeter[num].slaveAddr,0x0001,2,buf);
	  //485����buf  len  �ȴ�modbus��Ӧ
		crackMeterUartSend(num,buf,len);
	  rt_kprintf("%s send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		while(rt_mq_recv(&uartmque[sheet.crackMeter[num].useUartNum], buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
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
		int ret2=modbusRespCheck(sheet.crackMeter[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//ˢ�¶�ȡ����ֵ
        int offset =3;//   2 4 8 3b ae fe 70 41 d4 0 0 26 35 
			  crackMeter[num].distanc.intVal=(uint32_t)((uint32_t)buf[offset]<<24)+((uint32_t)buf[offset+1]<<16)+((uint32_t)buf[offset+2]<<8)+buf[offset+3];
			  offset+=4;
			  crackMeter[num].temp.intVal   =(uint32_t)((uint32_t)buf[offset]<<24)+((uint32_t)buf[offset+1]<<16)+((uint32_t)buf[offset+2]<<8)+buf[offset+3];
				crackMeter[num].respStat=1;
				pressStlCheckSetFlag(num);
				if(len!=0){
						rt_kprintf("%srec2:",sign);
						for(int j=0;j<len;j++){
								rt_kprintf("%x ",buf[j]);
						}
						rt_kprintf("\n");
				}	 
			  rt_kprintf("%stemp:%0.8f*C distance:%0.8fmm read ok\n",sign,crackMeter[num].temp.flotVal,crackMeter[num].distanc.flotVal);
		} 
		else{//��������0
				if(ret2==2){
						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
				}
				crackMeter[num].respStat=0;
			  crackMeter[num].temp.intVal	=0;
			  crackMeter[num].distanc.intVal=0;
			  rt_kprintf("%stemp distance read fail\n",sign);
		}
		rt_free(buf);
	  buf=RT_NULL;

}


//������json��ʽ���
//���� respFlag Ϊtrue���ǻ�Ӧ
//              Ϊfalse����report����
uint16_t crackMeterJsonPack(bool respFlag)
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
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20���ַ������� ������
		// ����ڵ㣨��ֵ�ԣ�
		
		if(respFlag==true){
				cJSON_AddNumberToObject(root, "mid",respMid);
				cJSON_AddStringToObject(root, "packetType","PROPERTIES_485DATA_GET_RESP");
				cJSON_AddNumberToObject(root, "code",0);
		}
		else
		{
				cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
				cJSON_AddStringToObject(root, "packetType","PROPERTIES_485DATA_REP");
		}
		cJSON_AddStringToObject(root, "identifier","crackmeter_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		{
			Array = cJSON_CreateArray();
			if (Array == NULL) return 0;
			cJSON_AddItemToObject(root, "params", Array);
			for (int i = 0; i < CRACKMETER_485_NUM; i++)
			{		
				if(sheet.crackMeter[i].workFlag==RT_TRUE){
					nodeobj = cJSON_CreateObject();
					cJSON_AddItemToArray(Array, nodeobj);
					cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.crackMeter[i].ID));
					cJSON_AddNumberToObject(nodeobj,"responseStatus",crackMeter[i].respStat);
					
					nodeobj_p= cJSON_CreateObject();
					cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
					sprintf(sprinBuf,"%02f",crackMeter[i].temp.flotVal);
					cJSON_AddItemToObject(nodeobj_p,"temperature",cJSON_CreateString(sprinBuf));

					sprintf(sprinBuf,"%02f",crackMeter[i].distanc.flotVal );
					cJSON_AddItemToObject(nodeobj_p,"distance",cJSON_CreateString(sprinBuf));
					sprintf(sprinBuf,"%llu",utcTime());
					cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
				}
			}
		}
	
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// ��ӡJSON���ݰ�  
		// �ͷ��ڴ�  
		out = cJSON_Print(root);
		NetTxBuffer[0]=0xff;
		NetTxBuffer[1]=0xff;
		NetTxBuffer[2]=0xff;
		NetTxBuffer[3]=0xff;
		rt_strcpy((char *)NetTxBuffer+PACK_HEAD_LEN,out);
		if(out!=NULL){
				rt_free(out);
				out=NULL;
		}
		if(root!=NULL){
			cJSON_Delete(root);
			out=NULL;
		}
	
		if(respFlag==false){
				mcu.repDataMessID =mcu.upMessID;
				upMessIdAdd();
		}
		int len=strlen((char *)NetTxBuffer);
		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}





//��λ��ʪ�ȵ�warn״ֵ̬
void resetCrackMeterWarnFlag()
{
		for (int i = 0; i < CRACKMETER_485_NUM; i++)
		{		
				inpoutpFlag.modbusCrackMeter[i].tempUpFlag =false;
				inpoutpFlag.modbusCrackMeter[i].tempLowFlag=false;
				inpoutpFlag.modbusCrackMeter[i].distancLowFlag=false;
				inpoutpFlag.modbusCrackMeter[i].distancUpFlag =false;
		}
}





//ģ���¶Ⱥ�ʪ��ֵ��ȡ�Լ������json��ʽ  ����true �и澯 false �޸澯
bool modCrackMeterWarn2Send()
{

		char* out = NULL;
		//��������
		cJSON* Array = NULL;
		// ����JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return false;
		// ����ڵ㣨��ֵ�ԣ�
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","EVENTS_485_ALARM");
		cJSON_AddStringToObject(root, "identifier","crackmeter_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20���ַ������� ������
		{
				Array = cJSON_CreateArray();
				if (Array == NULL) return false;
				cJSON_AddItemToObject(root, "params", Array);
				for (int i = 0; i < CRACKMETER_485_NUM; i++)
				{		
						if(sheet.crackMeter[i].workFlag==RT_TRUE){
							nodeobj = cJSON_CreateObject();
							cJSON_AddItemToArray(Array, nodeobj);
							cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.crackMeter[i].ID));
							cJSON_AddNumberToObject(nodeobj,"alarmStatus",1);
							nodeobj_p= cJSON_CreateObject();
							cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
							cJSON_AddNumberToObject(nodeobj_p,"temperature_low_alarm",inpoutpFlag.modbusCrackMeter[i].tempLowFlag);//cJSON_CreateNumber("10"));
							cJSON_AddNumberToObject(nodeobj_p,"temperature_high_alarm",inpoutpFlag.modbusCrackMeter[i].tempUpFlag);
							cJSON_AddNumberToObject(nodeobj_p,"distance_low_alarm",inpoutpFlag.modbusCrackMeter[i].tempLowFlag);
							cJSON_AddNumberToObject(nodeobj_p,"distancedistance_high_alarm",inpoutpFlag.modbusCrackMeter[i].tempUpFlag);		
							
							sprintf(sprinBuf,"%llu",utcTime());
							cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
						}
				}
		}
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		//���
		// �ͷ��ڴ�  
		out = cJSON_Print(root);
		NetTxBuffer[0]=0xff;
		NetTxBuffer[1]=0xff;
		NetTxBuffer[2]=0xff;
		NetTxBuffer[3]=0xff;
		rt_strcpy((char *)NetTxBuffer+PACK_HEAD_LEN,out);
		if(out!=NULL){
				rt_free(out);
				out=NULL;
		}
		if(root!=NULL){
			cJSON_Delete(root);
			out=NULL;
		}

		int len=strlen((char *)NetTxBuffer);
		rt_free(sprinBuf);
		sprinBuf=RT_NULL;
		mcu.repDataMessID =mcu.upMessID;
		upMessIdAdd();
		return true;
}



//�����Ƕ�ȡ�����  ����ĺ�������
void crackMeterRead2Send(bool respFlag)
{
	  int workFlag=RT_FALSE;
		for(int i=0;i<CRACKMETER_485_NUM;i++){
				if(sheet.crackMeter[i].workFlag==RT_TRUE){
						readCrackMeter(i);
						workFlag=RT_TRUE;
				}
		}
		if(workFlag==RT_TRUE){
				rt_kprintf("%s����ɼ���CRACK_METER����\r\n",sign);
				crackMeterJsonPack(respFlag);
			  packMqttSend();

				rt_thread_mdelay(500);
				if(modCrackMeterWarn2Send()==true){
							resetCrackMeterWarnFlag();//ÿ���жϺ�λwarnflag״ֵ̬
							packMqttSend();
				}
		}
}
