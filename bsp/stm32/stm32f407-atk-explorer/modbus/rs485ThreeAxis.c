#include "board.h"
//#include "stmflash.h"
//<<压差式沉降仪 GY-STC-2000>> 默认波特率9600  modbus地址 0xb1(有误) 见机身标签后2位   协议文档有误
/*
// 同禾修改地址 修改设备地址 FF FF 03 0A＋设备完整的长地址＋FF(01)＋短地址   沉降仪和裂缝仪用FF  三轴测振仪01
三轴验证短地址＋04 00 01 00 04

压差验证短地址＋04 00 01 00 02
波特率改为9600
*/
//修改设备地址 FF FF 03 0A＋设备完整的长地址＋01＋短地址 
//   FF FF 03 0A 6E 01 20 21 09 08 00 B1 FF 01 
//  24+红色，24-黑色，A+蓝色，B-绿色
// 发 01 04 00 01 00 04 A0 09 
// 收 01 04 08 0B CA FE 8D 00 03 03 80 C7 23 
// 发                   01 04 00 01 00 04 A0 09 
//[防外破]threeAxis send:9  4  0  1  0  4 a1 41 
// 收         01 04 08 0B CA FE 8D 00 03 03 80 C7 23 
//[防外破]rec: 9  4  6  b 73  0 9a  0 17 fc 32 69 b4
typedef struct{
		float temp;
	  uint16_t acclrationX;
		uint16_t acclrationY;
		uint16_t acclrationZ;
	  uint8_t  respStat;
}threeAxisStru;
const static char sign[]="[防外破]";
threeAxisStru threeAxisp[THREEAXIS_485_NUM];
//返回三轴的通讯状态 true--通讯成功 false--通讯失败
int threeAxisState(int i)
{
		return threeAxisp[i].respStat;
}
//#define   SLAVE_ADDR     0X02 
//#define   LENTH          50  //工作环流用到的最大接收buf长度

static  bool alarmFLag=false;
//打包串口发送 
static void threeAxisUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.threeAxiss[num].useUartNum,buf, len);
}

///////////////////////////////////////读写寄存器相关操作////////////////////////////////////////

//与压差式传感器共用一个读命令
extern uint8_t tongHeModbusRead(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out);

