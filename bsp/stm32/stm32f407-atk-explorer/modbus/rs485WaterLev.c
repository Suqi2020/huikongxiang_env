#include "board.h"
//#include "stmflash.h"
//ˮλĬ�ϵ�ַ02  ������9600  ����0~49��
//��ȡˮλ
const static char sign[]="[ˮλ]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //���������õ���������buf����

float waterLev[WATERDEPTH_485_NUM];
//������ڷ��� 
static void waterLevUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.waterLev[num].useUartNum,buf, len);
}



//�� 1A 04 00 01 00 02 23 E0
//�� 1A 04 04 0B 1B 00 1C 23 6F
void readWaterDepth(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.waterLev[num].slaveAddr,0X0002,READ_04,2,buf);
		rt_mutex_take(uartDev[sheet.waterLev[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		waterLevUartSend(num,buf,len);
	  rt_kprintf("%swater send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		

		while(rt_mq_recv(uartDev[sheet.waterLev[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
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
//		uartDev[modbusFlash[WATERLEVEL].useUartNum].offline=RT_FALSE;
		int ret2=modbusRespCheck(sheet.waterLev[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//ˢ�¶�ȡ����ֵ
        uint32_t read	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
        extern float write_hex_to_float(uint32_t number);
			  float waterLev_p= write_hex_to_float(read);
         waterLev[num]=(float)(waterLev_p	/9.8);
			  rt_kprintf("%sˮ��:%0.4f��\n",sign,waterLev[num]);  
		} 
		else{//��������0
				if(ret2==2){
						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
//					  uartDev[modbusFlash[WATERLEVEL].useUartNum].offline=RT_TRUE;
				}
			  waterLev[num]	=0;
			  rt_kprintf("%s read fail\n",sign);
		}
	  rt_mutex_release(uartDev[sheet.waterLev[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}




static uint16_t waterDepthJsonPack()
{
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20���ַ������� ������
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
		cJSON_AddStringToObject(root, "identifier","water_depth");
		cJSON_AddStringToObject(root, "acuId","100000000000001");
		
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < WATERDEPTH_485_NUM; i++)
		{		
			if(sheet.waterLev[i].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.waterLev[i].ID));
				
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				sprintf(sprinBuf,"%02f",waterLev[i]);
				cJSON_AddItemToObject(nodeobj_p,"depth",cJSON_CreateString(sprinBuf));

				sprintf(sprinBuf,"%d",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		}
		}
	
		sprintf(sprinBuf,"%d",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// ��ӡJSON���ݰ�  
		out = cJSON_Print(root);
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

		//���
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json�����������д
		
		// �ͷ��ڴ�  
		
		
		rt_strcpy((char *)packBuf+len,out);
    len+=rt_strlen(out);
	

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
		
		mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%s len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen��%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}

void waterDepthRead2Send(rt_bool_t netStat)
{
	 int workFlag=RT_FALSE;
			for(int i=0;i<WATERDEPTH_485_NUM;i++){
					if(sheet.waterLev[i].workFlag==RT_TRUE){
							readWaterDepth(i);
							workFlag=RT_TRUE;
					}
			}
			if(workFlag==RT_TRUE){
					rt_kprintf("%s����ɼ���waterdepth����\r\n",sign);
					waterDepthJsonPack();
					if(netStat==RT_TRUE)
							rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
			}
}

