#include "rs485ThreeAxis.h"

//<<ѹ��ʽ������ GY-STC-2000>> Ĭ�ϲ�����9600  modbus��ַ 0xb1(����) ��������ǩ��2λ
/*
������֤�̵�ַ��04 00 01 00 04

ѹ����֤�̵�ַ��04 00 01 00 02
�����ʸ�Ϊ9600��ѹ�������ö̵�ַ�����붼��һ���ģ�FF FF 03 0A���豸�����ĳ���ַ��01���̵�ַ
*/
//�޸��豸��ַ FF FF 03 0A���豸�����ĳ���ַ��01���̵�ַ 
//   FF FF 03 0A 6E 01 20 21 09 08 00 B1 01 01 
//  24+��ɫ��24-��ɫ��A+��ɫ��B-��ɫ
// �� 01 04 00 01 00 04 A0 09 
// �� 01 04 08 0B CA FE 8D 00 03 03 80 C7 23 
const static char sign[]="[����]";

threeAxisStru threeAxis;
static rt_mutex_t threeAxisMutex = RT_NULL;
#define  MSGPOOL_LEN   200 //485���������  ����1k��Ҫ�޸Ĵ˴�
//���еĶ���
static struct  rt_messagequeue threeAxismque;
static uint8_t threeAxisQuePool[MSGPOOL_LEN];  //
static rt_bool_t  recFlag = RT_FALSE; //ÿ��ѭ������һ�� ���� RT_TRUE ������ɻ��߽��ճ�ʱ��Ϊ RT_FALSE


#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //���������õ���������buf����
//#define   LARGE_TIMES    100 //�Ŵ���  ���������Ҫ ��ȡ�Ĵ���0x000b �п��ܷŴ�10��
extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];



//������ڷ��� 
 void threeAxisUartSend(uint8_t *buf,int len)
{

		rs485UartSend(chanl.threeAxis,buf, len);

}
//���ڽ��պ󶪵�������
rt_err_t threeAxisUartRec(uint8_t dat)
{
	
		if(recFlag==RT_TRUE){
				return rt_mq_send(&threeAxismque, &dat, 1);  //�յ����ݺ���������ﶪ
		}
		else
			  return RT_FALSE;
}

//
//���������õ��Ļ���������Ϣ����  main�е���
void  threeAxisMutexQueueCreat()
{
	  threeAxisMutex = rt_mutex_create("threeAxisMutex", RT_IPC_FLAG_FIFO);
    if (threeAxisMutex == RT_NULL)
    {
        rt_kprintf("%screate threeAxisMutex failed.\n",sign);
        return ;
    }
		
//////////////////////////////////��Ϣ����//////////////////////////////////
		
		int result = rt_mq_init(&threeAxismque,"threeAxismque",&threeAxisQuePool[0],1,sizeof(threeAxisQuePool),RT_IPC_FLAG_FIFO);       
		if (result != RT_EOK)
    {
        rt_kprintf("%sinit threeAxismque failed.\n",sign);
        return ;
    }
}

///////////////////////////////////////��д�Ĵ�����ز���////////////////////////////////////////


//��ѹ��ʽ����������һ��������
extern uint8_t psReadReg(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out);



//�� 1A 04 00 01 00 02 23 E0
//�� 1A 04 04 0B 1B 00 1C 23 6F
void readThreeTempAcc()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = psReadReg(SLAVE_ADDR,0X0001,4,buf);
//	  uint16_t ret =0;
	  recFlag = RT_TRUE;
		rt_mutex_take(threeAxisMutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		threeAxisUartSend(buf,len);
	  rt_kprintf("%sthreeAxis send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		if(rt_mq_recv(&threeAxismque, buf+len, 1, 3000) == RT_EOK){//��һ�ν���ʱ��ų���  ��Ӧʱ���п��ܱȽϾ�
				len++;
		}
		while(rt_mq_recv(&threeAxismque, buf+len, 1, 10) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
		modDev[chanl.threeAxis].offline=RT_FALSE;
		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
		int ret2=modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE);
		if(0 ==  ret2){//ˢ�¶�ȡ����ֵ

        threeAxis.temp	=(buf[offset]<<8)+buf[offset+1];offset+=2;
			  threeAxis.acclrationX = (buf[offset]<<8)+buf[offset+1];offset+=2;
				threeAxis.acclrationY = (buf[offset]<<8)+buf[offset+1];offset+=2;
				threeAxis.acclrationZ = (buf[offset]<<8)+buf[offset+1];offset+=2;
        float temp=(float)((float)threeAxis.temp/100); 

			  rt_kprintf("%stemp:%0.2f*C ACC:X%dmg Y%dmg Z%dmg ok\n",sign,temp,threeAxis.acclrationX,threeAxis.acclrationY,threeAxis.acclrationZ);  
		} 
		else{//��������0
				if(ret2==2){
						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
					  modDev[chanl.threeAxis].offline=RT_TRUE;
				}
			  threeAxis.acclrationX	= 0;
			  threeAxis.acclrationY = 0;
			  threeAxis.acclrationY = 0;
			  rt_kprintf("%stemp height read fail\n",sign);
		}
    //
		recFlag = RT_FALSE;
	  rt_mutex_release(threeAxisMutex);
		rt_free(buf);
	  buf=RT_NULL;

}



/////////////////////////////////////////JSON��ʽ���//////////////////////////////////////////
//�¶ȸ߶�ֵ���

/*
{
    "mid":1234,
    "packetType�":"CMD_REPORTDATA",  
    "param":
    {
        "identifier":" vibration_meter_monitor",
        "acuId":"100000000000001",
        "deviceId":"1000000000004",�
        "data":
        {
            "temp":"22.75", //?
            "accelerationX":"1234",//mg
"accelerationY":"1234",//mg
"accelerationZ":"1234",//mg
            "monitoringTime":"1655172531937"
        }
    },
    "timestamp":"1655172531937"
}
*/

void t3AxisTempAccPack()
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
		
		rt_strcpy(str,"\"packetType\":\"CMD_REPORTDATA\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"param\":{");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);

		rt_strcpy(str,"\"identifier\":\"pressure_settler_monitor\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_sprintf(str,"\"deviceId\":\"%s\",",dev[chanl.threeAxis].ID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_strcpy(str,"\"data\":{");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		
	 	//sprintf(str,"test:%0.2f",(float)121/100);				 
		sprintf(str,"\"temp\":\"%0.2f\",",(float)((float)threeAxis.temp/100));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"accelerationX\":\"%d\",",threeAxis.acclrationX);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		sprintf(str,"\"accelerationY\":\"%d\",",threeAxis.acclrationY);	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		sprintf(str,"\"accelerationZ\":\"%d\",",threeAxis.acclrationZ);	 
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
		
		mcu.repDataMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%sreg len:%d\r\n",sign,len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\n%slen��%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
}