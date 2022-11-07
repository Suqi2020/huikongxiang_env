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
#define TIM_NUM  2 //Ŀǰ֧��6·��ʱ��  
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
					  devRegJsonPack();//devRegPack();//devRegJsonPack();
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
					  timeStop(REG_TIME);//��ʽʹ��ʱ����Ҫȥ��
				}
				else
						timeStop(REG_TIME);
				rt_kprintf("%sreg timer out\r\n",task);

				break;
			default:
				break;
		}
}
//������ʱ���б�
void startTimeList()
{
		timeInit(HEART_TIME, 120,2);//������ʱ  ��ʱ30�� ��һ��28�����
		timeInit(REG_TIME, 5,0);//ע�� ע��ɹ���ʱ���͹ر�
}


//�������ݵ�ά���Լ��ط�
void   upKeepStateTask(void *para)
{
		extern void  modbusPrintRead();
		extern void  uartReconfig();
		extern void  uartIrqEnaAfterQueue();
	  extern void  printfWorkModbus();
	  extern void  modbusReadData(int count);
	  extern void  modbusDevErrCheck();
    extern void  clearUartData();
	  extern uint16_t analogJsonPack(int count);
	  extern void  printfWorkAnalog();
	  int count;
	  uartMutexQueueCfg();//����flash�洢�������ô���
	  //devIDFlashRead();//����ŵ�uartMutexQueueCfg��� ����chanl������������ʹ��
		//modbusPrintRead();//modbus���ô�flash�ж�ȡ
	  uartReconfig();//������������
		uartIrqEnaAfterQueue();//�����ж����õ��˶���  �����ж���Ҫ�ŵ����
    startTimeList();//������ʱ���б�
	  printfWorkModbus();
	  printfWorkAnalog();
	  modbusDevErrCheck();
	  clearUartData();
	  uint8_t test[]="test\n";
		while(1){

				timeOutRunFun();
				timeInc();
			  count++;
			  modbusReadData( count);//modbus���ݶ����ϴ�
			  analogJsonPack( count);//analog���ݶ����ϴ�
				rt_thread_mdelay(1000);
//			rt_kprintf("%ssend ok\r\n",task);
		}
}

