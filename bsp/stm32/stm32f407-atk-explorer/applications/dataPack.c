#include  "dataPack.h"


//heartUpStru  heartUp;
extern uint16_t RTU_CRC(uint8_t *puchMsg ,uint16_t usDataLen);
uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];  //��net����buff��Сһ��  ͨ�����䴫�ݸ�NetTxBuffer ���з��ͳ�ȥ
mcuParazStru mcu ={0};

rs485ParaStru dev[NUM];//Ŀǰ4·485�豸
//����messageID����  ÿ�δ��������1
uint32_t upMessIdAdd()
{
	  rt_kprintf("messID:%d\r\n",mcu.upMessID);
		return mcu.upMessID++;
}

//������Ҫ��flash�ж�ȡID
void  devIDRead()
{
		rt_strcpy(mcu.devID  ,"1000000000001");
		rt_strcpy(dev[0].ID,"A000000000001");
		rt_strcpy(dev[0].model,"GY280");
	  rt_strcpy(dev[0].type,"1");
	  rt_strcpy(dev[0].name,"�ӵػ���");
		rt_strcpy(dev[1].ID,"A000000000002");
		rt_strcpy(dev[1].model,"GY281");
	  rt_strcpy(dev[1].type,"2");
	  rt_strcpy(dev[1].name,"�ַż��");
	
		rt_strcpy(dev[2].ID,"A000000000003");
		rt_strcpy(dev[2].model,"GY282");
	  rt_strcpy(dev[2].type,"3");
	  rt_strcpy(dev[2].name,"������");
	
		rt_strcpy(dev[3].ID,"A000000000004");
		rt_strcpy(dev[3].model,"GY283");
	  rt_strcpy(dev[3].type,"4");
	  rt_strcpy(dev[3].name,"���������");
//		rt_strcpy(dev[2].ID,"A000000000003");
//		rt_strcpy(dev[3].ID,"A000000000004");
}


uint64_t subTimeStamp=0;

