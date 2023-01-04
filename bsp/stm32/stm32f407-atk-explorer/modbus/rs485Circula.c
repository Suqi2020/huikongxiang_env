#include "board.h"
//#include "stmflash.h"
//<<公众环流 GY-JDHL03>>  目前测试只采集环流值
//响应时间 200ms  默认波特率115200
//  1、读取环流值   
//  2、读取报警标志 
//  3、配置报警阈值 
//  4、读取报警阈值
//  5、设置采集间隔
//  6、读取采集间隔
//调整RS485串口需要调整 UART5_IRQHandler以及HAL_UART_Transmit发送串口  
//还需要更换对应串口的队列 调整 char num=0;
// 9600波特率 注意修改此处 rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, &buf, sizeof(buf), 2)
//迅速切换其它485接口来使用 方法：只需要修改串口发送接口 和中断接收接口即可
// rs485Circula.c-cirCurrUartSend(uint8_t *buf,int len) 和drv_uart.c-USART2_IRQHandler中
// cirCurrUartSend(uint8_t *buf,int len)   cirCurrUartRec(uint8_t dat)
typedef struct
{
	  //环流值 放大了100倍
		float circlCurA;
		float circlCurB;
	  float circlCurC;
	  float circlCurD;//备用
	  //阈值
	  //uint32_t thresholdVal;

	  
	  //报警状态  一个字节足够 方便对接modbus回应
	  uint16_t warningA;
	  uint16_t warningB;
	  uint16_t warningC;
	  uint16_t warningD;
	  
	  //采集间隔 单位秒
		//uint16_t AcqInterv;
	//小数点计算数值
    uint16_t point; //非modbus真实值  此处读取modbus后经过了转换便于直接计算  0-值为100  1-2 值为10
	  uint8_t respStat;
} CIRCURStru;

const static char sign[]="[环流]";

//#define   SLAVE_ADDR     0X02
#define   LENTH          50  //工作环流用到的最大接收buf长度
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
//打包串口发送 
void cirCurrUartSend(int num,uint8_t *buf,int len)
{

		rs485UartSend(sheet.cirCula[num].useUartNum,buf, len);

}


