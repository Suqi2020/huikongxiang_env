#include  "board.h"
#include  "cJSON.h"
const static char sign[]="[dataPack]";
//heartUpStru  heartUp;
extern uint16_t RTU_CRC(uint8_t *puchMsg ,uint16_t usDataLen);
//uint8_t   NetTxBuffer[TX_RX_MAX_BUF_SIZE];  //与net发送buff大小一致  通过邮箱传递给NetTxBuffer 进行发送出去
mcuParazStru mcu ={0};

//rs485ParaStru devi[MODBUS_NUM];//目前4路485设备
//上行messageID自增  每次打包后自增1
uint32_t upMessIdAdd()
{
	  rt_kprintf("%smessID:%d\r\n",sign,mcu.upMessID);
		return mcu.upMessID++;
}


uint64_t subTimeStamp=0;

//获取到服务器时间戳差值
uint64_t subTimeStampGet()
{
		return subTimeStamp;
}
//存储服务器的时间戳差值  
 void  subTimeStampSet(uint64_t time)
{
	  if(time>=rt_tick_get())
				subTimeStamp=time-rt_tick_get();//服务器rtc值-当前tick值
		else
				subTimeStamp = 0;

}
//获取当前的utc时间8个字节长度
uint64_t  utcTime()
{
	return (uint64_t)rt_tick_get()+(uint64_t)subTimeStampGet();
}





//上行数据打包
uint16_t heartUpJsonPack()
{
		char* out = NULL;
		//创建数组
//		cJSON* Array = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
//		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return 0;
		// 加入节点（键值对）
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_HEARTBEAT");
	  cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		nodeobj = cJSON_CreateObject();
	  cJSON_AddStringToObject(nodeobj, "identifier","area_control_unit");

		//cJSON_AddItemToObject(nodeobj,"acuId",cJSON_CreateString((char *)"100000000000001"));
		cJSON_AddItemToObject(root, "params", nodeobj);

		
		// 释放内存  
		out = cJSON_Print(root);
		NetTxBuffer[0]=0xff;
		NetTxBuffer[1]=0xff;
		NetTxBuffer[2]=0xff;
		NetTxBuffer[3]=0xff;
		rt_strcpy((char *)NetTxBuffer+PACK_HEAD_LEN,out);
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

		mcu.upHeartMessID =mcu.upMessID;
		//mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return 1;
}



