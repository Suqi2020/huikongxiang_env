#include "board.h"
//<<TH-DCM数字式裂缝计串口通讯协议说明-20210622>>
//  24+红色，24-黑色，A+蓝色，B-绿色
// 同禾修改地址 修改设备地址 FF FF 03 0A＋设备完整的长地址＋FF(01)＋短地址   沉降仪和裂缝仪用FF  三轴测振仪01
const static char sign[]="[裂缝仪]";

//#define   SLAVE_ADDR     0X02 
#define   LENTH          50  //工作环流用到的最大接收buf长度

typedef struct{
	   union {
				float flotVal;
				uint32_t   intVal;
		} distanc;
		 union {
				float flotVal;
				uint32_t   intVal;
		} temp;
		uint8_t respStat;
}crackMeterStru;
crackMeterStru crackMeter[CRACKMETER_485_NUM];
//返回沉降仪的通讯状态 true--通讯成功 false--通讯失败
int  crackMeterState(int i)
{
		return crackMeter[i].respStat;
}
//打包串口发送 
static void  crackMeterUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.crackMeter[num].useUartNum,buf, len);
}

///////////////////////////////////////读写寄存器相关操作////////////////////////////////////////



//沉降仪比较阈值并设置相应的flag标记
static void pressStlCheckSetFlag(int num)
{

	  if(sheet.modbusCrackMeter[num].tempUpLimit!=0){
			if(crackMeter[num].temp.flotVal>=sheet.modbusCrackMeter[num].tempUpLimit){
					inpoutpFlag.modbusCrackMeter[num].tempUpFlag=true;
			}
		}
		if(sheet.modbusCrackMeter[num].tempLowLimit!=0){
			if(crackMeter[num].temp.flotVal<=sheet.modbusCrackMeter[num].tempLowLimit){
					inpoutpFlag.modbusCrackMeter[num].tempLowFlag=true;
			}
		}
		if(sheet.modbusCrackMeter[num].distancUpLimit!=0){
			if(crackMeter[num].distanc.flotVal>=sheet.modbusCrackMeter[num].distancUpLimit){
					inpoutpFlag.modbusCrackMeter[num].distancUpFlag=true;
			}
		}
		if(sheet.modbusCrackMeter[num].distancLowLimit!=0){
			if(crackMeter[num].distanc.flotVal<=sheet.modbusCrackMeter[num].distancLowLimit){
					inpoutpFlag.modbusCrackMeter[num].distancLowFlag=true;
			}
		}
}

