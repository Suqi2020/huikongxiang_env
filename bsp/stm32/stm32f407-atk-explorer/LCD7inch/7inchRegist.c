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
const static char sign[]="[LCD]";
extern struct  rt_messagequeue LCDmque;

uint8_t  sendLCDBuf[LCD_BUF_LEN];
uint8_t  recLCDBuf[LCD_BUF_LEN];



//往LCD屏幕发送数据 调用底层串口发送函数
static void LCDDataSend(uint8_t *buf,int lenth)
{
	 //rt_kprintf("%s LCD send:",sign);
	 for(int i=0;i<lenth;i++){
		 HAL_UART_Transmit(&huart5,buf+i,1,1000); 
		 //rt_kprintf("%02x ",buf[i]);
	 }
	 //rt_kprintf("\n");
	
}
//结果 return 0-回应错误  1-回复正确
//5A A5 03 82 4F 4B 
static int LCDWriteResp(uint8_t *recBuf,int lenth)
{
		if(lenth<4)
				return 0;
		if((recBuf[0]==(uint8_t)(LCD_HEAD>>8))&&\
		 (recBuf[1]==(uint8_t)(LCD_HEAD))&&\
		 (recBuf[3]==LCD_WRITE)&&\
		 (recBuf[4]==(uint8_t)(LCD_RUN>>8))&&\
		 (recBuf[5]==(uint8_t)(LCD_RUN))){
			  // rt_kprintf("%sresponse\n",sign);
				 return 1;
		}
		return 0;
}
//结果 return 0 无响应  1 正常响应 
int LCDWtite(uint16_t addr,uint8_t *data,uint8_t dataLen)
{
	  //data pack
	  int len=0;
		rt_memset(sendLCDBuf,0,LCD_BUF_LEN);
		rt_memset(recLCDBuf,0,LCD_BUF_LEN);
		sendLCDBuf[len++]=(uint8_t)(LCD_HEAD>>8);
	  sendLCDBuf[len++]=(uint8_t)LCD_HEAD;		
	  sendLCDBuf[len++]=dataLen+2+1;						//长度暂时填充0  2-headlen 1-writelen
	  sendLCDBuf[len++]=LCD_WRITE;  						
		sendLCDBuf[len++]=(uint8_t)(addr>>8);	
		sendLCDBuf[len++]=(uint8_t)addr;		
	  for (int i=0;i<dataLen;i++){
				sendLCDBuf[len++]=data[i];
		}
    int repTimes=2;
		while(repTimes--){
			//data send
				LCDDataSend(sendLCDBuf,len);
			//data rec
				int revLen=0;
				if(rt_mq_recv(&LCDmque, recLCDBuf+revLen, 1, 25) == RT_EOK){
					revLen++;
				}
				while(rt_mq_recv(&LCDmque, recLCDBuf+revLen, 1, 2) == RT_EOK){
					revLen++;
				}
				if(revLen){
					return LCDWriteResp(recLCDBuf,revLen);
				}
		}
		return 0;
}



