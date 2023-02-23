#include  "board.h"


//4-20ma 对应 adc是0.6-3V 对应温度是-40度--80度  湿度值是0-100
// 0-3.3对应0-4096
const char *sign="[analogTempH]";
//num 定时器启动时候找到的对应的0-7通道
//uint8_t analogTemChanl=0;

float temp =0.00;
float humi =0.00;

char copyID[MODBID_LEN];//读取ana温湿度时候 顺便把ID拷贝出来
//模拟的温湿度传感器读取温度湿度值并与阈值比较来做标记

void  anaTempHumiReadAndSetFlag()
{
	
	
//	adcGetTest();
//	
//	
//					int rt=Get_Adc_Average(ADC_CHANNEL_8,10);
//				rt_kprintf("ADC_CHANNEL_8 ,adc tick:%d,voltgge 扩大100倍:%dV\n",rt,rt*330/4096);
//				 rt=Get_Adc_Average(ADC_CHANNEL_9,10);
//		
	
	
		uint16_t  ADCtemp,ADChumi,porttest;
		float Vtemp,Vhumi;
		for (int i = 0; i < ANALOG_NUM; i++){		
			if(sheet.analog[i].workFlag==RT_TRUE){
				if(rt_strcmp(sheet.analog[i].name,analogName[0])==0){
						if(sheet.analog[i].subName==1){//温度  参考《汇控箱modbus串口配置V0.4》
							  porttest=sheet.analog[i].port-1;
								ADCtemp=Get_Adc_Average(chanl[sheet.analog[i].port-1],5);
								Vtemp=ADCtemp*33*10/4096; //扩大100倍
								temp=((Vtemp-60)*12/24)-40;////	temp=(10*Vtemp-6)*(80+40)/(30-6)-40;
								rt_kprintf("%stemp=%f\n",sign,temp);
								rt_strcpy(copyID,sheet.analog[i].ID);
								if(temp>=sheet.analogTempHum.tempUpLimit){
										inpoutpFlag.analogTempHum.tempUpFlag=true;
								}
								else
										inpoutpFlag.analogTempHum.tempUpFlag=false;
								if(temp<=sheet.analogTempHum.tempLowLimit){
										inpoutpFlag.analogTempHum.tempLowFlag=true;
								}
								else
										inpoutpFlag.analogTempHum.tempLowFlag=false;
						}
						else if(sheet.analog[i].subName==2){//湿度
							  porttest=sheet.analog[i].port-1;
								ADChumi=Get_Adc_Average(chanl[sheet.analog[i].port-1],5);
								Vhumi=ADChumi*33*10/4096;//扩大100倍
								humi=10*(Vhumi-60)/(30-6);
								rt_kprintf("%shumi=%f\n",sign,humi);
								rt_strcpy(copyID,sheet.analog[i].ID);
								if(humi>=sheet.analogTempHum.humUpLimit){
										inpoutpFlag.analogTempHum.humUpFlag=true;
								}
								else
										inpoutpFlag.analogTempHum.humUpFlag=false;
								if(humi<=sheet.analogTempHum.humLowLimit){
										inpoutpFlag.analogTempHum.humLowFlag=true;
								}
								else
										inpoutpFlag.analogTempHum.humLowFlag=true;
						}		
				}
//						 if(0==strcmp(sheet.analog[i].funName,analogName1Val[0])){//temperature
//								float temp=1.23;
//								sprintf(sprinBuf,"%02f",temp);
//								cJSON_AddItemToObject(nodeobj_p,analogName1Val[0],cJSON_CreateString(sprinBuf));
//						 }
//						 else if(0==strcmp(sheet.analog[i].funName,analogName1Val[1])){//humidity
//								float hum=1.23;
//								sprintf(sprinBuf,"%02f",hum );
//								cJSON_AddItemToObject(nodeobj_p,analogName1Val[1],cJSON_CreateString(sprinBuf));
//						 }
			}
		}
}
//模拟温度湿度json数据打包
uint16_t analogTempHumJsonPack()
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
		cJSON_AddStringToObject(root, "identifier","temperature_humidity");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);

				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(copyID));
				
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
			
			
				sprintf(sprinBuf,"%02f",temp);
				cJSON_AddItemToObject(nodeobj_p,"温度",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%02f",humi );
				cJSON_AddItemToObject(nodeobj_p,"湿度",cJSON_CreateString(sprinBuf));

				sprintf(sprinBuf,"%llu",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
		}
	
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);

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
		rt_kprintf("%s len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}



//模拟温度和湿度值读取以及打包成json格式
void anaTempHumReadPack()
{
		anaTempHumiReadAndSetFlag();
	  analogTempHumJsonPack();
}
