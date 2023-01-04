#include "board.h"
//#include "stmflash.h"
//<<���ڻ��� GY-JDHL03>>  Ŀǰ����ֻ�ɼ�����ֵ
//��Ӧʱ�� 200ms  Ĭ�ϲ�����115200
//  1����ȡ����ֵ   
//  2����ȡ������־ 
//  3�����ñ�����ֵ 
//  4����ȡ������ֵ
//  5�����òɼ����
//  6����ȡ�ɼ����
//����RS485������Ҫ���� UART5_IRQHandler�Լ�HAL_UART_Transmit���ʹ���  
//����Ҫ������Ӧ���ڵĶ��� ���� char num=0;
// 9600������ ע���޸Ĵ˴� rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, &buf, sizeof(buf), 2)
//Ѹ���л�����485�ӿ���ʹ�� ������ֻ��Ҫ�޸Ĵ��ڷ��ͽӿ� ���жϽ��սӿڼ���
// rs485Circula.c-cirCurrUartSend(uint8_t *buf,int len) ��drv_uart.c-USART2_IRQHandler��
// cirCurrUartSend(uint8_t *buf,int len)   cirCurrUartRec(uint8_t dat)
typedef struct
{
	  //����ֵ �Ŵ���100��
		float circlCurA;
		float circlCurB;
	  float circlCurC;
	  float circlCurD;//����
	  //��ֵ
	  //uint32_t thresholdVal;

	  
	  //����״̬  һ���ֽ��㹻 ����Խ�modbus��Ӧ
	  uint16_t warningA;
	  uint16_t warningB;
	  uint16_t warningC;
	  uint16_t warningD;
	  
	  //�ɼ���� ��λ��
		//uint16_t AcqInterv;
	//С���������ֵ
    uint16_t point; //��modbus��ʵֵ  �˴���ȡmodbus�󾭹���ת������ֱ�Ӽ���  0-ֵΪ100  1-2 ֵΪ10
	  uint8_t respStat;
} CIRCURStru;

const static char sign[]="[����]";

//#define   SLAVE_ADDR     0X02
#define   LENTH          50  //���������õ���������buf����
extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];

static CIRCURStru  cirCurStru_p[CIRCULA_485_NUM];
static uint16_t readAcqInterv(int num);
static uint16_t readPoint(int num);
static uint32_t readThresholdVal(int num);
static rt_bool_t writeAcqInterv(uint16_t value);
static rt_bool_t writeThresholdVal(uint32_t value);
static rt_bool_t writePoint(uint16_t value);

int cirCurrtState(int i)
{
		return cirCurStru_p[i].respStat;
}
//������ڷ��� 
void cirCurrUartSend(int num,uint8_t *buf,int len)
{

		rs485UartSend(sheet.cirCula[num].useUartNum,buf, len);

}