//网络配置显示
void LCDDispIP()
{
	  uint8_t buf[10]={0};
		//显示IP
		buf[0]=0;
		buf[1]=packFLash.netIpFlash.localIp[0];
		LCDWtite(LOCAL_IP1_ADDR,buf,1*2);
		buf[1]=packFLash.netIpFlash.localIp[1];
		LCDWtite(LOCAL_IP2_ADDR,buf,1*2);
		buf[1]=packFLash.netIpFlash.localIp[2];
		LCDWtite(LOCAL_IP3_ADDR,buf,1*2);
		buf[1]=packFLash.netIpFlash.localIp[3];
		LCDWtite(LOCAL_IP4_ADDR,buf,1*2);
	  //显示端口
		buf[1]=packFLash.netIpFlash.macaddr;
		LCDWtite(PHY_PORT_ADDR,buf,1*2);
		//远端IP
		buf[1]=packFLash.netIpFlash.remoteIp[0];
		LCDWtite(REMOTE_IP1_ADDR,buf,1*2);
		buf[1]=packFLash.netIpFlash.remoteIp[1];
		LCDWtite(REMOTE_IP2_ADDR,buf,1*2);
		buf[1]=packFLash.netIpFlash.remoteIp[2];
		LCDWtite(REMOTE_IP3_ADDR,buf,1*2);
		buf[1]=packFLash.netIpFlash.remoteIp[3];
		LCDWtite(REMOTE_IP4_ADDR,buf,1*2);
		//远端port
		buf[0]=0;
		buf[1]=0;
		buf[2]=(uint8_t)(packFLash.netIpFlash.remotePort>>8);
		buf[3]=(uint8_t)(packFLash.netIpFlash.remotePort);
		LCDWtite(REMOTE_PORT_ADDR,buf,2*2);
}
//串口配置显示
void LCDDispUart()
{
	  uint8_t buf[10]={0};
		//显示端口1波特率
		buf[0]=(uint8_t)(packFLash.uartBps[0]>>24);
		buf[1]=(uint8_t)(packFLash.uartBps[0]>>16);
		buf[2]=(uint8_t)(packFLash.uartBps[0]>>8);
		buf[3]=(uint8_t)(packFLash.uartBps[0]>>0);
		LCDWtite(PORT1_ADDR,buf,2*2);
		
		//显示端口2波特率
		buf[0]=(uint8_t)(packFLash.uartBps[1]>>24);
		buf[1]=(uint8_t)(packFLash.uartBps[1]>>16);
		buf[2]=(uint8_t)(packFLash.uartBps[1]>>8);
		buf[3]=(uint8_t)(packFLash.uartBps[1]>>0);
		LCDWtite(PORT2_ADDR,buf,2*2);
		//显示端口3波特率
		buf[0]=(uint8_t)(packFLash.uartBps[2]>>24);
		buf[1]=(uint8_t)(packFLash.uartBps[2]>>16);
		buf[2]=(uint8_t)(packFLash.uartBps[2]>>8);
		buf[3]=(uint8_t)(packFLash.uartBps[2]>>0);
		LCDWtite(PORT3_ADDR,buf,2*2);
		//显示端口4波特率
		buf[0]=(uint8_t)(packFLash.uartBps[3]>>24);
		buf[1]=(uint8_t)(packFLash.uartBps[3]>>16);
		buf[2]=(uint8_t)(packFLash.uartBps[3]>>8);
		buf[3]=(uint8_t)(packFLash.uartBps[3]>>0);
		LCDWtite(PORT4_ADDR,buf,2*2);
}


//lcd屏幕配置串口波特率
//void LCDConfUart()111
//{
//	  uint8_t buf[10]={0};
//		//显示端口1波特率
//		buf[0]=(uint8_t)(packFLash.uartBps[0]>>24);
//		buf[1]=(uint8_t)(packFLash.uartBps[0]>>16);
//		buf[2]=(uint8_t)(packFLash.uartBps[0]>>8);
//		buf[3]=(uint8_t)(packFLash.uartBps[0]>>0);
//		LCDWtite(PORT1_ADDR,buf,2*2);
//		
//		//显示端口2波特率
//		buf[0]=(uint8_t)(packFLash.uartBps[1]>>24);
//		buf[1]=(uint8_t)(packFLash.uartBps[1]>>16);
//		buf[2]=(uint8_t)(packFLash.uartBps[1]>>8);
//		buf[3]=(uint8_t)(packFLash.uartBps[1]>>0);
//		LCDWtite(PORT2_ADDR,buf,2*2);
//		//显示端口3波特率
//		buf[0]=(uint8_t)(packFLash.uartBps[2]>>24);
//		buf[1]=(uint8_t)(packFLash.uartBps[2]>>16);
//		buf[2]=(uint8_t)(packFLash.uartBps[2]>>8);
//		buf[3]=(uint8_t)(packFLash.uartBps[2]>>0);
//		LCDWtite(PORT3_ADDR,buf,2*2);
//		//显示端口4波特率
//		buf[0]=(uint8_t)(packFLash.uartBps[3]>>24);
//		buf[1]=(uint8_t)(packFLash.uartBps[3]>>16);
//		buf[2]=(uint8_t)(packFLash.uartBps[3]>>8);
//		buf[3]=(uint8_t)(packFLash.uartBps[3]>>0);
//		LCDWtite(PORT4_ADDR,buf,2*2);
//}
//LCD显示MCUID
int k=0;
void LCDDispMCUID()
{
	  uint8_t buf[ACUID_LEN]={0};
		k=sizeof(packFLash.acuId);
		for(int i=0;i<sizeof(packFLash.acuId);i++){//ACUID_LEN
			  if((uint8_t)packFLash.acuId[i]==0xff){
					 packFLash.acuId[i]=0;
				}
			 	buf[i]=packFLash.acuId[i];
		}
		int j=0;
		k=strlen(packFLash.acuId);
		while((strlen(packFLash.acuId)+j)<ACUID_LEN){
				buf[strlen(packFLash.acuId)+j]=0xff;
				j++;
		}
		LCDWtite(MCUID_ADDR,buf,ACUID_LEN);
}


