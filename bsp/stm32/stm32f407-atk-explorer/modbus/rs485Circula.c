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
// 9600波特率 注意修改此处 rt_mq_recv(uartDev[modbusFlash[CIRCULA].useUartNum], &buf, sizeof(buf), 2)
//迅速切换其它485接口来使用 方法：只需要修改串口发送接口 和中断接收接口即可
// rs485Circula.c-cirCurrUartSend(uint8_t *buf,int len) 和drv_uart.c-USART2_IRQHandler中
// cirCurrUartSend(uint8_t *buf,int len)   cirCurrUartRec(uint8_t dat)
static  bool alarmFLag=false;
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
extern uint8_t NetTxBuffer[TX_RX_MAX_BUF_SIZE];

 CIRCURStru  cirCurStru_p[CIRCULA_485_NUM];
//static uint16_t readAcqInterv(int num);
//static uint16_t readPoint(int num);
//static uint32_t readThresholdVal(int num);
//static rt_bool_t writeAcqInterv(uint16_t value);
//static rt_bool_t writeThresholdVal(uint32_t value);
//static rt_bool_t writePoint(uint16_t value);

//返回环流的通讯状态 true--通讯成功 false--通讯失败
int cirCurrtState(int i)
{
		return cirCurStru_p[i].respStat;
}
//打包串口发送 
void cirCurrUartSend(int num,uint8_t *buf,int len)
{

		rs485UartSend(sheet.cirCula[num].useUartNum,buf, len);

}
//环流比较阈值并设置相应的flag标记
static void circulaCheckSetFlag(int num)
{
	  alarmFLag=false;
	  if(sheet.modbusCircul[num].cirCurAUpLimit!=0){
			if(cirCurStru_p[num].circlCurA>=sheet.modbusCircul[num].cirCurAUpLimit){
					inpoutpFlag.modbusCircul[num].cirCurAUpFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusCircul[num].cirCurALowLimit!=0){
			if(cirCurStru_p[num].circlCurA<=sheet.modbusCircul[num].cirCurALowLimit){
					inpoutpFlag.modbusCircul[num].cirCurALowFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusCircul[num].cirCurBUpLimit!=0){
			if(cirCurStru_p[num].circlCurB>=sheet.modbusCircul[num].cirCurBUpLimit){
					inpoutpFlag.modbusCircul[num].cirCurBUpFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusCircul[num].cirCurBLowLimit!=0){
			if(cirCurStru_p[num].circlCurB<=sheet.modbusCircul[num].cirCurBLowLimit){
					inpoutpFlag.modbusCircul[num].cirCurBLowFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusCircul[num].cirCurCUpLimit!=0){
			if(cirCurStru_p[num].circlCurC>=sheet.modbusCircul[num].cirCurCUpLimit){
					inpoutpFlag.modbusCircul[num].cirCurCUpFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusCircul[num].cirCurCUpLimit!=0){
			if(cirCurStru_p[num].circlCurC<=sheet.modbusCircul[num].cirCurCLowLimit){
					inpoutpFlag.modbusCircul[num].cirCurCLowFlag=true;alarmFLag=true;
			}
		}

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
//	  rt_mutex_take(uartDev[sheet.cirCula[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
		memset(buf,0,LENTH);
    len=0;
		while(rt_mq_recv(&uartmque[sheet.cirCula[num].useUartNum], buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
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
				circulaCheckSetFlag(num);
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

//		circulaCheckSetFlag(num);
		//recFlag = RT_FALSE;
	//  rt_mutex_release(&uartmque[sheet.cirCula[num].useUartNum]);
	//	 rt_kprintf("release\r\n");
		rt_free(buf);
	//	 rt_kprintf("free\r\n");
	  buf=RT_NULL;
		
}

//读取小数处理方式
static uint16_t readPoint(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.cirCula[num].slaveAddr,0x000B,READ_03,1,buf);
	  uint16_t ret =0;
	//	recFlag = RT_TRUE;
//		rt_mutex_take(uartDev[sheet.cirCula[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		cirCurrUartSend(num,buf,len);
	  rt_kprintf("%sreadPoint send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
		memset(buf,0,LENTH);
    len=0;
		while(rt_mq_recv(&uartmque[sheet.cirCula[num].useUartNum], buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
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
//	  rt_mutex_release(uartDev[sheet.cirCula[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;
		return ret;
}
//读取多路小数点处理方式
void readMultiCirCulaPoint()
{
		for (int i = 0; i < CIRCULA_485_NUM; i++)
		{		
				if(sheet.cirCula[i].workFlag==RT_TRUE){
						readPoint(i);
				}
		}
}




//环流json格式打包
	//输入 respFlag 为true就是回应
//              为false就是report数据
uint16_t circulaJsonPack(bool respFlag)
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
		
		if(respFlag==true){
			  cJSON_AddNumberToObject(root, "mid",respMid);
				cJSON_AddStringToObject(root, "packetType","PROPERTIES_485DATA_GET_RESP");
				cJSON_AddNumberToObject(root, "code",0);
		}
		else
		{
				cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
				cJSON_AddStringToObject(root, "packetType","PROPERTIES_485DATA_REP");
		}
		cJSON_AddStringToObject(root, "identifier","grounding_current_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
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
			
				  cJSON_AddNumberToObject(nodeobj,"responseStatus",cirCurStru_p[i].respStat);
					
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
					sprintf(sprinBuf,"%llu",utcTime());
					cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
				}
			}
		}
	
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// 打印JSON数据包  

		
		// 释放内存  
		
		
		out = cJSON_Print(root);
		NetTxBuffer[0]=0xff;
		NetTxBuffer[1]=0xff;
		NetTxBuffer[2]=0xff;
		NetTxBuffer[3]=0xff;
		rt_strcpy((char *)NetTxBuffer+PACK_HEAD_LEN,out);
		if(out!=NULL){
//				for(int i=0;i<rt_strlen(out);i++)
//						rt_kprintf("%c",out[i]);
//				rt_kprintf("\n");
				rt_free(out);
				out=NULL;
		}
		if(root!=NULL){
			cJSON_Delete(root);
			out=NULL;
		}
	
		if(respFlag==false){
				mcu.repDataMessID =mcu.upMessID;
				//mcu.devRegMessID =mcu.upMessID;
				upMessIdAdd();
		}

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return 1;
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








//复位温湿度的warn状态值
void resetCirCurrlWarnFlag()
{
		for (int i = 0; i < CIRCULA_485_NUM; i++)
		{		
				inpoutpFlag.modbusCircul[i].cirCurALowFlag =false;
				inpoutpFlag.modbusCircul[i].cirCurAUpFlag=false;
				inpoutpFlag.modbusCircul[i].cirCurBLowFlag=false;
				inpoutpFlag.modbusCircul[i].cirCurBUpFlag =false;
				inpoutpFlag.modbusCircul[i].cirCurCLowFlag=false;
				inpoutpFlag.modbusCircul[i].cirCurCUpFlag =false;
		}
}





//模拟温度和湿度值读取以及打包成json格式  返回true 有告警 false 无告警
bool modCirCurrWarn2Send()
{
		if(alarmFLag==false)//TEST
			return false;
		char* out = NULL;
		//创建数组
		cJSON* Array = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return false;
		// 加入节点（键值对）
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","EVENTS_485_ALARM");
		cJSON_AddStringToObject(root, "identifier","grounding_current_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		{
				Array = cJSON_CreateArray();
				if (Array == NULL) return false;
				cJSON_AddItemToObject(root, "params", Array);
				for (int i = 0; i < CIRCULA_485_NUM; i++)
				{		
						if(sheet.cirCula[i].workFlag==RT_TRUE){
							nodeobj = cJSON_CreateObject();
							cJSON_AddItemToArray(Array, nodeobj);
							cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.cirCula[i].ID));
							cJSON_AddNumberToObject(nodeobj,"alarmStatus",1);
							nodeobj_p= cJSON_CreateObject();
							cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
							
							cJSON_AddNumberToObject(nodeobj_p,"earthCurA_low_alarm",inpoutpFlag.modbusCircul[i].cirCurALowFlag );//cJSON_CreateNumber("10"));
							cJSON_AddNumberToObject(nodeobj_p,"earthCurA_high_alarm",inpoutpFlag.modbusCircul[i].cirCurAUpFlag);
							cJSON_AddNumberToObject(nodeobj_p,"runCurA_low_alarm",0);
							cJSON_AddNumberToObject(nodeobj_p,"runCurA_high_alarm",0);		
							cJSON_AddNumberToObject(nodeobj_p,"loadRatioA_low_alarm",0);
							cJSON_AddNumberToObject(nodeobj_p,"loadRatioA_high_alarm",0);		
							cJSON_AddNumberToObject(nodeobj_p,"earthCurB_low_alarm",inpoutpFlag.modbusCircul[i].cirCurBLowFlag );//cJSON_CreateNumber("10"));
							cJSON_AddNumberToObject(nodeobj_p,"earthCurB_high_alarm",inpoutpFlag.modbusCircul[i].cirCurBUpFlag);
							cJSON_AddNumberToObject(nodeobj_p,"runCurB_low_alarm",0);
							cJSON_AddNumberToObject(nodeobj_p,"runCurB_high_alarm",0);		
							cJSON_AddNumberToObject(nodeobj_p,"loadRatioB_low_alarm",0);
							cJSON_AddNumberToObject(nodeobj_p,"loadRatioB_high_alarm",0);	
							cJSON_AddNumberToObject(nodeobj_p,"earthCurC_low_alarm",inpoutpFlag.modbusCircul[i].cirCurCLowFlag );//cJSON_CreateNumber("10"));
							cJSON_AddNumberToObject(nodeobj_p,"earthCurC_high_alarm",inpoutpFlag.modbusCircul[i].cirCurCUpFlag);
							cJSON_AddNumberToObject(nodeobj_p,"runCurC_low_alarm",0);
							cJSON_AddNumberToObject(nodeobj_p,"runCurC_high_alarm",0);		
							cJSON_AddNumberToObject(nodeobj_p,"loadRatioC_low_alarm",0);
							cJSON_AddNumberToObject(nodeobj_p,"loadRatioC_high_alarm",0);								
							sprintf(sprinBuf,"%llu",utcTime());
							cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
						}
				}
		}
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);

		// 释放内存  
		out = cJSON_Print(root);
		NetTxBuffer[0]=0xff;
		NetTxBuffer[1]=0xff;
		NetTxBuffer[2]=0xff;
		NetTxBuffer[3]=0xff;
		rt_strcpy((char *)NetTxBuffer+PACK_HEAD_LEN,out);
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

		mcu.repDataMessID =mcu.upMessID;
		//mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_free(sprinBuf);
		sprinBuf=RT_NULL;
		return true;
}



//环流读取并打包发送  仅仅做封装而已
	//输入 respFlag 为true就是回应
//              为false就是report数据
void circulaRead2Send(bool respFlag)
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
				circulaJsonPack(respFlag);
				packMqttSend();
				rt_thread_mdelay(500);
				if(modCirCurrWarn2Send()==true){
							resetCirCurrlWarnFlag();//每次判断后复位warnflag状态值
							packMqttSend();
				}
		}
}