//三轴比较阈值并设置相应的flag标记
static void threeAccCheckSetFlag(int num)
{
	  alarmFLag=false;
	  if(sheet.modbusThreAxis[num].tempUpLimit!=0){
			if(threeAxisp[num].temp>=sheet.modbusThreAxis[num].tempUpLimit){
					inpoutpFlag.modbusThreAxis[num].tempUpFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusThreAxis[num].tempLowLimit!=0){
			if(threeAxisp[num].temp<=sheet.modbusThreAxis[num].tempLowLimit){
					inpoutpFlag.modbusThreAxis[num].tempLowFlag=true;alarmFLag=true;
			}
		}

		if(sheet.modbusThreAxis[num].accXUpLimit!=0){
			if(threeAxisp[num].acclrationX>=sheet.modbusThreAxis[num].accXUpLimit){
					inpoutpFlag.modbusThreAxis[num].accXUpFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusThreAxis[num].accXLowLimit!=0){
			if(threeAxisp[num].acclrationX<=sheet.modbusThreAxis[num].accXLowLimit){
					inpoutpFlag.modbusThreAxis[num].accXLowFlag=true;alarmFLag=true;
			}
		}

		if(sheet.modbusThreAxis[num].accYUpLimit!=0){
			if(threeAxisp[num].acclrationY>=sheet.modbusThreAxis[num].accYUpLimit){
					inpoutpFlag.modbusThreAxis[num].accYUpFlag=true;alarmFLag=true;
			}
		}
		if(sheet.modbusThreAxis[num].accYLowLimit!=0){
			if(threeAxisp[num].acclrationY<=sheet.modbusThreAxis[num].accYLowLimit){
					inpoutpFlag.modbusThreAxis[num].accYLowFlag=true;alarmFLag=true;
			}
		}

		if(sheet.modbusThreAxis[num].accZUpLimit!=0){
			if(threeAxisp[num].acclrationZ>=sheet.modbusThreAxis[num].accZUpLimit){
					inpoutpFlag.modbusThreAxis[num].accZUpFlag=true;alarmFLag=true;
			}
		}
    if(sheet.modbusThreAxis[num].accZLowLimit!=0){
			if(threeAxisp[num].acclrationZ<=sheet.modbusThreAxis[num].accZLowLimit){
					inpoutpFlag.modbusThreAxis[num].accZLowFlag=true;alarmFLag=true;
			}
		}

}
//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
//通过485接口读取三轴的值
void readThreeTempAcc(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = tongHeModbusRead(sheet.threeAxiss[num].slaveAddr,0X0001,4,buf);
//		rt_mutex_take(.uartMessque[sheet.threeAxiss[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		threeAxisUartSend(num,buf,len);
	  rt_kprintf("%sthreeAxis send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		while(rt_mq_recv(&uartmque[sheet.threeAxiss[num].useUartNum], buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
//		.uartMessque[modbusFlash[THREEAXIS].useUartNum].offline=RT_FALSE;
		//提取环流值 第一步判断crc 第二部提取
		int ret2=modbusRespCheck(sheet.threeAxiss[num].slaveAddr,buf,len,RT_TRUE);
		if(0 ==  ret2){//刷新读取到的值
        int temp	=(buf[offset]<<8)+buf[offset+1];offset+=2;
			  threeAxisp[num].acclrationX = (buf[offset]<<8)+buf[offset+1];offset+=2;
				threeAxisp[num].acclrationY = (buf[offset]<<8)+buf[offset+1];offset+=2;
				threeAxisp[num].acclrationZ = (buf[offset]<<8)+buf[offset+1];offset+=2;
        threeAxisp[num].temp=(float)temp/100; 
			  threeAxisp[num].respStat=1;
				threeAccCheckSetFlag(num);
			  rt_kprintf("%stemp:%0.2f*C ACC:X%dmg Y%dmg Z%dmg ok\n",sign,threeAxisp[num].temp,threeAxisp[num].acclrationX,threeAxisp[num].acclrationY,threeAxisp[num].acclrationZ);  
		} 
		else{//读不到给0
				if(ret2==2){
//					  .uartMessque[modbusFlash[THREEAXIS].useUartNum].offline=RT_TRUE;
				}
				threeAxisp[num].respStat=0;
				threeAxisp[num].temp=0;
			  threeAxisp[num].acclrationX	= 0;
			  threeAxisp[num].acclrationY = 0;
			  threeAxisp[num].acclrationY = 0;
			  rt_kprintf("%stemp height read fail\n",sign);
		}
//		threeAccCheckSetFlag(num);
//	  rt_mutex_release(.uartMessque[sheet.threeAxiss[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

		
		

		
}

/////////////////////////////////////////JSON格式打包//////////////////////////////////////////
//温度高度值打包

/*
{
    "mid":1234,
    "packetType�":"CMD_REPORTDATA",  
    "param":
    {
        "identifier":" vibration_meter_monitor",
        "acuId":"100000000000001",
        "deviceId":"1000000000004",�
        "data":
        {
            "temp":"22.75", //?
            "accelerationX":"1234",//mg
"accelerationY":"1234",//mg
"accelerationZ":"1234",//mg
            "monitoringTime":"1655172531937"
        }
    },
    "timestamp":"1655172531937"
}
*/
//三轴相关值通过json打包


//输入 respFlag 为true就是回应
//              为false就是report数据
static uint16_t threeAxisJsonPack(bool respFlag)
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
		cJSON_AddStringToObject(root, "identifier","vibration_monitor");
	  cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < THREEAXIS_485_NUM; i++)
		{		

			if(sheet.threeAxiss[i].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.threeAxiss[i].ID));
				
			//	sprintf(sprinBuf,"%d",threeAxisp[i].respStat);
				cJSON_AddNumberToObject(nodeobj,"responseStatus",threeAxisp[i].respStat);
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				
				sprintf(sprinBuf,"%02f",threeAxisp[i].temp);
				cJSON_AddItemToObject(nodeobj_p,"temperature",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",threeAxisp[i].acclrationX);
				cJSON_AddItemToObject(nodeobj_p,"accelerationX",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",threeAxisp[i].acclrationY);
				cJSON_AddItemToObject(nodeobj_p,"accelerationY",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%d",threeAxisp[i].acclrationZ);
				cJSON_AddItemToObject(nodeobj_p,"accelerationZ",cJSON_CreateString(sprinBuf));

				sprintf(sprinBuf,"%llu",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		}
		}
	
		sprintf(sprinBuf,"%llu",utcTime());
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


		if(respFlag==false){
			mcu.repDataMessID =mcu.upMessID;
			//mcu.devRegMessID =mcu.upMessID;
			upMessIdAdd();
		}

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return 1;
}
//温湿度传感器读取值与阈值比较并设置flag
 void threeAxisCheckSetFlag(int num)
{
	  alarmFLag=false;
		
		if(sheet.modbusThreAxis[num].tempUpLimit!=0){
			  if(threeAxisp[num].temp>=sheet.modbusThreAxis[num].tempUpLimit){
					inpoutpFlag.modbusThreAxis[num].tempUpFlag=true;
					alarmFLag=true;
				}
		}
		
		if(sheet.modbusThreAxis[num].tempLowLimit!=0){
				if(threeAxisp[num].temp<=sheet.modbusThreAxis[num].tempLowLimit){
					inpoutpFlag.modbusThreAxis[num].tempLowFlag=true;
					alarmFLag=true;
				}
		}
		if(sheet.modbusThreAxis[num].accXUpLimit!=0){
				if(threeAxisp[num].acclrationX>=sheet.modbusThreAxis[num].accXUpLimit){
					inpoutpFlag.modbusThreAxis[num].accXUpFlag=true;
					alarmFLag=true;
				}
		}
		if(sheet.modbusThreAxis[num].accXLowLimit!=0){
				if(threeAxisp[num].temp<=sheet.modbusThreAxis[num].accXLowLimit){
					inpoutpFlag.modbusThreAxis[num].accXLowFlag=true;
					alarmFLag=true;
				}
		}

		if(sheet.modbusThreAxis[num].accYUpLimit!=0){
				if(threeAxisp[num].acclrationY>=sheet.modbusThreAxis[num].accYUpLimit){
					inpoutpFlag.modbusThreAxis[num].accYUpFlag=true;
					alarmFLag=true;
				}
		}
		if(sheet.modbusThreAxis[num].accYLowLimit!=0){
				if(threeAxisp[num].temp<=sheet.modbusThreAxis[num].accYLowLimit){
					inpoutpFlag.modbusThreAxis[num].accYLowFlag=true;
					alarmFLag=true;
				}
		}
		if(sheet.modbusThreAxis[num].accZUpLimit!=0){
				if(threeAxisp[num].acclrationX>=sheet.modbusThreAxis[num].accZUpLimit){
					inpoutpFlag.modbusThreAxis[num].accZUpFlag=true;
					alarmFLag=true;
				}
		}
		if(sheet.modbusThreAxis[num].accZLowLimit!=0){
				if(threeAxisp[num].temp<=sheet.modbusThreAxis[num].accZLowLimit){
					inpoutpFlag.modbusThreAxis[num].accZLowFlag=true;
					alarmFLag=true;
				}
		}
}
//复位温湿度的warn状态值
void resetThreeAxisWarnFlag()
{
		for (int i = 0; i < THREEAXIS_485_NUM; i++)
		{		
				inpoutpFlag.modbusThreAxis[i].tempUpFlag =false;
				inpoutpFlag.modbusThreAxis[i].tempLowFlag=false;
				inpoutpFlag.modbusThreAxis[i].accXLowFlag=false;
				inpoutpFlag.modbusThreAxis[i].accXUpFlag =false;
				inpoutpFlag.modbusThreAxis[i].accYLowFlag=false;
				inpoutpFlag.modbusThreAxis[i].accYUpFlag =false;
				inpoutpFlag.modbusThreAxis[i].accZLowFlag=false;
				inpoutpFlag.modbusThreAxis[i].accZUpFlag =false;
		}
}





//模拟温度和湿度值读取以及打包成json格式  返回true 有告警 false 无告警
bool modThreeAxisWarn2Send()
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
		cJSON_AddStringToObject(root, "identifier","vibration_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		{
				Array = cJSON_CreateArray();
				if (Array == NULL) return false;
				cJSON_AddItemToObject(root, "params", Array);
				for (int i = 0; i < THREEAXIS_485_NUM; i++)
				{		
						if(sheet.threeAxiss[i].workFlag==RT_TRUE){
							nodeobj = cJSON_CreateObject();
							cJSON_AddItemToArray(Array, nodeobj);
							cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.threeAxiss[i].ID));
							cJSON_AddNumberToObject(nodeobj,"alarmStatus",1);
							nodeobj_p= cJSON_CreateObject();
							cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
							cJSON_AddNumberToObject(nodeobj_p,"temperature_low_alarm",inpoutpFlag.modbusThreAxis[i].tempLowFlag);//cJSON_CreateNumber("10"));
							cJSON_AddNumberToObject(nodeobj_p,"temperature_high_alarm",inpoutpFlag.modbusThreAxis[i].tempUpFlag);
							cJSON_AddNumberToObject(nodeobj_p,"accelerationX_low_alarm",inpoutpFlag.modbusThreAxis[i].accXLowFlag);
							cJSON_AddNumberToObject(nodeobj_p,"accelerationX_high_alarm",inpoutpFlag.modbusThreAxis[i].accXUpFlag);		
							cJSON_AddNumberToObject(nodeobj_p,"accelerationY_low_alarm",inpoutpFlag.modbusThreAxis[i].accYLowFlag);
							cJSON_AddNumberToObject(nodeobj_p,"accelerationY_high_alarm",inpoutpFlag.modbusThreAxis[i].accYUpFlag);
							cJSON_AddNumberToObject(nodeobj_p,"accelerationZ_low_alarm",inpoutpFlag.modbusThreAxis[i].accZLowFlag);
							cJSON_AddNumberToObject(nodeobj_p,"accelerationZ_high_alarm",inpoutpFlag.modbusThreAxis[i].accZUpFlag);			
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





//三轴读取modbus数据并打包发送 给其它函数调用
void threeAxisRead2Send(bool respFlag)
{					
		int workFlag=RT_FALSE;
		for(int i=0;i<THREEAXIS_485_NUM;i++){
				if(sheet.threeAxiss[i].workFlag==RT_TRUE){
							readThreeTempAcc(i);
							workFlag=RT_TRUE;
					}
			}
			if(workFlag==RT_TRUE){
					rt_kprintf("%s打包采集的THREEAXIS数据\r\n",sign);
					threeAxisJsonPack(respFlag);//circulaJsonPack();
					packMqttSend();
					rt_thread_mdelay(500);
					if(modThreeAxisWarn2Send()==true){
							resetThreeAxisWarnFlag();//每次判断后复位warnflag状态值

							packMqttSend();
					}
			}
}
