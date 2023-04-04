#include  "board.h"
//传感器阈值的配置
const static char sign[]="[THCfg]";


void senseTHGetJsonResp(cJSON   *Json,bool modbusFlag)
{
		char* out = NULL;
		//创建数组
		cJSON* arrayNew = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return;


		cJSON   *arrayGet=cJSON_GetObjectItem(Json,"params");
	  cJSON  *pkIdentf = cJSON_GetObjectItem(Json,"identifier");
		int arrayGet_size = cJSON_GetArraySize(arrayGet);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了

		
		
	  char *string =pkIdentf->valuestring;
	  cJSON_AddNumberToObject(root,"mid",respMid);

	  bool result=false;

	  cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		cJSON_AddStringToObject(root, "identifier",string);
		
		
		
		if(modbusFlag==true){

				cJSON_AddStringToObject(root,"packetType","PROPERTIES_485TH_GET_RESP");
				if(rt_strcmp(pkIdentf->valuestring,"partial_discharge_monitor")==0){
						
						arrayNew = cJSON_CreateArray();
						if(arrayNew==NULL) return;
						cJSON_AddItemToObject(root, "params", arrayNew);
						for(int i=0;i<arrayGet_size;i++)
						{
								nodeobj = cJSON_CreateObject();
								cJSON_AddItemToArray(arrayNew, nodeobj);
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<PARTDISCHAG_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.partDischag[j].ID,devID->valuestring)){
											  result=true;
												cJSON_AddStringToObject(nodeobj, "deviceId", devID->valuestring);
												nodeobj_p = cJSON_CreateObject();
												cJSON_AddItemToObject(nodeobj,"data",nodeobj_p);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].amplitudeALowLimit);
												cJSON_AddStringToObject(nodeobj_p,"pdA_low",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].amplitudeAUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"pdA_high",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].freqALowLimit);
												cJSON_AddStringToObject(nodeobj_p,"freqA_low",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].freqAUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"freqA_high",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].dischargeALowLimit);
												cJSON_AddStringToObject(nodeobj_p,"dischargeDateA_low",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].dischargeAUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"dischargeDateA_high",sprinBuf);
											
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].amplitudeBLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"pdB_low",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].amplitudeBUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"pdB_high",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].freqBLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"freqB_low",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].freqBUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"freqB_high",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].dischargeBLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"dischargeDateB_low",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].dischargeBUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"dischargeDateB_high",sprinBuf);

												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].amplitudeCLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"pdC_low",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].amplitudeCUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"pdC_high",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].freqCLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"freqC_low",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].freqCUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"freqC_high",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].dischargeCLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"dischargeDateC_low",sprinBuf);
												sprintf(sprinBuf,"%u",sheet.modbusPartDisChg[j].dischargeCUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"dischargeDateC_high",sprinBuf);
										}
								}
						}
				}

				else if(rt_strcmp(string,"grounding_current_monitor")==0){

						arrayNew = cJSON_CreateArray();
						if(arrayNew==NULL) return;
						cJSON_AddItemToObject(root, "params", arrayNew);
						for(int i=0;i<arrayGet_size;i++)
						{
								nodeobj = cJSON_CreateObject();
								cJSON_AddItemToArray(arrayNew, nodeobj);
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<CIRCULA_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.cirCula[j].ID,devID->valuestring)){
											  result=true;
												cJSON_AddStringToObject(nodeobj, "deviceId", devID->valuestring);
												nodeobj_p = cJSON_CreateObject();
												cJSON_AddItemToObject(nodeobj,"data",nodeobj_p);
											
												sprintf(sprinBuf,"%02f",sheet.modbusCircul[j].cirCurALowLimit );
												cJSON_AddStringToObject(nodeobj_p,"earthCurA_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusCircul[j].cirCurAUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"earthCurA_high",sprinBuf);
												cJSON_AddStringToObject(nodeobj_p,"runCurA_low","0.0");
												cJSON_AddStringToObject(nodeobj_p,"runCurA_high","0.0");
												cJSON_AddStringToObject(nodeobj_p,"loadRatioA_low","0.0");
												cJSON_AddStringToObject(nodeobj_p,"loadRatioA_high","0.0");
											
												sprintf(sprinBuf,"%02f",sheet.modbusCircul[j].cirCurBLowLimit );
												cJSON_AddStringToObject(nodeobj_p,"earthCurB_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusCircul[j].cirCurBUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"earthCurB_high",sprinBuf);
												cJSON_AddStringToObject(nodeobj_p,"runCurB_low","0.0");
												cJSON_AddStringToObject(nodeobj_p,"runCurB_high","0.0");
												cJSON_AddStringToObject(nodeobj_p,"loadRatioB_low","0.0");
												cJSON_AddStringToObject(nodeobj_p,"loadRatioB_high","0.0");

												sprintf(sprinBuf,"%02f",sheet.modbusCircul[j].cirCurCLowLimit );
												cJSON_AddStringToObject(nodeobj_p,"earthCurC_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusCircul[j].cirCurCUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"earthCurC_high",sprinBuf);
												cJSON_AddStringToObject(nodeobj_p,"runCurC_low","0.0");
												cJSON_AddStringToObject(nodeobj_p,"runCurC_high","0.0");
												cJSON_AddStringToObject(nodeobj_p,"loadRatioC_low","0.0");
												cJSON_AddStringToObject(nodeobj_p,"loadRatioC_high","0.0");
										}
								}
						}
				}
				else if(rt_strcmp(string,"water_level_monitor")==0){

						arrayNew = cJSON_CreateArray();
						if(arrayNew==NULL) return;
						cJSON_AddItemToObject(root, "params", arrayNew);
						for(int i=0;i<arrayGet_size;i++)
						{
								nodeobj = cJSON_CreateObject();
								cJSON_AddItemToArray(arrayNew, nodeobj);
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<WATERDEPTH_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.cirCula[j].ID,devID->valuestring)){
											  result=true;
												cJSON_AddStringToObject(nodeobj, "deviceId", devID->valuestring);
												nodeobj_p = cJSON_CreateObject();
												cJSON_AddItemToObject(nodeobj,"data",nodeobj_p);
											
												sprintf(sprinBuf,"%02f",sheet.modbusWaterDepth[j].depthLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"depth_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusWaterDepth[j].depthUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"depth_high",sprinBuf);
	
										}
								}
						}
				}
				else if(rt_strcmp(string,"crackmeter_monitor")==0){

						arrayNew = cJSON_CreateArray();
						if(arrayNew==NULL) return;
						cJSON_AddItemToObject(root, "params", arrayNew);
						for(int i=0;i<arrayGet_size;i++)
						{
								nodeobj = cJSON_CreateObject();
								cJSON_AddItemToArray(arrayNew, nodeobj);
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<CRACKMETER_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.crackMeter[j].ID,devID->valuestring)){
											  result=true;
												cJSON_AddStringToObject(nodeobj, "deviceId", devID->valuestring);
												nodeobj_p = cJSON_CreateObject();
												cJSON_AddItemToObject(nodeobj,"data",nodeobj_p);
											
												sprintf(sprinBuf,"%02f",sheet.modbusCrackMeter[j].tempLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"temperature_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusCrackMeter[j].tempUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"temperature_high",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusCrackMeter[j].distancLowLimit);
											
												cJSON_AddStringToObject(nodeobj_p,"distance_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusCrackMeter[j].distancUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"distance_high",sprinBuf);
										}
								}
						}
				}
				else if(rt_strcmp(string,"temperature_and_humidity_monitor")==0){
	
						arrayNew = cJSON_CreateArray();
						if(arrayNew==NULL) return;
						cJSON_AddItemToObject(root, "params", arrayNew);
						for(int i=0;i<arrayGet_size;i++)
						{
								nodeobj = cJSON_CreateObject();
								cJSON_AddItemToArray(arrayNew, nodeobj);
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<TEMPHUM_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.tempHum[j].ID,devID->valuestring)){
											  result=true;
												cJSON_AddStringToObject(nodeobj, "deviceId", devID->valuestring);
												nodeobj_p = cJSON_CreateObject();
												cJSON_AddItemToObject(nodeobj,"data",nodeobj_p);
											
												sprintf(sprinBuf,"%02f",sheet.modbusTempHum[j].tempLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"temperature_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusTempHum[j].tempUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"temperature_high",sprinBuf);
											
												sprintf(sprinBuf,"%02f",sheet.modbusTempHum[j].humLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"humidity_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusTempHum[j].humUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"humidity_high",sprinBuf);
	
										}
								}
						}
				}
				else if(rt_strcmp(string,"environment_mointor")==0){
			
						arrayNew = cJSON_CreateArray();
						if(arrayNew==NULL) return;
						cJSON_AddItemToObject(root, "params", arrayNew);
						for(int i=0;i<arrayGet_size;i++)
						{
								nodeobj = cJSON_CreateObject();
								cJSON_AddItemToArray(arrayNew, nodeobj);
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<CO_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.co[j].ID,devID->valuestring)){
											  result=true;
												cJSON_AddStringToObject(nodeobj, "deviceId", devID->valuestring);
												nodeobj_p = cJSON_CreateObject();
												cJSON_AddItemToObject(nodeobj,"data",nodeobj_p);
											
												sprintf(sprinBuf,"%02f",sheet.modbusO2[j].o2LowLimit);
												cJSON_AddStringToObject(nodeobj_p,"oxy_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusO2[j].o2UpLimit );
												cJSON_AddStringToObject(nodeobj_p,"oxy_high",sprinBuf);
											
												sprintf(sprinBuf,"%02f",sheet.modbusCo[j].coLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"monoxide_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusCo[j].coUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"monoxide_high",sprinBuf);
											
												sprintf(sprinBuf,"%02f",sheet.modbusH2s[j].h2sLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"hydrogenSulfide_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusH2s[j].h2sUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"hydrogenSulfide_high",sprinBuf);
											
												sprintf(sprinBuf,"%02f",sheet.modbusCh4[j].ch4LowLimit);
												cJSON_AddStringToObject(nodeobj_p,"methane_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusCh4[j].ch4UpLimit);
												cJSON_AddStringToObject(nodeobj_p,"methane_high",sprinBuf);
	
										}
								}
						}
				}
				else if(rt_strcmp(string,"vibration_monitor")==0){

					arrayNew = cJSON_CreateArray();
						if(arrayNew==NULL) return;
						cJSON_AddItemToObject(root, "params", arrayNew);
						for(int i=0;i<arrayGet_size;i++)
						{
								nodeobj = cJSON_CreateObject();
								cJSON_AddItemToArray(arrayNew, nodeobj);
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<THREEAXIS_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.threeAxiss[j].ID,devID->valuestring)){
											  result=true;
												cJSON_AddStringToObject(nodeobj, "deviceId", devID->valuestring);
												nodeobj_p = cJSON_CreateObject();
												cJSON_AddItemToObject(nodeobj,"data",nodeobj_p);
											
												sprintf(sprinBuf,"%02f",sheet.modbusThreAxis[j].tempLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"temperature_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusThreAxis[j].tempUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"temperature_high",sprinBuf);
											
												sprintf(sprinBuf,"%d",sheet.modbusThreAxis[j].accXLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"accelerationX_low",sprinBuf);
												sprintf(sprinBuf,"%d",sheet.modbusThreAxis[j].accXUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"accelerationX_high",sprinBuf);
											
												sprintf(sprinBuf,"%d",sheet.modbusThreAxis[j].accYLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"accelerationY_low",sprinBuf);
												sprintf(sprinBuf,"%d",sheet.modbusThreAxis[j].accYUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"accelerationY_high",sprinBuf);

												sprintf(sprinBuf,"%d",sheet.modbusThreAxis[j].accZLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"accelerationZ_low",sprinBuf);
												sprintf(sprinBuf,"%d",sheet.modbusThreAxis[j].accZUpLimit);
												cJSON_AddStringToObject(nodeobj_p,"accelerationZ_high",sprinBuf);
	
										}
								}
						}
				}
				else if(rt_strcmp(string,"settlement_monitor")==0){
				
						arrayNew = cJSON_CreateArray();
						if(arrayNew==NULL) return;
						cJSON_AddItemToObject(root, "params", arrayNew);
						for(int i=0;i<arrayGet_size;i++)
						{
								nodeobj = cJSON_CreateObject();
								cJSON_AddItemToArray(arrayNew, nodeobj);
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<PRESSSETTL_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.pressSetl[j].ID,devID->valuestring)){
											  result=true;
												cJSON_AddStringToObject(nodeobj, "deviceId", devID->valuestring);
												nodeobj_p = cJSON_CreateObject();
												cJSON_AddItemToObject(nodeobj,"data",nodeobj_p);
											
												sprintf(sprinBuf,"%02f",sheet.modbusPreSettl[j].tempLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"temperature_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusPreSettl[j].tempUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"temperature_high",sprinBuf);
											
												sprintf(sprinBuf,"%02f",sheet.modbusPreSettl[j].heightLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"height_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.modbusPreSettl[j].heightUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"height_high",sprinBuf);
	
										}
								}
						}
				}
		}
		else{
			#ifndef     ANA_MASK
				cJSON_AddStringToObject(root,"packetType","PROPERTIES_ANATH_GET_RESP");
				if(rt_strcmp(string,"temperature_and_humidity_monitor")==0){
						arrayNew = cJSON_CreateArray();
						if(arrayNew==NULL) return;
						cJSON_AddItemToObject(root, "params", arrayNew);
						for(int i=0;i<arrayGet_size;i++)
						{
								nodeobj = cJSON_CreateObject();
								cJSON_AddItemToArray(arrayNew, nodeobj);
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<ANALOG_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.analog[j].ID,devID->valuestring)){
											  result=true;
												cJSON_AddStringToObject(nodeobj, "deviceId", devID->valuestring);
												nodeobj_p = cJSON_CreateObject();
												cJSON_AddItemToObject(nodeobj,"data",nodeobj_p);
											
												sprintf(sprinBuf,"%02f",sheet.analogTempHum.tempLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"temperature_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.analogTempHum.tempUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"temperature_high",sprinBuf);
											
												sprintf(sprinBuf,"%02f",sheet.analogTempHum.humLowLimit);
												cJSON_AddStringToObject(nodeobj_p,"humidity_low",sprinBuf);
												sprintf(sprinBuf,"%02f",sheet.analogTempHum.humUpLimit );
												cJSON_AddStringToObject(nodeobj_p,"humidity_high",sprinBuf);
											 break;
										}
							 }
						}
				}
				#endif
		}
		if(result==true){
			
			cJSON_AddNumberToObject(root, "code",0);

		}
		else{
			cJSON_AddNumberToObject(root, "code",1);
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
		rt_free(sprinBuf);
		sprinBuf=RT_NULL;
}

