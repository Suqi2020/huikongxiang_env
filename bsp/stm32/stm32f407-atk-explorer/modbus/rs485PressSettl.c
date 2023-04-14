#include "board.h"
//参考 《H-STC系列压差式沉降仪Modbus RTU协议说明-20191031》
//<<压差式沉降仪 GY-STC-2000>> 默认波特率9600  modbus地址 0xb1(有误) 见机身标签后2位  协议文档有误
//修改设备地址 FF FF 03 0A＋设备完整的长地址＋01＋短地址 
//   FF FF 03 0A 6E 01 20 21 09 08 00 B1 01 01
//  24+红色，24-黑色，A+蓝色，B-绿色
const static char sign[]="[沉降仪]";
static  bool alarmFLag=false;
//#define   SLAVE_ADDR     0X02 
#define   LENTH          50  //工作环流用到的最大接收buf长度

typedef struct{
	  float temp;//除以100 传输float类型  单位0C
	   union {
				float flotVal;
				int   intVal;
		} height;
		uint8_t respStat;
//float height;//除以10 传输float类型  单位mm
}pressSettlStru;
pressSettlStru pressSettle[PRESSSETTL_485_NUM];
//返回沉降仪的通讯状态 true--通讯成功 false--通讯失败
int pressSettleState(int i)
{
		return pressSettle[i].respStat;
}
//打包串口发送 
static void pressSettlUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.pressSetl[num].useUartNum,buf, len);
}

///////////////////////////////////////读写寄存器相关操作////////////////////////////////////////



