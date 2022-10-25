#include "rs485Circula.h"

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
// 9600波特率 注意修改此处 rt_mq_recv(&cirCurrmque, &buf, sizeof(buf), 2)
//迅速切换其它485接口来使用 方法：只需要修改串口发送接口 和中断接收接口即可
// rs485Circula.c-cirCurrUartSend(uint8_t *buf,int len) 和drv_uart.c-USART2_IRQHandler中
// cirCurrUartSend(uint8_t *buf,int len)   cirCurrUartRec(uint8_t dat)

const static char sign[]="[环流]";
static rt_mutex_t cirCurrMutex = RT_NULL;
#define  MSGPOOL_LEN   200 //485数据最大量  大于1k需要修改此处
//队列的定义
static struct  rt_messagequeue cirCurrmque;
static uint8_t cirCurrQuePool[MSGPOOL_LEN];  //
static rt_bool_t  recFlag = RT_FALSE; //每个循环发送一次 发完 RT_TRUE 接收完成或者接收超时置为 RT_FALSE


#define   SLAVE_ADDR     0X01
#define   LENTH          50  //工作环流用到的最大接收buf长度
//#define   LARGE_TIMES    100 //放大倍数  后期如果需要 读取寄存器0x000b 有可能放大10倍
extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];

CIRCURStru  cirCurStru_p;
static uint16_t readAcqInterv(void);
static uint16_t readPoint(void);
static uint32_t readThresholdVal(void);
static rt_bool_t writeAcqInterv(uint16_t value);
static rt_bool_t writeThresholdVal(uint32_t value);
static rt_bool_t writePoint(uint16_t value);


//打包串口发送 
void cirCurrUartSend(uint8_t *buf,int len)
{

		rs485UartSend(chanl.cirCula,buf, len);

}
//串口接收后丢到队列里
rt_err_t cirCurrUartRec(uint8_t dat)
{
	
		if(recFlag==RT_TRUE){
				return rt_mq_send(&cirCurrmque, &dat, 1);  //收到数据后就往队列里丢
		}
		else
			  return RT_FALSE;
}

//
//创建环流用到的互斥量和消息队列
void  cirCurrMutexQueueCreat()
{
	  cirCurrMutex = rt_mutex_create("cirCurrMutex", RT_IPC_FLAG_FIFO);
    if (cirCurrMutex == RT_NULL)
    {
        rt_kprintf("%screate rs485_1Mutex failed.\n",sign);
        return ;
    }
		
//////////////////////////////////消息队列//////////////////////////////////
		
		int result = rt_mq_init(&cirCurrmque,"cirCurrmque",&cirCurrQuePool[0],1,sizeof(cirCurrQuePool),RT_IPC_FLAG_FIFO);       
		if (result != RT_EOK)
    {
        rt_kprintf("%sinit rs485_1mque failed.\n",sign);
        return ;
    }
}

