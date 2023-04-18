#include "board.h"
//#include "stmflash.h"
//水位默认地址02  波特率9600  量程0~49米
//读取水位
const static char sign[]="[水位]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //工作环流用到的最大接收buf长度

float waterDepth[WATERDEPTH_485_NUM];
static uint8_t respStat[WATERDEPTH_485_NUM];
//打包串口发送 
static void waterDepthUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.waterDepth[num].useUartNum,buf, len);
}
//返回水位的通讯状态 true--通讯成功 false--通讯失败
int waterDepthState(int i)
{
		return respStat[i];
}
static  bool alarmFLag=false;

//水位传感器比较阈值并设置相应的flag标记
static void waterLevCheckSetFlag(int num)
{
	  alarmFLag=false;
		if(sheet.modbusWaterDepth[num].depthUpLimit!=0){
			if(waterDepth[num]>=sheet.modbusWaterDepth[num].depthUpLimit)
					inpoutpFlag.modbusWaterDepth[num].depthUpFlag=true;alarmFLag=true;
		}
		if(sheet.modbusWaterDepth[num].depthLowLimit!=0){
			if(waterDepth[num]<=sheet.modbusWaterDepth[num].depthLowLimit)
					inpoutpFlag.modbusWaterDepth[num].depthLowFlag=true;alarmFLag=true;
		}
}




//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
//读取水位
static void readWaterDepth(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.waterDepth[num].slaveAddr,0X0002,READ_04,2,buf);
		//rt_mutex_take(.uartMessque[sheet.waterDepth[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		waterDepthUartSend(num,buf,len);
	  rt_kprintf("%swater send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		

		while(rt_mq_recv(&uartmque[sheet.waterDepth[num].useUartNum], buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
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
		int ret2=modbusRespCheck(sheet.waterDepth[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值
        uint32_t read	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
        extern float write_hex_to_float(uint32_t number);
			  float waterDepth_p= write_hex_to_float(read);
        waterDepth[num]=(float)(waterDepth_p	/9.8);
			  rt_kprintf("%s水深:%0.4f米\n",sign,waterDepth[num]);  
			  respStat[num]=1;
				waterLevCheckSetFlag(num);
		} 
		else{//读不到给0
				if(ret2==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
				respStat[num]=0;
			  waterDepth[num]	=0;
			  rt_kprintf("%s read fail\n",sign);
		}
		//waterLevCheckSetFlag(num);
	  //rt_mutex_release(.uartMessque[sheet.waterDepth[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}



//水位值打包成json格式
//输入 respFlag 为true就是回应
//              为false就是report数据
uint16_t waterDepthJsonPack(bool respFlag)
{
//		char *sprinBuf=RT_NULL;
//		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
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
		cJSON_AddStringToObject(root, "identifier","water_level_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
//	  cJSON_AddItemToObject(root,"acuId",cJSON_CreateString(packFlash.acuId));
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了	
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < WATERDEPTH_485_NUM; i++)
		{		
			if(sheet.waterDepth[i].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.waterDepth[i].ID));
				//sprintf(sprinBuf,"%d",respStat[i]);
				cJSON_AddNumberToObject(nodeobj,"responseStatus",respStat[i]);
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				sprintf(sprinBuf,"%02f",waterDepth[i]);
				cJSON_AddItemToObject(nodeobj_p,"depth",cJSON_CreateString(sprinBuf));

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
		rt_strcpy((char *)packBuf,out);
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
	

		if(respFlag==false){
			mcu.repDataMessID =mcu.upMessID;
			upMessIdAdd();
		}

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return 1;
}

void resetWaterLevWarnFlag()
{
		for (int i = 0; i < WATERDEPTH_485_NUM; i++)
		{		
				inpoutpFlag.modbusWaterDepth[i].depthLowFlag =false;
				inpoutpFlag.modbusWaterDepth[i].depthUpFlag=false;
		}
}





//模拟温度和湿度值读取以及打包成json格式  返回true 有告警 false 无告警
bool modPWaterLevWarn2Send()
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
		cJSON_AddStringToObject(root, "identifier","water_level_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		{
				Array = cJSON_CreateArray();
				if (Array == NULL) return false;
				cJSON_AddItemToObject(root, "params", Array);
				for (int i = 0; i < WATERDEPTH_485_NUM; i++)
				{		
						if(sheet.waterDepth[i].workFlag==RT_TRUE){
							nodeobj = cJSON_CreateObject();
							cJSON_AddItemToArray(Array, nodeobj);
							cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.waterDepth[i].ID));
							cJSON_AddNumberToObject(nodeobj,"alarmStatus",1);
							nodeobj_p= cJSON_CreateObject();
							cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
							cJSON_AddNumberToObject(nodeobj_p,"depth_high_alarm",inpoutpFlag.modbusWaterDepth[i].depthUpFlag);//cJSON_CreateNumber("10"));
							cJSON_AddNumberToObject(nodeobj_p,"depth_low_alarm",inpoutpFlag.modbusWaterDepth[i].depthLowFlag);
//							cJSON_AddNumberToObject(nodeobj_p,"height_low_alarm",inpoutpFlag.modbusPreSettl[i].tempLowFlag);
//							cJSON_AddNumberToObject(nodeobj_p,"height_high_alarm",inpoutpFlag.modbusPreSettl[i].tempUpFlag);		
							
							sprintf(sprinBuf,"%llu",utcTime());
							cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
						}
				}
		}
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);

		// 释放内存  
		out = cJSON_Print(root);
		rt_strcpy((char *)packBuf,out);
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
//水位值的读取和打包发送  仅仅做封装 供别的函数来调用
void waterDepthRead2Send(bool respFlag)
{
	 int workFlag=RT_FALSE;
			for(int i=0;i<WATERDEPTH_485_NUM;i++){
					if(sheet.waterDepth[i].workFlag==RT_TRUE){
							readWaterDepth(i);
							workFlag=RT_TRUE;
					}
			}
			if(workFlag==RT_TRUE){
					rt_kprintf("%s打包采集的waterdepth数据\r\n",sign);
					waterDepthJsonPack(respFlag);
					
					packMqttSend();
					
					rt_thread_mdelay(500);
					if(modPWaterLevWarn2Send()==true){
							resetWaterLevWarnFlag();//每次判断后复位warnflag状态值
							packMqttSend();
					}
			}
}

