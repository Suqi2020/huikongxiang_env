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
const static char sign[]="[LCDInputCfg]";

digitStru digInputLCD={0};




#define   KEY_INPUTCFG_NAME_ADDR        	 	0X5004
#define   KEY_INPUTCFG_ID_ADDR        	 	  0X5010
#define   KEY_INPUTCFG_TYPE_ADDR        	  0X5030
#define   KEY_INPUTCFG_PORT_ADDR        	  0X5038



void lcdCopyInputName(uint8_t *rec)
{
	for(int i=0;i<NAME_LEN;i++){
		digInputLCD.name[i]=rec[7+i];
		if((rec[7+i]==0xff)||(rec[7+i]==0)){
				digInputLCD.name[i]=0;
				break;
		}
	}
}


//MODEL_LEN
void lcdCopyInputID(uint8_t *rec)
{
	for(int i=0;i<DEVID_LEN;i++){
		digInputLCD.devID[i]=rec[7+i];
		if((rec[7+i]==0xff)||(rec[7+i]==0)){
				digInputLCD.devID[i]=0;
				break;
		}
	}
}

void lcdCopyInputModel(uint8_t *rec)
{
	for(int i=0;i<MODEL_LEN;i++){
		digInputLCD.model[i]=rec[7+i];
		if((rec[7+i]==0xff)||(rec[7+i]==0)){
				digInputLCD.model[i]=0;
				break;
		}
	}
}

void lcdCopyInputPort(uint8_t *rec)
{

		digInputLCD.port=rec[8];
}



void  lcdInputConfig()
{
		int port=digInputLCD.port;
		if((port<=DI_NUM)&&(port>0)){//添加
				packFlash.diginput[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFlash.diginput[port-1].name, digInputLCD.name);
				rt_strcpy(packFlash.diginput[port-1].devID,digInputLCD.devID);
				rt_strcpy(packFlash.diginput[port-1].model,digInputLCD.model);
				packFlash.diginput[port-1].port=port;
				rt_kprintf("%s add diginput chanl %d\n",sign,port);
        rt_kprintf("%s digInput OK\n",sign);
		}
}








#define   DISP_INNPUT_NAME_ADDR             0x5040
#define   DISP_INNPUT_ID_ADDR               0x5050
#define   DISP_INNPUT_TYPE_ADDR             0x5064
#define   DISP_INNPUT_PORT_ADDR             0x5070
#define   DISP_INNPUT_TOTALNUM_ADDR         0x5072
#define   DISP_INNPUT_THENUM_ADDR           0x5074

#define   KEY_INNPUT_LAST_ADDR              0x5076
#define   KEY_INNPUT_NEXT_ADDR              0x5078
#define   KEY_INNPUT_DEL_ADDR               0x507A
#define   KEY_INNPUT_RETURN_ADDR            0x507C


uint8_t  su8inputTotalNum=0;
uint8_t  su8inputTheNum=0;
uint8_t  su8WorkInput[DI_NUM]={0};//记录真正配置过的角标  没有配置过登记为0

//每次进去界面和删除时候调用此函数
void getInputTotalNum()
{
	su8inputTotalNum=0;
	for(int i=0;i<DI_NUM;i++){
			if(packFlash.diginput[i].workFlag==RT_TRUE){
				su8WorkInput[su8inputTotalNum]=i;
				su8inputTotalNum++;
				rt_kprintf("su8inputTotalNum:%d  %d\n",su8inputTotalNum,packFlash.diginput[i].workFlag);
			}
	}
	for(int j=su8inputTotalNum;j<DI_NUM;j++)
			su8WorkInput[su8inputTotalNum+j]=0;
	if(su8inputTotalNum>0){
			su8inputTheNum=1;
	}
	else{
			su8inputTheNum=0;
	}
}

