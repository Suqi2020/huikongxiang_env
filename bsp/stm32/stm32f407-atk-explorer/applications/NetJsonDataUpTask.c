#include   "NetJsonDataTask.h"
#include   "board.h"


rt_bool_t gbRegFlag = RT_FALSE;
extern struct rt_mailbox mbNetSendData;
extern uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];
extern rt_bool_t gbNetState;


#define TIM_NUM  6 //Ŀǰ֧��6·��ʱ��  ��� 65536��
typedef struct
{
		uint16_t count;     //����
	  uint16_t threshoVal;//��ֵ
}timerStru;
static timerStru tim[TIM_NUM];
//������ʱֵ
static void timeInit(int num,int value)//������ʱֵ
{
	  if((num>=TIM_NUM)||(value==0)){
				rt_kprintf("tim inint err\n");
			return;
		}
		tim[num].threshoVal=value;
		 
}
//ÿ1�����һ��
static void timeInc()
{
	  for(int i=0;i<TIM_NUM;i++){
			  if(tim[i].count!=0xFFFF)
						tim[i].count++;
		}
}

//����
static void timeStart(int num)
{
		tim[num].count=0;
}
//ֹͣ
static void timeStop(int num)
{
		tim[num].count=0xFFFF;
}

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
	  void readPdFreqDischarge(void);
		switch(timeOut()){
			case 0://����
//				heartUpPack();
//				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
			  rt_kprintf("heart timer out\r\n");
				break;
			case 1://ע�� ע��ɹ���ʱ���͹ر�
			  if(gbRegFlag==RT_FALSE){
					  devRegPack();
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
						rt_kprintf("reg timer out\r\n");
					  timeStop(1);//��ʽʹ��ʱ����Ҫȥ��
				}
				else
						timeStop(1);
				break;
			case 2://��ȡ����
//			  readCirCurrAndWaring();
//			  if(cirCurrWaringcheck()==RT_TRUE){//�ȷ�����״̬ �ٷ�����
//						cirCurrWaringEventPack();
//					  rt_thread_mdelay(1000);//��ʱ1���ٷ���һ��
//				}
//				cirCulaDataPack();
//				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("timer 2 out\r\n");
				break;
			case 3://��ȡ�ַ�
//        readPdFreqDischarge();
//			  if(readPartDischgWarning()==RT_TRUE){
//						partDisWaringEventPack();
//					  rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
//					  rt_thread_mdelay(1000);//��ʱ1���ٷ���һ��
//						rt_kprintf("\r\n");
//				}
//				partDisDataPack();
//				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("timer 3 out\r\n");
				break;
			case 4:
				rt_kprintf("timer 4 in\r\n");
				w5500_16KDataTest();
			  rt_kprintf("timer 4 in2\r\n");
			  extern void netSend(uint8_t *data,int len);
			  netSend(packBuf,TX_RX_MAX_BUF_SIZE);
									
												
				rt_kprintf("timer 4 out\r\n");
				break;
			case 5:
				rt_kprintf("timer 5 out\r\n");
				break;
			default:
				break;
		}
}

//�������ݵ�ά���Լ��ط�
void   upKeepStateTask(void *para)
{
	 
	  extern  void devIDRead();
    devIDRead();
	  extern void cirCurrConf();
	  cirCurrConf();//���ڵ�����ʼ�� modbus����
		timeInit(0, 120);//������ʱ
		timeInit(1, 2);//ע�� ע��ɹ���ʱ���͹ر�
		timeInit(2, 60);//��ȡ����
		timeInit(3, 60);//��ȡ�ַ�
		timeInit(4, 30);
		timeInit(5, 70);
		while(1){
			  if(gbNetState ==RT_TRUE){
					  timeOutRunFun();
					  timeInc();
				}
				rt_thread_mdelay(1000);
		}
}

