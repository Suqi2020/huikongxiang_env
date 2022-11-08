#include   "NetJsonDataTask.h"
#include   "board.h"


rt_bool_t gbRegFlag = RT_FALSE;
extern struct rt_mailbox mbNetSendData;
extern uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];
extern rt_bool_t gbNetState;
const static char task[]="[dataUp]";




extern void 	readPSTempHeight(int num);
extern void  	PSTempHeightPack(void);
extern void  	readThreeTempAcc(int num);
extern void		t3AxisTempAccPack(void);
//extern void 	devIDFlashRead(void);
extern void 	cirCurrConf(void);
#define TIM_NUM  MODBUS_NUM+2 //Ŀǰ֧��6·��ʱ��  
typedef struct 
{
		uint16_t count;     //����
	  uint16_t threshoVal;//��ֵ
}timerStru;
static timerStru tim[TIM_NUM];
//������ʱֵ


//������0��ʼ��ʱ
static void timeStart(upDataTimEnum num)
{
		tim[num].count=0;
}
//��ʼ������Զ�����
//num �ڼ�����ʱ��
//value ��ʱ��ֵ
//firstCnt ��һ�μ���ֵ  Ϊ�˷�ֹ��ʱ��ֵͬʱ���� 
 void timeInit(upDataTimEnum num,int value,int firstCnt)
{
	  if(value==0){   // if((num>=TIM_NUM)||(value==0)){ 
				rt_kprintf("%stim inint err\n",task);
				return;
		}
	//	rt_kprintf("tim init %d %d\n",num,value);
		tim[num].threshoVal=value;
		tim[num].count=firstCnt;
}
//ÿ1�����һ��
static void timeInc()
{
	  for(int i=0;i<TIM_NUM;i++){
			  if(tim[i].count!=0xFFFF)
						tim[i].count++;
		}
}

//ֹͣ
 void timeStop(upDataTimEnum num)
{
		tim[num].count=0xFFFF;
}
//void stopModbusDev(upDataTimEnum num)
//{
//	   timeStop(num);//ƫ�Ƶ�ַ2
//}