//5A A5 05 82 1A0C 0001 停止
//5A A5 05 82 1A0C 0000  开始
//更新lcd网络状态 上下线或者触摸按键时候调用
void LCDDispNetErrState()
{
	  extern rt_bool_t gbNetState;
//	  static rt_bool_t state =RT_FALSE;//屏幕上电后默认显示也是掉线的
//	  if(state!=gbNetState){//状态变更时候再去更新
				uint8_t buf[10]={0};
				buf[0]=0;
				buf[1]=gbNetState;
				LCDWtite(NET_ERR_DISPLAY_ADDR,buf,1*2);

}
//串口显示掉线次数
int offLineIndex=1;
void  LCDDispNetOffline()
{
	  uint8_t buf[10]={0};
		extern rt_bool_t gbNetState;
		//显示总共掉线次数
		buf[0]=(uint8_t)(offLine.times>>24);
		buf[1]=(uint8_t)(offLine.times>>16);
		buf[2]=(uint8_t)(offLine.times>>8);
		buf[3]=(uint8_t)(offLine.times>>0);
		LCDWtite(NET_OFFLINE_TOTALTIMES_ADDR,buf,2*2);
		if(offLine.times==0){
				//显示第几次掉线
				buf[0]=0;
				buf[1]=0;
				buf[2]=0;
				buf[3]=0;
				LCDWtite(NET_OFFLINE_TIMES_ADDR,buf,2*2);
				
						//显示总共掉线的时长
				if(gbNetState==RT_FALSE){
					  int tick = rt_tick_get()/1000;
			
						buf[0]=(uint8_t)(tick>>24);
						buf[1]=(uint8_t)(tick>>16);
						buf[2]=(uint8_t)(tick>>8);
						buf[3]=(uint8_t)(tick>>0);
				}

				LCDWtite(NET_OFFLINE_RELAYTIME_ADDR,buf,2*2);
		}
		else{ 
				buf[0]=(uint8_t)(offLineIndex>>24);
				buf[1]=(uint8_t)(offLineIndex>>16);
				buf[2]=(uint8_t)(offLineIndex>>8);
				buf[3]=(uint8_t)(offLineIndex>>0);
				LCDWtite(NET_OFFLINE_TIMES_ADDR,buf,2*2);
			  int offTime;
				if(gbNetState==RT_FALSE){
						//rt_kprintf("[offLine]a the %d Time,relayTimer %d %d秒\r\n",offLineIndex,rt_tick_get()/1000,offLine.relayTimer[offLineIndex]);
						if(offLineIndex==offLine.times)
								offTime=(rt_tick_get()/1000-offLine.relayTimer[offLineIndex]);//掉线了此次计数一直++
						else
								offTime=offLine.relayTimer[offLineIndex];
				}
				else{
						//rt_kprintf("[offLine]b the %d Times,relayTimer %d 秒\r\n",offLineIndex,offLine.relayTimer[offLineIndex]);
						offTime=offLine.relayTimer[offLineIndex];
				}
				buf[0]=(uint8_t)(offTime>>24);
				buf[1]=(uint8_t)(offTime>>16);
				buf[2]=(uint8_t)(offTime>>8);
				buf[3]=(uint8_t)(offTime>>0);
				LCDWtite(NET_OFFLINE_RELAYTIME_ADDR,buf,2*2);
		}
		LCDDispNetErrState();
}






									
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
					case TEMPHUM:
						sheet.tempHum[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
					case WATERDEPTH:
						sheet.waterDepth[modPosit[modbDevReadIndex].Y].workFlag=RT_FALSE;
						break;
					default:
						rt_kprintf("%serror delModbusDevbyID\n",sign);
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

//故障传感器读取界面显示所有信息
void LDCDispErrMosbusInfo()
{
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
				default:
					rt_kprintf("%serror LCDDispModbusGet\n",sign);
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
//上电后控件第一次显示的中文名称拷贝 
//传感器读取界面显示所有信息
void LDCDispMosbusInfo()
{
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
		else
				buf[1]=modbDevReadIndex+1;
		LCDWtite(MODBUSDISP_NOWNUM_ADDR,buf,2);
		
		
		rt_free(buf);
		buf=RT_NULL;
}

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
				case CH4:
					for(int j=0;j<CH4_485_NUM;j++){//核对有没有配置过
							if(posit_p.Y==j){
								 if(posit_p.flag==1){
									 rt_strcpy(lcdRead->name,modbusName[posit_p.X]);
									 rt_strcpy(lcdRead->ID,  sheet.ch4[posit_p.Y].ID);
									 rt_strcpy(lcdRead->model,sheet.ch4[posit_p.Y].model);
									 lcdRead->port = sheet.ch4[posit_p.Y].useUartNum;
									 lcdRead->addr = sheet.ch4[posit_p.Y].slaveAddr;
									 lcdRead->colTime =sheet.coColTime;// //sheet.ch4ColTime;
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
									 lcdRead->colTime = sheet.coColTime;//sheet.o2ColTime;
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
									 lcdRead->colTime = sheet.coColTime;//sheet.h2sColTime;
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
										 lcdRead->colTime = sheet.coColTime;
								 }
								 break;
							}
					}
					break;
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
				default:
					rt_kprintf("%serror delModbusDevbyID\n",sign);
					break;
			}
}

