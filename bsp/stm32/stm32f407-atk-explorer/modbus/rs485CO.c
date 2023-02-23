#include "board.h"
//#include "stmflash.h"
//һ����̼Ĭ�ϵ�ַ04 ������9600
//04 03 00 02 00 02 65 9E
//04 03 04 00 00 00 00 AF 33 
//ÿ������co��ȡ��ʱ������ȡ4�����岢ʵ���ϴ� 
//4��������һ���ȡ ����ǰ������ʹ�� ֻ��CO��ʱ���ط���ȡ���������  �ر�ʱ��ֻ��Ҫ�ر�CO�Ϳ��԰���������ȫ���ر�
#ifdef USE_4GAS
const static char sign[]="[һ����̼]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //���������õ���������buf����


static float co[CO_485_NUM];
static uint8_t respStat[CO_485_NUM];

//������ڷ��� 
static void coUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.co[num].useUartNum,buf, len);
}
//����co��ͨѶ״̬ true--ͨѶ�ɹ� false--ͨѶʧ��
int coState(int i)
{
		return respStat[i];
}

//co�Ƚ���ֵ��������Ӧ��flag���
static void coCheckSetFlag(int num)
{
	
		if(co[num]>=sheet.modbusCo[num].coUpLimit)
				inpoutpFlag.modbusCo[num].coUpFlag=true;
		else
				inpoutpFlag.modbusCo[num].coUpFlag=false;
		if(co[num]<=sheet.modbusCo[num].coLowLimit)
				inpoutpFlag.modbusCo[num].coLowFlag=true;
		else
				inpoutpFlag.modbusCo[num].coLowFlag=false;
}
//�� 1A 04 00 01 00 02 23 E0
//�� 1A 04 04 0B 1B 00 1C 23 6F
//��ȡcoֵ
void readCO(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.co[num].slaveAddr,0X0002,READ_03,2,buf);
		rt_mutex_take(uartDev[sheet.co[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		coUartSend(num,buf,len);
	  rt_kprintf("%sCO send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		

		while(rt_mq_recv(uartDev[sheet.co[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
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

		int ret2=modbusRespCheck(sheet.co[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//ˢ�¶�ȡ����ֵ
        int value	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
        co[num]=(float)((float)value	/1000);
			  respStat[num]=1;
			  rt_kprintf("%sŨ��ֵ:%0.2fmol/Lread ok\n",sign,co[num]);  
			
			
				coCheckSetFlag(num);
		} 
		else{//��������0
			  co[num]	=0;
			  respStat[num]=0;
			  rt_kprintf("%s read fail\n",sign);
		}
		coCheckSetFlag(num);
	  rt_mutex_release(uartDev[sheet.co[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

		
		
		

		

}




/*gasJsonPack��������
static uint16_t coJsonPack()
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
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_REPORTDATA");
		cJSON_AddStringToObject(root, "identifier","environment_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < CO_485_NUM; i++)
		{		
			if(sheet.co[i].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.co[i].ID));
				sprintf(sprinBuf,"%d",respStat[i]);
				cJSON_AddItemToObject(nodeobj,"responseStatus",cJSON_CreateString(sprinBuf));
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				sprintf(sprinBuf,"%02f",co[i]);
				cJSON_AddItemToObject(nodeobj_p,"deepness",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%llu",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		}
		}
	
		sprintf(sprinBuf,"%llu",utcTime());
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

*/
//co��������¶�ȡֵ
void coRead2Send()
{
	  //int workFlag=RT_FALSE;
	  for(int i=0;i<CO_485_NUM;i++){
			if(sheet.co[i].workFlag==RT_TRUE){
						readCO(i);
					//	workFlag=RT_TRUE;
				}
		}

}
//4��������һ���ڹ�������Ϊ���崫�����ڹ���
rt_bool_t gasWork(int num)
{
		if((sheet.o2[num].workFlag==RT_TRUE)||\
			(sheet.h2s[num].workFlag==RT_TRUE)||\
		  (sheet.ch4[num].workFlag==RT_TRUE)||\
		(sheet.co[num].workFlag==RT_TRUE)){
				return   RT_TRUE;
		}
		return RT_FALSE;
}
extern  float o2[O2_485_NUM];;
extern  float h2s[H2S_485_NUM];
extern   float ch4[CH4_485_NUM];
//4��������
int  gasPack(int num)
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
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_REPORTDATA");
		cJSON_AddStringToObject(root, "identifier","gas_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		//for (int i = 0; i < CO_485_NUM; i++)
		{		
			//if(sheet.co[num].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.co[num].ID));
				sprintf(sprinBuf,"%d",1);//respStat[num]);
				cJSON_AddItemToObject(nodeobj,"responseStatus",cJSON_CreateString(sprinBuf));
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				if(sheet.co[num].workFlag==RT_TRUE){
					sprintf(sprinBuf,"%02f",co[num]);
					cJSON_AddItemToObject(nodeobj_p,"monoxide",cJSON_CreateString(sprinBuf));
				}
				if(sheet.o2[num].workFlag==RT_TRUE){
					sprintf(sprinBuf,"%02f",o2[num]);
					cJSON_AddItemToObject(nodeobj_p,"oxy",cJSON_CreateString(sprinBuf));
				}
				if(sheet.h2s[num].workFlag==RT_TRUE){
					sprintf(sprinBuf,"%02f",h2s[num]);
					cJSON_AddItemToObject(nodeobj_p,"hydrogenSulfide",cJSON_CreateString(sprinBuf));
				}
				if(sheet.ch4[num].workFlag==RT_TRUE){
					sprintf(sprinBuf,"%02f",ch4[num]);
					cJSON_AddItemToObject(nodeobj_p,"methane",cJSON_CreateString(sprinBuf));
				}
				sprintf(sprinBuf,"%llu",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		//}
		}
	
		sprintf(sprinBuf,"%llu",utcTime());
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
//4������json����Ķ��η�װ
void  gasJsonPack(rt_bool_t netStat)
{
	

				rt_kprintf("%s����ɼ���gas����\r\n",sign);
	
	
	      for(int i=0;i<CO_485_NUM;i++){
					  if(gasWork(i)==RT_TRUE){
								gasPack(i);
							if(netStat==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
							rt_thread_mdelay(2000);//��ʱ����
						}
				}


}

#endif

