#include <board.h>




const static char sign[]="[threshold]";



//必须启用情况下才能得到阈值
void printfThresholdList()
{
	  int i;
		for(int j=0;j<ANALOG_NUM;j++){//查一遍 找到 温湿度的模拟阈值
			  if(sheet.analog[j].workFlag==RT_TRUE){
						if(rt_strcmp(sheet.analog[j].name,analogName[0])==0){
								rt_kprintf("%s threshold ",sign);
								rt_kprintf("%s ",analogName[0]);
								rt_kprintf("%s ",sheet.analog[j].ID);
								rt_kprintf("1 ");
								rt_kprintf("%d ",sheet.analog[j].subName);
							  if(sheet.analog[j].subName==1){//温度
										rt_kprintf("%f ",sheet.analogTempHum.tempUpLimit);
										rt_kprintf("%f /n",sheet.analogTempHum.tempLowLimit);
								}
								else if(sheet.analog[j].subName==2){//湿度
										rt_kprintf("%f ",sheet.analogTempHum.humUpLimit);
										rt_kprintf("%f /n",sheet.analogTempHum.humLowLimit);

								}
						}
				}
		}
		for(i=0;i<CIRCULA_485_NUM;i++){
						if(sheet.cirCula[i].workFlag==RT_TRUE){//找到工作的  环流需要连续打印三次
								rt_kprintf("%s threshold ",sign);
								rt_kprintf("%s ",modbusName[i]);
								rt_kprintf("%s ",sheet.cirCula[i].ID);
								rt_kprintf("2 ");
								rt_kprintf("1 ");
								rt_kprintf("%f ",sheet.modbusCircul[i].cirCurAUpLimit);
								rt_kprintf("%f \n",sheet.modbusCircul[i].cirCurALowLimit);
			
								rt_kprintf("%s threshold ",sign);
								rt_kprintf("%s ",modbusName[i]);
								rt_kprintf("%s ",sheet.cirCula[i].ID);
								rt_kprintf("2 ");
								rt_kprintf("2 ");
								rt_kprintf("%f ",sheet.modbusCircul[i].cirCurBUpLimit);
								rt_kprintf("%f \n",sheet.modbusCircul[i].cirCurBLowLimit);
							
								rt_kprintf("%s threshold ",sign);
								rt_kprintf("%s ",modbusName[i]);
								rt_kprintf("%s ",sheet.cirCula[i].ID);
								rt_kprintf("2 ");
								rt_kprintf("3 ");
								rt_kprintf("%f ",sheet.modbusCircul[i].cirCurCUpLimit);
								rt_kprintf("%f \n",sheet.modbusCircul[i].cirCurCLowLimit);
						}
				}
		//打印modbus设置的阈值
}
//与modbus传感器不同之处在于 除了判断ID还需要判断子选项
bool   analogThresholdConfig(int num,char *ID,int sensorType,float upLimit,float lowLimit)
{ 
		switch(num)
		{
			case  0://modbus 温湿度
				for(int i=0;i<ANALOG_NUM;i++){//
						if(rt_strcmp(sheet.analog[i].ID,ID)==0){//先核对ID
								if(sheet.analog[i].subName==sensorType){
									 if(sensorType==1){//温度
											sheet.analogTempHum.humLowLimit=lowLimit;
											sheet.analogTempHum.humUpLimit=upLimit;
											return true;
									 }
									 else if(sensorType==2){//湿度
											sheet.analogTempHum.tempLowLimit=lowLimit;
											sheet.analogTempHum.tempUpLimit =upLimit;	
											return true;
									 }
								}
						}
				}
			break;
		}
		return false;
}
//modbus设备  输入依次为 传感器种类 ID号 传感器类型  上限制  下限制
bool   modbusThresholdConfig(int num,char *ID,int sensorType,float upLimit,float lowLimit)
{
	  int i=0;
		switch(num)
		{
			case CIRCULA:
				for(i=0;i<CIRCULA_485_NUM;i++){
						if(rt_strcmp(sheet.cirCula[i].ID,ID)==0){//找到相同ID的
								if(sensorType==1){
										sheet.modbusCircul[i].cirCurAUpLimit  = upLimit;
									  sheet.modbusCircul[i].cirCurALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==2){
										sheet.modbusCircul[i].cirCurBUpLimit  = upLimit;
										sheet.modbusCircul[i].cirCurBLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==3){
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
								if(sensorType==1){
										sheet.modbusPartDisChg[i].amplitudeAUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].amplitudeALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==2){
										sheet.modbusPartDisChg[i].freqAUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].freqALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==3){
										sheet.modbusPartDisChg[i].dischargeAUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].dischargeALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==4){
										sheet.modbusPartDisChg[i].amplitudeBUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].amplitudeBLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==5){
										sheet.modbusPartDisChg[i].freqBUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].freqBLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==6){
										sheet.modbusPartDisChg[i].dischargeBUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].dischargeBLowLimit = lowLimit;
									  return  true;
								}
								if(sensorType==7){
										sheet.modbusPartDisChg[i].amplitudeCUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].amplitudeCLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==8){
										sheet.modbusPartDisChg[i].freqCUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].freqCLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==9){
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
								if(sensorType==1){
										sheet.modbusPreSettl[i].tempUpLimit  = upLimit;
									  sheet.modbusPreSettl[i].tempLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==2){
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
								if(sensorType==1){
										sheet.modbusThreAxis[i].tempUpLimit  = upLimit;
									  sheet.modbusThreAxis[i].tempLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==2){
										sheet.modbusThreAxis[i].accXUpLimit  = upLimit;
										sheet.modbusThreAxis[i].accXLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==3){
										sheet.modbusThreAxis[i].accYUpLimit  = upLimit;
										sheet.modbusThreAxis[i].accYLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==4){
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
								if(sensorType==1){
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
								if(sensorType==1){
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
								if(sensorType==1){
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
								if(sensorType==1){
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
								if(sensorType==1){
										sheet.modbusTempHum[i].tempUpLimit  = upLimit;
									  sheet.modbusTempHum[i].tempLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==2){
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
								if(sensorType==1){
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

static void threshold(int argc,char *argv[])
{
	  if(0==rt_strcmp((char *)"list", argv[1])){
				printfThresholdList();
				return;
		}
		if(argc!=7){
				goto ERR;
		}
		int sensorType= atoi32(argv[3],10);
		int sensorSubName=atoi32(argv[4],10);
		if((sensorType!=1)||(sensorType!=2)){
				rt_kprintf("%s sensorType argv[3] should be 1 or 2\n",sign);
			  goto ERR;
		}
		if(sensorSubName>100){
				rt_kprintf("%s sensorSubName argv[4] should be <100\n",sign);
			  goto ERR;
		}
		float uplimit  = atof(argv[5]);
		float lowlimit = atof(argv[6]);
		if(sensorType==1){//modbus传感器
			  int i=0;
			  for(i=0;i<MODBUS_NUM;i++){
						if(rt_strcmp(argv[1],modbusName[i])==0){
								if(true==modbusThresholdConfig(i,argv[2],sensorType,uplimit,lowlimit)){
									  rt_kprintf("%s threshold mb OK\n",sign);
										return;
								}
								else{
										goto ERR;
								}
						}
				}
		}
		if(sensorType==2){//analog传感器
			  int i=0;
			  for(i=0;i<ANALOGNAME_NUM;i++){
						if(rt_strcmp(argv[1],modbusName[i])==0){
								if(true==analogThresholdConfig(i,argv[2],sensorType,uplimit,lowlimit)){
										rt_kprintf("%s threshold ana OK\n",sign);
										return;
								}
								else{
										goto ERR;
								}
						}
				}	
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[threshold 温湿度 GYNJLXSD000000164 1 1 10 80]\n",sign);
		rt_kprintf("%s[阈值上下限为0时候 不启用阈值]\n",sign);

}
MSH_CMD_EXPORT(threshold,threshold config);