//void LCDDispModInfoCpy()
//{
//	 
//		switch(modPosit[modbDevReadIndex].X){
//				case CIRCULA:	
//					for(int j=0;j<CIRCULA_485_NUM;j++){//核对有没有配置过
//							if(modPosit[modbDevReadIndex].Y==j){
//									if(modPosit[modbDevReadIndex].flag==1){
//											 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//											 rt_strcpy(modbusLCDRead.ID,  sheet.cirCula[modPosit[modbDevReadIndex].Y].ID);//ID第一次上电存储的可能是0xff  拷贝会导致内存泄漏
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
//					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//核对有没有配置过
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
//					for(int j=0;j<PRESSSETTL_485_NUM;j++){//核对有没有配置过
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
//					for(int j=0;j<THREEAXIS_485_NUM;j++){//核对有没有配置过
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
//					for(int j=0;j<CH4_485_NUM;j++){//核对有没有配置过
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
//					for(int j=0;j<O2_485_NUM;j++){//核对有没有配置过
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
//					for(int j=0;j<H2S_485_NUM;j++){//核对有没有配置过
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
//					for(int j=0;j<CO_485_NUM;j++){//核对有没有配置过
//							if(modPosit[modbDevReadIndex].Y==j){
//								 if(modPosit[modbDevReadIndex].flag==1){
//										 rt_strcpy(modbusLCDRead.name,modbusName[modPosit[modbDevReadIndex].X]);
//										 rt_strcpy(modbusLCDRead.ID,  sheet.co[modPosit[modbDevReadIndex].Y].ID);
//										 rt_strcpy(modbusLCDRead.model,sheet.co[modPosit[modbDevReadIndex].Y].model);
//										 modbusLCDRead.port = sheet.co[modPosit[modbDevReadIndex].Y].useUartNum;
//										 modbusLCDRead.addr = sheet.co[modPosit[modbDevReadIndex].Y].slaveAddr;
//										 modbusLCDRead.colTime = sheet.coColTime;
//								 }
//								 break;
//							}
//					}
//					break;
//				case TEMPHUM:
//					for(int j=0;j<TEMPHUM_485_NUM;j++){//核对有没有配置过
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
//					for(int j=0;j<WATERDEPTH_485_NUM;j++){//核对有没有配置过
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
				default:
					rt_kprintf("%serror LCDDispModbusGet\n",sign);
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
uint8_t numTable[]={CIRCULA_485_NUM,PARTDISCHAG_485_NUM ,PRESSSETTL_485_NUM,THREEAXIS_485_NUM,\
										CH4_485_NUM,CO_485_NUM,H2S_485_NUM,O2_485_NUM ,WATERDEPTH_485_NUM,TEMPHUM_485_NUM };

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
static void dispCinaName(uint8_t *buf)
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


