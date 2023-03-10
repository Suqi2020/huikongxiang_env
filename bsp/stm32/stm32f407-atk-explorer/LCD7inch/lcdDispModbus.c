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
	  uint8_t X;//modbus�豸������
	  uint8_t Y;//ÿ��modbus�ĸ���
	  uint8_t flag;//���ڱ��Ϊ1
}modbusPositStru;
modbusPositStru  modPosit[TOTOLA_485_NUM]={0}; 


uint8_t  modbTotalIndex=0;
uint8_t  modbDevReadIndex=0;
char  modCurrtID[20];

//ͨ��IDɾ��modbus�б�
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
/******************���ϴ������б�*****************/
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


//LCD��ʾ����modbus�����б��������ȡ  �л��������Ҫ���ô˺���
void LCDDispErrModbusGet()
{
		
	  modbErrTotalIndex=0;
		for(int i=0;i<MODBUS_NUM;i++){
			switch(i)
			{
				case CIRCULA:
					for(int j=0;j<CIRCULA_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<PRESSSETTL_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<THREEAXIS_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<CH4_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<O2_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<H2S_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<CO_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<TEMPHUM_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<WATERDEPTH_485_NUM;j++){//�˶���û�����ù�
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


//��ʾ��ǰ��modbus�豸��Ϣ  modbDevReadIndex
//ÿ����ʾʱ����ȡ��ǰID���Լ�������ʾ����Ϣ �Ա�����ʾɾ��
//��LDCDispMosbusInfo()ǰ����
//���� modPositErr,modbErrDevReadIndex
void LCDDispModInfoCpy(modbusPositStru *posit,uint8_t readIndex,LCDDispModInfoStru *lcdRead)
{
	  //modbusPositStru  modPositErr[TOTOLA_485_NUM]
	  modbusPositStru posit_p=posit[readIndex];
	 // LCDDispModInfoStru *lcdRead_p=lcdRead;
		switch(posit_p.X){
				case CIRCULA:	
					for(int j=0;j<CIRCULA_485_NUM;j++){//�˶���û�����ù�
							if(posit_p.Y==j){
									if(posit_p.flag==1){
											 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
											 rt_strcpy(lcdRead->ID,  sheet.cirCula[posit_p.Y].ID);//ID��һ���ϵ�洢�Ŀ�����0xff  �����ᵼ���ڴ�й©
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
					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<PRESSSETTL_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<THREEAXIS_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<CH4_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<O2_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<H2S_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<CO_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<TEMPHUM_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<WATERDEPTH_485_NUM;j++){//�˶���û�����ù�
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
				default:
					rt_kprintf("%serror LCDDispModInfoCpy %d\n",sign,posit_p.X);
					break;
			}
}




//�ϵ��ؼ���һ����ʾ���������ƿ��� 
//��������ȡ������ʾ������Ϣ
void LDCDispMosbusInfo()
{
	  LCDDispModInfoCpy(modPosit,modbDevReadIndex,&modbusLCDRead);
	  uint8_t *buf=NULL;
	  buf=rt_malloc(50);
	  //��ʾ������
	 
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
		//��ʾID
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
	  LCDWtite(MODBUSDISP_ID_ADDR,buf,MODBID_LEN-2);//7������ʾ18
		//��ʾmodel
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
		//��ʾPORT
		buf[0]=0;
		buf[1]=modbusLCDRead.port+1;
		LCDWtite(MODBUSDISP_PORT_ADDR,buf,2);
		//��ʾaddr
		buf[0]=0;
		buf[1]=modbusLCDRead.addr;
		LCDWtite(MODBUSDISP_ADDR_ADDR,buf,2);
	  //��ʾcolTime
		buf[0]=(uint8_t)(modbusLCDRead.colTime>>24);
		buf[1]=(uint8_t)(modbusLCDRead.colTime>>16);
		buf[2]=(uint8_t)(modbusLCDRead.colTime>>8);
		buf[3]=(uint8_t)(modbusLCDRead.colTime>>0);
		LCDWtite(MODBUSDISP_TIME_ADDR,buf,4);
		//��ʾ�ܹ�ҳ
		buf[0]=0;
		buf[1]=modbTotalIndex;
		LCDWtite(MODBUSDISP_TOTALNUM_ADDR,buf,2);
		rt_kprintf("%s total %d\n",sign,modbTotalIndex);
		//��ʾ��ǰҳ
		buf[0]=0;
		if(modbTotalIndex==0)
				buf[1]=0;
		else
				buf[1]=modbDevReadIndex+1;
		LCDWtite(MODBUSDISP_NOWNUM_ADDR,buf,2);
		
		
		rt_free(buf);
		buf=RT_NULL;
}


//���ϴ�������ȡ������ʾ������Ϣ
void LDCDispErrMosbusInfo()
{
	  LCDDispModInfoCpy(modPositErr,modbErrDevReadIndex,&modbusLCDErrRead);
	  uint8_t *buf=NULL;
	  buf=rt_malloc(50);
	  //��ʾ������
	 
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
		//��ʾID
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
	  LCDWtite(MODBUSDISP_ERRID_ADDR,buf,MODBID_LEN-2);//7������ʾ18
		//��ʾmodel
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
		//��ʾPORT
		buf[0]=0;
		buf[1]=modbusLCDErrRead.port+1;
		LCDWtite(MODBUSDISP_ERRPORT_ADDR,buf,2);
		//��ʾaddr
		buf[0]=0;
		buf[1]=modbusLCDErrRead.addr;
		LCDWtite(MODBUSDISP_ERRADDR_ADDR,buf,2);
	  //��ʾcolTime
		buf[0]=(uint8_t)(modbusLCDErrRead.colTime>>24);
		buf[1]=(uint8_t)(modbusLCDErrRead.colTime>>16);
		buf[2]=(uint8_t)(modbusLCDErrRead.colTime>>8);
		buf[3]=(uint8_t)(modbusLCDErrRead.colTime>>0);
		LCDWtite(MODBUSDISP_ERRTIME_ADDR,buf,4);
		//��ʾ�ܹ�ҳ
		buf[0]=0;
		buf[1]=modbErrTotalIndex;
		LCDWtite(MODBUSDISP_ERRTOTALNUM_ADDR,buf,2);
		rt_kprintf("%s total %d\n",sign,modbErrTotalIndex);
		//��ʾ��ǰҳ
		buf[0]=0;
		if(modbErrTotalIndex==0)
				buf[1]=0;
		else
				buf[1]=modbErrDevReadIndex+1;
		LCDWtite(MODBUSDISP_ERRNOWNUM_ADDR,buf,2);
		
		
		rt_free(buf);
		buf=RT_NULL;
}
//void LCDDispModInfoCpy()
//{
//	 
//		switch(modPosit[modbDevReadIndex].X){
//				case CIRCULA:	
//					for(int j=0;j<CIRCULA_485_NUM;j++){//�˶���û�����ù�
//							if(modPosit[modbDevReadIndex].Y==j){
//									if(modPosit[modbDevReadIndex].flag==1){
//											 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//											 rt_strcpy(modbusLCDRead.ID,  sheet.cirCula[modPosit[modbDevReadIndex].Y].ID);//ID��һ���ϵ�洢�Ŀ�����0xff  �����ᵼ���ڴ�й©
//											 rt_strcpy(modbusLCDRead.model,sheet.cirCula[modPosit[modbDevReadIndex].Y].model);
//											 modbusLCDRead.port = sheet.cirCula[modPosit[modbDevReadIndex].Y].useUartNum;
//											 modbusLCDRead.addr = sheet.cirCula[modPosit[modbDevReadIndex].Y].slaveAddr;
//											 modbusLCDRead.colTime = sheet.cirCulaColTime;
//									}
//								 break;
//							}
//					}
//					break;
//				case PARTDISCHAG:
//					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//�˶���û�����ù�
//							if(modPosit[modbDevReadIndex].Y==j){
//								if(modPosit[modbDevReadIndex].flag==1){
//									 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//									 rt_strcpy(modbusLCDRead.ID,  sheet.partDischag[modPosit[modbDevReadIndex].Y].ID);
//									 rt_strcpy(modbusLCDRead.model,sheet.partDischag[modPosit[modbDevReadIndex].Y].model);
//									 modbusLCDRead.port = sheet.partDischag[modPosit[modbDevReadIndex].Y].useUartNum;
//									 modbusLCDRead.addr = sheet.partDischag[modPosit[modbDevReadIndex].Y].slaveAddr;
//									 modbusLCDRead.colTime = sheet.partDischagColTime;
//								}
//								 break;
//							}
//					}
//					break;
//				case PRESSSETTL:
//					for(int j=0;j<PRESSSETTL_485_NUM;j++){//�˶���û�����ù�
//							if(modPosit[modbDevReadIndex].Y==j){
//								if(modPosit[modbDevReadIndex].flag==1){
//									 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//									 rt_strcpy(modbusLCDRead.ID,  sheet.pressSetl[modPosit[modbDevReadIndex].Y].ID);
//									 rt_strcpy(modbusLCDRead.model,sheet.pressSetl[modPosit[modbDevReadIndex].Y].model);
//									 modbusLCDRead.port = sheet.pressSetl[modPosit[modbDevReadIndex].Y].useUartNum;
//									 modbusLCDRead.addr = sheet.pressSetl[modPosit[modbDevReadIndex].Y].slaveAddr;
//									 modbusLCDRead.colTime = sheet.pressSetlColTime;
//								}
//								 break;
//							}
//					}
//					break;
//				case THREEAXIS:
//					for(int j=0;j<THREEAXIS_485_NUM;j++){//�˶���û�����ù�
//							if(modPosit[modbDevReadIndex].Y==j){
//								 if(modPosit[modbDevReadIndex].flag==1){
//									 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//									 rt_strcpy(modbusLCDRead.ID,  sheet.threeAxiss[modPosit[modbDevReadIndex].Y].ID);
//									 rt_strcpy(modbusLCDRead.model,sheet.threeAxiss[modPosit[modbDevReadIndex].Y].model);
//									 modbusLCDRead.port = sheet.threeAxiss[modPosit[modbDevReadIndex].Y].useUartNum;
//									 modbusLCDRead.addr = sheet.threeAxiss[modPosit[modbDevReadIndex].Y].slaveAddr;
//									 modbusLCDRead.colTime = sheet.threeAxissColTime;
//								 }
//								 break;
//							}
//					}
//					break;
//				case CH4:
//					for(int j=0;j<CH4_485_NUM;j++){//�˶���û�����ù�
//							if(modPosit[modbDevReadIndex].Y==j){
//								 if(modPosit[modbDevReadIndex].flag==1){
//									 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//									 rt_strcpy(modbusLCDRead.ID,  sheet.ch4[modPosit[modbDevReadIndex].Y].ID);
//									 rt_strcpy(modbusLCDRead.model,sheet.ch4[modPosit[modbDevReadIndex].Y].model);
//									 modbusLCDRead.port = sheet.ch4[modPosit[modbDevReadIndex].Y].useUartNum;
//									 modbusLCDRead.addr = sheet.ch4[modPosit[modbDevReadIndex].Y].slaveAddr;
//									 modbusLCDRead.colTime = sheet.ch4ColTime;
//								 }
//								 break;
//							}
//					}
//					break;
//				case O2:
//					for(int j=0;j<O2_485_NUM;j++){//�˶���û�����ù�
//							if(modPosit[modbDevReadIndex].Y==j){
//								if(modPosit[modbDevReadIndex].flag==1){
//									 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//									 rt_strcpy(modbusLCDRead.ID,  sheet.o2[modPosit[modbDevReadIndex].Y].ID);
//									 rt_strcpy(modbusLCDRead.model,sheet.o2[modPosit[modbDevReadIndex].Y].model);
//									 modbusLCDRead.port = sheet.o2[modPosit[modbDevReadIndex].Y].useUartNum;
//									 modbusLCDRead.addr = sheet.o2[modPosit[modbDevReadIndex].Y].slaveAddr;
//									 modbusLCDRead.colTime = sheet.o2ColTime;
//								 }
//								 break;
//							}
//					}
//					break;
//				case H2S:
//					for(int j=0;j<H2S_485_NUM;j++){//�˶���û�����ù�
//							if(modPosit[modbDevReadIndex].Y==j){
//								 if(modPosit[modbDevReadIndex].flag==1){
//									 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//									 rt_strcpy(modbusLCDRead.ID,  sheet.h2s[modPosit[modbDevReadIndex].Y].ID);
//									 rt_strcpy(modbusLCDRead.model,sheet.h2s[modPosit[modbDevReadIndex].Y].model);
//									 modbusLCDRead.port = sheet.h2s[modPosit[modbDevReadIndex].Y].useUartNum;
//									 modbusLCDRead.addr = sheet.h2s[modPosit[modbDevReadIndex].Y].slaveAddr;
//									 modbusLCDRead.colTime = sheet.h2sColTime;
//								 }
//								 break;
//							}
//					}
//					break;
//				case CO:
//					for(int j=0;j<CO_485_NUM;j++){//�˶���û�����ù�
//							if(modPosit[modbDevReadIndex].Y==j){
//								 if(modPosit[modbDevReadIndex].flag==1){
//										 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//										 rt_strcpy(modbusLCDRead.ID,  sheet.co[modPosit[modbDevReadIndex].Y].ID);
//										 rt_strcpy(modbusLCDRead.model,sheet.co[modPosit[modbDevReadIndex].Y].model);
//										 modbusLCDRead.port = sheet.co[modPosit[modbDevReadIndex].Y].useUartNum;
//										 modbusLCDRead.addr = sheet.co[modPosit[modbDevReadIndex].Y].slaveAddr;
//										 modbusLCDRead.colTime = sheet.gasColTime;
//								 }
//								 break;
//							}
//					}
//					break;
//				case TEMPHUM:
//					for(int j=0;j<TEMPHUM_485_NUM;j++){//�˶���û�����ù�
//							if(modPosit[modbDevReadIndex].Y==j){
//								 if(modPosit[modbDevReadIndex].flag==1){
//									 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//									 rt_strcpy(modbusLCDRead.ID,  sheet.tempHum[modPosit[modbDevReadIndex].Y].ID);
//									 rt_strcpy(modbusLCDRead.model,sheet.tempHum[modPosit[modbDevReadIndex].Y].model);
//									 modbusLCDRead.port = sheet.tempHum[modPosit[modbDevReadIndex].Y].useUartNum;
//									 modbusLCDRead.addr = sheet.tempHum[modPosit[modbDevReadIndex].Y].slaveAddr;
//									 modbusLCDRead.colTime = sheet.tempHumColTime;
//								 }
//								 break;
//							}
//					}
//					break;
//				case WATERDEPTH:
//					for(int j=0;j<WATERDEPTH_485_NUM;j++){//�˶���û�����ù�
//							if(modPosit[modbDevReadIndex].Y==j){
//								 if(modPosit[modbDevReadIndex].flag==1){
//									 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//									 rt_strcpy(modbusLCDRead.ID,  sheet.waterDepth[modPosit[modbDevReadIndex].Y].ID);
//									 rt_strcpy(modbusLCDRead.model,sheet.waterDepth[modPosit[modbDevReadIndex].Y].model);
//									 modbusLCDRead.port = sheet.waterDepth[modPosit[modbDevReadIndex].Y].useUartNum;
//									 modbusLCDRead.addr = sheet.waterDepth[modPosit[modbDevReadIndex].Y].slaveAddr;
//									 modbusLCDRead.colTime = sheet.waterDepthColTime;
//								 }
//								 break;
//							}
//					}
//					break;
//				default:
//					rt_kprintf("%serror delModbusDevbyID\n",sign);
//					break;
//			}
//}
void  delModbusDevbyID_p()
{
	delModbusDevbyID(modbusLCDRead.ID);
}



//LCD��ʾmodbus�����б��������ȡ  ɾ�������ú���ߵ�һ���ϵ����Ҫ���ô˺���
void LCDDispModbusGet()
{
		rt_kprintf("%s modbus device num get\n",sign);
	  modbTotalIndex=0;
		for(int i=0;i<MODBUS_NUM;i++){
			switch(i)
			{
				case CIRCULA:
					for(int j=0;j<CIRCULA_485_NUM;j++){//�˶���û�����ù�
							if(sheet.cirCula[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case PARTDISCHAG:
					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//�˶���û�����ù�
							if(sheet.partDischag[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case PRESSSETTL:
					for(int j=0;j<PRESSSETTL_485_NUM;j++){//�˶���û�����ù�
							if(sheet.pressSetl[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case THREEAXIS:
					for(int j=0;j<THREEAXIS_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<CH4_485_NUM;j++){//�˶���û�����ù�
							if(sheet.ch4[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case O2:
					for(int j=0;j<O2_485_NUM;j++){//�˶���û�����ù�
							if(sheet.o2[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case H2S:
					for(int j=0;j<H2S_485_NUM;j++){//�˶���û�����ù�
							if(sheet.h2s[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case CO:
					for(int j=0;j<CO_485_NUM;j++){//�˶���û�����ù�
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
					for(int j=0;j<TEMPHUM_485_NUM;j++){//�˶���û�����ù�
							if(sheet.tempHum[j].workFlag==RT_TRUE){
								  modPosit[modbTotalIndex].X=i;
								  modPosit[modbTotalIndex].Y=j;
								  modPosit[modbTotalIndex].flag=1;
									modbTotalIndex++;
							}
					}
				break;
				case WATERDEPTH:
					for(int j=0;j<WATERDEPTH_485_NUM;j++){//�˶���û�����ù�
							if(sheet.waterDepth[j].workFlag==RT_TRUE){
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

//modbus�豸������ӳ��� ��Ҫ��modbNumEnum���б�һһ��Ӧ
#ifdef	USE_4GAS
uint8_t numTable[]={CIRCULA_485_NUM,PARTDISCHAG_485_NUM ,PRESSSETTL_485_NUM,THREEAXIS_485_NUM,\
										CH4_485_NUM,CO_485_NUM,H2S_485_NUM,O2_485_NUM ,WATERDEPTH_485_NUM,TEMPHUM_485_NUM };
#else
uint8_t numTable[]={CIRCULA_485_NUM,PARTDISCHAG_485_NUM ,PRESSSETTL_485_NUM,THREEAXIS_485_NUM,\
										WATERDEPTH_485_NUM,TEMPHUM_485_NUM };
#endif

//����modbus�豸�ֱ�����  ˼· ����ӳ���ϵ��

//��ͬ����modbus�豸���� ����Ϊmodbus����
static int singlModbConf(int num)
{
//		int i=0;
		int ret=0;
		singlConfDev=sheet.cirCula;//ָ��ָ��
		for(int z=0;z<num;z++){
				singlConfDev=singlConfDev+numTable[z];//ָ��++
		}
		singlConcalTime=&sheet.cirCulaColTime+num*1;//ָ��ָ�� 
		*singlConcalTime=LCDInputTime;
//		for( i=0;i<numTable[num];i++){//�˶���û�����ù�
//				if(rt_strcmp(singlConfDev[i].ID,LCDInput.ID)==0){//���ù�
//						singlConfDev[i].workFlag=RT_TRUE;//��
//						singlConfDev[i].slaveAddr=LCDInput.slaveAddr;	
//						singlConfDev[i].useUartNum=UartNum[LCDInput.useUartNum];
//						rt_strcpy(singlConfDev[i].model,LCDInput.model);
//						rt_kprintf("%s %s reconfig %d\n",sign,modbusName[num],i+1);
//						ret =1;
//						LCDDispModbusGet();
//						break;
//				}
//		}
//		if(i==numTable[num]){//û�����ù�
				for(int j=0;j<numTable[num];j++){
						if(singlConfDev[j].workFlag!=RT_TRUE){
								singlConfDev[j].workFlag=RT_TRUE;//��
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


static int chinaNameIndex=0;//��ǰ�õ������ֱ��  ����modbNumEnum��Ӧ����

//ͨ����Ļ������modbus����Ϣ 



//��ʾ������������ lcd���ô�����������������Ƶ�ѡ��
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
		LCDWtite(MODBUS_CFG_NAME_ADDR,buf,sizeof(modbusName[chinaNameIndex])); 
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
		if(modbDevReadIndex+1==modbTotalIndex)
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

//modbusName[MODBUS_NUM][20]
////�����������غ���
//void  keyReturn(uint16_t keyAddr)
//{
//	  uint8_t *buf=NULL;
//	  buf=rt_malloc(50);
//	 // int nameLen=0;
//		switch(keyAddr)
//		{
//			case	KEY_MODBUS_CFG_NAME_ADDR://�������������������ʾ����� ģ������  ��ʾ��1380
//				//5A A5 1182 1380 C9CF BAA3 B9C8 D4AA BFC6 BCBC FFFF
//        dispCinaName(buf);
//				break;
//			case	KEY_MODBUS_LASTNAME_ADDR:
//				dispChinaNameIndexLow();
//				dispCinaName(buf);
//				break;
//			case	KEY_MODBUS_NEXTNAME_ADDR:
//				dispChinaNameIndexAdd();
//				dispCinaName(buf);
//				break;
//			case	KEY_MODBUS_SURENAME_ADDR:// ��ʾ��1360
////			  dispCinaName(buf);   singlModbConf(chinaNameIndex);
////			  rt_strcpy(LCDInput.);
////				LCDWtite(MODBUS_CFG_NAME_ADDR,buf,sizeof(modbusName[chinaNameNum]));
//				break;	
//			case  KEY_IP_READ_ADDR:
//				LCDDispIP();
//				break;
//			case KEY_PORT_READ_ADDR:
//				LCDDispUart();
//				break;
//			case KEY_MCUID_READ_ADDR:
//				LCDDispMCUID();
//				break;
//			case KEY_SAVE_ADDR://����
//				
//				rt_kprintf("%sflash save OK\n",sign);
//				stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFlash));//ÿ�β���128k�ֽ����� �洢ʱ����Ҫһ��洢
//				stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFlash,sizeof(packFlash));
//				stm32_flash_write(FLASH_MODBUS_SAVE_ADDR,(uint8_t*)&sheet,sizeof(sheet));
//				break;
//			case KEY_RESET_ADDR://��λ
//				rt_hw_cpu_reset();
//				break;
//			case KEY_MODBUS_CFG_WATCH_ADDR:
//				LCDDispModInfoCpy(modPosit,modbDevReadIndex,&modbusLCDRead);
//				LDCDispMosbusInfo();
//				break;
//			case KEY_MODBUS_CFG_SURE_ADDR:
//				
//				modbusConfIDCheck(LCDInput.ID);
//				singlModbConf(chinaNameIndex);
//				break;
//			case KEY_MODBUSDISP_LAST_ADDR:
////			  if(modbDevReadIndex==0)
////					 modbDevReadIndex=modbTotalIndex-1;
////				else
////					 modbDevReadIndex--;
//				modbDevReadIndexLow();
//				LCDDispModInfoCpy(modPosit,modbDevReadIndex,&modbusLCDRead);
//				LDCDispMosbusInfo();
//				break;

//			case  KEY_MODBUSDISP_NEXT_ADDR:
////			  if(modbDevReadIndex+1==modbTotalIndex)
////					 modbDevReadIndex=0;
////				else 
////					 modbDevReadIndex++;
//				modbDevReadIndexAdd();
//				LCDDispModInfoCpy(modPosit,modbDevReadIndex,&modbusLCDRead);
//				LDCDispMosbusInfo();
//			  break;
//			
//		
////LCDDispModInfoStru  modbusLCDErrRead={0};
////uint8_t  modbErrTotalIndex=0;
////uint8_t  modbErrDevReadIndex=0;
////modbusPositStru  modPositErr[TOTOLA_485_NUM]={0};
////////////////////////////////////////////////////

//			case KEY_MODBUSDISP_ERRLAST_ADDR:
////			  if(modbErrDevReadIndex==0)
////					 modbErrDevReadIndex=modbErrTotalIndex-1;
////				else
////					 modbErrDevReadIndex--;
//				modbErrDevReadIndexLow();
//				LCDDispModInfoCpy(modPositErr,modbErrDevReadIndex,&modbusLCDErrRead);
//				LDCDispErrMosbusInfo();
//				break;
//			case KEY_MODBUSDISP_ERRNEXT_ADDR:
////				if(modbErrDevReadIndex+1==modbErrTotalIndex)
////					 modbErrDevReadIndex=0;
////				else 
////					 modbErrDevReadIndex++;
//				modbErrDevReadIndexAdd();
//				LCDDispModInfoCpy(modPositErr,modbErrDevReadIndex,&modbusLCDErrRead);
//				LDCDispErrMosbusInfo();
//				break;
////			case  MODBUS_ERR_DISPLAY_ADDR:
////				LCDDispModInfoCpy(modPositErr,modbErrDevReadIndex);
////				LDCDispErrMosbusInfo();
////			break;
//			case  KEY_MODBUSDISP_DEL_ADDR:
//				delModbusDevbyID(modbusLCDRead.ID);
//			  LCDDispModbusGet();
//			 
//			  LCDDispModInfoCpy(modPosit,modbDevReadIndex,&modbusLCDRead);
//			  LDCDispMosbusInfo();
//				break;
//			case  KEY_NETERROR_ADDR:
//				//rt_kprintf("%s��������\n",sign);
//				LCDDispNetOffline();
//				break;
//			case  KEY_MODBUSERR_ADDR:
//				LCDDispErrModbusGet();
//				LCDDispModInfoCpy(modPositErr,modbErrDevReadIndex,&modbusLCDErrRead);
//				LDCDispErrMosbusInfo();
//				break;
//			case	NET_OFFLINE_LAST_ADDR:
////				offLineIndex--;
////			  if(offLineIndex==0){
////						offLineIndex = offLine.times;
////				}
//				offLineIndexLow();
//				LCDDispNetOffline();
//				break;
//			case  NET_OFFLINE_NEXT_ADDR:
////				offLineIndex++;
////			  if(offLineIndex>offLine.times){
////						offLineIndex = 1;
////				}
//				offLineIndexAdd();
//				LCDDispNetOffline();
//				break;
//		//���ؿ���start


//			case  KEY_SWITCH_INTERFACE_ADDR:
//				rt_kprintf("%sKEY_SWITCH_INTERFACE_ADDR \n",sign);
//				dispInterFaceIndexFun();
//				break;
//			
//			case KEY_SWITCH_PORT_ADDR:
//				break;
//			case KEY_SWITCH_LEVEL_ADDR:
//				break;
//			
//			case KEY_SWITCH_SURE_ADDR:
//				break;
//			
//			case KEY_SWITCH_RETURN_ADDR:
//				break;
//			

//			case KEY_SWITCHINTERF_SURE_ADDR://do nothing
//				break;
//			case KEY_SWITCHINTERF_NEXT_ADDR:
//				rt_kprintf("%sKEY_SWITCHINTERF_NEXT_ADDR \n",sign);
//				dispInterFaceIndexAdd();
//			  dispInterFaceIndexFun();
//				break;
//			case KEY_SWITCHINTERF_LAST_ADDR:
//				rt_kprintf("%sKEY_SWITCHINTERF_LAST_ADDR \n",sign);
//				dispInterFaceIndexReduc();
//			  dispInterFaceIndexFun();
//				break;
//			case KEY_SWITCHINTERF_RETURN_ADDR://do nothing
//				break;
//			case KEY_SWITCHPORT_SURE_ADDR:
//				break;
//			case KEY_SWITCHPORT_NEXT_ADDR:
//				break;
//			case KEY_SWITCHPORT_LAST_ADDR:
//				break;
//			case KEY_SWITCHPORT_RETURN_ADDR:
//				break;
//			case KEY_SWITCHLEVEL_ON_ADDR:
//				break;
//			case KEY_SWITCHLEVEL_OFF_ADDR:
//				break;
//			case KEY_SWITCHLEVEL_SURE_ADDR:
//				break;
//			case KEY_SWITCHLEVEL_RETURN_ADDR:
//				break;

//		//���ؿ���end
//		}
//		rt_free(buf);
//		buf=RT_NULL;
//		
//}

//lcd ���������ý���
void LCDDispConfig(uint8_t *recBuf,int len)
{
	  
		if((uint16_t)((recBuf[0]<<8)+recBuf[1])!=LCD_HEAD){
				rt_kprintf("%s head err\n",sign);
				return;
		}			
		if((uint16_t)(recBuf[2]+2+1)!=len){//+2+1  ͷ������2 ���ݳ���1
			  rt_kprintf("%s lenth err\n",sign);
				return;
		}	
		if(recBuf[3]!=LCD_READ){//+2+1  ͷ������2 ���ݳ���1
			  rt_kprintf("%s 0x83 err\n",sign);
				return;
		}				
		uint16_t CMD_ADDR=(uint16_t)(recBuf[4]<<8)+recBuf[5];
		switch(CMD_ADDR){
			case  LOCAL_IP1_ADDR:
				packFlash.netIpFlash.localIp[0]=recBuf[8];
				break;
			case LOCAL_IP2_ADDR:
				packFlash.netIpFlash.localIp[1]=recBuf[8];
				break;       
			case LOCAL_IP3_ADDR:
				packFlash.netIpFlash.localIp[2]=recBuf[8];
				break;        
			case LOCAL_IP4_ADDR:
				packFlash.netIpFlash.localIp[3]=recBuf[8];
				break;        
			case PHY_PORT_ADDR:
				packFlash.netIpFlash.macaddr=recBuf[8];
				break;        
			case REMOTE_IP1_ADDR:
				packFlash.netIpFlash.remoteIp[0]=recBuf[8];
				break;       
			case REMOTE_IP2_ADDR:
				packFlash.netIpFlash.remoteIp[1]=recBuf[8];
				break;        
			case REMOTE_IP3_ADDR:
				packFlash.netIpFlash.remoteIp[2]=recBuf[8];
				break;        
			case REMOTE_IP4_ADDR:
				packFlash.netIpFlash.remoteIp[3]=recBuf[8];
				break;    		 
			case REMOTE_PORT_ADDR:
				packFlash.netIpFlash.remotePort=(uint16_t)(recBuf[9]<<8)+recBuf[10];
				break;   		 

			case PORT1_ADDR:
				packFlash.uartBps[0]=(uint32_t)(recBuf[7]<<24)+(uint32_t)(recBuf[8]<<16)+(uint32_t)(recBuf[9]<<8)+recBuf[10];
				break;     		 			
			case PORT2_ADDR:
				packFlash.uartBps[1]=(uint32_t)(recBuf[7]<<24)+(uint32_t)(recBuf[8]<<16)+(uint32_t)(recBuf[9]<<8)+recBuf[10];
				break;     		 			
			case PORT3_ADDR:
				packFlash.uartBps[2]=(uint32_t)(recBuf[7]<<24)+(uint32_t)(recBuf[8]<<16)+(uint32_t)(recBuf[9]<<8)+recBuf[10];
				break;     		 			
			case PORT4_ADDR:
				packFlash.uartBps[3]=(uint32_t)(recBuf[7]<<24)+(uint32_t)(recBuf[8]<<16)+(uint32_t)(recBuf[9]<<8)+recBuf[10];
				break;     		 			

			//MCUID
			case MCUID_ADDR:
				for(int i=0;i<ACUID_LEN+1;i++){
					packFlash.acuId[i]=recBuf[7+i];
					if((recBuf[7+i]==0xff)||(recBuf[7+i]==0)){
							packFlash.acuId[i]=0;
							break;
					}
				}
				break;         			
			case MODBUS_CFG_NAME_ADDR:
				break; 		
			case MODBUS_CFG_ID_ADDR:
				for(int i=0;i<20;i++){
					LCDInput.ID[i]=recBuf[7+i];
					if((recBuf[7+i]==0xff)||(recBuf[7+i]==0)){
							LCDInput.ID[i]=0;
							break;
					}
				}

				break;    		
			case MODBUS_CFG_SORT_ADDR:
				//LCDInput.
				break; 		
			case MODBUS_CFG_TYPE_ADDR:
				for(int i=0;i<8;i++){
					LCDInput.model[i]=recBuf[7+i];
					if((recBuf[7+i]==0xff)||(recBuf[7+i]==0)){
							LCDInput.model[i]=0;
							break;
					}
				}
				break;  		
			case MODBUS_CFG_PORT_ADDR:
				LCDInput.useUartNum=(uartEnum)recBuf[8];
				break;  		
			case MODBUS_CFG_ADDR_ADDR:
				LCDInput.slaveAddr=recBuf[8];
				break;  		
			case MODBUS_CFG_TIME_ADDR:
				LCDInputTime=(uint32_t)(recBuf[7]<<24)+(uint32_t)(recBuf[8]<<16)+(uint32_t)(recBuf[9]<<8)+recBuf[10];
				break; 		
			case MODBUS_CFG_NAME2_ADDR:
				break;	
			case KEY_ADDR:
				keyReturn((uint16_t)(recBuf[7]<<8)+recBuf[8]);
				break;
		}
}



void keyModbusCfgSure()
{
		modbusConfIDCheck(LCDInput.ID);
		singlModbConf(chinaNameIndex);
}