///////////////////////////////////////读写寄存器相关操作////////////////////////////////////////
//读取环流值和报警信息 寄存器地址 0x0023 长度12
void readCirCurrAndWaring()
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  //uint8_t   buf[100]
	  uint16_t len = modbusReadReg(SLAVE_ADDR,0x0023,12,buf);
	  //485发送buf  len  等待modbus回应
	  recFlag = RT_TRUE;
	  cirCurrUartSend(buf,len);

	  rt_kprintf("%sreadCirCurrAndWaring send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
	  rt_mutex_take(cirCurrMutex,RT_WAITING_FOREVER);
    len=0;
		while(rt_mq_recv(&cirCurrmque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
		modDev[chanl.cirCula].offline=RT_FALSE;
		//提取环流值 第一步判断crc 第二部提取
		int ret=modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE);
		if(0 ==  ret){//刷新读取到的值
				cirCurStru_p.circlCurA=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				cirCurStru_p.circlCurB=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				cirCurStru_p.circlCurC=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
				cirCurStru_p.circlCurD=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
			  cirCurStru_p.warningA	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  cirCurStru_p.warningB	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  cirCurStru_p.warningC	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  cirCurStru_p.warningD	=(buf[offset]<<8)	+buf[offset+1];	offset+=2;
			  rt_kprintf("%s提取电流、报警值成功\r\n",sign);
		} 
		else{//读不到给0
			  if(ret==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
						modDev[chanl.cirCula].offline=RT_TRUE;
				}
				cirCurStru_p.circlCurA=0;
				cirCurStru_p.circlCurB=0;
				cirCurStru_p.circlCurC=0;
				cirCurStru_p.circlCurD=0;
			  cirCurStru_p.warningA	=0;
			  cirCurStru_p.warningB	=0;
			  cirCurStru_p.warningC	=0;
			  cirCurStru_p.warningD	=0;
			  rt_kprintf("%s提取电流、报警值fail\r\n",sign);
		}
		recFlag = RT_FALSE;
	  rt_mutex_release(cirCurrMutex);
	//	 rt_kprintf("release\r\n");
		rt_free(buf);
	//	 rt_kprintf("free\r\n");
	  buf=RT_NULL;
}
//每次上电后需要从flash中读出存储的值与modbus回复的值进行比较
void cirCurrConf()
{
		//给个初值 后期需要从flash中读取
	  rt_thread_mdelay(1000); //初始化时候需要延时 等待设备初始化完成
		cirCurStru_p.AcqInterv=5;
		cirCurStru_p.point =100;       //非modbus真实值  此处读取modbus后经过了转换便于直接计算  0 -扩大了100倍 1-2 扩大了10倍
	  cirCurStru_p.thresholdVal=350;//3.5A扩大了100倍
//		rt_kprintf("readAcqInterv:%d\n",readAcqInterv());
//		rt_kprintf("readThresholdVal:%d\n",readThresholdVal());
//		rt_kprintf("readPoint:%d\n",readPoint());
 
	  if(readAcqInterv()!=cirCurStru_p.AcqInterv){
				if(RT_FALSE==writeAcqInterv(cirCurStru_p.AcqInterv)){
						rt_kprintf("%swriteAcqInterv err\n",sign);
				}
		}
		if(readThresholdVal()!=cirCurStru_p.thresholdVal){
				if(RT_FALSE==writeThresholdVal(cirCurStru_p.thresholdVal)){
						rt_kprintf("%swriteThresholdVal err\n",sign);
				}
		}
		if(readPoint()!=cirCurStru_p.point){
				if(RT_FALSE==writePoint(cirCurStru_p.point)){
						rt_kprintf("%swritePoint err\n",sign);
				}
		}

}

//读取采集间隔
uint16_t readAcqInterv()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(SLAVE_ADDR,0x0004,1,buf);
	  uint16_t ret =0;
		recFlag = RT_TRUE;
		rt_mutex_take(cirCurrMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		cirCurrUartSend(buf,len);
	  rt_kprintf("%sreadAcqInterv send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		while(rt_mq_recv(&cirCurrmque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		rt_kprintf("%srec:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		modDev[chanl.cirCula].offline=RT_FALSE;
		//提取环流值 第一步判断crc 第二部提取
		int ret2=modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值

			  ret	=(buf[offset]<<8)	+buf[offset+1];	//offset+=2;
			  rt_kprintf("%s提取采集间隔成功 %d\r\n",sign,ret);
		} 
		else{
				if(ret2==2){
					   modDev[chanl.cirCula].offline=RT_TRUE;
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
		}
		
    //
		recFlag = RT_FALSE;
	  rt_mutex_release(cirCurrMutex);
		rt_free(buf);
	  buf=RT_NULL;
		return ret;
}

//设置采集间隔
//01 06 00 04 00 05 08 08	# RECV HEX>
//01 06 00 04 00 05 08 08
rt_bool_t writeAcqInterv(uint16_t value)
{
		
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusWriteOneReg(SLAVE_ADDR,0x0004,value,buf);//modbusWriteReg(SLAVE_ADDR,0x0004,1,buf);
		rt_mutex_take(cirCurrMutex,RT_WAITING_FOREVER);
	  recFlag = RT_TRUE;
	  rt_bool_t ret=RT_FALSE;
	  //485发送buf  len  等待modbus回应
		cirCurrUartSend(buf,len);
	  rt_kprintf("%swrAcqInterv send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		while(rt_mq_recv(&cirCurrmque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		rt_kprintf("%srec:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		modDev[chanl.cirCula].offline=RT_FALSE;
		//提取环流值 第一步判断crc 第二判断对错
		int ret2= modbusRespCheck(SLAVE_ADDR,buf,len,RT_FALSE);
		if(0 ==  ret2){//刷新读取到的值
        if(buf[1]==WRITE){
						ret= RT_TRUE;
				}
		} 
		else{
				if(ret2==2){
					  modDev[chanl.cirCula].offline=RT_TRUE;
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
		}
    //
		recFlag = RT_FALSE;
	  rt_mutex_release(cirCurrMutex);
		rt_free(buf);
	  buf=RT_NULL;
	  
	
		return ret;
}
//读取报警阈值
uint32_t readThresholdVal()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(SLAVE_ADDR,0x0009,4,buf);
	  uint32_t ret =0;		
	  recFlag = RT_TRUE;

		rt_mutex_take(cirCurrMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		cirCurrUartSend(buf,len);
	  rt_kprintf("%sreadthresholdVal send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		while(rt_mq_recv(&cirCurrmque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		rt_kprintf("%srec:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		modDev[chanl.cirCula].offline=RT_FALSE;
		//提取环流值 第一步判断crc 第二部提取
		int ret2=modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE);
		if(0 ==ret2){//刷新读取到的值

			  ret	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];//offset+=2;
			  rt_kprintf("%s提取报警阈值 %d\r\n",sign,ret);
		} 
		else{
				if(ret2==2){
						modDev[chanl.cirCula].offline=RT_TRUE;
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
		}
		recFlag = RT_FALSE;
	  rt_mutex_release(cirCurrMutex);
		rt_free(buf);
	  buf=RT_NULL;
		return ret;
}
//设置报警阈值  rt_true 成功 rt-false 失败
rt_bool_t writeThresholdVal(uint32_t value)
{
    rt_bool_t ret=RT_FALSE;
		uint8_t  *buf = rt_malloc(LENTH);
	  uint8_t sendD[4]={0};
		sendD[0]=(uint8_t)(value>>24);
		sendD[1]=(uint8_t)(value>>16);
		sendD[2]=(uint8_t)(value>>8);
		sendD[3]=value;
	  uint16_t len = modbusWriteMultReg(SLAVE_ADDR,0x0009,sizeof(uint32_t),sendD,buf);
    recFlag = RT_TRUE;
		rt_mutex_take(cirCurrMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		cirCurrUartSend(buf,len);
	  rt_kprintf("%swrAcqInterv send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
 
    len=0;
		while(rt_mq_recv(&cirCurrmque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		rt_kprintf("%srec:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		modDev[chanl.cirCula].offline=RT_FALSE;
		//提取环流值 第一步判断crc 第二判断对错
		int ret2 = modbusRespCheck(SLAVE_ADDR,buf,len,RT_FALSE);
		if(0 ==  ret2){//刷新读取到的值
        if(buf[1]==WRITE_MUL){
						ret= RT_TRUE;
				}
		} 
		else{
				if(ret2==2){
					  modDev[chanl.cirCula].offline=RT_TRUE;
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
		}
		recFlag = RT_FALSE;
	  rt_mutex_release(cirCurrMutex);
		rt_free(buf);
	  buf=RT_NULL;
	
	
		return ret;
	
}
//读取小数处理方式
uint16_t readPoint()
{
	  uint8_t offset=3;//add+regadd+len
		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(SLAVE_ADDR,0x000B,1,buf);
	  uint16_t ret =0;
		recFlag = RT_TRUE;
		rt_mutex_take(cirCurrMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		cirCurrUartSend(buf,len);
	  rt_kprintf("%sreadPoint send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		while(rt_mq_recv(&cirCurrmque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		rt_kprintf("%srec:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		//提取环流值 第一步判断crc 第二部提取
		modDev[chanl.cirCula].offline=RT_FALSE;
		int ret2=modbusRespCheck(SLAVE_ADDR,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值

			  ret	=(buf[offset]<<8)	+buf[offset+1];	//offset+=2;
			  if(ret==0)
						cirCurStru_p.point =100;
				else
						cirCurStru_p.point =10;
			  rt_kprintf("%s提取小数点 %d\r\n",sign,cirCurStru_p.point);
		} 
		else{
				if(ret2==2){
					  modDev[chanl.cirCula].offline=RT_TRUE;
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
		}
		recFlag = RT_FALSE;
	  rt_mutex_release(cirCurrMutex);
		rt_free(buf);
	  buf=RT_NULL;
		return ret;
}
//设置小数处理方式
//设置采集间隔
rt_bool_t writePoint(uint16_t value)
{

		uint8_t  *buf = rt_malloc(LENTH);
	  uint16_t len = modbusWriteOneReg(SLAVE_ADDR,0x000b,value,buf);//modbusWriteReg(SLAVE_ADDR,0x0004,1,buf);
		rt_mutex_take(cirCurrMutex,RT_WAITING_FOREVER);
		recFlag = RT_TRUE;
		rt_bool_t ret=RT_FALSE;
	  //485发送buf  len  等待modbus回应
		cirCurrUartSend(buf,len);
	  rt_kprintf("%swrAcqInterv send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");

    len=0;
		while(rt_mq_recv(&cirCurrmque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
		modDev[chanl.cirCula].offline=RT_FALSE;
		//提取环流值 第一步判断crc 第二判断对错
		int ret2=modbusRespCheck(SLAVE_ADDR,buf,len,RT_FALSE);
		if(0 == ret2 ){//刷新读取到的值
        if(buf[1]==WRITE){
						ret= RT_TRUE;
				}
		} 
		else{
				if(ret2==2){
					  modDev[chanl.cirCula].offline=RT_TRUE;
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
		}
		recFlag = RT_FALSE;
	  rt_mutex_release(cirCurrMutex);
		rt_free(buf);
	  buf=RT_NULL;
	
	
		return ret;
	

}
//判断是否有报警 需要在readCirCurrAndWaring()后边使用
rt_bool_t cirCurrWaringcheck()
{
		if((cirCurStru_p.warningA)||(cirCurStru_p.warningA)||(cirCurStru_p.warningA)|(cirCurStru_p.warningA)){
			 rt_kprintf("%sERR:环流值过高 触发报警 \n\r",sign);
				return RT_TRUE;
		}
		return RT_FALSE;
}
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
uint16_t 	cirCulaDataPack()
{
	 
	  memset(packBuf,0,sizeof(packBuf));
		int len=0;
    //head+lenth
	  packBuf[len]= (uint8_t)(HEAD>>8); len++;
	  packBuf[len]= (uint8_t)(HEAD);    len++;
	  len+=LENTH_LEN;//json长度最后再填写
	  //json
	  char str[50]={0};//临时使用的数组
		rt_sprintf(str,"{\"mid\":%lu,",mcu.upMessID);
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"packetType\":\"CMD_REPORTDATA\",");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_strcpy(str,"\"param\":{");
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);

		rt_strcpy(str,"\"identifier\":\"grounding_current_monitor\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_sprintf(str,"\"acuId\":\"%s\",",mcu.devID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_sprintf(str,"\"deviceId\":\"%s\",",dev[chanl.cirCula].ID);
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);

		rt_strcpy(str,"\"data\":{");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
	 	//sprintf(str,"test:%0.2f",(float)121/100);				 
		sprintf(str,"\"earthCurA\":\"%0.2f\",",(float)((float)cirCurStru_p.circlCurA/cirCurStru_p.point));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"runCurA\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"loadRatioA\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		sprintf(str,"\"earthCurB\":\"%0.2f\",",(float)((float)cirCurStru_p.circlCurB/cirCurStru_p.point));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"runCurB\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"loadRatioB\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		sprintf(str,"\"earthCurC\":\"%0.2f\",",(float)((float)cirCurStru_p.circlCurC/cirCurStru_p.point));	 
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_strcpy(str,"\"runCurC\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		rt_strcpy(str,"\"loadRatioC\":\"\",");
		rt_strcpy((char *)packBuf+len,str);
		len+=rt_strlen(str);
		
		rt_sprintf(str,"\"monitoringTime\":\"%lu\"}},",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		rt_sprintf(str,"\"timestamp\":\"%lu\"}",utcTime());
		rt_strcpy((char *)packBuf+len,str);
    len+=rt_strlen(str);
		
		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新json长度
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]= (uint8_t)(TAIL>>8); len++;
		packBuf[len]= (uint8_t)(TAIL);    len++;
		packBuf[len] =0;//len++;//结尾 补0
		
		mcu.repDataMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%sreg len:%d\r\n",sign,len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;
}
//告警信息的打包  readCirCurrAndWaring()
void  cirCurrWaringEventPack()
{
		rt_kprintf("%s后期加入 \n\r",sign);
		
}