extern int modbusConfIDCheck(char *inputID);
//modbusName[MODBUS_NUM][20]
//按键触摸返回函数
void  keyReturn(uint16_t keyAddr)
{
	  uint8_t *buf=NULL;
	  buf=rt_malloc(50);
	 // int nameLen=0;
		switch(keyAddr)
		{
			case	KEY_MODBUS_CFG_NAME_ADDR://点击传感器设置名称显示框调出 模糊界面  显示到1380
				//5A A5 1182 1380 C9CF BAA3 B9C8 D4AA BFC6 BCBC FFFF
        dispCinaName(buf);
				break;
			case	KEY_MODBUS_LASTNAME_ADDR:
				if(chinaNameIndex==0)
						chinaNameIndex=MODBUS_NUM-1;
				else
						chinaNameIndex--;
				dispCinaName(buf);
				break;
			case	KEY_MODBUS_NEXTNAME_ADDR:
				if(chinaNameIndex==(MODBUS_NUM-1))
						chinaNameIndex=0;
				else
						chinaNameIndex++;
				dispCinaName(buf);
				break;
			case	KEY_MODBUS_SURENAME_ADDR:// 显示到1360
//			  dispCinaName(buf);   singlModbConf(chinaNameIndex);
//			  rt_strcpy(LCDInput.);
//				LCDWtite(MODBUS_CFG_NAME_ADDR,buf,sizeof(modbusName[chinaNameNum]));
				break;	
			case  KEY_IP_READ_ADDR:
				LCDDispIP();
				break;
			case KEY_PORT_READ_ADDR:
				LCDDispUart();
				break;
			case KEY_MCUID_READ_ADDR:
				LCDDispMCUID();
				break;
			case KEY_SAVE_ADDR://保存
				
				rt_kprintf("%sflash save OK\n",sign);
				stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFLash));//每次擦除128k字节数据 存储时候需要一起存储
				stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFLash,sizeof(packFLash));
				stm32_flash_write(FLASH_MODBUS_SAVE_ADDR,(uint8_t*)&sheet,sizeof(sheet));
				break;
			case KEY_RESET_ADDR://复位
				rt_hw_cpu_reset();
				break;
			case KEY_MODBUS_CFG_WATCH_ADDR:
				LCDDispModInfoCpy(modPosit,modbDevReadIndex,&modbusLCDRead);
				LDCDispMosbusInfo();
				break;
			case KEY_MODBUS_CFG_SURE_ADDR:
				
				modbusConfIDCheck(LCDInput.ID);
				singlModbConf(chinaNameIndex);
				break;
			case KEY_MODBUSDISP_LAST_ADDR:
			  if(modbDevReadIndex==0)
					 modbDevReadIndex=modbTotalIndex-1;
				else
					 modbDevReadIndex--;
				LCDDispModInfoCpy(modPosit,modbDevReadIndex,&modbusLCDRead);
				LDCDispMosbusInfo();
				break;

			case  KEY_MODBUSDISP_NEXT_ADDR:
			  if(modbDevReadIndex+1==modbTotalIndex)
					 modbDevReadIndex=0;
				else 
					 modbDevReadIndex++;
				
				LCDDispModInfoCpy(modPosit,modbDevReadIndex,&modbusLCDRead);
				LDCDispMosbusInfo();
			  break;
			
		
//LCDDispModInfoStru  modbusLCDErrRead={0};
//uint8_t  modbErrTotalIndex=0;
//uint8_t  modbErrDevReadIndex=0;
//modbusPositStru  modPositErr[TOTOLA_485_NUM]={0};
//////////////////////////////////////////////////

			case KEY_MODBUSDISP_ERRLAST_ADDR:
			  if(modbErrDevReadIndex==0)
					 modbErrDevReadIndex=modbErrTotalIndex-1;
				else
					 modbErrDevReadIndex--;
				LCDDispModInfoCpy(modPositErr,modbErrDevReadIndex,&modbusLCDErrRead);
				LDCDispErrMosbusInfo();
				break;
			case KEY_MODBUSDISP_ERRNEXT_ADDR:
				if(modbErrDevReadIndex+1==modbErrTotalIndex)
					 modbErrDevReadIndex=0;
				else 
					 modbErrDevReadIndex++;
				LCDDispModInfoCpy(modPositErr,modbErrDevReadIndex,&modbusLCDErrRead);
				LDCDispErrMosbusInfo();
				break;