///////////////////////////////////////读写寄存器相关操作////////////////////////////////////////
//读取环流值和报警信息 寄存器地址 0x0023 长度12
void readCirCurrAndWaring(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  //uint8_t   buf[100]
	  uint16_t len = modbusReadReg(sheet.cirCula[num].slaveAddr,0x0023,READ_03,12,buf);
	  //485发送buf  len  等待modbus回应
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
		while(rt_mq_recv(uartDev[sheet.cirCula[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
		//提取环流值 第一步判断crc 第二部提取
		int ret=modbusRespCheck(sheet.cirCula[num].slaveAddr,buf,len,RT_TRUE);
		if(0 ==  ret){//刷新读取到的值
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
			  rt_kprintf("%s提取电流、报警值成功\r\n",sign);
			  cirCurStru_p[num].respStat=1;
		} 
		else{//读不到给0
			  if(ret==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
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
			  rt_kprintf("%s提取电流、报警值fail\r\n",sign);
		}
		//recFlag = RT_FALSE;
	  rt_mutex_release(uartDev[sheet.cirCula[num].useUartNum].uartMutex);
	//	 rt_kprintf("release\r\n");
		rt_free(buf);
	//	 rt_kprintf("free\r\n");
	  buf=RT_NULL;
}
//每次上电后需要从flash中读出存储的值与modbus回复的值进行比较
//void cirCurrConf()
//{
//		//给个初值 后期需要从flash中读取
//	  rt_thread_mdelay(1000); //初始化时候需要延时 等待设备初始化完成
//		cirCurStru_p.AcqInterv=5;
//		cirCurStru_p.point =100;       //非modbus真实值  此处读取modbus后经过了转换便于直接计算  0 -扩大了100倍 1-2 扩大了10倍
//	  cirCurStru_p.thresholdVal=350;//3.5A扩大了100倍
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

////读取采集间隔
//uint16_t readAcqInterv()
//{
//	  uint8_t offset=3;//add+regadd+len
//	  uint8_t  *buf = RT_NULL;
//		buf = rt_malloc(LENTH);
//	  uint16_t len = modbusReadReg(modbusFlash[CIRCULA].slaveAddr,0x0004,READ_03,1,buf);
//	  uint16_t ret =0;
//		//recFlag = RT_TRUE;
//		rt_mutex_take(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex,RT_WAITING_FOREVER);
//	  //485发送buf  len  等待modbus回应
//		cirCurrUartSend(buf,len);
//	  rt_kprintf("%sreadAcqInterv send:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
//		memset(buf,0,LENTH);
//    len=0;
//		while(rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
//				len++;
//		}
//		rt_kprintf("%srec:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
////		uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_FALSE;
//		//提取环流值 第一步判断crc 第二部提取
//		int ret2=modbusRespCheck(modbusFlash[CIRCULA].slaveAddr,buf,len,RT_TRUE);
//		if(0 == ret2){//刷新读取到的值

//			  ret	=(buf[offset]<<8)	+buf[offset+1];	//offset+=2;
//			  rt_kprintf("%s提取采集间隔成功 %d\r\n",sign,ret);
//		} 
//		else{
//				if(ret2==2){
////					   uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_TRUE;
//						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
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

//设置采集间隔
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
//	  //485发送buf  len  等待modbus回应
//		cirCurrUartSend(buf,len);
//	  rt_kprintf("%swrAcqInterv send:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
//		memset(buf,0,LENTH);
//    len=0;
//		while(rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
//				len++;
//		}
//		rt_kprintf("%srec:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
////		uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_FALSE;
//		//提取环流值 第一步判断crc 第二判断对错
//		int ret2= modbusRespCheck(modbusFlash[CIRCULA].slaveAddr,buf,len,RT_FALSE);
//		if(0 ==  ret2){//刷新读取到的值
//        if(buf[1]==WRITE){
//						ret= RT_TRUE;
//				}
//		} 
//		else{
//				if(ret2==2){
////					  uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_TRUE;
//						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
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
//读取报警阈值
//uint32_t readThresholdVal(int num)
//{
//	  uint8_t offset=3;//add+regadd+len
//	  uint8_t  *buf = RT_NULL;
//		buf = rt_malloc(LENTH);
//	  uint16_t len = modbusReadReg(modbusFlash[CIRCULA].slaveAddr,0x0009,READ_03,4,buf);
//	  uint32_t ret =0;		
//	  //recFlag = RT_TRUE;

//		rt_mutex_take(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex,RT_WAITING_FOREVER);
//	  //485发送buf  len  等待modbus回应
//		cirCurrUartSend(buf,len);
//	  rt_kprintf("%sreadthresholdVal send:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
//		memset(buf,0,LENTH);
//    len=0;
//		while(rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
//				len++;
//		}
//		rt_kprintf("%srec:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
////		uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_FALSE;
//		//提取环流值 第一步判断crc 第二部提取
//		int ret2=modbusRespCheck(modbusFlash[CIRCULA].slaveAddr,buf,len,RT_TRUE);
//		if(0 ==ret2){//刷新读取到的值

//			  ret	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];//offset+=2;
//			  rt_kprintf("%s提取报警阈值 %d\r\n",sign,ret);
//		} 
//		else{
//				if(ret2==2){
////						uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_TRUE;
//						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
//				}
//		}
//		//recFlag = RT_FALSE;
//	  rt_mutex_release(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex);
//		rt_free(buf);
//	  buf=RT_NULL;
//		return ret;
//}
//设置报警阈值  rt_true 成功 rt-false 失败
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
//	  //485发送buf  len  等待modbus回应
//		cirCurrUartSend(buf,len);
//	  rt_kprintf("%swrAcqInterv send:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
//		memset(buf,0,LENTH);
//    len=0;
//		while(rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
//				len++;
//		}
//		rt_kprintf("%srec:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
////		uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_FALSE;
//		//提取环流值 第一步判断crc 第二判断对错
//		int ret2 = modbusRespCheck(modbusFlash[CIRCULA].slaveAddr,buf,len,RT_FALSE);
//		if(0 ==  ret2){//刷新读取到的值
//        if(buf[1]==WRITE_MUL){
//						ret= RT_TRUE;
//				}
//		} 
//		else{
//				if(ret2==2){
/////					  uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_TRUE;
//						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
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
//读取小数处理方式
static uint16_t readPoint(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.cirCula[num].slaveAddr,0x000B,READ_03,1,buf);
	  uint16_t ret =0;
	//	recFlag = RT_TRUE;
		rt_mutex_take(uartDev[sheet.cirCula[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		cirCurrUartSend(num,buf,len);
	  rt_kprintf("%sreadPoint send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		memset(buf,0,LENTH);
    len=0;
		while(rt_mq_recv(uartDev[sheet.cirCula[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		rt_kprintf("%srec:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//提取环流值 第一步判断crc 第二部提取
		int ret2=modbusRespCheck(sheet.cirCula[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值

			  ret	=(buf[offset]<<8)	+buf[offset+1];	//offset+=2;
			  if(ret==0)
						cirCurStru_p[num].point =100;
				else
						cirCurStru_p[num].point =10;
			  rt_kprintf("%s提取小数点 %d\r\n",sign,cirCurStru_p[num].point);
		} 
		else{
			  cirCurStru_p[num].point =100;//给个默认值

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
//设置小数处理方式
//设置采集间隔
//rt_bool_t writePoint(uint16_t value)
//{

//	  uint8_t  *buf = RT_NULL;
//		buf = rt_malloc(LENTH);
//	  uint16_t len = modbusWriteOneReg(modbusFlash[CIRCULA].slaveAddr,0x000b,value,buf);//modbusWriteReg(SLAVE_ADDR,0x0004,1,buf);
//		rt_mutex_take(uartDev[modbusFlash[CIRCULA].useUartNum].uartMutex,RT_WAITING_FOREVER);
//		//recFlag = RT_TRUE;
//		rt_bool_t ret=RT_FALSE;
//	  //485发送buf  len  等待modbus回应
//		cirCurrUartSend(buf,len);
//	  rt_kprintf("%swrAcqInterv send:",sign);
//		for(int j=0;j<len;j++){
//				rt_kprintf("%x ",buf[j]);
//		}
//		rt_kprintf("\n");
//		memset(buf,0,LENTH);
//    len=0;
//		while(rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
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
//		//提取环流值 第一步判断crc 第二判断对错
//		int ret2=modbusRespCheck(modbusFlash[CIRCULA].slaveAddr,buf,len,RT_FALSE);
//		if(0 == ret2 ){//刷新读取到的值
//        if(buf[1]==WRITE){
//						ret= RT_TRUE;
//				}
//		} 
//		else{
//				if(ret2==2){
////					  uartDev[modbusFlash[CIRCULA].useUartNum].offline=RT_TRUE;
//						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
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

//数据主动定时上行的打包  打包前需要调用 readCirCurrAndWaring()
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



/////////////////////////////////////////JSON格式打包//////////////////////////////////////////
//电流值打包
//uint16_t 	cirCulaDataPack()
//{
//	 
//	  memset(packBuf,0,sizeof(packBuf));
//		int len=0;
//    //head+lenth
//	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
//	  packBuf[len]= (uint8_t)(HEAD);    len++;
//	  len+=LENTH_LEN;//json长度最后再填写
//	  //json
//	  char str[50]={0};//临时使用的数组
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
//	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新json长度
//	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
//	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
//	  //crc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
//	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

//		//tail
//		packBuf[len]= (uint8_t)(TAIL>>8); len++;
//		packBuf[len]= (uint8_t)(TAIL);    len++;
//		packBuf[len] =0;//len++;//结尾 补0
//		
//		mcu.repDataMessID =mcu.upMessID;
//		upMessIdAdd();
//		rt_kprintf("%scircula len:%d\r\n",sign,len);
//		
////		for(int i=0;i<len;i++)
////				rt_kprintf("%02x",packBuf[i]);
//		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
//		//rt_kprintf("heart:%s \n",packBuf);
//		return len;
//}



uint16_t circulaJsonPack()
{

		char* out = NULL;
		//创建数组
		cJSON* Array = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return 0;
		// 加入节点（键值对）
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_REPORTDATA");
		cJSON_AddStringToObject(root, "identifier","grounding_current_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFLash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		
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
		// 打印JSON数据包  
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

		//打包
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json长度最后再填写
		
		// 释放内存  
		
		
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
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新json长度
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]=(uint8_t)(TAIL>>8); len++;
		packBuf[len]=(uint8_t)(TAIL);    len++;
		packBuf[len]=0;//len++;//结尾 补0
		mcu.repDataMessID =mcu.upMessID;
		//mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%scirCula len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}



//判断是否有报警 需要在readCirCurrAndWaring()后边使用
rt_bool_t cirCurrWaringcheck(int num)
{
		if((cirCurStru_p[num].warningA)||(cirCurStru_p[num].warningA)||(cirCurStru_p[num].warningA)|(cirCurStru_p[num].warningA)){
			 rt_kprintf("%sERR:环流值过高 触发报警 \n\r",sign);
				return RT_TRUE;
		}
		return RT_FALSE;
}


//告警信息的打包  readCirCurrAndWaring()
void  cirCurrWaringEventPack()
{
		rt_kprintf("%s后期加入 \n\r",sign);
		
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
				rt_kprintf("%s打包采集的circula数据\r\n",sign);
				circulaJsonPack();
				if(netStat==RT_TRUE)
						rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
		}
}