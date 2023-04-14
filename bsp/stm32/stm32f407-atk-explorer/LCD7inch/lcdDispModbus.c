/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include "7inchRegist.h"
const static char sign[]="[LCDDispMod]";
extern int offLineIndex;

									
typedef struct{
	  uint8_t X;//modbus设备的种类
	  uint8_t Y;//每种modbus的个数
	  uint8_t flag;//存在标记为1
}modbusPositStru;
modbusPositStru  modPosit[TOTOLA_485_NUM]={0}; 


uint8_t  modbTotalIndex=0;
uint8_t  modbDevReadIndex=0;
char  modCurrtID[20];

//通过ID删除modbus列表
void delModbusDevbyID(char *ID)
{
		//if(rt_strcmp(sheet.cirCula[modPosit[modbDevReadIndex].Y].ID,ID)==0){
				switch(modPosit[modbDevReadIndex].X){
					case CIRCULA:	
						sheet.cirCula[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
					case PARTDISCHAG:
						sheet.partDischag[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
					case PRESSSETTL:
						sheet.pressSetl[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
					case THREEAXIS:
						sheet.threeAxiss[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
		#ifdef USE_4GAS 	
					case CH4:
						sheet.ch4[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
					case O2:
						sheet.o2[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
					case H2S:
						sheet.h2s[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
					case CO:
						sheet.co[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
		#endif
					case TEMPHUM:
						sheet.tempHum[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
					case WATERDEPTH:
						sheet.waterDepth[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
					case CRACKMETER:
						sheet.crackMeter[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
					  rt_kprintf("%sdel crackMeter\n %d",sign);
						break;
					default:
						rt_kprintf("%serror delModbusDevbyID\n %d",sign,modPosit[modbDevReadIndex].X);
						break;
				}
				rt_kprintf("%sfind ID\n",sign);
				modPosit[modbDevReadIndex].flag=0;
		//}
//		else{
//			rt_kprintf("%sID[%s][%s]\n",sign,sheet.cirCula[modPosit[modbDevReadIndex].Y].ID,ID);
//		}
}	
typedef struct{
		char  name[25];
		char  ID[20];
		char  model[8];
		uint8_t   port;
		uint8_t   addr;
		uint32_t  colTime;
}LCDDispModInfoStru;



///////////////////////////////////////////////////
/******************故障传感器列表*****************/
LCDDispModInfoStru  modbusLCDErrRead={0};
uint8_t  modbErrTotalIndex=0;
uint8_t  modbErrDevReadIndex=0;
modbusPositStru  modPositErr[TOTOLA_485_NUM]={0};
//////////////////////////////////////////////////

extern int ch4State(int i);
extern int waterDepthState(int i);
extern int pressSettleState(int i);
extern int tempHumState(int i);
extern int coState(int i);
extern int partDisState(int i);
extern int cirCurrtState(int i);
extern int threeAxisState(int i);
extern int o2State(int i);
extern int h2sState(int i);
extern int crackMeterState(int i);

//LCD显示故障modbus所有列表的坐标获取  切换界面后需要调用此函数
void LCDDispErrModbusGet()
{
		
	  modbErrTotalIndex=0;
		for(int i=0;i<MODBUS_NUM;i++){
			switch(i)
			{
				case CIRCULA:
					for(int j=0;j<CIRCULA_485_NUM;j++){//核对有没有配置过
							if(sheet.cirCula[j].workFlag==RT_TRUE){
								  if(cirCurrtState(j)!=1){
											modPositErr[modbErrTotalIndex].X=i;
											modPositErr[modbErrTotalIndex].Y=j;
											modPositErr[modbErrTotalIndex].flag=1;
											modbErrTotalIndex++;
//										  rt_kprintf("err cirCula %d\n",j);
									}
							}
					}
				break;
				case PARTDISCHAG:
					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//核对有没有配置过
							if(sheet.partDischag[j].workFlag==RT_TRUE){
									if(partDisState(j)!=1){
											modPositErr[modbErrTotalIndex].X=i;
											modPositErr[modbErrTotalIndex].Y=j;
											modPositErr[modbErrTotalIndex].flag=1;
											modbErrTotalIndex++;
									}
							}
					}
				break;
				case PRESSSETTL:
					for(int j=0;j<PRESSSETTL_485_NUM;j++){//核对有没有配置过
							if(sheet.pressSetl[j].workFlag==RT_TRUE){
									if(pressSettleState(j)!=1){
											modPositErr[modbErrTotalIndex].X=i;
											modPositErr[modbErrTotalIndex].Y=j;
											modPositErr[modbErrTotalIndex].flag=1;
											modbErrTotalIndex++;
									}
							}
					}
				break;
				case THREEAXIS:
					for(int j=0;j<THREEAXIS_485_NUM;j++){//核对有没有配置过
							if(sheet.threeAxiss[j].workFlag==RT_TRUE){
								  if(threeAxisState(j)!=1){
											modPositErr[modbErrTotalIndex].X=i;
											modPositErr[modbErrTotalIndex].Y=j;
											modPositErr[modbErrTotalIndex].flag=1;
											modbErrTotalIndex++;
									}
							}
					}
				break;
#ifdef	USE_4GAS
				case CH4:
					for(int j=0;j<CH4_485_NUM;j++){//核对有没有配置过
							if(sheet.ch4[j].workFlag==RT_TRUE){
								  if(ch4State(j)!=1){
										modPositErr[modbErrTotalIndex].X=i;
										modPositErr[modbErrTotalIndex].Y=j;
										modPositErr[modbErrTotalIndex].flag=1;
										modbErrTotalIndex++;
									}
							}
					}
				break;
				case O2:
					for(int j=0;j<O2_485_NUM;j++){//核对有没有配置过
							if(sheet.o2[j].workFlag==RT_TRUE){
								 if(o2State(j)!=1){
										modPositErr[modbErrTotalIndex].X=i;
										modPositErr[modbErrTotalIndex].Y=j;
										modPositErr[modbErrTotalIndex].flag=1;
										modbErrTotalIndex++;
								 }
							}
					}
				break;
				case H2S:
					for(int j=0;j<H2S_485_NUM;j++){//核对有没有配置过
							if(sheet.h2s[j].workFlag==RT_TRUE){
									if(h2sState(j)!=1){
										modPositErr[modbErrTotalIndex].X=i;
										modPositErr[modbErrTotalIndex].Y=j;
										modPositErr[modbErrTotalIndex].flag=1;
										modbErrTotalIndex++;
									}
							}
					}
				break;
				case CO:
					for(int j=0;j<CO_485_NUM;j++){//核对有没有配置过
							if(sheet.co[j].workFlag==RT_TRUE){
									if(coState(j)!=1){
										modPositErr[modbErrTotalIndex].X=i;
										modPositErr[modbErrTotalIndex].Y=j;
										modPositErr[modbErrTotalIndex].flag=1;
										modbErrTotalIndex++;
									}
							}
					}
				break;
#endif
				case TEMPHUM:
					for(int j=0;j<TEMPHUM_485_NUM;j++){//核对有没有配置过
							if(sheet.tempHum[j].workFlag==RT_TRUE){
									if(tempHumState(j)!=1){
											modPositErr[modbErrTotalIndex].X=i;
											modPositErr[modbErrTotalIndex].Y=j;
											modPositErr[modbErrTotalIndex].flag=1;
											modbErrTotalIndex++;
									}
							}
					}
				break;
				case WATERDEPTH:
					for(int j=0;j<WATERDEPTH_485_NUM;j++){//核对有没有配置过
							if(sheet.waterDepth[j].workFlag==RT_TRUE){
									if(waterDepthState(j)!=1){
											modPositErr[modbErrTotalIndex].X=i;
											modPositErr[modbErrTotalIndex].Y=j;
											modPositErr[modbErrTotalIndex].flag=1;
											modbErrTotalIndex++;
									}
							}
					}
				break;
				case CRACKMETER:
					for(int j=0;j<CRACKMETER_485_NUM;j++){//核对有没有配置过
							if(sheet.crackMeter[j].workFlag==RT_TRUE){
								if(crackMeterState(j)!=1){
											modPositErr[modbErrTotalIndex].X=i;
											modPositErr[modbErrTotalIndex].Y=j;
											modPositErr[modbErrTotalIndex].flag=1;
											modbErrTotalIndex++;
									}
							}
					}
					break;
				default:
					rt_kprintf("%serror LCDDispErrModbusGet %d\n",sign,i);
				break;
			}
		}
		rt_kprintf("%s errmodbus device num[%d]get\n",sign,modbErrTotalIndex);
}
//char name[25]={0};
//char  ID[20]={0};
//char  model[8]={0};
//uint8_t   port=0;
//uint8_t   addr=0;
//uint32_t  colTime=0;
LCDDispModInfoStru  modbusLCDRead={0};


//显示当前的modbus设备信息  modbDevReadIndex
//每次显示时候提取当前ID号以及所有显示的信息 以便于显示删除
//在LDCDispMosbusInfo()前调用
//参数 modPositErr,modbErrDevReadIndex
void LCDDispModInfoCpy(modbusPositStru *posit,uint8_t readIndex,LCDDispModInfoStru *lcdRead)
{
	  //modbusPositStru  modPositErr[TOTOLA_485_NUM]
	  modbusPositStru posit_p=posit[readIndex];
	 // LCDDispModInfoStru *lcdRead_p=lcdRead;
		switch(posit_p.X){
				case CIRCULA:	
					for(int j=0;j<CIRCULA_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
									if(posit_p.flag==1){
											 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
											 rt_strcpy(lcdRead->ID,  sheet.cirCula[posit_p.Y].ID);//ID第一次上电存储的可能是0xff  拷贝会导致内存泄漏
											 rt_strcpy(lcdRead->model,sheet.cirCula[posit_p.Y].model);
											 lcdRead->port = sheet.cirCula[posit_p.Y].useUartNum;
											 lcdRead->addr = sheet.cirCula[posit_p.Y].slaveAddr;
											 lcdRead->colTime = sheet.cirCulaColTime;
									}
								 break;
							}
					}
					break;
				case PARTDISCHAG:
					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								if(posit_p.flag==1){
									 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
									 rt_strcpy(lcdRead->ID,  sheet.partDischag[posit_p.Y].ID);
									 rt_strcpy(lcdRead->model,sheet.partDischag[posit_p.Y].model);
									 lcdRead->port = sheet.partDischag[posit_p.Y].useUartNum;
									 lcdRead->addr = sheet.partDischag[posit_p.Y].slaveAddr;
									 lcdRead->colTime = sheet.partDischagColTime;
								}
								 break;
							}
					}
					break;
				case PRESSSETTL:
					for(int j=0;j<PRESSSETTL_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								if(posit_p.flag==1){
									 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
									 rt_strcpy(lcdRead->ID,  sheet.pressSetl[posit_p.Y].ID);
									 rt_strcpy(lcdRead->model,sheet.pressSetl[posit_p.Y].model);
									 lcdRead->port = sheet.pressSetl[posit_p.Y].useUartNum;
									 lcdRead->addr = sheet.pressSetl[posit_p.Y].slaveAddr;
									 lcdRead->colTime = sheet.pressSetlColTime;
								}
								 break;
							}
					}
					break;
				case THREEAXIS:
					for(int j=0;j<THREEAXIS_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								 if(posit_p.flag==1){
									 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
									 rt_strcpy(lcdRead->ID,  sheet.threeAxiss[posit_p.Y].ID);
									 rt_strcpy(lcdRead->model,sheet.threeAxiss[posit_p.Y].model);
									 lcdRead->port = sheet.threeAxiss[posit_p.Y].useUartNum;
									 lcdRead->addr = sheet.threeAxiss[posit_p.Y].slaveAddr;
									 lcdRead->colTime = sheet.threeAxissColTime;
								 }
								 break;
							}
					}
					break;
	#ifdef USE_4GAS
				case CH4:
					for(int j=0;j<CH4_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								 if(posit_p.flag==1){
									 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
									 rt_strcpy(lcdRead->ID,  sheet.ch4[posit_p.Y].ID);
									 rt_strcpy(lcdRead->model,sheet.ch4[posit_p.Y].model);
									 lcdRead->port = sheet.ch4[posit_p.Y].useUartNum;
									 lcdRead->addr = sheet.ch4[posit_p.Y].slaveAddr;
									 lcdRead->colTime =sheet.gasColTime;// //sheet.ch4ColTime;
								 }
								 break;
							}
					}
					break;
				case O2:
					for(int j=0;j<O2_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								if(posit_p.flag==1){
									 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
									 rt_strcpy(lcdRead->ID,  sheet.o2[posit_p.Y].ID);
									 rt_strcpy(lcdRead->model,sheet.o2[posit_p.Y].model);
									 lcdRead->port = sheet.o2[posit_p.Y].useUartNum;
									 lcdRead->addr = sheet.o2[posit_p.Y].slaveAddr;
									 lcdRead->colTime = sheet.gasColTime;//sheet.o2ColTime;
								 }
								 break;
							}
					}
					break;
				case H2S:
					for(int j=0;j<H2S_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								 if(posit_p.flag==1){
									 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
									 rt_strcpy(lcdRead->ID,  sheet.h2s[posit_p.Y].ID);
									 rt_strcpy(lcdRead->model,sheet.h2s[posit_p.Y].model);
									 lcdRead->port = sheet.h2s[posit_p.Y].useUartNum;
									 lcdRead->addr = sheet.h2s[posit_p.Y].slaveAddr;
									 lcdRead->colTime = sheet.gasColTime;//sheet.h2sColTime;
								 }
								 break;
							}
					}
					break;
				case CO:
					for(int j=0;j<CO_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								 if(posit_p.flag==1){
										 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
										 rt_strcpy(lcdRead->ID,  sheet.co[posit_p.Y].ID);
										 rt_strcpy(lcdRead->model,sheet.co[posit_p.Y].model);
										 lcdRead->port = sheet.co[posit_p.Y].useUartNum;
										 lcdRead->addr = sheet.co[posit_p.Y].slaveAddr;
										 lcdRead->colTime = sheet.gasColTime;
								 }
								 break;
							}
					}
					break;
	#endif
				case TEMPHUM:
					for(int j=0;j<TEMPHUM_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								 if(posit_p.flag==1){
									 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
									 rt_strcpy(lcdRead->ID,  sheet.tempHum[posit_p.Y].ID);
									 rt_strcpy(lcdRead->model,sheet.tempHum[posit_p.Y].model);
									 lcdRead->port = sheet.tempHum[posit_p.Y].useUartNum;
									 lcdRead->addr = sheet.tempHum[posit_p.Y].slaveAddr;
									 lcdRead->colTime = sheet.tempHumColTime;
								 }
								 break;
							}
					}
					break;
				case WATERDEPTH:
					for(int j=0;j<WATERDEPTH_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								 if(posit_p.flag==1){
									 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
									 rt_strcpy(lcdRead->ID,  sheet.waterDepth[posit_p.Y].ID);
									 rt_strcpy(lcdRead->model,sheet.waterDepth[posit_p.Y].model);
									 lcdRead->port = sheet.waterDepth[posit_p.Y].useUartNum;
									 lcdRead->addr = sheet.waterDepth[posit_p.Y].slaveAddr;
									 lcdRead->colTime = sheet.waterDepthColTime;
								 }
								 break;
							}
					}
					break;
				case CRACKMETER:
					for(int j=0;j<CRACKMETER_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								 if(posit_p.flag==1){
									 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
									 rt_strcpy(lcdRead->ID,  sheet.crackMeter[posit_p.Y].ID);
									 rt_strcpy(lcdRead->model,sheet.crackMeter[posit_p.Y].model);
									 lcdRead->port = sheet.crackMeter[posit_p.Y].useUartNum;
									 lcdRead->addr = sheet.crackMeter[posit_p.Y].slaveAddr;
									 lcdRead->colTime = sheet.crackMeterColTime;
								 }
								 break;
							}
					}
					break;
				default:
					rt_kprintf("%serror LCDDispModInfoCpy %d\n",sign,posit_p.X);
					break;
			}
}