//��ʱʱ�䵽
static int timeOut()
{
	  for(int i=0;i<TIM_NUM;i++){
				if(tim[i].count!=0xFFFF){
						if(tim[i].count>=tim[i].threshoVal){
							timeStart(i);
									//rt_kprintf("tim out %d %d\n",i,tim[i].threshoVal);
							return i;
						}
				}
		}
		return 0xff;
}




	
//��ʱʱ�䵽  ִ����Ӧ�¼�
static void  timeOutRunFun()
{
    extern uint16_t circulaJsonPack();
	  rt_bool_t workFlag=RT_FALSE;
		switch(timeOut()){
			case HEART_TIME://����
				heartUpPack();
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
			  rt_kprintf("%sheart timer out\r\n",task);
				break;
			case REG_TIME://ע�� ע��ɹ���ʱ���͹ر�
			  if(gbRegFlag==RT_FALSE){
					  devRegPack();//devRegJsonPack();
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
					  timeStop(REG_TIME);//��ʽʹ��ʱ����Ҫȥ��
				}
				else
						timeStop(REG_TIME);
				rt_kprintf("%sreg timer out\r\n",task);
				break;
			case CIRCULA_TIME://��ȡ����
				for(int i=0;i<CIRCULA_485_NUM;i++){
					if(sheet.cirCula[i].workFlag==RT_TRUE){
								readCirCurrAndWaring(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s����ɼ���circula����\r\n",task);
					  circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
				rt_kprintf("%sCIRCULA_TIME out\r\n",task);
				break;
			case PARTDISCHAG_TIME://��ȡ�ַ�
				for(int i=0;i<PARTDISCHAG_485_NUM;i++){
					if(sheet.partDischag[i].workFlag==RT_TRUE){
								readPdFreqDischarge(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s����ɼ���PARTDISCHAG����\r\n",task);
					  //���ڼ���
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
//        readPdFreqDischarge();
//			  if(readPartDischgWarning()==RT_TRUE){
//						partDisWaringEventPack();
//					  rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
//					  rt_thread_mdelay(1000);//��ʱ1���ٷ���һ��
//						rt_kprintf("\r\n");
//				}
//				partDisDataPack();
//				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("%sPARTDISCHAG_TIME out\r\n",task);
				break;
			case PRESSSETTL_TIME:
				for(int i=0;i<PRESSSETTL_485_NUM;i++){
					if(sheet.pressSetl[i].workFlag==RT_TRUE){
								readPSTempHeight(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s����ɼ���PRESSSETTL����\r\n",task);
					  //circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
				rt_kprintf("%sPRESSSETTL_TIME out\r\n",task);
//				readPSTempHeight();
//				PSTempHeightPack();
//				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
//				rt_kprintf("%sPRESSSETTL_TIMEout\r\n",task);
				break;
			case THREEAXIS_TIME:
				for(int i=0;i<THREEAXIS_485_NUM;i++){
					if(sheet.threeAxiss[i].workFlag==RT_TRUE){
								readThreeTempAcc(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s����ɼ���THREEAXIS����\r\n",task);
					  //circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}

//				readThreeTempAcc();
//				t3AxisTempAccPack();
//				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("%sTHREEAXIS_TIMEout\r\n",task);
				break;
			case  CH4_TIME:
				for(int i=0;i<CH4_485_NUM;i++){
					if(sheet.ch4[i].workFlag==RT_TRUE){
								readCH4(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s����ɼ���ch4����\r\n",task);
					  //circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
				break;
			case  O2_TIME:
				for(int i=0;i<O2_485_NUM;i++){
					if(sheet.o2[i].workFlag==RT_TRUE){
								readO2(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s����ɼ���O2����\r\n",task);
					  //circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
				break;
			case  H2S_TIME:
				for(int i=0;i<H2S_485_NUM;i++){
					if(sheet.h2s[i].workFlag==RT_TRUE){
								readH2S(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s����ɼ���h2s����\r\n",task);
					  //circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
				break;
			case  CO_TIME://4��������һ���ȡ ����ǰ������ʹ�� ֻ�ڴ˴���ȡ���������  �ر�ʱ��ֻ��Ҫ�ر�CO�Ϳ��԰���������ȫ���ر�
				for(int i=0;i<CO_485_NUM;i++){
					if(sheet.co[i].workFlag==RT_TRUE){
								readCO(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s����ɼ���co����\r\n",task);
					  //circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
				break;
			case  TEMPHUM_TIME:
				for(int i=0;i<TEMPHUM_485_NUM;i++){
					if(sheet.tempHum[i].workFlag==RT_TRUE){
								readTempHum(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s����ɼ���temphum����\r\n",task);
					  //circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
				break;
			case  WATERLEVEL_TIME:
				for(int i=0;i<WATERLEV_485_NUM;i++){
						if(sheet.waterLev[i].workFlag==RT_TRUE){
								readWaterLevel(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s����ɼ���waterlev����\r\n",task);
					  //circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
				break;
			default:
				break;
		}
}

modbusFunStru modbusFun[MODBUS_NUM];
//������ʱ���б�
void startTimeList()
{

		timeInit(HEART_TIME,      120,2);//������ʱ  ��ʱ30�� ��һ��28�����
		timeInit(REG_TIME,        5,0);//ע�� ע��ɹ���ʱ���͹ر�
		timeInit(CIRCULA_TIME, 		sheet.cirCulaColTime,5);
		timeInit(PARTDISCHAG_TIME,sheet.partDischagColTime,10);
		timeInit(PRESSSETTL_TIME, sheet.pressSetlColTime,15);
		timeInit(THREEAXIS_TIME,  sheet.threeAxissColTime,20);
	  timeInit(H2S_TIME, 				sheet.h2sColTime,24);
		timeInit(CH4_TIME, 				sheet.ch4ColTime,28);
		timeInit(O2_TIME, 				sheet.o2ColTime,30);
		timeInit(CO_TIME, 				sheet.coColTime,35);
		timeInit(TEMPHUM_TIME, 		sheet.tempHumColTime,40);
		timeInit(WATERLEVEL_TIME, sheet.waterLevColTime,45);
}



//�������ݵ�ά���Լ��ط�
void   upKeepStateTask(void *para)
{
		//extern void modbusPrintRead();
		extern void uartReconfig();
		extern void uartIrqEnaAfterQueue();
	  extern void clearUartData();
	  extern void printModbusDevList();
	  uartMutexQueueCfg();//����flash�洢�������ô���
		//modbusPrintRead();//modbus���ô�flash�ж�ȡ
	  uartReconfig();//������������
		uartIrqEnaAfterQueue();//�����ж����õ��˶���  �����ж���Ҫ�ŵ����
    startTimeList();//������ʱ���б�
	  
	  printModbusDevList();
	  clearUartData();
		while(1){

				timeOutRunFun();
				timeInc();
				rt_thread_mdelay(1000);
		}
}

