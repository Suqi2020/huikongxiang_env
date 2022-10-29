#include   "NetJsonDataTask.h"
#include   "board.h"


rt_bool_t gbRegFlag = RT_FALSE;
extern struct rt_mailbox mbNetSendData;
extern uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];
extern rt_bool_t gbNetState;
const static char task[]="[dataUp]";



extern void 	readPdFreqDischarge(void);
extern void 	readPSTempHeight(void);
extern void  	PSTempHeightPack(void);
extern void  	readThreeTempAcc(void);
extern void		t3AxisTempAccPack(void);
extern void 	devIDFlashRead(void);
extern void 	cirCurrConf(void);
#define TIM_NUM  6 //Ŀǰ֧��6·��ʱ��  ��� 65536��
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
static void timeInit(upDataTimEnum num,int value,int firstCnt)
{
	  if((num>=TIM_NUM)||(value==0)){  
				rt_kprintf("%stim inint err\n",task);
			return;
		}
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
static void timeStop(upDataTimEnum num)
{
		tim[num].count=0xFFFF;
}

//��ʱʱ�䵽
static int timeOut()
{
	  for(upDataTimEnum i=0;i<TIM_NUM;i++){
				if(tim[i].count!=0xFFFF){
						if(tim[i].count>=tim[i].threshoVal){
							timeStart(i);
							return i;
						}
				}
		}
		return 0xff;
}


void  w5500_16KDataTest()
{
	extern uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];
	for(int i=0;i<TX_RX_MAX_BUF_SIZE;i++){
		packBuf[i]=0x30+i%0x4d;
	}
}

	
//��ʱʱ�䵽  ִ����Ӧ�¼�
static void  timeOutRunFun()
{
    extern uint16_t devRegJsonPack(void);
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
			  readCirCurrAndWaring();
			  if(cirCurrWaringcheck()==RT_TRUE){//�ȷ�����״̬ �ٷ�����
						cirCurrWaringEventPack();
					  rt_thread_mdelay(1000);//��ʱ1���ٷ���һ��
				}
				cirCulaDataPack();
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("%stimer 2 out\r\n",task);
				break;
			case PARTDISCHAG_TIME://��ȡ�ַ�
        readPdFreqDischarge();
			  if(readPartDischgWarning()==RT_TRUE){
						partDisWaringEventPack();
					  rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
					  rt_thread_mdelay(1000);//��ʱ1���ٷ���һ��
						rt_kprintf("\r\n");
				}
				partDisDataPack();
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("%stimer 3 out\r\n",task);
				break;
			case PRESSSETTL_TIME:
//				rt_kprintf("timer 4 in\r\n");
//				w5500_16KDataTest();
//			  rt_kprintf("timer 4 in2\r\n");
//			  extern void netSend(uint8_t *data,int len);
//			  netSend(packBuf,TX_RX_MAX_BUF_SIZE);
				readPSTempHeight();
				PSTempHeightPack();
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("%stimer 4 out\r\n",task);
				break;
			case THREEAXIS_TIME:
				readThreeTempAcc();
				t3AxisTempAccPack();
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("%stimer 5 out\r\n",task);
				break;
			default:
				break;
		}
}
//������ʱ���б�
void startTimeList()
{
	if(modbusFlash[CIRCULA].workFlag==RT_TRUE){
				cirCurrConf();//���ڵ�����ʼ�� modbus����
		    timeInit(CIRCULA_TIME,modbusFlash[CIRCULA].colTime,10);//��ȡ����
		}
		else
			 timeStop(CIRCULA_TIME);//��ȡ����
		
		if(modbusFlash[PRESSSETTL].workFlag==RT_TRUE){
				readPSTempHeight();//��ȡѹ��ʽ������
			  timeInit(PRESSSETTL_TIME,modbusFlash[PRESSSETTL].colTime,11);//��ȡѹ��ʽ������
		}
		else
				timeStop(PRESSSETTL_TIME);//��ȡ����
		
		if(modbusFlash[THREEAXIS].workFlag==RT_TRUE){
				readThreeTempAcc();//��ȡ����
			  timeInit(THREEAXIS_TIME,modbusFlash[THREEAXIS].colTime,12);//��ȡ����
		}
		else
			  timeStop(THREEAXIS_TIME);
		
		rt_thread_mdelay(2000);//��ʱ2��Ϊ�˾ַŶ�ȡ
		if(modbusFlash[PARTDISCHAG].workFlag==RT_TRUE){
				readPartDischgWarning();//�ַų�ʼ���ȽϾ� �ź�߶�ȡ 
				timeInit(PARTDISCHAG_TIME,modbusFlash[PARTDISCHAG].colTime,13);//��ȡ�ַ�
		}
		else
				timeStop(PARTDISCHAG_TIME);//��ȡ����

		timeInit(HEART_TIME, 120,2);//������ʱ  ��ʱ30�� ��һ��28�����
		timeInit(REG_TIME, 5,0);//ע�� ע��ɹ���ʱ���͹ر�
}


//�������ݵ�ά���Լ��ط�
void   upKeepStateTask(void *para)
{
		extern void modbusFlashRead();
		extern void uartReconfig();
		extern void uartIrqEnaAfterQueue();
	  uartMutexQueueCfg();//����flash�洢�������ô���
	  devIDFlashRead();//����ŵ�uartMutexQueueCfg��� ����chanl������������ʹ��
		modbusFlashRead();//modbus���ô�flash�ж�ȡ
	  uartReconfig();//������������
		uartIrqEnaAfterQueue();//�����ж����õ��˶���  �����ж���Ҫ�ŵ����
    startTimeList();//������ʱ���б�
	
		while(1){

				timeOutRunFun();
				timeInc();
				rt_thread_mdelay(1000);
		}
}