//设置阈值
void senseTHSetJsonResp(cJSON   *Json,bool  modbusFlag)
{
		char* out = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return;
		cJSON   *arrayGet=cJSON_GetObjectItem(Json,"params");
	  cJSON  *pkIdentf = cJSON_GetObjectItem(Json,"identifier");
		int arrayGet_size = cJSON_GetArraySize(arrayGet);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
	  char *string =pkIdentf->valuestring;
	  cJSON_AddNumberToObject(root,"mid",respMid);
	  bool result=false;
	  cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		cJSON_AddStringToObject(root, "identifier",string);
		if(modbusFlag==true){
				cJSON_AddStringToObject(root,"packetType","PROPERTIES_485TH_SET_RESP");
				if(rt_strcmp(pkIdentf->valuestring,"partial_discharge_monitor")==0){
						for(int i=0;i<arrayGet_size;i++)
						{
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<PARTDISCHAG_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.partDischag[j].ID,devID->valuestring)){
											  result=true;
											  cJSON  *data =cJSON_GetObjectItem(item,"data");
											  cJSON  *fvalue= cJSON_GetObjectItem(data,"pdA_low");
											  sheet.modbusPartDisChg[j].amplitudeALowLimit =atol(fvalue->valuestring);
										  	fvalue= cJSON_GetObjectItem(data,"pdA_high");
											  sheet.modbusPartDisChg[j].amplitudeAUpLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"freqA_low");
											  sheet.modbusPartDisChg[j].freqALowLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"freqA_high");
											  sheet.modbusPartDisChg[j].freqAUpLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"dischargeDateA_low");
											  sheet.modbusPartDisChg[j].dischargeALowLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"dischargeDateA_high");
											  sheet.modbusPartDisChg[j].dischargeAUpLimit =atol(fvalue->valuestring);
											
											  fvalue= cJSON_GetObjectItem(data,"pdB_low");
											  sheet.modbusPartDisChg[j].amplitudeBLowLimit =atol(fvalue->valuestring);
										  	fvalue= cJSON_GetObjectItem(data,"pdB_high");
											  sheet.modbusPartDisChg[j].amplitudeBUpLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"freqB_low");
											  sheet.modbusPartDisChg[j].freqBLowLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"freqB_high");
											  sheet.modbusPartDisChg[j].freqBUpLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"dischargeDateB_low");
											  sheet.modbusPartDisChg[j].dischargeBLowLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"dischargeDateB_high");
											  sheet.modbusPartDisChg[j].dischargeBUpLimit =atol(fvalue->valuestring);
							
										    fvalue= cJSON_GetObjectItem(data,"pdC_low");
											  sheet.modbusPartDisChg[j].amplitudeCLowLimit =atol(fvalue->valuestring);
										  	fvalue= cJSON_GetObjectItem(data,"pdC_high");
											  sheet.modbusPartDisChg[j].amplitudeCUpLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"freqC_low");
											  sheet.modbusPartDisChg[j].freqCLowLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"freqC_high");
											  sheet.modbusPartDisChg[j].freqCUpLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"dischargeDateC_low");
											  sheet.modbusPartDisChg[j].dischargeCLowLimit =atol(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"dischargeDateC_high");
											  sheet.modbusPartDisChg[j].dischargeCUpLimit =atol(fvalue->valuestring);
										}
								}
						}
				}
				else if(rt_strcmp(string,"grounding_current_monitor")==0){
						for(int i=0;i<arrayGet_size;i++)
						{
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");

								for(int j=0;j<CIRCULA_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.cirCula[j].ID,devID->valuestring)){
											  result=true;
											  cJSON  *data =cJSON_GetObjectItem(item,"data");
											  cJSON  *fvalue= cJSON_GetObjectItem(data,"earthCurA_low");
											  sheet.modbusCircul[j].cirCurALowLimit=atof(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"earthCurA_high");
											  sheet.modbusCircul[j].cirCurAUpLimit=atof(fvalue->valuestring);
											
//												fvalue= cJSON_GetObjectItem(data,"runCurA_low");
//											  sheet.modbusCircul[j].cirCurAUpLimit=atof(fvalue->valuestring);
//												fvalue= cJSON_GetObjectItem(data,"runCurA_high");
//											  sheet.modbusCircul[j].cirCurAUpLimit=atof(fvalue->valuestring);
//												fvalue= cJSON_GetObjectItem(data,"loadRatioA_low");
//											  sheet.modbusCircul[j].cirCurAUpLimit=atof(fvalue->valuestring);
//												fvalue= cJSON_GetObjectItem(data,"loadRatioA_high");
//											  sheet.modbusCircul[j].cirCurAUpLimit=atof(fvalue->valuestring);		
											
												fvalue= cJSON_GetObjectItem(data,"earthCurB_low");
											  sheet.modbusCircul[j].cirCurBLowLimit=atof(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"earthCurB_high");
											  sheet.modbusCircul[j].cirCurBUpLimit=atof(fvalue->valuestring);
											
												fvalue= cJSON_GetObjectItem(data,"earthCurC_low");
											  sheet.modbusCircul[j].cirCurCLowLimit=atof(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"earthCurC_high");
											  sheet.modbusCircul[j].cirCurCUpLimit=atof(fvalue->valuestring);
										}
								}
						}
				}
				else if(rt_strcmp(string,"water_level_monitor")==0){
						for(int i=0;i<arrayGet_size;i++)
						{
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<WATERDEPTH_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.waterDepth[j].ID,devID->valuestring)){
											  result=true;
												cJSON  *data =cJSON_GetObjectItem(item,"data");
											  cJSON  *fvalue= cJSON_GetObjectItem(data,"depth_low");
											  sheet.modbusWaterDepth[j].depthLowLimit=atof(fvalue->valuestring);

											  fvalue= cJSON_GetObjectItem(data,"depth_high");
											  sheet.modbusWaterDepth[j].depthUpLimit=atof(fvalue->valuestring);
										}
								}
						}
				}
				else if(rt_strcmp(string,"crackmeter_monitor")==0){
						for(int i=0;i<arrayGet_size;i++)
						{
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<CRACKMETER_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.crackMeter[j].ID,devID->valuestring)){
											  result=true;
												cJSON  *data =cJSON_GetObjectItem(item,"data");
											  cJSON  *fvalue= cJSON_GetObjectItem(data,"temperature_low");
											  sheet.modbusCrackMeter[j].tempLowLimit=atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"temperature_high");
											  sheet.modbusCrackMeter[j].tempUpLimit=atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"distance_low");
											  sheet.modbusCrackMeter[j].distancLowLimit=atof(fvalue->valuestring);							
											  fvalue= cJSON_GetObjectItem(data,"distance_high");
											  sheet.modbusCrackMeter[j].distancUpLimit=atof(fvalue->valuestring);
										}
								}
						}
				}
				else if(rt_strcmp(string,"temperature_and_humidity_monitor")==0){
						for(int i=0;i<arrayGet_size;i++)
						{
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<TEMPHUM_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.tempHum[j].ID,devID->valuestring)){
											  result=true;
												cJSON  *data =cJSON_GetObjectItem(item,"data");
											  cJSON  *fvalue= cJSON_GetObjectItem(data,"temperature_low");
											  sheet.modbusTempHum[j].tempLowLimit=atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"temperature_high");
											  sheet.modbusTempHum[j].tempUpLimit=atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"humidity_low");
											  sheet.modbusTempHum[j].humLowLimit=atof(fvalue->valuestring);							
											  fvalue= cJSON_GetObjectItem(data,"humidity_high");
											  sheet.modbusTempHum[j].humUpLimit=atof(fvalue->valuestring);
										}
								}
						}
				}
				else if(rt_strcmp(string,"environment_mointor")==0){
						for(int i=0;i<arrayGet_size;i++)
						{
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<CO_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.co[j].ID,devID->valuestring)){
											  result=true;
												cJSON  *data =cJSON_GetObjectItem(item,"data");
											  cJSON  *fvalue= cJSON_GetObjectItem(data,"oxy_low");
											  sheet.modbusO2[j].o2LowLimit =atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"oxy_high");
											  sheet.modbusO2[j].o2UpLimit =atof(fvalue->valuestring);
											
												fvalue= cJSON_GetObjectItem(data,"monoxide_low");
											  sheet.modbusCo[j].coLowLimit =atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"monoxide_high");
											  sheet.modbusCo[j].coUpLimit =atof(fvalue->valuestring);
											
										  	fvalue= cJSON_GetObjectItem(data,"hydrogenSulfide_low");
											  sheet.modbusH2s[j].h2sLowLimit =atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"hydrogenSulfide_high");
											  sheet.modbusH2s[j].h2sUpLimit =atof(fvalue->valuestring);
											
											  fvalue= cJSON_GetObjectItem(data,"methane_low");
											  sheet.modbusCh4[j].ch4LowLimit =atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"methane_high");
											  sheet.modbusCh4[j].ch4UpLimit =atof(fvalue->valuestring);
										
	
										}
								}
						}
				}
				else if(rt_strcmp(string,"vibration_monitor")==0){
						for(int i=0;i<arrayGet_size;i++)
						{
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<THREEAXIS_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.threeAxiss[j].ID,devID->valuestring)){
											  result=true;
												cJSON  *data =cJSON_GetObjectItem(item,"data");
											  cJSON  *fvalue= cJSON_GetObjectItem(data,"temperature_low");
											  sheet.modbusThreAxis[j].tempLowLimit=atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"temperature_high");
											  sheet.modbusThreAxis[j].tempUpLimit=atof(fvalue->valuestring);
											
												fvalue= cJSON_GetObjectItem(data,"accelerationX_low");
												sheet.modbusThreAxis[j].accXLowLimit=atof(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"accelerationX_high");
												sheet.modbusThreAxis[j].accXUpLimit=atof(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"accelerationY_low");
												sheet.modbusThreAxis[j].accYLowLimit=atof(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"accelerationY_high");
												sheet.modbusThreAxis[j].accYUpLimit=atof(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"accelerationZ_low");
												sheet.modbusThreAxis[j].accZLowLimit=atof(fvalue->valuestring);
												fvalue= cJSON_GetObjectItem(data,"accelerationZ_high");
												sheet.modbusThreAxis[j].accZUpLimit=atof(fvalue->valuestring);
	
										}
								}
						}
				}
				else if(rt_strcmp(string,"settlement_monitor")==0){
						for(int i=0;i<arrayGet_size;i++)
						{
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<PRESSSETTL_485_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.pressSetl[j].ID,devID->valuestring)){
											  result=true;
												cJSON  *data =cJSON_GetObjectItem(item,"data");
											  cJSON  *fvalue= cJSON_GetObjectItem(data,"temperature_low");
												sheet.modbusPreSettl[j].tempLowLimit=atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"temperature_high");
											  sheet.modbusPreSettl[j].tempUpLimit=atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"height_low");
											  sheet.modbusPreSettl[j].heightLowLimit=atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"height_high");
											  sheet.modbusPreSettl[j].heightUpLimit=atof(fvalue->valuestring);
	
										}
								}
						}
				}
		}
		else{
			#ifndef     ANA_MASK
				cJSON_AddStringToObject(root,"packetType","PROPERTIES_ANATH_SET_RESP");
				if(rt_strcmp(string,"temperature_and_humidity_monitor")==0){
						for(int i=0;i<arrayGet_size;i++)
						{
								cJSON *item=cJSON_GetArrayItem(arrayGet,i);
								cJSON  *devID =cJSON_GetObjectItem(item,"deviceId");
								for(int j=0;j<ANALOG_NUM;j++){//核对有没有配置过
										if(0==rt_strcmp(sheet.analog[j].ID,devID->valuestring)){
											  result=true;
												cJSON  *data =cJSON_GetObjectItem(item,"data");
											  cJSON  *fvalue= cJSON_GetObjectItem(data,"temperature_low");
											  sheet.analogTempHum.tempLowLimit=atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"temperature_high");
											  sheet.analogTempHum.tempUpLimit =atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"humidity_low");
											  sheet.analogTempHum.humLowLimit =atof(fvalue->valuestring);
											  fvalue= cJSON_GetObjectItem(data,"humidity_high");
											  sheet.analogTempHum.humUpLimit  =atof(fvalue->valuestring);

											 break;
										}
							 }
						}
				}
				#endif
		}
		if(result==true){
			
			cJSON_AddNumberToObject(root, "code",0);

		}
		else{
			cJSON_AddNumberToObject(root, "code",1);
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
		rt_free(sprinBuf);
		sprinBuf=RT_NULL;	
}


