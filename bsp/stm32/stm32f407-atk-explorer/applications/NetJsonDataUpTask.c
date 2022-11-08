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




	
//定时时间到  执行相应事件
static void  timeOutRunFun()
{
    extern uint16_t circulaJsonPack();
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
				for(int i=0;i<CIRCULA_485_NUM;i++){
					if(sheet.cirCula[i].workFlag==RT_TRUE){
								readCirCurrAndWaring(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s打包采集的circula数据\r\n",task);
					  circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
				rt_kprintf("%sCIRCULA_TIME out\r\n",task);
				break;
			case PARTDISCHAG_TIME://读取局放
				for(int i=0;i<PARTDISCHAG_485_NUM;i++){
					if(sheet.partDischag[i].workFlag==RT_TRUE){
								readPdFreqDischarge(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s打包采集的PARTDISCHAG数据\r\n",task);
					  //后期加入
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
//        readPdFreqDischarge();
//			  if(readPartDischgWarning()==RT_TRUE){
//						partDisWaringEventPack();
//					  rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
//					  rt_thread_mdelay(1000);//延时1秒再发下一包
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
					  rt_kprintf("%s打包采集的PRESSSETTL数据\r\n",task);
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
					  rt_kprintf("%s打包采集的THREEAXIS数据\r\n",task);
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
					  rt_kprintf("%s打包采集的ch4数据\r\n",task);
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
					  rt_kprintf("%s打包采集的O2数据\r\n",task);
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
					  rt_kprintf("%s打包采集的h2s数据\r\n",task);
					  //circulaJsonPack();
					  if(gbNetState==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
				break;
			case  CO_TIME://4种气体在一起读取 所以前三个不使用 只在此处读取并打包发送  关闭时候只需要关闭CO就可以把所有气体全部关闭
				for(int i=0;i<CO_485_NUM;i++){
					if(sheet.co[i].workFlag==RT_TRUE){
								readCO(i);
								workFlag=RT_TRUE;
						}
				}
				if(workFlag==RT_TRUE){
					  rt_kprintf("%s打包采集的co数据\r\n",task);
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
					  rt_kprintf("%s打包采集的temphum数据\r\n",task);
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
					  rt_kprintf("%s打包采集的waterlev数据\r\n",task);
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
		timeInit(WATERLEVEL_TIME, sheet.waterLevColTime,45);
}



//上行数据的维护以及重发
void   upKeepStateTask(void *para)
{
		//extern void modbusPrintRead();
		extern void uartReconfig();
		extern void uartIrqEnaAfterQueue();
	  extern void clearUartData();
	  extern void printModbusDevList();
	  uartMutexQueueCfg();//根据flash存储重新配置串口
		//modbusPrintRead();//modbus配置从flash中读取
	  uartReconfig();//串口重新配置
		uartIrqEnaAfterQueue();//串口中断中用到了队列  开启中断需要放到后边
    startTimeList();//开启计时器列表
	  
	  printModbusDevList();
	  clearUartData();
		while(1){

				timeOutRunFun();
				timeInc();
				rt_thread_mdelay(1000);
		}
}