//上电后控件第一次显示的中文名称拷贝 
//传感器读取界面显示所有信息
void LDCDispMosbusInfo()
{
	  if(modbDevReadIndex>=modbTotalIndex)
			modbDevReadIndex=0;//最后一页删除的时候要从第一页开始显示
	  LCDDispModInfoCpy(modPosit,modbDevReadIndex,&modbusLCDRead);
	  uint8_t *buf=NULL;
	  buf=rt_malloc(50);
	  //显示中文名
	 
		int Len=strlen(modbusLCDRead.name);
		for(int i=0;i<Len;i++){
				buf[i]=modbusLCDRead.name[i];
		}
		int j=0;
		while((Len+j)<sizeof(modbusName[0])){
				buf[Len+j]=0xff;
				j++;
		}
	  LCDWtite(MODBUSDISP_NAME_ADDR,buf,sizeof(modbusName[0]));
		//显示ID
		 Len=strlen(modbusLCDRead.ID);
		for(int i=0;i<Len;i++){
				buf[i]=modbusLCDRead.ID[i];
		}
		 j=0;
		while((Len+j)<MODBID_LEN-2){
				buf[Len+j]=0xff;
				j++;
			  if(j>=2)
					break;
		}
	  LCDWtite(MODBUSDISP_ID_ADDR,buf,MODBID_LEN-2);//7寸屏显示18
		//显示model
		Len=strlen(modbusLCDRead.model);
		for(int i=0;i<Len;i++){
				buf[i]=modbusLCDRead.model[i];
		}
		 j=0;
		while((Len+j)<MODL_LEN-2){
				buf[Len+j]=0xff;
				j++;
		}
	  LCDWtite(MODBUSDISP_TYPE_ADDR,buf,MODL_LEN-2);
		//显示PORT
		buf[0]=0;
		buf[1]=modbusLCDRead.port+1;
		LCDWtite(MODBUSDISP_PORT_ADDR,buf,2);
		//显示addr
		buf[0]=0;
		buf[1]=modbusLCDRead.addr;
		LCDWtite(MODBUSDISP_ADDR_ADDR,buf,2);
	  //显示colTime
		buf[0]=(uint8_t)(modbusLCDRead.colTime>>24);
		buf[1]=(uint8_t)(modbusLCDRead.colTime>>16);
		buf[2]=(uint8_t)(modbusLCDRead.colTime>>8);
		buf[3]=(uint8_t)(modbusLCDRead.colTime>>0);
		LCDWtite(MODBUSDISP_TIME_ADDR,buf,4);
		//显示总共页
		buf[0]=0;
		buf[1]=modbTotalIndex;
		LCDWtite(MODBUSDISP_TOTALNUM_ADDR,buf,2);
		rt_kprintf("%s total %d\n",sign,modbTotalIndex);
		//显示当前页
		buf[0]=0;
		if(modbTotalIndex==0)
				buf[1]=0;
		else{
				buf[1]=modbDevReadIndex+1;
		}
		LCDWtite(MODBUSDISP_NOWNUM_ADDR,buf,2);
		
		
		rt_free(buf);
		buf=RT_NULL;
}