extern uint8_t tongHeModbusRead(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out);
//读取沉降仪的温度和高度
void readCrackMeter(int num)
{
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = tongHeModbusRead(sheet.crackMeter[num].slaveAddr,0x0001,2,buf);
	  //485发送buf  len  等待modbus回应
		crackMeterUartSend(num,buf,len);
	  rt_kprintf("%s send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		while(rt_mq_recv(&uartmque[sheet.crackMeter[num].useUartNum], buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
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
		int ret2=modbusRespCheck(sheet.crackMeter[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值
        int offset =3;//   2 4 8 3b ae fe 70 41 d4 0 0 26 35 
			  crackMeter[num].distanc.intVal=(uint32_t)((uint32_t)buf[offset]<<24)+((uint32_t)buf[offset+1]<<16)+((uint32_t)buf[offset+2]<<8)+buf[offset+3];
			  offset+=4;
			  crackMeter[num].temp.intVal   =(uint32_t)((uint32_t)buf[offset]<<24)+((uint32_t)buf[offset+1]<<16)+((uint32_t)buf[offset+2]<<8)+buf[offset+3];
				crackMeter[num].respStat=1;
				pressStlCheckSetFlag(num);
				if(len!=0){
						rt_kprintf("%srec2:",sign);
						for(int j=0;j<len;j++){
								rt_kprintf("%x ",buf[j]);
						}
						rt_kprintf("\n");
				}	 
			  rt_kprintf("%stemp:%0.8f*C distance:%0.8fmm read ok\n",sign,crackMeter[num].temp.flotVal,crackMeter[num].distanc.flotVal);
		} 
		else{//读不到给0
				if(ret2==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
				crackMeter[num].respStat=0;
			  crackMeter[num].temp.intVal	=0;
			  crackMeter[num].distanc.intVal=0;
			  rt_kprintf("%stemp distance read fail\n",sign);
		}
		rt_free(buf);
	  buf=RT_NULL;

}


//沉降仪json格式打包
//输入 respFlag 为true就是回应
//              为false就是report数据
uint16_t crackMeterJsonPack(bool respFlag)
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
		cJSON_AddStringToObject(root, "identifier","crackmeter_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		{
			Array = cJSON_CreateArray();
			if (Array == NULL) return 0;
			cJSON_AddItemToObject(root, "params", Array);
			for (int i = 0; i < CRACKMETER_485_NUM; i++)
			{		
				if(sheet.crackMeter[i].workFlag==RT_TRUE){
					nodeobj = cJSON_CreateObject();
					cJSON_AddItemToArray(Array, nodeobj);
					cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.crackMeter[i].ID));
					cJSON_AddNumberToObject(nodeobj,"responseStatus",crackMeter[i].respStat);
					
					nodeobj_p= cJSON_CreateObject();
					cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
					sprintf(sprinBuf,"%02f",crackMeter[i].temp.flotVal);
					cJSON_AddItemToObject(nodeobj_p,"temperature",cJSON_CreateString(sprinBuf));

					sprintf(sprinBuf,"%02f",crackMeter[i].distanc.flotVal );
					cJSON_AddItemToObject(nodeobj_p,"distance",cJSON_CreateString(sprinBuf));
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
		int len=strlen((char *)NetTxBuffer);
		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}





//复位温湿度的warn状态值
void resetCrackMeterWarnFlag()
{
		for (int i = 0; i < CRACKMETER_485_NUM; i++)
		{		
				inpoutpFlag.modbusCrackMeter[i].tempUpFlag =false;
				inpoutpFlag.modbusCrackMeter[i].tempLowFlag=false;
				inpoutpFlag.modbusCrackMeter[i].distancLowFlag=false;
				inpoutpFlag.modbusCrackMeter[i].distancUpFlag =false;
		}
}





//模拟温度和湿度值读取以及打包成json格式  返回true 有告警 false 无告警
bool modCrackMeterWarn2Send()
{

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
		cJSON_AddStringToObject(root, "identifier","crackmeter_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		{
				Array = cJSON_CreateArray();
				if (Array == NULL) return false;
				cJSON_AddItemToObject(root, "params", Array);
				for (int i = 0; i < CRACKMETER_485_NUM; i++)
				{		
						if(sheet.crackMeter[i].workFlag==RT_TRUE){
							nodeobj = cJSON_CreateObject();
							cJSON_AddItemToArray(Array, nodeobj);
							cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.crackMeter[i].ID));
							cJSON_AddNumberToObject(nodeobj,"alarmStatus",1);
							nodeobj_p= cJSON_CreateObject();
							cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
							cJSON_AddNumberToObject(nodeobj_p,"temperature_low_alarm",inpoutpFlag.modbusCrackMeter[i].tempLowFlag);//cJSON_CreateNumber("10"));
							cJSON_AddNumberToObject(nodeobj_p,"temperature_high_alarm",inpoutpFlag.modbusCrackMeter[i].tempUpFlag);
							cJSON_AddNumberToObject(nodeobj_p,"distance_low_alarm",inpoutpFlag.modbusCrackMeter[i].tempLowFlag);
							cJSON_AddNumberToObject(nodeobj_p,"distancedistance_high_alarm",inpoutpFlag.modbusCrackMeter[i].tempUpFlag);		
							
							sprintf(sprinBuf,"%llu",utcTime());
							cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
						}
				}
		}
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		//打包
		// 释放内存  
		out = cJSON_Print(root);
		NetTxBuffer[0]=0xff;
		NetTxBuffer[1]=0xff;
		NetTxBuffer[2]=0xff;
		NetTxBuffer[3]=0xff;
		rt_strcpy((char *)NetTxBuffer+PACK_HEAD_LEN,out);
		if(out!=NULL){
				rt_free(out);
				out=NULL;
		}
		if(root!=NULL){
			cJSON_Delete(root);
			out=NULL;
		}

		int len=strlen((char *)NetTxBuffer);
		rt_free(sprinBuf);
		sprinBuf=RT_NULL;
		mcu.repDataMessID =mcu.upMessID;
		upMessIdAdd();
		return true;
}



//沉降仪读取并打包  供别的函数调用
void crackMeterRead2Send(bool respFlag)
{
	  int workFlag=RT_FALSE;
		for(int i=0;i<CRACKMETER_485_NUM;i++){
				if(sheet.crackMeter[i].workFlag==RT_TRUE){
						readCrackMeter(i);
						workFlag=RT_TRUE;
				}
		}
		if(workFlag==RT_TRUE){
				rt_kprintf("%s打包采集的CRACK_METER数据\r\n",sign);
				crackMeterJsonPack(respFlag);
			  packMqttSend();

				rt_thread_mdelay(500);
				if(modCrackMeterWarn2Send()==true){
							resetCrackMeterWarnFlag();//每次判断后复位warnflag状态值
							packMqttSend();
				}
		}
}