//设备注册json格式打包
uint16_t devRegJsonPack()
{
	char* out = NULL;
	//创建数组
	cJSON* Array = NULL;
	// 创建JSON Object  
	cJSON* root = NULL;
	cJSON* nodeobj = NULL;
	root = cJSON_CreateObject();
	if (root == NULL) return 0;
	// 加入节点（键值对）
	cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
	cJSON_AddStringToObject(root, "packetType","CMD_DEVICE_REGISTER");
	cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
	char *sprinBuf=RT_NULL;
	sprinBuf=rt_malloc(20);//20个字符串长度 够用了
	{
			Array = cJSON_CreateArray();
			if (Array == NULL) return 0;
			cJSON_AddItemToObject(root, "params", Array);
////////////////////////modbus格式设备注册打包////////////////////////		
			for(int i=0;i<MODBUS_NUM;i++){
			switch(i)
			{
				case CIRCULA:
					for(int j=0;j<CIRCULA_485_NUM;j++){//核对有没有配置过
							if(sheet.cirCula[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.cirCula[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.cirCula[j].ID));
							}
					}
				break;
				case PARTDISCHAG:
					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//核对有没有配置过
							if(sheet.partDischag[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.partDischag[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.partDischag[j].ID));
							}
					}
				break;
				case PRESSSETTL:
					for(int j=0;j<PRESSSETTL_485_NUM;j++){//核对有没有配置过
							if(sheet.pressSetl[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.pressSetl[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.pressSetl[j].ID));
							}
					}
				break;
				case THREEAXIS:
					for(int j=0;j<THREEAXIS_485_NUM;j++){//核对有没有配置过
							if(sheet.threeAxiss[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.threeAxiss[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.threeAxiss[j].ID));
							}
					}
				break;
#ifdef USE_4GAS
				case CH4:
					for(int j=0;j<CH4_485_NUM;j++){//核对有没有配置过
							if(sheet.ch4[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.ch4[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.ch4[j].ID));
							}
					}
				break;
				case O2:
					for(int j=0;j<O2_485_NUM;j++){//核对有没有配置过
							if(sheet.o2[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.o2[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.o2[j].ID));
							}
					}
				break;
				case H2S:
					for(int j=0;j<H2S_485_NUM;j++){//核对有没有配置过
							if(sheet.h2s[j].workFlag==RT_TRUE){
							    nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
								  //for(int z=0;z<27;z++){
		
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.h2s[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.h2s[j].ID));
									//}
							}
					}
				break;
				case CO:
					for(int j=0;j<CO_485_NUM;j++){//核对有没有配置过
							if(sheet.co[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.co[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.co[j].ID));
							}
					}
				break;
#endif
				case TEMPHUM:
					for(int j=0;j<TEMPHUM_485_NUM;j++){//核对有没有配置过
							if(sheet.tempHum[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.tempHum[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.tempHum[j].ID));
							}
					}
				break;
				case WATERDEPTH:
					for(int j=0;j<WATERDEPTH_485_NUM;j++){//核对有没有配置过
							if(sheet.waterDepth[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.waterDepth[j].model));
									cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.waterDepth[j].ID));
							}
					}
				break;
				case CRACKMETER:
					for(int j=0;j<CRACKMETER_485_NUM;j++){//核对有没有配置过
							if(sheet.crackMeter[j].workFlag==RT_TRUE){
									nodeobj = cJSON_CreateObject();
									cJSON_AddItemToArray(Array, nodeobj);
									cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.crackMeter[j].model));
								  cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(modbusName[i]));//name
									cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.crackMeter[j].ID));
							}
					}
				break;
				default:
				break;
			}	
		}
		int i,j;
		bool  add=true;
////////////////////////////////////////////////模拟数据打包////////////////////////////////////////////////////
#ifndef     ANA_MASK
		for(i=0;i<ANALOG_NUM;i++){
			  add=true;
				if(i>=1){
						for(j=0;j<i;j++){
								if(rt_strcmp(sheet.analog[i].ID,sheet.analog[j].ID)==0){
										add=false;//找到了重复的 跳过此次i的计数
									  break;
								}
						}
				}
				if(add==false){
					  continue;//跳过此次循环 执行下一次
				}
				if(sheet.analog[i].workFlag==RT_TRUE){
						nodeobj = cJSON_CreateObject();
						cJSON_AddItemToArray(Array, nodeobj);
						cJSON_AddItemToObject(nodeobj,"model",cJSON_CreateString(sheet.analog[i].model));
						cJSON_AddItemToObject(nodeobj,"name",cJSON_CreateString(sheet.analog[i].name));
						cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.analog[i].ID));
				}
		 }
#endif
////////////////////////////////////////////////数字输入打包////////////////////////////////////////////////////
		 for(i=0;i<DI_NUM;i++){
			  add=true;
				if(i>=1){
						for(j=0;j<i;j++){
								if(rt_strcmp(packFlash.diginput[i].devID,packFlash.diginput[j].devID)==0){
										add=false;//找到了重复的 跳过此次i的计数
									  break;
								}
						}
				}
				if(add==false){
					  continue;//跳过此次循环 执行下一次
				}
				if(packFlash.diginput[i].workFlag==RT_TRUE){
						nodeobj = cJSON_CreateObject();
						cJSON_AddItemToArray(Array, nodeobj);
						cJSON_AddItemToObject(nodeobj,"model",  cJSON_CreateString(packFlash.diginput[i].model));
						cJSON_AddItemToObject(nodeobj,"name",   cJSON_CreateString(packFlash.diginput[i].name));
						cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(packFlash.diginput[i].devID));
				}
		 }