//故障传感器读取界面显示所有信息
void LDCDispErrMosbusInfo()
{
	  LCDDispModInfoCpy(modPositErr,modbErrDevReadIndex,&modbusLCDErrRead);
	  uint8_t *buf=NULL;
	  buf=rt_malloc(50);
	  //显示中文名
	 
		int Len=strlen(modbusLCDErrRead.name);
		for(int i=0;i<Len;i++){
				buf[i]=modbusLCDErrRead.name[i];
		}
		int j=0;
		while((Len+j)<sizeof(modbusName[0])){
				buf[Len+j]=0xff;
				j++;
		}
	  LCDWtite(MODBUSDISP_ERRNAME_ADDR,buf,sizeof(modbusName[0]));
		//显示ID
		 Len=strlen(modbusLCDErrRead.ID);
		for(int i=0;i<Len;i++){
				buf[i]=modbusLCDErrRead.ID[i];
		}
		 j=0;
		while((Len+j)<MODBID_LEN-2){
				buf[Len+j]=0xff;
				j++;
			  if(j>=2)
					break;
		}
	  LCDWtite(MODBUSDISP_ERRID_ADDR,buf,MODBID_LEN-2);//7寸屏显示18
		//显示model
		Len=strlen(modbusLCDErrRead.model);
		for(int i=0;i<Len;i++){
				buf[i]=modbusLCDErrRead.model[i];
		}
		 j=0;
		while((Len+j)<MODL_LEN-2){
				buf[Len+j]=0xff;
				j++;
		}
	  LCDWtite(MODBUSDISP_ERRTYPE_ADDR,buf,MODL_LEN-2);
		//显示PORT
		buf[0]=0;
		buf[1]=modbusLCDErrRead.port+1;
		LCDWtite(MODBUSDISP_ERRPORT_ADDR,buf,2);
		//显示addr
		buf[0]=0;
		buf[1]=modbusLCDErrRead.addr;
		LCDWtite(MODBUSDISP_ERRADDR_ADDR,buf,2);
	  //显示colTime
		buf[0]=(uint8_t)(modbusLCDErrRead.colTime>>24);
		buf[1]=(uint8_t)(modbusLCDErrRead.colTime>>16);
		buf[2]=(uint8_t)(modbusLCDErrRead.colTime>>8);
		buf[3]=(uint8_t)(modbusLCDErrRead.colTime>>0);
		LCDWtite(MODBUSDISP_ERRTIME_ADDR,buf,4);
		//显示总共页
		buf[0]=0;
		buf[1]=modbErrTotalIndex;
		LCDWtite(MODBUSDISP_ERRTOTALNUM_ADDR,buf,2);
		rt_kprintf("%s total %d\n",sign,modbErrTotalIndex);
		//显示当前页
		buf[0]=0;
		if(modbErrTotalIndex==0)
				buf[1]=0;
		else
				buf[1]=modbErrDevReadIndex+1;
		LCDWtite(MODBUSDISP_ERRNOWNUM_ADDR,buf,2);
		
		
		rt_free(buf);
		buf=RT_NULL;
}

