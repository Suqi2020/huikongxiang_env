#include <board.h>
//阈值设置参考 《汇控箱modbus串口配置V0.4.doc》中 8、阈值配置（主要对modbus和模拟传感器进行配置）



const static char sign[]="[threshold]";



//必须启用情况下才能得到阈值  即workFlag为true
void printfThresholdList()
{
	  int i;
	#ifndef     ANA_MASK
		for(int j=0;j<ANALOG_NUM;j++){//查一遍 找到 温湿度的模拟阈值
			  if(sheet.analog[j].workFlag==RT_TRUE){
						//if(rt_strcmp(sheet.analog[j].name,analogName[i])==0){
								rt_kprintf("%s threshold ",sign);
								rt_kprintf("%s ",sheet.analog[j].name);
								rt_kprintf("%s ",sheet.analog[j].ID);
								rt_kprintf("2 ");
								rt_kprintf("%s ",sheet.analog[j].subName);
							  if(sheet.analog[j].subName==1){//温度
										
										rt_kprintf("%0.2f ",sheet.analogTempHum.tempLowLimit);
									  rt_kprintf("%0.2f \n",sheet.analogTempHum.tempUpLimit);
								}
								else if(sheet.analog[j].subName==2){//湿度
										
										rt_kprintf("%0.2f ",sheet.analogTempHum.humLowLimit);
									  rt_kprintf("%0.2f \n",sheet.analogTempHum.humUpLimit);

								}
						}
				
		}
		#endif
		
		for(i=0;i<CIRCULA_485_NUM;i++){
				if(sheet.cirCula[i].workFlag==RT_TRUE){//找到工作的设备  环流需要连续打印三次
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[CIRCULA]);
						rt_kprintf("%s ",sheet.cirCula[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%0.2f ",sheet.modbusCircul[i].cirCurALowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusCircul[i].cirCurAUpLimit);
	
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[CIRCULA]);
						rt_kprintf("%s ",sheet.cirCula[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("2 ");
						rt_kprintf("%0.2f ",sheet.modbusCircul[i].cirCurBLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusCircul[i].cirCurBUpLimit);
					
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[CIRCULA]);
						rt_kprintf("%s ",sheet.cirCula[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("3 ");
						rt_kprintf("%0.2f ",sheet.modbusCircul[i].cirCurCLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusCircul[i].cirCurCUpLimit);
						rt_kprintf("\n");
				}
				
		}

		for(i=0;i<PARTDISCHAG_485_NUM;i++){
				if(sheet.partDischag[i].workFlag==RT_TRUE){//找到相同ID的
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PARTDISCHAG]);
						rt_kprintf("%s ",sheet.partDischag[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%u ",sheet.modbusPartDisChg[i].amplitudeALowLimit);
						rt_kprintf("%u \n",sheet.modbusPartDisChg[i].amplitudeAUpLimit);
					
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PARTDISCHAG]);
						rt_kprintf("%s ",sheet.partDischag[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("2 ");
						rt_kprintf("%u ",sheet.modbusPartDisChg[i].freqALowLimit);
						rt_kprintf("%u \n",sheet.modbusPartDisChg[i].freqAUpLimit);
					
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PARTDISCHAG]);
						rt_kprintf("%s ",sheet.partDischag[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("3 ");
						rt_kprintf("%u ",sheet.modbusPartDisChg[i].dischargeALowLimit);
						rt_kprintf("%u \n",sheet.modbusPartDisChg[i].dischargeAUpLimit);


						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PARTDISCHAG]);
						rt_kprintf("%s ",sheet.partDischag[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("4 ");
						rt_kprintf("%u ",sheet.modbusPartDisChg[i].amplitudeBLowLimit);
						rt_kprintf("%u \n",sheet.modbusPartDisChg[i].amplitudeBUpLimit);
						
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PARTDISCHAG]);
						rt_kprintf("%s ",sheet.partDischag[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("5 ");
						rt_kprintf("%u ",sheet.modbusPartDisChg[i].freqBLowLimit);
						rt_kprintf("%u \n",sheet.modbusPartDisChg[i].freqBUpLimit);
						
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PARTDISCHAG]);
						rt_kprintf("%s ",sheet.partDischag[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("6 ");
						rt_kprintf("%u ",sheet.modbusPartDisChg[i].dischargeBLowLimit);
						rt_kprintf("%u \n",sheet.modbusPartDisChg[i].dischargeBUpLimit);	


						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PARTDISCHAG]);
						rt_kprintf("%s ",sheet.partDischag[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("7 ");
						rt_kprintf("%u ",sheet.modbusPartDisChg[i].amplitudeCLowLimit);
						rt_kprintf("%u \n",sheet.modbusPartDisChg[i].amplitudeCUpLimit);	
						
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PARTDISCHAG]);
						rt_kprintf("%s ",sheet.partDischag[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("8 ");
						rt_kprintf("%u ",sheet.modbusPartDisChg[i].freqCLowLimit);
						rt_kprintf("%u \n",sheet.modbusPartDisChg[i].freqCUpLimit);	
						
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PARTDISCHAG]);
						rt_kprintf("%s ",sheet.partDischag[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("9 ");
						rt_kprintf("%u ",sheet.modbusPartDisChg[i].dischargeCLowLimit);
						rt_kprintf("%u \n",sheet.modbusPartDisChg[i].dischargeCUpLimit);
						rt_kprintf("\n");
				}
		}
		for(i=0;i<PRESSSETTL_485_NUM;i++){
				if(sheet.pressSetl[i].workFlag==RT_TRUE){//找到相同ID的
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PRESSSETTL]);
						rt_kprintf("%s ",sheet.pressSetl[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%0.2f ",sheet.modbusPreSettl[i].tempLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusPreSettl[i].tempUpLimit);
					
					
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[PRESSSETTL]);
						rt_kprintf("%s ",sheet.pressSetl[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("2 ");
						rt_kprintf("%0.2f ",sheet.modbusPreSettl[i].heightLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusPreSettl[i].heightUpLimit);
						rt_kprintf("\n");

				}
		}

		for(i=0;i<THREEAXIS_485_NUM;i++){
				if(sheet.threeAxiss[i].workFlag==RT_TRUE){//找到相同ID的
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[THREEAXIS]);
						rt_kprintf("%s ",sheet.threeAxiss[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%0.2f ",sheet.modbusThreAxis[i].tempLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusThreAxis[i].tempUpLimit);
		
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[THREEAXIS]);
						rt_kprintf("%s ",sheet.threeAxiss[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("2 ");
						rt_kprintf("%0.2f ",sheet.modbusThreAxis[i].accXLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusThreAxis[i].accXUpLimit);
	
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[THREEAXIS]);
						rt_kprintf("%s ",sheet.threeAxiss[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("3 ");
						rt_kprintf("%0.2f ",sheet.modbusThreAxis[i].accYLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusThreAxis[i].accYUpLimit);

						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[THREEAXIS]);
						rt_kprintf("%s ",sheet.threeAxiss[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("4 ");
						rt_kprintf("%0.2f ",sheet.modbusThreAxis[i].accZLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusThreAxis[i].accZUpLimit);
						rt_kprintf("\n");
				}

		}

#ifdef  USE_4GAS

		for(i=0;i<CH4_485_NUM;i++){
				if(sheet.ch4[i].workFlag==RT_TRUE){//找到相同ID的
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[CH4]);
						rt_kprintf("%s ",sheet.ch4[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%0.2f ",sheet.modbusCh4[i].ch4LowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusCh4[i].ch4UpLimit);
					  rt_kprintf("\n");
				}
		}
		for(i=0;i<O2_485_NUM;i++){
				if(sheet.ch4[i].workFlag==RT_TRUE){//找到相同ID的
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[O2]);
						rt_kprintf("%s ",sheet.o2[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%0.2f ",sheet.modbusO2[i].o2LowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusO2[i].o2UpLimit);
				
						rt_kprintf("\n");
				}
		}
		for(i=0;i<H2S_485_NUM;i++){
				if(sheet.ch4[i].workFlag==RT_TRUE){//找到相同ID的
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[H2S]);
						rt_kprintf("%s ",sheet.h2s[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%0.2f ",sheet.modbusH2s[i].h2sLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusH2s[i].h2sUpLimit);
				
				    rt_kprintf("\n");
				}
		}
		for(i=0;i<CO_485_NUM;i++){
				if(sheet.co[i].workFlag==RT_TRUE){//找到相同ID的
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[CO]);
						rt_kprintf("%s ",sheet.co[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%0.2f ",sheet.modbusCo[i].coLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusCo[i].coUpLimit);
					  rt_kprintf("\n");
				}
				
		}
#endif
		for(i=0;i<TEMPHUM_485_NUM;i++){
				if(sheet.tempHum[i].workFlag==RT_TRUE){//找到相同ID的
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[TEMPHUM]);
						rt_kprintf("%s ",sheet.tempHum[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%0.2f ",sheet.modbusTempHum[i].tempLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusTempHum[i].tempUpLimit);
					  rt_kprintf("\n");
				}
				//rt_kprintf("\n");
		}
		for(i=0;i<WATERDEPTH_485_NUM;i++){
				if(sheet.waterDepth[i].workFlag==RT_TRUE){//找到相同ID的
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[WATERDEPTH]);
						rt_kprintf("%s ",sheet.waterDepth[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%0.2f ",sheet.modbusWaterDepth[i].depthLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusWaterDepth[i].depthUpLimit);
					  rt_kprintf("\n");
				}
				//rt_kprintf("\n");
		}
		for(i=0;i<CRACKMETER_485_NUM;i++){
				if(sheet.crackMeter[i].workFlag==RT_TRUE){//找到相同ID的
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[CRACKMETER]);
						rt_kprintf("%s ",sheet.crackMeter[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("1 ");
						rt_kprintf("%0.2f ",sheet.modbusCrackMeter[i].tempLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusCrackMeter[i].tempUpLimit);
					  rt_kprintf("\n");
					
						rt_kprintf("%s threshold ",sign);
						rt_kprintf("%s ",modbusName[CRACKMETER]);
						rt_kprintf("%s ",sheet.crackMeter[i].ID);
						rt_kprintf("1 ");
						rt_kprintf("2 ");
						rt_kprintf("%0.2f ",sheet.modbusCrackMeter[i].distancLowLimit);
						rt_kprintf("%0.2f \n",sheet.modbusCrackMeter[i].distancUpLimit);
					  rt_kprintf("\n");
				}
				//rt_kprintf("\n");
		}
		//打印modbus设置的阈值
}
//与modbus传感器不同之处在于 除了判断ID还需要判断子选项
//模拟传感器阈值配置
#ifndef     ANA_MASK
bool  analogThresholdConfig(int num,char *ID,int sensorSubName,float upLimit,float lowLimit)
{ 
		switch(num)
		{
			case  0://modbus 温湿度
				for(int i=0;i<ANALOG_NUM;i++){//
						if(rt_strcmp(sheet.analog[i].ID,ID)==0){//先核对ID
								if(sheet.analog[i].subName==sensorSubName){
									 if(sensorSubName==1){//温度
											sheet.analogTempHum.tempLowLimit=lowLimit;
											sheet.analogTempHum.tempUpLimit=upLimit;
											return true;
									 }
									 else if(sensorSubName==2){//湿度
											sheet.analogTempHum.humLowLimit=lowLimit;
											sheet.analogTempHum.humUpLimit =upLimit;	
											return true;
									 }
								}
						}
				}
			break;
		}
		return false;
}
#endif
//modbus设备  输入依次为 传感器种类 ID号 传感器类型  上限制  下限制
//modbus传感器阈值配置
bool   modbusThresholdConfig(int num,char *ID,int sensorSubName,float upLimit,float lowLimit)
{
	  int i=0;
		switch(num)
		{
			case CIRCULA:
				for(i=0;i<CIRCULA_485_NUM;i++){
						if(rt_strcmp(sheet.cirCula[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusCircul[i].cirCurAUpLimit  = upLimit;
									  sheet.modbusCircul[i].cirCurALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==2){
										sheet.modbusCircul[i].cirCurBUpLimit  = upLimit;
										sheet.modbusCircul[i].cirCurBLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==3){
										sheet.modbusCircul[i].cirCurCUpLimit  = upLimit;
										sheet.modbusCircul[i].cirCurCLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
				break;
			case PARTDISCHAG:
				for(i=0;i<PARTDISCHAG_485_NUM;i++){
						if(rt_strcmp(sheet.partDischag[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusPartDisChg[i].amplitudeAUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].amplitudeALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==2){
										sheet.modbusPartDisChg[i].freqAUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].freqALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==3){
										sheet.modbusPartDisChg[i].dischargeAUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].dischargeALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==4){
										sheet.modbusPartDisChg[i].amplitudeBUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].amplitudeBLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==5){
										sheet.modbusPartDisChg[i].freqBUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].freqBLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==6){
										sheet.modbusPartDisChg[i].dischargeBUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].dischargeBLowLimit = lowLimit;
									  return  true;
								}
								if(sensorSubName==7){
										sheet.modbusPartDisChg[i].amplitudeCUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].amplitudeCLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==8){
										sheet.modbusPartDisChg[i].freqCUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].freqCLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==9){
										sheet.modbusPartDisChg[i].dischargeCUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].dischargeCLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
				break;
			case PRESSSETTL:
				for(i=0;i<PRESSSETTL_485_NUM;i++){
						if(rt_strcmp(sheet.pressSetl[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusPreSettl[i].tempUpLimit  = upLimit;
									  sheet.modbusPreSettl[i].tempLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==2){
										sheet.modbusPreSettl[i].heightUpLimit = upLimit;
										sheet.modbusPreSettl[i].heightLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
				break;
			case THREEAXIS:
				for(i=0;i<THREEAXIS_485_NUM;i++){
						if(rt_strcmp(sheet.threeAxiss[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusThreAxis[i].tempUpLimit  = upLimit;
									  sheet.modbusThreAxis[i].tempLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==2){
										sheet.modbusThreAxis[i].accXUpLimit  = upLimit;
										sheet.modbusThreAxis[i].accXLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==3){
										sheet.modbusThreAxis[i].accYUpLimit  = upLimit;
										sheet.modbusThreAxis[i].accYLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==4){
										sheet.modbusThreAxis[i].accZUpLimit  = upLimit;
										sheet.modbusThreAxis[i].accZLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
				break;
#ifdef  USE_4GAS
			case CH4:
				for(i=0;i<CH4_485_NUM;i++){
						if(rt_strcmp(sheet.ch4[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusCh4[i].ch4UpLimit  = upLimit;
									  sheet.modbusCh4[i].ch4LowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
			case O2:
				for(i=0;i<O2_485_NUM;i++){
						if(rt_strcmp(sheet.o2[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusO2[i].o2UpLimit  = upLimit;
									  sheet.modbusO2[i].o2LowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
			case H2S:
				for(i=0;i<H2S_485_NUM;i++){
						if(rt_strcmp(sheet.h2s[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusH2s[i].h2sUpLimit  = upLimit;
									  sheet.modbusH2s[i].h2sLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
			case CO:
				for(i=0;i<CO_485_NUM;i++){
						if(rt_strcmp(sheet.co[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusCo[i].coUpLimit  = upLimit;
									  sheet.modbusCo[i].coLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
#endif
			case TEMPHUM:
				for(i=0;i<TEMPHUM_485_NUM;i++){
						if(rt_strcmp(sheet.tempHum[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusTempHum[i].tempUpLimit  = upLimit;
									  sheet.modbusTempHum[i].tempLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==2){
										sheet.modbusTempHum[i].humUpLimit  = upLimit;
									  sheet.modbusTempHum[i].humLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
			case WATERDEPTH:
				for(i=0;i<WATERDEPTH_485_NUM;i++){
						if(rt_strcmp(sheet.waterDepth[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusWaterDepth[i].depthUpLimit  = upLimit;
									  sheet.modbusWaterDepth[i].depthLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
			default:
				
			case CRACKMETER:
				for(i=0;i<CRACKMETER_485_NUM;i++){
						if(rt_strcmp(sheet.crackMeter[i].ID,ID)==0){//找到相同ID的
								if(sensorSubName==1){
										sheet.modbusCrackMeter[i].tempUpLimit  = upLimit;
									  sheet.modbusCrackMeter[i].tempLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorSubName==2){
										sheet.modbusCrackMeter[i].distancUpLimit  = upLimit;
									  sheet.modbusCrackMeter[i].distancLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
				rt_kprintf("%s ERR:modbusThresholdConfig %d\n",sign,num);
				break;
		}
		return false;
}
/*
命令(threshold+名称+ID+类型(1-modbus 2-analog)+选项(1.2.3 只有一种时候设置为1)+阈值上限+阈值下限)(阈值为0或者0xffffffff时此阈值不启用)
threshold温湿度 GYNJLXSD000000164 1 1 10 80
阈值命令详解如下
threshold：固定头部
名称：modbus和模拟传感器名称
类型：(1-modbus 2-analog)  为0或255时候删除对应的阈值设置
选项：传感器功能只有一种情况下值为1，有多种情况下依次递增(1.2.3)如下
     模拟温湿度：1-温度  2-湿度
	   modbus温湿度：1-温度 2-湿度
     modbus防外破：1-温度 2-x方向加速度 3-y方向加速度 4-z方向加速度
     modbus沉降仪：1-温度 2-高度
     modbus环流： 1-cirCurA 2-cirCurB 3-cirCurC
     modbus局放：1-pdA 2-freqA 3-dischargeDataA            
				   4-pdB 5-freqB 6-dischargeDataB				
7-pdC 8-freqC 9-dischargeDataC 
阈值上限：0-99999999(为0阈值不启用)
阈值下限：0-99999999(为0阈值不启用)

*/
//传感器（模拟和485）的阈值配置
static void threshold(int argc,char *argv[])
{
		float uplimit ;
		float lowlimit ;
		int sensorType;
		int sensorSubName;
	  if(0==rt_strcmp((char *)"list", argv[1])){
				printfThresholdList();
				return;
		}
		if(argc!=7){
				goto ERR;
		}
		sensorType= atoi32(argv[3],10);
		sensorSubName=atoi32(argv[4],10);
		if((sensorType!=1)&&(sensorType!=2)){
				rt_kprintf("%ssensorType argv[3] should be 1 or 2\n",sign);
			  goto ERR;
		}
		if(sensorSubName>100){
				rt_kprintf("%ssensorSubName argv[4] should be <100\n",sign);
			  goto ERR;
		}
		uplimit  = atof(argv[6]);
		lowlimit = atof(argv[5]);
		if(sensorType==1){//modbus传感器
			  int i=0;
			  for(i=0;i<MODBUS_NUM;i++){
						if(rt_strcmp(argv[1],modbusName[i])==0){
								if(true==modbusThresholdConfig(i,argv[2],sensorSubName,uplimit,lowlimit)){
									  rt_kprintf("%sthreshold mb cfg OK\n",sign);
										return;
								}
								else{
										goto ERR;
								}
						}
				}
		}
#ifndef     ANA_MASK
		if(sensorType==2){//analog传感器
			  int i=0;
			  for(i=0;i<ANALOGNAME_NUM;i++){
						if(rt_strcmp(argv[1],analogName[i])==0){
								if(true==analogThresholdConfig(i,argv[2],sensorSubName,uplimit,lowlimit)){
										rt_kprintf("%sthreshold ana cfg OK\n",sign);
										return;
								}
								else{
										goto ERR;
								}
						}
				}	
		}
#endif
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[threshold 温湿度 GYNJLXSD000000164 1 1 10 80]\n",sign);
		rt_kprintf("%s[阈值上下限为0时候 不启用阈值]\n",sign);

}
MSH_CMD_EXPORT(threshold,threshold config);

