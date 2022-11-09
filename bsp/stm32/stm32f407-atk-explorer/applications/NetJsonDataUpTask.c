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
#define TIM_NUM  MODBUS_NUM+2 //目前支持6路定时器  
typedef struct 
{
		uint16_t count;     //计数
	  uint16_t threshoVal;//阈值
}timerStru;
static timerStru tim[TIM_NUM];
//给个定时值


//启动从0开始计时
static void timeStart(upDataTimEnum num)
{
		tim[num].count=0;
}
//初始化后会自动运行
//num 第几个定时器
//value 定时器值
//firstCnt 第一次计数值  为了防止定时器值同时到达 
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
//每1秒递增一次
static void timeInc()
{
	  for(int i=0;i<TIM_NUM;i++){
			  if(tim[i].count!=0xFFFF)
						tim[i].count++;
		}
}

//停止
 void timeStop(upDataTimEnum num)
{
		tim[num].count=0xFFFF;
}
//void stopModbusDev(upDataTimEnum num)
//{
//	   timeStop(num);//偏移地址2
//}

//定时时间到
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



void pressSettRead2Send(rt_bool_t netStat);
void threeAxisRead2Send(rt_bool_t netStat);
void partDischagRead2Send(rt_bool_t netStat);
void circulaRead2Send(rt_bool_t netStat);
void waterDepthRead2Send(rt_bool_t netStat);
void tempHumRead2Send(rt_bool_t netStat);
void o2Read2Send(rt_bool_t netStat);
void h2sRead2Send(rt_bool_t netStat);	
void ch4Read2Send(rt_bool_t netStat);	
void coRead2Send(rt_bool_t netStat);	
//定时时间到  执行相应事件
static void  timeOutRunFun()
{

	  rt_bool_t workFlag=RT_FALSE;
		switch(timeOut()){
			case HEART_TIME://心跳
				heartUpPack();
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
			  rt_kprintf("%sheart timer out\r\n",task);
				break;
			case REG_TIME://注册 注册成功后定时器就关闭
			  if(gbRegFlag==RT_FALSE){
					  devRegPack();//devRegJsonPack();
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
					  timeStop(REG_TIME);//正式使用时候需要去掉
				}
				else
						timeStop(REG_TIME);
				rt_kprintf("%sreg timer out\r\n",task);
				break;
			case CIRCULA_TIME://读取环流
				circulaRead2Send(gbNetState);
				rt_kprintf("%sCIRCULA_TIME out\r\n",task);
				break;
			case PARTDISCHAG_TIME://读取局放
				partDischagRead2Send(gbNetState);
				rt_kprintf("%sPARTDISCHAG_TIME out\r\n",task);
				break;
			case PRESSSETTL_TIME:
        pressSettRead2Send(gbNetState);
				rt_kprintf("%sPRESSSETTL_TIME out\r\n",task);
				break;
			case THREEAXIS_TIME:
				threeAxisRead2Send(gbNetState);
				rt_kprintf("%sTHREEAXIS_TIMEout\r\n",task);
				break;
			case  CH4_TIME:
				coRead2Send(gbNetState);
				break;
			case  O2_TIME:
				o2Read2Send(gbNetState);
				break;
			case  H2S_TIME:
				h2sRead2Send(gbNetState);
				break;
			case  CO_TIME://4种气体在一起读取 所以前三个不使用 只在此处读取并打包发送  关闭时候只需要关闭CO就可以把所有气体全部关闭
				coRead2Send(gbNetState);
				break;
			case  TEMPHUM_TIME:
				tempHumRead2Send(gbNetState);
				break;
			case  WATERDEPTH_TIME:
				waterDepthRead2Send(gbNetState);
				break;
			default:
				break;
		}
}

modbusFunStru modbusFun[MODBUS_NUM];
//启动定时器列表
void startTimeList()
{

		timeInit(HEART_TIME,      120,2);//心跳定时  定时30秒 第一次28秒就来
		timeInit(REG_TIME,        5,0);//注册 注册成功后定时器就关闭
		timeInit(CIRCULA_TIME, 		sheet.cirCulaColTime,5);
		timeInit(PARTDISCHAG_TIME,sheet.partDischagColTime,10);
		timeInit(PRESSSETTL_TIME, sheet.pressSetlColTime,15);
		timeInit(THREEAXIS_TIME,  sheet.threeAxissColTime,20);
	  timeInit(H2S_TIME, 				sheet.h2sColTime,24);
		timeInit(CH4_TIME, 				sheet.ch4ColTime,28);
		timeInit(O2_TIME, 				sheet.o2ColTime,30);
		timeInit(CO_TIME, 				sheet.coColTime,35);
		timeInit(TEMPHUM_TIME, 		sheet.tempHumColTime,40);
		timeInit(WATERDEPTH_TIME, sheet.waterLevColTime,45);
}



//上行数据的维护以及重发
void   upKeepStateTask(void *para)
{
		//extern void modbusPrintRead();
		extern void uartReconfig();
		extern void uartIrqEnaAfterQueue();
	  extern void clearUartData();
	  extern void printModbusDevList();
	  extern void readMultiCirCulaPoint();
	  uartMutexQueueCfg();//根据flash存储重新配置串口
		//modbusPrintRead();//modbus配置从flash中读取
	  uartReconfig();//串口重新配置
		uartIrqEnaAfterQueue();//串口中断中用到了队列  开启中断需要放到后边
    startTimeList();//开启计时器列表
	  
	  printModbusDevList();
	  clearUartData();
	
	  readMultiCirCulaPoint();//对于环流来讲 运行前需要提取扩大方式
		while(1){

				timeOutRunFun();
				timeInc();
				rt_thread_mdelay(1000);
		}
}