void  delModbusDevbyID_p()
{
	delModbusDevbyID(modbusLCDRead.ID);
}



//LCD显示modbus所有列表的坐标获取  删除、配置后或者第一次上电后需要调用此函数
void LCDDispModbusGet()
{
		rt_kprintf("%s modbus device num get\n",sign);
	  modbTotalIndex=0;
		for(int i=0;i<MODBUS_NUM;i++){
			switch(i)
			{
				case CIRCULA:
					for(int j=0;j<CIRCULA_485_NUM;j++){//核对有没有配置过
							if(sheet.cirCula[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case PARTDISCHAG:
					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//核对有没有配置过
							if(sheet.partDischag[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case PRESSSETTL:
					for(int j=0;j<PRESSSETTL_485_NUM;j++){//核对有没有配置过
							if(sheet.pressSetl[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case THREEAXIS:
					for(int j=0;j<THREEAXIS_485_NUM;j++){//核对有没有配置过
							if(sheet.threeAxiss[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
	#ifdef USE_4GAS 
				case CH4:
					for(int j=0;j<CH4_485_NUM;j++){//核对有没有配置过
							if(sheet.ch4[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case O2:
					for(int j=0;j<O2_485_NUM;j++){//核对有没有配置过
							if(sheet.o2[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case H2S:
					for(int j=0;j<H2S_485_NUM;j++){//核对有没有配置过
							if(sheet.h2s[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case CO:
					for(int j=0;j<CO_485_NUM;j++){//核对有没有配置过
							if(sheet.co[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
	#endif
				case TEMPHUM:
					for(int j=0;j<TEMPHUM_485_NUM;j++){//核对有没有配置过
							if(sheet.tempHum[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case WATERDEPTH:
					for(int j=0;j<WATERDEPTH_485_NUM;j++){//核对有没有配置过
							if(sheet.waterDepth[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case CRACKMETER:
					for(int j=0;j<CRACKMETER_485_NUM;j++){//核对有没有配置过
							if(sheet.crackMeter[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
					break;
				default:
					rt_kprintf("%serror LCDDispModbusGet %d\n",sign,i);
				break;
			}
		}
}
extern  const  uartEnum UartNum[UART_NUM];
modbusStru LCDInput;
uint32_t   LCDInputTime=0;
uint32_t   *singlConcalTime=RT_NULL;//
modbusStru *singlConfDev=RT_NULL;//

//modbus设备数量的映射表 需要跟modbNumEnum的列表一一对应
#ifdef	USE_4GAS
uint8_t numTable[]={CIRCULA_485_NUM,PARTDISCHAG_485_NUM ,PRESSSETTL_485_NUM,THREEAXIS_485_NUM,\
										CH4_485_NUM,CO_485_NUM,H2S_485_NUM,O2_485_NUM ,WATERDEPTH_485_NUM,TEMPHUM_485_NUM,CRACKMETER_485_NUM };
#else
uint8_t numTable[]={CIRCULA_485_NUM,PARTDISCHAG_485_NUM ,PRESSSETTL_485_NUM,THREEAXIS_485_NUM,\
										WATERDEPTH_485_NUM,TEMPHUM_485_NUM,CRACKMETER_485_NUM  };
#endif

//单种modbus设备分别配置  思路 做好映射关系表

//不同种类modbus设备配置 输入为modbus种类
static int singlModbConf(int num)
{
//		int i=0;
		int ret=0;
		singlConfDev=sheet.cirCula;//指针指向
		for(int z=0;z<num;z++){
				singlConfDev=singlConfDev+numTable[z];//指针++
		}
		singlConcalTime=&sheet.cirCulaColTime+num*1;//指针指向 
		*singlConcalTime=LCDInputTime;
		rt_kprintf("%saddr 0x%08X 0x%08X 0x%08X 0x%08X \n",sign,&sheet.cirCulaColTime,singlConcalTime,&sheet.crackMeterColTime);
//		for( i=0;i<numTable[num];i++){//核对有没有配置过
//				if(rt_strcmp(singlConfDev[i].ID,LCDInput.ID)==0){//配置过
//						singlConfDev[i].workFlag=RT_TRUE;//打开
//						singlConfDev[i].slaveAddr=LCDInput.slaveAddr;	
//						singlConfDev[i].useUartNum=UartNum[LCDInput.useUartNum];
//						rt_strcpy(singlConfDev[i].model,LCDInput.model);
//						rt_kprintf("%s %s reconfig %d\n",sign,modbusName[num],i+1);
//						ret =1;
//						LCDDispModbusGet();
//						break;
//				}
//		}
//		if(i==numTable[num]){//没有配置过
				for(int j=0;j<numTable[num];j++){
						if(singlConfDev[j].workFlag!=RT_TRUE){
								singlConfDev[j].workFlag=RT_TRUE;//打开
								singlConfDev[j].slaveAddr=LCDInput.slaveAddr;	
								singlConfDev[j].useUartNum=UartNum[LCDInput.useUartNum];
								rt_strcpy(singlConfDev[j].model,LCDInput.model);
								rt_strcpy(singlConfDev[j].ID,LCDInput.ID);
								rt_kprintf("%s %s config %d\n",sign,modbusName[num],j);
								ret =1;
								LCDDispModbusGet();
								break;
						}
				}
//		}
		return ret;
}


static int chinaNameIndex=0;//当前用到的名字标记  根据modbNumEnum对应起来

//通过屏幕来配置modbus的信息 



//显示传感器中文名 lcd配置传感器界面的中文名称的选择
void dispCinaName(uint8_t *buf)
{
		rt_kprintf("index %d\n",chinaNameIndex);
		int nameLen=strlen(modbusName[chinaNameIndex]);
		for(int i=0;i<nameLen;i++){
				buf[i]=modbusName[chinaNameIndex][i];
		}
		int j=0;
		while((nameLen+j)<sizeof(modbusName[chinaNameIndex])){
				buf[nameLen+j]=0xff;
				j++;
		}
		LCDWtite(MODBUS_CFG_NAME2_ADDR,buf,sizeof(modbusName[chinaNameIndex]));
//		LCDWtite(MODBUS_CFG_NAME_ADDR,buf,sizeof(modbusName[chinaNameIndex])); 
}
void dispChinaNameIndexLow()
{
	if(chinaNameIndex==0)
			chinaNameIndex=MODBUS_NUM-1;
	else
			chinaNameIndex--;
}
void dispChinaNameIndexAdd()
{
		if(chinaNameIndex==(MODBUS_NUM-1))
				chinaNameIndex=0;
		else
				chinaNameIndex++;
}

void  modbDevReadIndexAdd()
{
		if(modbDevReadIndex+1>=modbTotalIndex)
			 modbDevReadIndex=0;
		else 
			 modbDevReadIndex++;
}
void  modbDevReadIndexLow()
{
	if(modbDevReadIndex==0)
		 modbDevReadIndex=modbTotalIndex-1;
	else
		 modbDevReadIndex--;
}
void  	modbErrDevReadIndexAdd()
{
	if(modbErrDevReadIndex+1==modbErrTotalIndex)
		 modbErrDevReadIndex=0;
	else 
		 modbErrDevReadIndex++;
}

void  	modbErrDevReadIndexLow()
{
		if(modbErrDevReadIndex==0)
			 modbErrDevReadIndex=modbErrTotalIndex-1;
		else
			 modbErrDevReadIndex--;
}
void  offLineIndexLow()  
{
		offLineIndex--;
		if(offLineIndex==0){
				offLineIndex = offLine.times;
		}
}
void  offLineIndexAdd()  
{
		offLineIndex++;
		if(offLineIndex>offLine.times){
				offLineIndex = 1;
		}
}



int modbusConfIDCheck(char *inputID);
void keyModbusCfgSure()
{
		modbusConfIDCheck(LCDInput.ID);
		singlModbConf(chinaNameIndex);
}