////////////////////////////////////////////////数字输出打包////////////////////////////////////////////////////
		 for(i=0;i<DO_NUM;i++){
			  add=true;
				if(i>=1){
						for(j=0;j<i;j++){
								if(rt_strcmp(packFlash.digoutput[i].devID,packFlash.digoutput[j].devID)==0){
										add=false;//找到了重复的 跳过此次i的计数
									  break;
								}
						}
				}
				if(add==false){
					  continue;//跳过此次循环 执行下一次
				}
				if(packFlash.digoutput[i].workFlag==RT_TRUE){
						nodeobj = cJSON_CreateObject();
						cJSON_AddItemToArray(Array, nodeobj);
						cJSON_AddItemToObject(nodeobj,"model",   cJSON_CreateString(packFlash.digoutput[i].model));
						cJSON_AddItemToObject(nodeobj,"name",    cJSON_CreateString(packFlash.digoutput[i].name));
						cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(packFlash.digoutput[i].devID));
				}
		 }
////////////////////////////////////////////////输出3V3打包////////////////////////////////////////////////////
		 for(i=0;i<V33O_NUM;i++){
			  add=true;
				if(i>=1){
						for(j=0;j<i;j++){
								if(rt_strcmp(packFlash.v33output[i].devID,packFlash.v33output[j].devID)==0){
										add=false;//找到了重复的 跳过此次i的计数
									  break;
								}
						}
				}
				if(add==false){
					  continue;//跳过此次循环 执行下一次
				}
				if(packFlash.v33output[i].workFlag==RT_TRUE){
						nodeobj = cJSON_CreateObject();
						cJSON_AddItemToArray(Array, nodeobj);
						cJSON_AddItemToObject(nodeobj,"model",   cJSON_CreateString(packFlash.v33output[i].model));
						cJSON_AddItemToObject(nodeobj,"name",    cJSON_CreateString(packFlash.v33output[i].name));
						cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(packFlash.v33output[i].devID));
				}
		 }
////////////////////////////////////////////////输出5V注册打包////////////////////////////////////////////////////
		 for(i=0;i<V5O_NUM;i++){
			  add=true;
				if(i>=1){
						for(j=0;j<i;j++){
								if(rt_strcmp(packFlash.v5output[i].devID,packFlash.v5output[j].devID)==0){
										add=false;//找到了重复的 跳过此次i的计数
									  break;
								}
						}
				}
				if(add==false){
					  continue;//跳过此次循环 执行下一次
				}
				if(packFlash.v5output[i].workFlag==RT_TRUE){
						nodeobj = cJSON_CreateObject();
						cJSON_AddItemToArray(Array, nodeobj);
						cJSON_AddItemToObject(nodeobj,"model",   cJSON_CreateString(packFlash.v5output[i].model));
						cJSON_AddItemToObject(nodeobj,"name",    cJSON_CreateString(packFlash.v5output[i].name));
						cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(packFlash.v5output[i].devID));
				}
		 }
////////////////////////////////////////////////输出12V注册打包////////////////////////////////////////////////////
		 for(i=0;i<V12O_NUM;i++){
			  add=true;
				if(i>=1){
						for(j=0;j<i;j++){
								if(rt_strcmp(packFlash.v12output[i].devID,packFlash.v12output[j].devID)==0){
										add=false;//找到了重复的 跳过此次i的计数
									  break;
								}
						}
				}
				if(add==false){
					  continue;//跳过此次循环 执行下一次
				}
				if(packFlash.v12output[i].workFlag==RT_TRUE){
						nodeobj = cJSON_CreateObject();
						cJSON_AddItemToArray(Array, nodeobj);
						cJSON_AddItemToObject(nodeobj,"model",   cJSON_CreateString(packFlash.v12output[i].model));
						cJSON_AddItemToObject(nodeobj,"name",    cJSON_CreateString(packFlash.v12output[i].name));
						cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(packFlash.v12output[i].devID));
				}
		 }
	}
	sprintf(sprinBuf,"%llu",utcTime());
	cJSON_AddStringToObject(root,"timestamp",sprinBuf);
	rt_free(sprinBuf);
	sprinBuf=RT_NULL;
	// 打印JSON数据包  

	
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

		
		mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		return 1;
}




