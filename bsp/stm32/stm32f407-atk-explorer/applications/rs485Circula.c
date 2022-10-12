#include "rs485Circula.h"

//<<���ڻ���>>  Ŀǰ����ֻ�ɼ�����ֵ
//  1����ȡ����ֵ   
//  2����ȡ������־ 
//  3�����ñ�����ֵ 
//  4����ȡ������ֵ
//  5�����òɼ����
//  6����ȡ�ɼ����
//����RS485������Ҫ���� UART5_IRQHandler�Լ�HAL_UART_Transmit���ʹ���  ����Ҫ������Ӧ���ڵĶ���
// 9600������ ע���޸Ĵ˴� rt_mq_recv(&rs485_2mque, &buf, sizeof(buf), 2)
#define   SLAVE_ADDR     0X01
#define   LENTH          50  //���������õ���������buf����
//#define   LARGE_TIMES    100 //�Ŵ���  ���������Ҫ ��ȡ�Ĵ���0x000b �п��ܷŴ�10��
extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];

CIRCURStru  cirCurStru_p;
uint16_t readAcqInterv(void);
rt_bool_t writeAcqInterv(uint16_t value);
uint32_t readThresholdVal(void);
rt_bool_t writeThresholdVal(uint32_t value);
uint16_t readPoint(void);
rt_bool_t writePoint(uint16_t value);
///////////////////////////////////////��д�Ĵ�����ز���////////////////////////////////////////
//��ȡ����ֵ�ͱ�����Ϣ �Ĵ�����ַ 0x0023 ����12
void readCirCurrAndWaring()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  //uint8_t   buf[100]
	  uint16_t len = modbusReadReg(SLAVE_ADDR,0x0023,12,buf);
	  //485����buf  len  �ȴ�modbus��Ӧ
		UART2_485_SEND;
	  HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,1000);
		UART2_485_REC;
	  rt_kprintf("readCirCurrAndWaring send:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
	  rt_mutex_take(rs485_2Mutex,RT_WAITING_FOREVER);
    len=0;
		while(rt_mq_recv(&rs485_2mque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		rt_kprintf("rec:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
		
		if(RT_TRUE ==  modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE)){//ˢ�¶�ȡ����ֵ
				cirCurStru_p.circlCurA=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				cirCurStru_p.circlCurB=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				cirCurStru_p.circlCurC=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				cirCurStru_p.circlCurD=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  cirCurStru_p.warningA	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  cirCurStru_p.warningB	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  cirCurStru_p.warningC	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  cirCurStru_p.warningD	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  rt_kprintf("��ȡ����������ֵ�ɹ�\r\n");
		} 

	  rt_mutex_release(rs485_2Mutex);
		rt_free(buf);
	  buf=RT_NULL;
}
//ÿ���ϵ����Ҫ��flash�ж����洢��ֵ��modbus�ظ���ֵ���бȽ�
void cirCurrConf()
{
		//������ֵ ������Ҫ��flash�ж�ȡ
	  rt_thread_mdelay(1000); //��ʼ��ʱ����Ҫ��ʱ �ȴ��豸��ʼ�����
		cirCurStru_p.AcqInterv=5;
		cirCurStru_p.point =100;       //��modbus��ʵֵ  �˴���ȡmodbus�󾭹���ת������ֱ�Ӽ���  0 -������100�� 1-2 ������10��
	  cirCurStru_p.thresholdVal=350;//3.5A������100��
//		rt_kprintf("readAcqInterv:%d\n",readAcqInterv());
//		rt_kprintf("readThresholdVal:%d\n",readThresholdVal());
//		rt_kprintf("readPoint:%d\n",readPoint());
 
	  if(readAcqInterv()!=cirCurStru_p.AcqInterv){
				if(RT_FALSE==writeAcqInterv(cirCurStru_p.AcqInterv)){
						rt_kprintf("writeAcqInterv err\n");
				}
		}
		if(readThresholdVal()!=cirCurStru_p.thresholdVal){
				if(RT_FALSE==writeThresholdVal(cirCurStru_p.thresholdVal)){
						rt_kprintf("writeThresholdVal err\n");
				}
		}
		if(readPoint()!=cirCurStru_p.point){
				if(RT_FALSE==writePoint(cirCurStru_p.point)){
						rt_kprintf("writePoint err\n");
				}
		}

}

//��ȡ�ɼ����
uint16_t readAcqInterv()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(SLAVE_ADDR,0x0004,1,buf);
	  uint16_t ret =0;
		rt_mutex_take(rs485_2Mutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		UART2_485_SEND;
	  HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,1000);
		UART2_485_REC;
	  rt_kprintf("readAcqInterv send:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		while(rt_mq_recv(&rs485_2mque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		rt_kprintf("rec:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
		
		if(RT_TRUE == modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE)){//ˢ�¶�ȡ����ֵ

			  ret	=(buf[offset]<<8)	+buf[offset+1];	//offset+=2;
			  rt_kprintf("��ȡ�ɼ�����ɹ� %d\r\n",ret);
		} 
    //
	  rt_mutex_release(rs485_2Mutex);
		rt_free(buf);
	  buf=RT_NULL;
		return ret;
}

//���òɼ����
//01 06 00 04 00 05 08 08	# RECV HEX>
//01 06 00 04 00 05 08 08
rt_bool_t writeAcqInterv(uint16_t value)
{
		
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusWriteOneReg(SLAVE_ADDR,0x0004,value,buf);//modbusWriteReg(SLAVE_ADDR,0x0004,1,buf);
		rt_mutex_take(rs485_2Mutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		UART2_485_SEND;
	  HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,1000);
		UART2_485_REC;
	  rt_kprintf("wrAcqInterv send:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		while(rt_mq_recv(&rs485_2mque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		rt_kprintf("rec:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//��ȡ����ֵ ��һ���ж�crc �ڶ��ж϶Դ�
		
		if(RT_TRUE ==  modbusRespCheck(SLAVE_ADDR,buf,len,RT_FALSE)){//ˢ�¶�ȡ����ֵ
        if(buf[1]==WRITE){
						return RT_TRUE;
				}
		} 
    //
	  rt_mutex_release(rs485_2Mutex);
		rt_free(buf);
	  buf=RT_NULL;
	
	
		return RT_FALSE;
}
//��ȡ������ֵ
uint32_t readThresholdVal()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(SLAVE_ADDR,0x0009,4,buf);
	  uint32_t ret =0;
		rt_mutex_take(rs485_2Mutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		UART2_485_SEND;
	  HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,1000);
		UART2_485_REC;
	  rt_kprintf("readthresholdVal send:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		while(rt_mq_recv(&rs485_2mque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		rt_kprintf(" rec:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
		
		if(RT_TRUE ==  modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE)){//ˢ�¶�ȡ����ֵ

			  ret	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];//offset+=2;
			  rt_kprintf("��ȡ������ֵ %d\r\n",ret);
		} 
    //
	  rt_mutex_release(rs485_2Mutex);
		rt_free(buf);
	  buf=RT_NULL;
		return ret;
}
//���ñ�����ֵ  rt_true �ɹ� rt-false ʧ��
rt_bool_t writeThresholdVal(uint32_t value)
{

		uint8_t  *buf = rt_malloc(LENTH);
	  uint8_t send[4]={0};
		send[0]=(uint8_t)(value>>24);
		send[1]=(uint8_t)(value>>16);
		send[2]=(uint8_t)(value>>8);
		send[3]=value;
	  uint16_t len = modbusWriteMultReg(SLAVE_ADDR,0x0009,sizeof(uint32_t),send,buf);
//	  uint16_t ret =0;
		rt_mutex_take(rs485_2Mutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		UART2_485_SEND;
	  HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,1000);
		UART2_485_REC;
	  rt_kprintf("wrAcqInterv send:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
 
    len=0;
		while(rt_mq_recv(&rs485_2mque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		rt_kprintf("rec:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//��ȡ����ֵ ��һ���ж�crc �ڶ��ж϶Դ�
		
		if(RT_TRUE ==  modbusRespCheck(SLAVE_ADDR,buf,len,RT_FALSE)){//ˢ�¶�ȡ����ֵ
        if(buf[1]==WRITE_MUL){
						return RT_TRUE;
				}
		} 
    //
	  rt_mutex_release(rs485_2Mutex);
		rt_free(buf);
	  buf=RT_NULL;
	
	
		return RT_FALSE;
	
}
//��ȡС������ʽ
uint16_t readPoint()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(SLAVE_ADDR,0x000B,1,buf);
	  uint16_t ret =0;
		rt_mutex_take(rs485_2Mutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		UART2_485_SEND;
	  HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,1000);
		UART2_485_REC;
	  rt_kprintf("readPoint send:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		while(rt_mq_recv(&rs485_2mque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		rt_kprintf(" rec:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
		
		if(RT_TRUE ==  modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE)){//ˢ�¶�ȡ����ֵ

			  ret	=(buf[offset]<<8)	+buf[offset+1];	//offset+=2;
			  if(ret==0)
						cirCurStru_p.point =100;
				else
						cirCurStru_p.point =10;
			  rt_kprintf("��ȡС���� %d\r\n",ret);
		} 
    //
	  rt_mutex_release(rs485_2Mutex);
		rt_free(buf);
	  buf=RT_NULL;
		return ret;
}
//����С������ʽ
//���òɼ����
rt_bool_t writePoint(uint16_t value)
{
//	uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusWriteOneReg(SLAVE_ADDR,0x000b,value,buf);//modbusWriteReg(SLAVE_ADDR,0x0004,1,buf);
//	  uint16_t ret =0;
		rt_mutex_take(rs485_2Mutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		UART2_485_SEND;
	  HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,1000);
		UART2_485_REC;
	  rt_kprintf("wrAcqInterv send:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		while(rt_mq_recv(&rs485_2mque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		rt_kprintf("rec:");
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//��ȡ����ֵ ��һ���ж�crc �ڶ��ж϶Դ�
		
		if(RT_TRUE ==  modbusRespCheck(SLAVE_ADDR,buf,len,RT_FALSE)){//ˢ�¶�ȡ����ֵ
        if(buf[1]==WRITE){
						return RT_TRUE;
				}
		} 
    //
	  rt_mutex_release(rs485_2Mutex);
		rt_free(buf);
	  buf=RT_NULL;
	
	
		return RT_FALSE;
	

}
//�ж��Ƿ��б��� ��Ҫ��readCirCurrAndWaring()���ʹ��
rt_bool_t waringcheck()
{
		if((cirCurStru_p.warningA)||(cirCurStru_p.warningA)||(cirCurStru_p.warningA)|(cirCurStru_p.warningA)){
			 rt_kprintf("ERR:����ֵ���� �������� \n\r");
				return RT_TRUE;
		}
		return RT_FALSE;
}
//����������ʱ���еĴ��  ���ǰ��Ҫ���� readCirCurrAndWaring()
/*
{
     "mid":1234,
     "packetType":"CMD_REPORTDATA",
     "param":
     {
          "identifier":"grounding_current_monitor",
          "acuId":"100000000000001",
          "deviceId":"1000000000003",?
          "data":
          {
               "earthCurA":"22.2",
               "runCurA":"",
               "loadRatioA":"",
               "earthCurB":"22.2",
               "runCurB":"",
               "loadRatioB":"",
               "earthCurC":"22.2",
               "runCurC":"",
               "loadRatioC":"",
               "monitoringTime":"1655172531937"
          }
     },
     "timestamp":"1655172531937"
}
*/



/////////////////////////////////////////JSON��ʽ���//////////////////////////////////////////
//����ֵ���
uint16_t 	cirCulaDataPack()
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

		rt_strcpy(str,"\"identifier\":\"grounding_current_monitor\",");
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
	 	//sprintf(str,"test:%0.2f",(float)121/100);				 
		sprintf(str,"\"earthCurA\":\"%0.2f\"}},",(float)(cirCurStru_p.circlCurA/cirCurStru_p.point));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"runCurA\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"loadRatioA\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		sprintf(str,"\"earthCurB\":\"%0.2f\"}},",(float)(cirCurStru_p.circlCurB/cirCurStru_p.point));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"runCurB\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"loadRatioB\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		sprintf(str,"\"earthCurC\":\"%0.2f\"}},",(float)(cirCurStru_p.circlCurC/cirCurStru_p.point));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"runCurC\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"loadRatioC\":\"\",");
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
		rt_kprintf("\r\nlen��%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;
}
//�澯��Ϣ�Ĵ��  readCirCurrAndWaring()
void  waringEventPack()
{
		rt_kprintf("���ڼ��� \n\r");
		
}




