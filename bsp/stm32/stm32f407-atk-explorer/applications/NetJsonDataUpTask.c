#include   "NetJsonDataTask.h"
#include   "board.h"

rt_bool_t gbRegFlag = RT_FALSE;
extern struct rt_mailbox mbNetSendData;
extern uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];
extern rt_bool_t gbNetState;

const uint16_t  heart_timTick=120; //120�뷢һ������
const uint16_t  reg_timTick=50;  //5��ע��һ��
uint16_t  rs485_1_timTick=60;//Ĭ��ֵ����60
uint16_t  rs485_2_timTick=60;//Ĭ��ֵ����60
uint16_t  rs485_3_timTick=60;//Ĭ��ֵ����60
uint16_t  rs485_4_timTick=60;//Ĭ��ֵ����60
//����upKeepStateTask��1��ļ����һ�����׶�ʱ��
//��ִ�ж�Ӧ�����а��ķ���
//����������Ȳ�ִ��count
static void easyUpTimer()
{
	  
	  static uint32_t count = 0;
	  count++;
		if((count%heart_timTick)==0){
//				heartUpPack();
//				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
			  rt_kprintf("heart timer out\r\n");
		}
		if(gbRegFlag==RT_FALSE){
				if(((count+1)%reg_timTick)==0){//������else �п��ܶ����ʱ����ͬʱ��
//						rt_kprintf("reg timer out\r\n");
//					  devRegPack();
				}
		}
		if(((count+3)%rs485_1_timTick)==0){
				rt_kprintf("485_1 timer out\r\n");
			  rs485_1DataPack();
		}
		if(((count+5)%rs485_2_timTick)==0){
				rt_kprintf("485_2 timer out\r\n");
		}
		if(((count+7)%rs485_3_timTick)==0){
				rt_kprintf("485_3 timer out\r\n");
		}
		if(((count+9)%rs485_4_timTick)==0){
				rt_kprintf("485_4 timer out\r\n");
		}
		//upDataTimEnum  
}
//�������ݵ�ά���Լ��ط�
void   upKeepStateTask(void *para)
{
	 
	  extern  void devIDRead();
    devIDRead();
//	  uint32_t count=20;
		while(1){
			  if(gbNetState ==RT_TRUE){
						easyUpTimer();
				}
				rt_thread_mdelay(1000);
		}
}

