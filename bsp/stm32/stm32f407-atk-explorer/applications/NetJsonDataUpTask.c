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
#define TIM_NUM  6 //目前支持6路定时器  最大 65536秒
typedef struct 
{
		uint16_t count;     //计数
	  uint16_t threshoVal;//阈值
}timerStru;
static timerStru tim[TIM_NUM];
//给个定时值
static void timeInit(int num,int value)//给个定时值
{
	  if((num>=TIM_NUM)||(value==0)){  
				rt_kprintf("%stim inint err\n",task);
			return;
		}
		tim[num].threshoVal=value;
		 
}
//每1秒递增一次
static void timeInc()
{
	  for(int i=0;i<TIM_NUM;i++){
			  if(tim[i].count!=0xFFFF)
						tim[i].count++;
		}
}

//启动
static void timeStart(int num)
{
		tim[num].count=0;
}
//停止
static void timeStop(int num)
{
		tim[num].count=0xFFFF;
}

//定时时间到
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
//定时时间到  执行相应事件
static void  timeOutRunFun()
{

		switch(timeOut()){
			case 0://心跳
//				heartUpPack();
//				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
			  rt_kprintf("%sheart timer out\r\n",task);
				break;
			case 1://注册 注册成功后定时器就关闭
			  if(gbRegFlag==RT_FALSE){
					  devRegPack();
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
						rt_kprintf("%sreg timer out\r\n",task);
					  timeStop(1);//正式使用时候需要去掉
				}
				else
						timeStop(1);
				break;
			case 2://读取环流
			  readCirCurrAndWaring();
			  if(cirCurrWaringcheck()==RT_TRUE){//先发报警状态 再发数据
						cirCurrWaringEventPack();
					  rt_thread_mdelay(1000);//延时1秒再发下一包
				}
				cirCulaDataPack();
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("%stimer 2 out\r\n",task);
				break;
			case 3://读取局放
        readPdFreqDischarge();
			  if(readPartDischgWarning()==RT_TRUE){
						partDisWaringEventPack();
					  rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
					  rt_thread_mdelay(1000);//延时1秒再发下一包
						rt_kprintf("\r\n");
				}
				partDisDataPack();
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("%stimer 3 out\r\n",task);
				break;
			case 4:
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
			case 5:
				readThreeTempAcc();
				t3AxisTempAccPack();
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				rt_kprintf("%stimer 5 out\r\n",task);
				break;
			default:
				break;
		}
}




//上行数据的维护以及重发
void   upKeepStateTask(void *para)
{

    
	  uartConfFlashRead();//根据flash存储重新配置串口
	  devIDFlashRead();//必须放到uartConfFlashRead后边 配置chanl各项参数后才能使用
		extern void uartIrqEnaAfterQueue();
		uartIrqEnaAfterQueue();//串口中断中用到了队列  开启中断需要放到后边

	
	  cirCurrConf();//公众电流初始化 modbus配置
	  readPSTempHeight();//读取压差式沉降仪
	  readThreeTempAcc();//读取三轴
		rt_thread_mdelay(2000);//延时2秒为了局放读取
		readPartDischgWarning();//局放初始化比较久 放后边读取
		timeInit(0, 120);//心跳定时
		timeInit(1, 2);//注册 注册成功后定时器就关闭
		timeInit(2, 120);//读取环流
		timeInit(3, 30);//读取局放
		timeInit(4, 120);//读取压差式沉降仪
		timeInit(5, 120);//读取三轴
		while(1){

				timeOutRunFun();
				timeInc();
				rt_thread_mdelay(1000);
		}
}