///////////////////////////////////////��д�Ĵ�����ز���////////////////////////////////////////
//��ȡ����ֵ�ͱ�����Ϣ �Ĵ�����ַ 0x0023 ����12
void readCirCurrAndWaring(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  //uint8_t   buf[100]
	  uint16_t len = modbusReadReg(sheet.cirCula[num].slaveAddr,0x0023,READ_03,12,buf);
	  //485����buf  len  �ȴ�modbus��Ӧ
	  //recFlag = RT_TRUE;
	  cirCurrUartSend(num,buf,len);

	  rt_kprintf("%sreadCirCurrAndWaring send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
	  rt_mutex_take(uartDev[sheet.cirCula[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
		memset(buf,0,LENTH);
    len=0;
		while(rt_mq_recv(uartDev[sheet.cirCula[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
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
		int ret=modbusRespCheck(sheet.cirCula[num].slaveAddr,buf,len,RT_TRUE);
		if(0 ==  ret){//ˢ�¶�ȡ����ֵ
			  int cula=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
		    cirCurStru_p[num].circlCurA=(float)cula/cirCurStru_p[num].point;
			  cula=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  cirCurStru_p[num].circlCurB=(float)cula/cirCurStru_p[num].point;
				cula=cirCurStru_p[num].circlCurC=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  cirCurStru_p[num].circlCurC=(float)cula/cirCurStru_p[num].point;
				cula=cirCurStru_p[num].circlCurD=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  cirCurStru_p[num].circlCurD=(float)cula/cirCurStru_p[num].point;
			  cirCurStru_p[num].warningA	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  cirCurStru_p[num].warningB	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  cirCurStru_p[num].warningC	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  cirCurStru_p[num].warningD	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  rt_kprintf("%s��ȡ����������ֵ�ɹ�\r\n",sign);
			  cirCurStru_p[num].respStat=1;
		} 
		else{//��������0
			  if(ret==2){
						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
				}
				cirCurStru_p[num].respStat=0;
				cirCurStru_p[num].circlCurA=0;
				cirCurStru_p[num].circlCurB=0;
				cirCurStru_p[num].circlCurC=0;
				cirCurStru_p[num].circlCurD=0;
			  cirCurStru_p[num].warningA	=0;
			  cirCurStru_p[num].warningB	=0;
			  cirCurStru_p[num].warningC	=0;
			  cirCurStru_p[num].warningD	=0;
			  rt_kprintf("%s��ȡ����������ֵfail\r\n",sign);
		}
		//recFlag = RT_FALSE;
	  rt_mutex_release(uartDev[sheet.cirCula[num].useUartNum].uartMutex);
	//	 rt_kprintf("release\r\n");
		rt_free(buf);
	//	 rt_kprintf("free\r\n");
	  buf=RT_NULL;
}
//ÿ���ϵ����Ҫ��flash�ж����洢��ֵ��modbus�ظ���ֵ���бȽ�
//void cirCurrConf()
//{
//		//������ֵ ������Ҫ��flash�ж�ȡ
//	  rt_thread_mdelay(1000); //��ʼ��ʱ����Ҫ��ʱ �ȴ��豸��ʼ�����
//		cirCurStru_p.AcqInterv=5;
//		cirCurStru_p.point =100;       //��modbus��ʵֵ  �˴���ȡmodbus�󾭹���ת������ֱ�Ӽ���  0 -������100�� 1-2 ������10��
//	  cirCurStru_p.thresholdVal=350;//3.5A������100��
////		rt_kprintf("readAcqInterv:%d\n",readAcqInterv());
////		rt_kprintf("readThresholdVal:%d\n",readThresholdVal());
////		rt_kprintf("readPoint:%d\n",readPoint());
// 
//	  if(readAcqInterv()!=cirCurStru_p.AcqInterv){
//				if(RT_FALSE==writeAcqInterv(cirCurStru_p.AcqInterv)){
//						rt_kprintf("%swriteAcqInterv err\n",sign);
//				}
//		}
//		if(readThresholdVal()!=cirCurStru_p.thresholdVal){
//				if(RT_FALSE==writeThresholdVal(cirCurStru_p.thresholdVal)){
//						rt_kprintf("%swriteThresholdVal err\n",sign);
//				}
//		}
//		if(readPoint()!=cirCurStru_p.point){
//				if(RT_FALSE==writePoint(cirCurStru_p.point)){
//						rt_kprintf("%swritePoint err\n",sign);
//				}
//		}

//}

////��ȡ�ɼ����
//uint16_t readAcqInterv()
//{
//	  uint8_t offset=3;//add+regadd+len
//	  uint8_t  *buf = RT_NULL;
//		buf = rt_malloc(LENTH);
//	  uint16_t len = modbusReadReg(modbusFlash[CIRCULA].slaveAddr,0x0004,READ_03,1,buf);
//	  uint16_t ret =0;
//		//recFlag = RT_TRUE;
//		rt_mutex_take(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex,RT_WAITING_FOREVER);
//	  //485����buf  len  �ȴ�modbus��Ӧ
//		cirCurrUartSend(buf,len);
//	  rt_kprintf("%sreadAcqInterv send:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
//		memset(buf,0,LENTH);
//    len=0;
//		while(rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
//				len++;
//		}
//		rt_kprintf("%srec:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
////		uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_FALSE;
//		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
//		int ret2=modbusRespCheck(modbusFlash[CIRCULA].slaveAddr,buf,len,RT_TRUE);
//		if(0 == ret2){//ˢ�¶�ȡ����ֵ

//			  ret	=(buf[offset]<<8)	+buf[offset+1];	//offset+=2;
//			  rt_kprintf("%s��ȡ�ɼ�����ɹ� %d\r\n",sign,ret);
//		} 
//		else{
//				if(ret2==2){
////					   uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_TRUE;
//						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
//				}
//		}
//		
//    //
//		//recFlag = RT_FALSE;
//	  rt_mutex_release(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex);
//		rt_free(buf);
//	  buf=RT_NULL;
//		return ret;
//}

//���òɼ����
//01 06 00 04 00 05 08 08	# RECV HEX>
//01 06 00 04 00 05 08 08
//rt_bool_t writeAcqInterv(uint16_t value)
//{
//		
//	  uint8_t  *buf = RT_NULL;
//		buf = rt_malloc(LENTH);
//	  uint16_t len = modbusWriteOneReg(modbusFlash[CIRCULA].slaveAddr,0x0004,value,buf);//modbusWriteReg(SLAVE_ADDR,0x0004,1,buf);
//		rt_mutex_take(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex,RT_WAITING_FOREVER);
//	  //recFlag = RT_TRUE;
//	  rt_bool_t ret=RT_FALSE;
//	  //485����buf  len  �ȴ�modbus��Ӧ
//		cirCurrUartSend(buf,len);
//	  rt_kprintf("%swrAcqInterv send:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
//		memset(buf,0,LENTH);
//    len=0;
//		while(rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
//				len++;
//		}
//		rt_kprintf("%srec:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
////		uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_FALSE;
//		//��ȡ����ֵ ��һ���ж�crc �ڶ��ж϶Դ�
//		int ret2= modbusRespCheck(modbusFlash[CIRCULA].slaveAddr,buf,len,RT_FALSE);
//		if(0 ==  ret2){//ˢ�¶�ȡ����ֵ
//        if(buf[1]==WRITE){
//						ret= RT_TRUE;
//				}
//		} 
//		else{
//				if(ret2==2){
////					  uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_TRUE;
//						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
//				}
//		}
//    //
//		//recFlag = RT_FALSE;
//	  rt_mutex_release(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex);
//		rt_free(buf);
//	  buf=RT_NULL;
//	  
//	
//		return ret;
//}
//��ȡ������ֵ
//uint32_t readThresholdVal(int num)
//{
//	  uint8_t offset=3;//add+regadd+len
//	  uint8_t  *buf = RT_NULL;
//		buf = rt_malloc(LENTH);
//	  uint16_t len = modbusReadReg(modbusFlash[CIRCULA].slaveAddr,0x0009,READ_03,4,buf);
//	  uint32_t ret =0;		
//	  //recFlag = RT_TRUE;

//		rt_mutex_take(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex,RT_WAITING_FOREVER);
//	  //485����buf  len  �ȴ�modbus��Ӧ
//		cirCurrUartSend(buf,len);
//	  rt_kprintf("%sreadthresholdVal send:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
//		memset(buf,0,LENTH);
//    len=0;
//		while(rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
//				len++;
//		}
//		rt_kprintf("%srec:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
////		uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_FALSE;
//		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
//		int ret2=modbusRespCheck(modbusFlash[CIRCULA].slaveAddr,buf,len,RT_TRUE);
//		if(0 ==ret2){//ˢ�¶�ȡ����ֵ

//			  ret	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];//offset+=2;
//			  rt_kprintf("%s��ȡ������ֵ %d\r\n",sign,ret);
//		} 
//		else{
//				if(ret2==2){
////						uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_TRUE;
//						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
//				}
//		}
//		//recFlag = RT_FALSE;
//	  rt_mutex_release(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex);
//		rt_free(buf);
//	  buf=RT_NULL;
//		return ret;
//}
//���ñ�����ֵ  rt_true �ɹ� rt-false ʧ��
//rt_bool_t writeThresholdVal(uint32_t value)
//{
//    rt_bool_t ret=RT_FALSE;
//	  uint8_t  *buf = RT_NULL;
//		buf = rt_malloc(LENTH);
//	  uint8_t sendD[4]={0};
//		sendD[0]=(uint8_t)(value>>24);
//		sendD[1]=(uint8_t)(value>>16);
//		sendD[2]=(uint8_t)(value>>8);
//		sendD[3]=value;
//	  uint16_t len = modbusWriteMultReg(modbusFlash[CIRCULA].slaveAddr,0x0009,sizeof(uint32_t),sendD,buf);
//    //recFlag = RT_TRUE;
//		rt_mutex_take(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex,RT_WAITING_FOREVER);  
//	  //485����buf  len  �ȴ�modbus��Ӧ
//		cirCurrUartSend(buf,len);
//	  rt_kprintf("%swrAcqInterv send:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
//		memset(buf,0,LENTH);
//    len=0;
//		while(rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
//				len++;
//		}
//		rt_kprintf("%srec:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
////		uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_FALSE;
//		//��ȡ����ֵ ��һ���ж�crc �ڶ��ж϶Դ�
//		int ret2 = modbusRespCheck(modbusFlash[CIRCULA].slaveAddr,buf,len,RT_FALSE);
//		if(0 ==  ret2){//ˢ�¶�ȡ����ֵ
//        if(buf[1]==WRITE_MUL){
//						ret= RT_TRUE;
//				}
//		} 
//		else{
//				if(ret2==2){
/////					  uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_TRUE;
//						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
//				}
//		}
//		//recFlag = RT_FALSE;
//	  rt_mutex_release(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex);
//		rt_free(buf);
//	  buf=RT_NULL;
//	
//	
//		return ret;
//	
//}
//��ȡС������ʽ
static uint16_t readPoint(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.cirCula[num].slaveAddr,0x000B,READ_03,1,buf);
	  uint16_t ret =0;
	//	recFlag = RT_TRUE;
		rt_mutex_take(uartDev[sheet.cirCula[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485����buf  len  �ȴ�modbus��Ӧ
		cirCurrUartSend(num,buf,len);
	  rt_kprintf("%sreadPoint send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		memset(buf,0,LENTH);
    len=0;
		while(rt_mq_recv(uartDev[sheet.cirCula[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
				len++;
		}
		rt_kprintf("%srec:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//��ȡ����ֵ ��һ���ж�crc �ڶ�����ȡ
		int ret2=modbusRespCheck(sheet.cirCula[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//ˢ�¶�ȡ����ֵ

			  ret	=(buf[offset]<<8)	+buf[offset+1];	//offset+=2;
			  if(ret==0)
						cirCurStru_p[num].point =100;
				else
						cirCurStru_p[num].point =10;
			  rt_kprintf("%s��ȡС���� %d\r\n",sign,cirCurStru_p[num].point);
		} 
		else{
			  cirCurStru_p[num].point =100;//����Ĭ��ֵ

		}
	  rt_mutex_release(uartDev[sheet.cirCula[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;
		return ret;
}

void readMultiCirCulaPoint()
{
		for (int i = 0; i < CIRCULA_485_NUM; i++)
		{		
				if(sheet.cirCula[i].workFlag==RT_TRUE){
						readPoint(i);
				}
		}
}
//����С������ʽ
//���òɼ����
//rt_bool_t writePoint(uint16_t value)
//{

//	  uint8_t  *buf = RT_NULL;
//		buf = rt_malloc(LENTH);
//	  uint16_t len = modbusWriteOneReg(modbusFlash[CIRCULA].slaveAddr,0x000b,value,buf);//modbusWriteReg(SLAVE_ADDR,0x0004,1,buf);
//		rt_mutex_take(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex,RT_WAITING_FOREVER);
//		//recFlag = RT_TRUE;
//		rt_bool_t ret=RT_FALSE;
//	  //485����buf  len  �ȴ�modbus��Ӧ
//		cirCurrUartSend(buf,len);
//	  rt_kprintf("%swrAcqInterv send:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
//		memset(buf,0,LENTH);
//    len=0;
//		while(rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 ������1ms 10������
//				len++;
//		}
//		if(len!=0){
//				rt_kprintf("%srec:",sign);
//				for(int j=0;j<len;j++){
//						rt_kprintf("%x ",buf[j]);
//				}
//				rt_kprintf("\n");
//		}
////		uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_FALSE;
//		//��ȡ����ֵ ��һ���ж�crc �ڶ��ж϶Դ�
//		int ret2=modbusRespCheck(modbusFlash[CIRCULA].slaveAddr,buf,len,RT_FALSE);
//		if(0 == ret2 ){//ˢ�¶�ȡ����ֵ
//        if(buf[1]==WRITE){
//						ret= RT_TRUE;
//				}
//		} 
//		else{
//				if(ret2==2){
////					  uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_TRUE;
//						//rt_kprintf("%sERR:����485���߻��߹���\r\n",sign);
//				}
//		}
//		//recFlag = RT_FALSE;
//	  rt_mutex_release(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex);
//		rt_free(buf);
//	  buf=RT_NULL;
//	
//	
//		return ret;
//	

//}

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
//uint16_t 	cirCulaDataPack()
//{
//	 
//	  memset(packBuf,0,sizeof(packBuf));
//		int len=0;
//    //head+lenth
//	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
//	  packBuf[len]= (uint8_t)(HEAD);    len++;
//	  len+=LENTH_LEN;//json�����������д
//	  //json
//	  char str[50]={0};//��ʱʹ�õ�����
//		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"packetType\":\"CMD_REPORTDATA\",");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"param\":{");
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);

//		rt_strcpy(str,"\"identifier\":\"grounding_current_monitor\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		sprintf(str,"\"deviceId\":\"%s\",",devi[CIRCULA].ID);
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);

//		rt_strcpy(str,"\"data\":{");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//	 	//sprintf(str,"test:%0.2f",(float)121/100);				 
//		sprintf(str,"\"earthCurA\":\"%0.2f\",",(float)((float)cirCurStru_p.circlCurA/cirCurStru_p.point));	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"runCurA\":\"\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		rt_strcpy(str,"\"loadRatioA\":\"\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		sprintf(str,"\"earthCurB\":\"%0.2f\",",(float)((float)cirCurStru_p.circlCurB/cirCurStru_p.point));	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"runCurB\":\"\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		rt_strcpy(str,"\"loadRatioB\":\"\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		sprintf(str,"\"earthCurC\":\"%0.2f\",",(float)((float)cirCurStru_p.circlCurC/cirCurStru_p.point));	 
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_strcpy(str,"\"runCurC\":\"\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		rt_strcpy(str,"\"loadRatioC\":\"\",");
//		rt_strcpy((char *)packBuf+len,str);
//		len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"monitoringTime\":\"%lu\"}},",utcTime());
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
//		rt_strcpy((char *)packBuf+len,str);
//    len+=rt_strlen(str);
//		
//		//lenth
//	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//����json����
//	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
//	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
//	  //crc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//����crc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

//		//tail
//		packBuf[len]= (uint8_t)(TAIL>>8); len++;
//		packBuf[len]= (uint8_t)(TAIL);    len++;
//		packBuf[len] =0;//len++;//��β ��0
//		
//		mcu.repDataMessID =mcu.upMessID;
//		upMessIdAdd();
//		rt_kprintf("%scircula len:%d\r\n",sign,len);
//		
////		for(int i=0;i<len;i++)
////				rt_kprintf("%02x",packBuf[i]);
//		rt_kprintf("\r\n%slen��%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
//		//rt_kprintf("heart:%s \n",packBuf);
//		return len;
//}



uint16_t circulaJsonPack()
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
		cJSON_AddStringToObject(root, "identifier","grounding_current_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFLash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20���ַ������� ������
		
		{
			Array = cJSON_CreateArray();
			if (Array == NULL) return 0;
			cJSON_AddItemToObject(root, "params", Array);
			for (int i = 0; i < CIRCULA_485_NUM; i++)
			{		

				if(sheet.cirCula[i].workFlag==RT_TRUE){
					nodeobj = cJSON_CreateObject();
					cJSON_AddItemToArray(Array, nodeobj);
					cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.cirCula[i].ID));
				  sprintf(sprinBuf,"%d",cirCurStru_p[i].respStat);
				  cJSON_AddItemToObject(nodeobj,"responseStatus",cJSON_CreateString(sprinBuf));
					
					nodeobj_p= cJSON_CreateObject();
					cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
					
					sprintf(sprinBuf,"%02f",cirCurStru_p[i].circlCurA);
					cJSON_AddItemToObject(nodeobj_p,"earthCurA",cJSON_CreateString(sprinBuf));
					cJSON_AddItemToObject(nodeobj_p,"runCurA",cJSON_CreateString(""));
					cJSON_AddItemToObject(nodeobj_p,"loadRatioA",cJSON_CreateString(""));
					
					sprintf(sprinBuf,"%02f",cirCurStru_p[i].circlCurB);
					cJSON_AddItemToObject(nodeobj_p,"earthCurB",cJSON_CreateString(sprinBuf));
					cJSON_AddItemToObject(nodeobj_p,"runCurB",cJSON_CreateString(""));
					cJSON_AddItemToObject(nodeobj_p,"loadRatioB",cJSON_CreateString(""));
					
					sprintf(sprinBuf,"%02f",cirCurStru_p[i].circlCurC);
					cJSON_AddItemToObject(nodeobj_p,"earthCurC",cJSON_CreateString(sprinBuf));
					cJSON_AddItemToObject(nodeobj_p,"runCurC",cJSON_CreateString(""));
					cJSON_AddItemToObject(nodeobj_p,"loadRatioC",cJSON_CreateString(""));
					sprintf(sprinBuf,"%u",utcTime());
					cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
				}
			}
		}
	
		sprintf(sprinBuf,"%u",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// ��ӡJSON���ݰ�  
//		out = cJSON_Print(root);
//		if(out!=NULL){
//			for(int i=0;i<rt_strlen(out);i++)
//					rt_kprintf("%c",out[i]);
//			rt_kprintf("\n");
//			rt_free(out);
//			out=NULL;
//		}
//		if(root!=NULL){
//			cJSON_Delete(root);
//			out=NULL;
//		}

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
		rt_kprintf("%scirCula len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen��%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}



//�ж��Ƿ��б��� ��Ҫ��readCirCurrAndWaring()���ʹ��
rt_bool_t cirCurrWaringcheck(int num)
{
		if((cirCurStru_p[num].warningA)||(cirCurStru_p[num].warningA)||(cirCurStru_p[num].warningA)|(cirCurStru_p[num].warningA)){
			 rt_kprintf("%sERR:����ֵ���� �������� \n\r",sign);
				return RT_TRUE;
		}
		return RT_FALSE;
}


//�澯��Ϣ�Ĵ��  readCirCurrAndWaring()
void  cirCurrWaringEventPack()
{
		rt_kprintf("%s���ڼ��� \n\r",sign);
		
}


void circulaRead2Send(rt_bool_t netStat)
{					
		int workFlag=RT_FALSE;
		for(int i=0;i<CIRCULA_485_NUM;i++){
			if(sheet.cirCula[i].workFlag==RT_TRUE){
						readCirCurrAndWaring(i);
						workFlag=RT_TRUE;
				}
		}
		if(workFlag==RT_TRUE){
				rt_kprintf("%s����ɼ���circula����\r\n",sign);
				circulaJsonPack();
				if(netStat==RT_TRUE)
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
		}
}