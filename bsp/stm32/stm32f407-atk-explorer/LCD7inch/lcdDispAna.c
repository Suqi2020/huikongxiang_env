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
const static char sign[]="[LCDDispAna]";



analogStru  agaInput={0};





uint8_t  su8AnaTotalNum=0;//显示总页数
uint8_t  su8AnaTheNum=0;  //显示第几页
uint8_t  su8AnaWork[ANALOG_NUM]={0};//记录真正配置过的角标  没有配置过登记为0



//						for(int j=0;j<ANALOG_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
//										if((rt_strcmp(sheet.analog[j].ID,argv[2])==0)&&(subName==sheet.analog[j].subName )){
//												sheet.analog[j].workFlag=RT_FALSE;
//												rt_kprintf("%s close chanl%d\n",sign,j+1);
//												return;
//										}
//								 }
						
								 
								 
//每次进去界面和删除时候调用此函数
void getAnaTotalNum()
{
	
		su8AnaTotalNum=0;
		for(int i=0;i<ANALOG_NUM;i++){
				if(sheet.analog[i].workFlag==RT_TRUE){
					su8AnaWork[su8AnaTotalNum]=i;
					su8AnaTotalNum++;
					rt_kprintf("su8anaTotalNum:%d  %d\n",su8AnaTotalNum,sheet.analog[i].workFlag);
				}
		}
		for(int j=su8AnaTotalNum;j<DI_NUM;j++)//没存储的标记的就清0
				su8AnaWork[su8AnaTotalNum+j]=0;
		if(su8AnaTotalNum>0){
				su8AnaTheNum=1;
		}
		else{
				su8AnaTheNum=0;
		}
}



//上一页
void  lastAna()
{
	  if(su8AnaTotalNum==0){
				return;
		}
		
		if(su8AnaTheNum==1)
				su8AnaTheNum=su8AnaTotalNum;//4
		else
				su8AnaTheNum--;
	rt_kprintf("last input:%d \n",su8AnaTheNum);
}
//下一页
void  nextAna()
{
		if(su8AnaTotalNum==0){
				return;
		}
		su8AnaTheNum++;
		if(su8AnaTheNum>su8AnaTotalNum)
				su8AnaTheNum=1;
		rt_kprintf("next input:%d \n",su8AnaTheNum);

}
//删除
void  delOneAna()
{
	 for(int p=0;p<DI_NUM;p++){
				if(rt_strcmp(sheet.analog[p].ID,sheet.analog[su8AnaWork[su8AnaTheNum-1]].ID)==0){

						sheet.analog[p].workFlag=RT_FALSE;
						rt_kprintf("del one:%d %d %d\n",p,su8AnaTotalNum,su8AnaTheNum);
					  rt_kprintf("del one:%s %s %s\n",sheet.analog[p].name,sheet.analog[p].ID,sheet.analog[p].model);
				}
		}
}



//输入显示发数据给LCD
void  dispAna()
{
		uint8_t *buf=NULL;
	  buf=rt_malloc(50);
		if(su8AnaTotalNum==0){
				buf[0]=0xff;
				buf[1]=0xff;
				//显示name
				LCDWtite(DISP_ANAREAD_NAME_ADDR,buf,2);
			  //显示子name
			  LCDWtite(DISP_ANAREAD_SUBNAME_ADDR,buf,2);
				//显示ID
				LCDWtite(DISP_ANAREAD_ID_ADDR,buf,2);//7寸屏显示18
				//显示model
				LCDWtite(DISP_ANAREAD_TYPE_ADDR,buf,2);
				//显示PORT
				LCDWtite(DISP_ANAREAD_PORT_ADDR,buf,2);
			  //显示tme
			  LCDWtite(DISP_ANAREAD_PORT_ADDR,buf,2);
				//显示总共页
				buf[0]=0;
				buf[1]=0;
				LCDWtite(DISP_ANAREAD_TOTALNUM_ADDR,buf,2);
				//显示当前页
				buf[0]=0;
				buf[1]=0;
				LCDWtite(DISP_ANAREAD_THENUM_ADDR,buf,2);

				return;
		}
	  //显示中文名
		uint8_t p=su8AnaWork[su8AnaTheNum-1];

		int Len=strlen(sheet.analog[p].name);
		for(int i=0;i<Len;i++){
				buf[i]=sheet.analog[p].name[i];
		}
		int j=0;
		while((Len+j)<NAME_LEN){
				buf[Len+j]=0xff;
				j++;
		}
		LCDWtite(DISP_ANAREAD_NAME_ADDR,buf,NAME_LEN);
		if((sheet.analog[p].subName>2)||(sheet.analog[p].subName==0)){
			sheet.analog[p].subName=1;
		}

		//显示sub中文名
		Len=strlen(anaSubName1Val[sheet.analog[p].subName-1]);
		for(int i=0;i<Len;i++){
				buf[i]=anaSubName1Val[sheet.analog[p].subName-1][i];
		}
		j=0;
		while((Len+j)<NAME_LEN){
				buf[Len+j]=0xff;
				j++;
		}
		LCDWtite(DISP_ANAREAD_SUBNAME_ADDR,buf,NAME_LEN);
		//显示ID
		 Len=strlen(sheet.analog[p].ID);
		for(int i=0;i<Len;i++){
				buf[i]=sheet.analog[p].ID[i];
		}
		 j=0;
		while((Len+j)<DEVID_LEN){
				buf[Len+j]=0xff;
				j++;
				if(j>=2)
					break;
		}
		LCDWtite(DISP_ANAREAD_ID_ADDR,buf,DEVID_LEN);//7寸屏显示18
		//显示model
		Len=strlen(sheet.analog[p].model);
		for(int i=0;i<Len;i++){
				buf[i]=sheet.analog[p].model[i];
		}
		 j=0;
		while((Len+j)<MODEL_LEN){
				buf[Len+j]=0xff;
				j++;
		}
		LCDWtite(DISP_ANAREAD_TYPE_ADDR,buf,MODEL_LEN);
		//显示PORT
		buf[0]=0;
		buf[1]=sheet.analog[p].port;
		LCDWtite(DISP_ANAREAD_PORT_ADDR,buf,2);

		//显示总共页
		buf[0]=0;
		buf[1]=su8AnaTotalNum;
		LCDWtite(DISP_ANAREAD_TOTALNUM_ADDR,buf,2);
		rt_kprintf("%s total %d\n",sign,su8AnaTotalNum);
		//显示当前页
		buf[0]=0;

		buf[1]=su8AnaTheNum;
		LCDWtite(DISP_ANAREAD_THENUM_ADDR,buf,2);
			
		rt_free(buf);
		buf=RT_NULL;
}