//			case  MODBUS_ERR_DISPLAY_ADDR:
//				LCDDispModInfoCpy(modPositErr,modbErrDevReadIndex);
//				LDCDispErrMosbusInfo();
//			break;
			case  KEY_MODBUSDISP_DEL_ADDR:
				delModbusDevbyID(modbusLCDRead.ID);
			  LCDDispModbusGet();
			 
			  LCDDispModInfoCpy(modPosit,modbDevReadIndex,&modbusLCDRead);
			  LDCDispMosbusInfo();
				break;
			case  KEY_NETERROR_ADDR:
				//rt_kprintf("%s按键按下\n",sign);
				LCDDispNetOffline();
				break;
			case  KEY_MODBUSERR_ADDR:
				LCDDispErrModbusGet();
				LCDDispModInfoCpy(modPositErr,modbErrDevReadIndex,&modbusLCDErrRead);
				LDCDispErrMosbusInfo();
				break;
			case	NET_OFFLINE_LAST_ADDR:
				offLineIndex--;
			  if(offLineIndex==0){
						offLineIndex = offLine.times;
				}
				LCDDispNetOffline();
				break;
			case  NET_OFFLINE_NEXT_ADDR:
				offLineIndex++;
			  if(offLineIndex>offLine.times){
						offLineIndex = 1;
				}
				LCDDispNetOffline();
				break;
		}
		rt_free(buf);
		buf=RT_NULL;
		
}

//lcd 发来的配置解析
void LCDDispConfig(uint8_t *recBuf,int len)
{
	  
		if((uint16_t)((recBuf[0]<<8)+recBuf[1])!=LCD_HEAD){
				rt_kprintf("%s head err\n",sign);
				return;
		}			
		if((uint16_t)(recBuf[2]+2+1)!=len){//+2+1  头部长度2 数据长度1
			  rt_kprintf("%s lenth err\n",sign);
				return;
		}	
		if(recBuf[3]!=LCD_READ){//+2+1  头部长度2 数据长度1
			  rt_kprintf("%s 0x83 err\n",sign);
				return;
		}				
		uint16_t CMD_ADDR=(uint16_t)(recBuf[4]<<8)+recBuf[5];
		switch(CMD_ADDR){
			case  LOCAL_IP1_ADDR:
				packFLash.netIpFlash.localIp[0]=recBuf[8];
				break;
			case LOCAL_IP2_ADDR:
				packFLash.netIpFlash.localIp[1]=recBuf[8];
				break;       
			case LOCAL_IP3_ADDR:
				packFLash.netIpFlash.localIp[2]=recBuf[8];
				break;        
			case LOCAL_IP4_ADDR:
				packFLash.netIpFlash.localIp[3]=recBuf[8];
				break;        
			case PHY_PORT_ADDR:
				packFLash.netIpFlash.macaddr=recBuf[8];
				break;        
			case REMOTE_IP1_ADDR:
				packFLash.netIpFlash.remoteIp[0]=recBuf[8];
				break;       
			case REMOTE_IP2_ADDR:
				packFLash.netIpFlash.remoteIp[1]=recBuf[8];
				break;        
			case REMOTE_IP3_ADDR:
				packFLash.netIpFlash.remoteIp[2]=recBuf[8];
				break;        
			case REMOTE_IP4_ADDR:
				packFLash.netIpFlash.remoteIp[3]=recBuf[8];
				break;    		 
			case REMOTE_PORT_ADDR:
				packFLash.netIpFlash.remotePort=(uint16_t)(recBuf[9]<<8)+recBuf[10];
				break;   		 

			case PORT1_ADDR:
				packFLash.uartBps[0]=(uint32_t)(recBuf[7]<<24)+(uint32_t)(recBuf[8]<<16)+(uint32_t)(recBuf[9]<<8)+recBuf[10];
				break;     		 			
			case PORT2_ADDR:
				packFLash.uartBps[1]=(uint32_t)(recBuf[7]<<24)+(uint32_t)(recBuf[8]<<16)+(uint32_t)(recBuf[9]<<8)+recBuf[10];
				break;     		 			
			case PORT3_ADDR:
				packFLash.uartBps[2]=(uint32_t)(recBuf[7]<<24)+(uint32_t)(recBuf[8]<<16)+(uint32_t)(recBuf[9]<<8)+recBuf[10];
				break;     		 			
			case PORT4_ADDR:
				packFLash.uartBps[3]=(uint32_t)(recBuf[7]<<24)+(uint32_t)(recBuf[8]<<16)+(uint32_t)(recBuf[9]<<8)+recBuf[10];
				break;     		 			

			//MCUID
			case MCUID_ADDR:
				for(int i=0;i<ACUID_LEN+1;i++){
					packFLash.acuId[i]=recBuf[7+i];
					if((recBuf[7+i]==0xff)||(recBuf[7+i]==0)){
							packFLash.acuId[i]=0;
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
				LCDInput.useUartNum=recBuf[8];
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