//��ȡ��������ʱ�����ֵ
uint64_t subTimeStampGet()
{
		return subTimeStamp;
}
//�洢��������ʱ�����ֵ  
 void  subTimeStampSet(uint64_t time)
{
	  subTimeStamp=time-rt_tick_get();//������rtcֵ-��ǰtickֵ

}
uint32_t  utcTime()
{
	return rt_tick_get()+subTimeStampGet();
}
//�������ݴ��
uint16_t heartUpPack()
{
	  
	  memset(packBuf,0,sizeof(packBuf));
		int len=0;
    //head+lenth
	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
	  packBuf[len]= (uint8_t)(HEAD);    len++;
	  len+=LENTH_LEN;//json�����������д
	  //json
	  char str[50]={0};//��ʱʹ�õ�����
		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"packetType\":\"CMD_HEARTBEAT\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_sprintf(str,"\"timestamp\":\"%lu\",",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
			
		rt_strcpy(str,"\"param\":{\"identifier\":\"area_control_unit\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
	
		rt_sprintf(str,"\"id\":\"%s\"}}",mcu.devID);
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//����json����
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//����crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]= (uint8_t)(TAIL>>8); len++;
		packBuf[len]= (uint8_t)(TAIL);    len++;
		packBuf[len] =0;//len++;//��β ��0
		
		mcu.upHeartMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("heart len:%d\r\n",len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\nlen��%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;
}
//���������� ע����Ϣ����һ��task  ����ά��
//1������������������ÿ�η��ͺ�������ʱ�� ����ղ�����Ӧ��һֱ�ط�  ���5��
//2�������÷������� 
//3�����ͷ�2�� �������͵�  �ͻظ��ģ���һ�Σ�
//  1��task


//extern struct rt_mailbox mbNetSendData;
//void netSendQueue()
//{
//	rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
//}

//����ע�����ݴ��
/*
{    
	"mid": 1234,
	    "packetType": "CMD_DEVICE_REGISTER",
	    "params":     [{        
			"model": "GY280",
			"name": "�������",
			"deviceId": "GYNJLXSD000000457",
			"ip": "192.168.1.108",
			"port": "",
			"type": "1"
		},
		{        
			"model": "GY281",
			"name": "�ӵػ���",
			"deviceId": "GYNJLXSD000000458",
			"ip": "192.168.1.108",
			"port": "",
			"type": "2"
		}
	],
	"timestamp": "1655172531937"
}
*/
uint16_t devRegPack()
{
	  memset(packBuf,0,sizeof(packBuf));
		int len=0;
    //head+lenth
	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
	  packBuf[len]= (uint8_t)(HEAD);    len++;
	  len+=LENTH_LEN;//json�����������д
	  //json
	  char str[50]={0};//��ʱʹ�õ�����
		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"packetType\":\"CMD_DEVICE_REGISTER\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"params\":[");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
	
		for(int i=0;i<NUM;i++){
				rt_sprintf(str,"{\"model\":\"%s\",",dev[i].model);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
			
				rt_sprintf(str,"\"name\":\"%s\",",dev[i].name);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
			
				rt_sprintf(str,"\"deviceId\":\"%s\",",dev[i].ID);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
			
				rt_sprintf(str,"\"ip\":\"%s\",",dev[i].ip);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
			
				rt_sprintf(str,"\"port\":\"%s\",",dev[i].port);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
			
				rt_sprintf(str,"\"type\":\"%s\"}",dev[i].type);
				rt_strcpy((char *)packBuf+len,str);
				len+=rt_strlen(str);
				if(i+1==NUM){//���� ] ��
						rt_strcpy((char *)packBuf+len,"],");
						len+=2;
				}
				else{//���� , ��
						rt_strcpy((char *)packBuf+len,",");
						len++;
				}
		}
		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
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
		rt_kprintf("reg len:%d\r\n",len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\nlen��%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;
}

//����ע�����ݴ��
/*
�������
{
    "mid":1234,
    "packetType":"CMD_REPORTDATA",
    "param":
    {
        "identifier":"environment_monitor",
        "acuId":"100000000000001",
        "deviceId":"1000000000002",
        "data":
        {
            "temperature":"22.2",
            "humidity":"80.1",
            "waterLevel":"0.1",
            "oxygen":"20.1",
            "carbonMonoxide":"0",
            "hydrogenSulfide":"0",
            "methane":"0",
            "fanState":"0",
            "pumpState":"0",
            "monitoringTime":"1655172531937"
        }
    },
    "timestamp":"1655172531937"
}
*/


//test only

//uint16_t rs485DataPack()
//{
//	  char num=0;//��1·485
//	  memset(packBuf,0,sizeof(packBuf));
//		int len=0;
//    //head+lenth
////	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
////	  packBuf[len]= (uint8_t)(HEAD);    len++;
////	  len+=LENTH_LEN;//json�����������д
//	  //json
//	  char *str=RT_NULL;//��ʱʹ�õ�����
//	  str= rt_malloc(1024);
//		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"packetType\":\"CMD_REPORTDATA12345678901http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmso1111111111111111122222222222222222222222233333333333\",");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"param\":{");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);

//		rt_strcpy(str,"\"identifier\":\"http:http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600//http://www.cmsoft.cn QQ:10865600www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600environment_monitor11111111111111111111111111111111333333333333333333333http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:1086560033http://www.cmsoft.cn QQ:1086560033\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"deviceId\":\"%s\",",dev[num].ID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_strcpy(str,"\"data\":{");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		
//		rt_strcpy(str,"\"temperature\":\"http://www.cmsoft.cnhttp://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http:http://www.cmsoft.cn QQ:10865600//www.cmsoft.cn QQ:10http://www.cmsoft.cn QQ:10865600865600 QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:108656001111111111111111111111111111111111111111111122.24444444444444444444444444444444444\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"humidity\":\"http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10http://www.cmsoft.cn QQ:10865600865600http://www.cmsoft.cn QQ:108656001112222http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:108656002222222222222222222222222222222222222222280.1444444444444444444444444444444444444http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:10865600http://www.cmsoft.cn QQ:1086560044\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_sprintf(str,"\"monitoringTime\":\"%lu\"}},",utcTime());
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		//lenth
////	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//����json����
////	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
////	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
////	  //crc
////	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//����crc
////	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

////		//tail
////		packBuf[len]= (uint8_t)(TAIL>>8); len++;
////		packBuf[len]= (uint8_t)(TAIL);    len++;
//		packBuf[len] =0;//len++;//��β ��0
//		
//		mcu.devRegMessID =mcu.upMessID;
//		upMessIdAdd();
//		rt_kprintf("reg len:%d\r\n",len);
//		
//		for(int i=0;i<len;i++)
//				rt_kprintf("%02x",packBuf[i]);
//		//rt_kprintf("\r\nlen��%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
//		//rt_kprintf("heart:%s \n",packBuf);
//		rt_free(str);
//		str=RT_NULL;
//		return len;
//}
uint16_t rs485DataPack()
{
	  char num=0;//��1·485
	  memset(packBuf,0,sizeof(packBuf));
		int len=0;
    //head+lenth
	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
	  packBuf[len]= (uint8_t)(HEAD);    len++;
	  len+=LENTH_LEN;//json�����������д
	  //json
	  char str[50]={0};//��ʱʹ�õ�����
		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"packetType\":\"CMD_REPORTDATA\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"param\":{");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);

		rt_strcpy(str,"\"identifier\":\"environment_monitor\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_sprintf(str,"\"deviceId\":\"%s\",",dev[num].ID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_strcpy(str,"\"data\":{");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		
		rt_strcpy(str,"\"temperature\":\"22.2\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"humidity\":\"80.1\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_sprintf(str,"\"monitoringTime\":\"%lu\"}},",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//����json����
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//����crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]= (uint8_t)(TAIL>>8); len++;
		packBuf[len]= (uint8_t)(TAIL);    len++;
		packBuf[len] =0;//len++;//��β ��0
		
		mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("reg len:%d\r\n",len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		//rt_kprintf("\r\nlen��%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;
}