uint8_t tongHeModbusRead(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out)
{
		int i=0;
	  out[i]=slavAddr;					 			i++;
	  out[i]=0x04;      					 		i++;
	  out[i]=(uint8_t)(regAddr>>8);   i++;
	  out[i]=(uint8_t) regAddr;       i++;
		out[i]=(uint8_t)(len>>8);       i++;
	  out[i]=(uint8_t) len;       		i++;
	  uint16_t crcRet=RTU_CRC(out ,i);
	  out[i]=(uint8_t)(crcRet>>8);    i++;
	  out[i]=crcRet;       						i++;
		return i;
}
//沉降仪比较阈值并设置相应的flag标记
static void pressStlCheckSetFlag(int num)
{
		alarmFLag=false;
	  if(sheet.modbusPreSettl[num].tempUpLimit!=0){
			if(pressSettle[num].temp>=sheet.modbusPreSettl[num].tempUpLimit){
					inpoutpFlag.modbusPreSettl[num].tempUpFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusPreSettl[num].tempLowLimit!=0){
			if(pressSettle[num].temp<=sheet.modbusPreSettl[num].tempLowLimit){
					inpoutpFlag.modbusPreSettl[num].tempLowFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusPreSettl[num].heightUpLimit!=0){
			if(pressSettle[num].height.flotVal>=sheet.modbusPreSettl[num].heightUpLimit){
					inpoutpFlag.modbusPreSettl[num].heightUpFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusPreSettl[num].heightLowLimit!=0){
			if(pressSettle[num].height.flotVal<=sheet.modbusPreSettl[num].heightLowLimit){
					inpoutpFlag.modbusPreSettl[num].heightLowFlag=true;alarmFLag=true;
			}
		}

}


//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
//读取沉降仪的温度和高度
void readPSTempHeight(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = tongHeModbusRead(sheet.pressSetl[num].slaveAddr,0X0001,2,buf);
		rt_mutex_take(uartDev[sheet.pressSetl[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		pressSettlUartSend(num,buf,len);
	  rt_kprintf("%spressSettl send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		
		while(rt_mq_recv(uartDev[sheet.pressSetl[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
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
		int ret2=modbusRespCheck(sheet.pressSetl[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值

        int temp	=(buf[offset]<<8)+buf[offset+1];offset+=2;
			  pressSettle[num].height.intVal=(buf[offset]<<8)+buf[offset+1];
			
				pressSettle[num].temp =temp/100;
				pressSettle[num].respStat=1;
				pressStlCheckSetFlag(num);
			
			  rt_kprintf("%stemp:%0.2f*C height:%0.1fmm read ok\n",sign,pressSettle[num].temp,pressSettle[num].height.flotVal);  
		} 
		else{//读不到给0
				if(ret2==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
				pressSettle[num].respStat=0;
			  pressSettle[num].temp	=0;
			  pressSettle[num].height.intVal=0;
			  rt_kprintf("%stemp height read fail\n",sign);
		}
//		pressStlCheckSetFlag(num);
	  rt_mutex_release(uartDev[sheet.pressSetl[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}


/////////////////////////////////////////JSON格式打包//////////////////////////////////////////
//温度高度值打包




//沉降仪json格式打包
//输入 respFlag 为true就是回应
//              为false就是report数据
uint16_t pressSettlJsonPack(bool respFlag)
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
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
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
		cJSON_AddStringToObject(root, "identifier","settlement_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		
		
		{
			Array = cJSON_CreateArray();
			if (Array == NULL) return 0;
			cJSON_AddItemToObject(root, "params", Array);
			for (int i = 0; i < PRESSSETTL_485_NUM; i++)
			{		
				if(sheet.pressSetl[i].workFlag==RT_TRUE){
					nodeobj = cJSON_CreateObject();
					cJSON_AddItemToArray(Array, nodeobj);
					cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.pressSetl[i].ID));
					//sprintf(sprinBuf,"%d",pressSettle[i].respStat);
					cJSON_AddNumberToObject(nodeobj,"responseStatus",pressSettle[i].respStat);
					
					nodeobj_p= cJSON_CreateObject();
					cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
					sprintf(sprinBuf,"%02f",pressSettle[i].temp);
					cJSON_AddItemToObject(nodeobj_p,"temperature",cJSON_CreateString(sprinBuf));

					sprintf(sprinBuf,"%02f",pressSettle[i].height.flotVal );
					cJSON_AddItemToObject(nodeobj_p,"height",cJSON_CreateString(sprinBuf));
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
		rt_strlen(out);
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
				//mcu.devRegMessID =mcu.upMessID;
				upMessIdAdd();
		}

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return 1;
}






//void pressSetlCheckSetFlag(int num)
//{
//	  alarmFLag=false;
//		
//		if(sheet.modbusPreSettl[num].tempUpLimit!=0){
//			  if(pressSettle[num].temp>=sheet.modbusPreSettl[num].tempUpLimit){
//					inpoutpFlag.modbusThreAxis[num].tempUpFlag=true;
//					alarmFLag=true;
//				}
//		}
//		
//		if(sheet.modbusPreSettl[num].tempLowLimit!=0){
//				if(pressSettle[num].temp<=sheet.modbusPreSettl[num].tempLowLimit){
//					inpoutpFlag.modbusThreAxis[num].tempLowFlag=true;
//					alarmFLag=true;
//				}
//		}
//		if(sheet.modbusPreSettl[num].heightUpLimit!=0){
//				if(pressSettle[num].height.flotVal>=sheet.modbusPreSettl[num].heightUpLimit){
//					inpoutpFlag.modbusThreAxis[num].accXUpFlag=true;
//					alarmFLag=true;
//				}
//		}
//		if(sheet.modbusPreSettl[num].heightLowLimit!=0){
//				if(pressSettle[num].height.flotVal<=sheet.modbusPreSettl[num].heightLowLimit){
//					inpoutpFlag.modbusThreAxis[num].accXLowFlag=true;
//					alarmFLag=true;
//				}
//		}


//}
//复位温湿度的warn状态值
void resetPressSetlWarnFlag()
{
		for (int i = 0; i < PRESSSETTL_485_NUM; i++)
		{		
				inpoutpFlag.modbusPreSettl[i].tempUpFlag =false;
				inpoutpFlag.modbusPreSettl[i].tempLowFlag=false;
				inpoutpFlag.modbusPreSettl[i].heightLowFlag=false;
				inpoutpFlag.modbusPreSettl[i].heightUpFlag =false;
		}
}





//模拟温度和湿度值读取以及打包成json格式  返回true 有告警 false 无告警
bool modPressSetlWarn2Send()
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
		cJSON_AddStringToObject(root, "identifier","settlement_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		{
				Array = cJSON_CreateArray();
				if (Array == NULL) return false;
				cJSON_AddItemToObject(root, "params", Array);
				for (int i = 0; i < PRESSSETTL_485_NUM; i++)
				{		
						if(sheet.pressSetl[i].workFlag==RT_TRUE){
							nodeobj = cJSON_CreateObject();
							cJSON_AddItemToArray(Array, nodeobj);
							cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.pressSetl[i].ID));
							cJSON_AddNumberToObject(nodeobj,"alarmStatus",1);
							nodeobj_p= cJSON_CreateObject();
							cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
							cJSON_AddNumberToObject(nodeobj_p,"temperature_low_alarm",inpoutpFlag.modbusPreSettl[i].tempLowFlag);//cJSON_CreateNumber("10"));
							cJSON_AddNumberToObject(nodeobj_p,"temperature_high_alarm",inpoutpFlag.modbusPreSettl[i].tempUpFlag);
							cJSON_AddNumberToObject(nodeobj_p,"height_low_alarm",inpoutpFlag.modbusPreSettl[i].tempLowFlag);
							cJSON_AddNumberToObject(nodeobj_p,"height_high_alarm",inpoutpFlag.modbusPreSettl[i].tempUpFlag);		
							
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




//沉降仪读取并打包  供别的函数调用
void pressSettRead2Send(rt_bool_t netStat,bool respFlag)
{
	  int workFlag=RT_FALSE;
		for(int i=0;i<PRESSSETTL_485_NUM;i++){
				if(sheet.pressSetl[i].workFlag==RT_TRUE){
						readPSTempHeight(i);
						workFlag=RT_TRUE;
				}
		}
		if(workFlag==RT_TRUE){
				rt_kprintf("%s打包采集的PRESSSETTL数据\r\n",sign);
				pressSettlJsonPack(respFlag);
				if(netStat==RT_TRUE)
						packMqttSend();
				rt_thread_mdelay(500);
				if(modPressSetlWarn2Send()==true){
							resetPressSetlWarnFlag();//每次判断后复位warnflag状态值
							//rt_thread_mdelay(500);
							if(netStat==RT_TRUE)
									packMqttSend();
				}
		}
}