//上一页
void  lastInput()
{
	  if(su8inputTotalNum==0){
			return;
		}
		
		if(su8inputTheNum==1)
				su8inputTheNum=su8inputTotalNum;//4
		else
				su8inputTheNum--;
	rt_kprintf("last input:%d \n",su8inputTheNum);
}
//下一页
void  nextInput()
{
		if(su8inputTotalNum==0){
			return;
		}
		su8inputTheNum++;
		if(su8inputTheNum>su8inputTotalNum)
				su8inputTheNum=1;
		rt_kprintf("next input:%d \n",su8inputTheNum);

}
//删除
void  delOneInput()
{
	 for(int p=0;p<DI_NUM;p++){
				if(rt_strcmp(packFlash.diginput[p].devID,packFlash.diginput[su8WorkInput[su8inputTheNum-1]].devID)==0){

						packFlash.diginput[p].workFlag=RT_FALSE;
						rt_kprintf("del one:%d %d %d\n",p,su8inputTotalNum,su8inputTheNum);
					  rt_kprintf("del one:%s %s %s\n",packFlash.diginput[p].name,packFlash.diginput[p].devID,packFlash.diginput[p].model);
				}
		}
//		if(su8inputTotalNum>0){
//				su8inputTotalNum--;
//				for(int p=0;p<DI_NUM;p++){
//					//if((p+1)==su8inputTheNum)
//						if(packFlash.diginput[p].workFlag==RT_TRUE){{
//								//su8inputTheNum=p+1;
//							break;
//						}
//						}
//				}
//		}
		
		
		
}


void  dispInput()
{
		uint8_t *buf=NULL;
	  buf=rt_malloc(50);
		if(su8inputTotalNum==0){
				buf[0]=0xff;
				buf[1]=0xff;
				//显示name
				LCDWtite(DISP_INNPUT_NAME_ADDR,buf,2);
				//显示ID
				LCDWtite(DISP_INNPUT_ID_ADDR,buf,2);//7寸屏显示18
				//显示model
				LCDWtite(DISP_INNPUT_TYPE_ADDR,buf,2);
				//显示PORT
				LCDWtite(DISP_INNPUT_PORT_ADDR,buf,2);
				//显示总共页
				buf[0]=0;
				buf[1]=0;
				LCDWtite(DISP_INNPUT_TOTALNUM_ADDR,buf,2);
				//显示当前页
				buf[0]=0;
				buf[1]=0;
				LCDWtite(DISP_INNPUT_THENUM_ADDR,buf,2);

				return;
		}
	  //显示中文名
//		for(int p=0;p<su8inputTheNum;p++){
//				if((p+1)==su8inputTheNum){
		        uint8_t p=su8WorkInput[su8inputTheNum-1];
						//if(packFlash.diginput[p].workFlag==RT_TRUE){
							int Len=strlen(packFlash.diginput[p].name);
							for(int i=0;i<Len;i++){
									buf[i]=packFlash.diginput[p].name[i];
							}
							int j=0;
							while((Len+j)<NAME_LEN){
									buf[Len+j]=0xff;
									j++;
							}
							LCDWtite(DISP_INNPUT_NAME_ADDR,buf,NAME_LEN);
							//显示ID
							 Len=strlen(packFlash.diginput[p].devID);
							for(int i=0;i<Len;i++){
									buf[i]=packFlash.diginput[p].devID[i];
							}
							 j=0;
							while((Len+j)<DEVID_LEN){
									buf[Len+j]=0xff;
									j++;
									if(j>=2)
										break;
							}
							LCDWtite(DISP_INNPUT_ID_ADDR,buf,DEVID_LEN);//7寸屏显示18
							//显示model
							Len=strlen(packFlash.diginput[p].model);
							for(int i=0;i<Len;i++){
									buf[i]=packFlash.diginput[p].model[i];
							}
							 j=0;
							while((Len+j)<MODEL_LEN){
									buf[Len+j]=0xff;
									j++;
							}
							LCDWtite(DISP_INNPUT_TYPE_ADDR,buf,MODEL_LEN);
							//显示PORT
							buf[0]=0;
							buf[1]=packFlash.diginput[p].port;
							LCDWtite(DISP_INNPUT_PORT_ADDR,buf,2);

							//显示总共页
							buf[0]=0;
							buf[1]=su8inputTotalNum;
							LCDWtite(DISP_INNPUT_TOTALNUM_ADDR,buf,2);
							rt_kprintf("%s total %d\n",sign,su8inputTotalNum);
							//显示当前页
							buf[0]=0;
				
							buf[1]=su8inputTheNum;
							LCDWtite(DISP_INNPUT_THENUM_ADDR,buf,2);
						//}
//						 break;
//				}
//		}
			
		rt_free(buf);
		buf=RT_NULL;
}


