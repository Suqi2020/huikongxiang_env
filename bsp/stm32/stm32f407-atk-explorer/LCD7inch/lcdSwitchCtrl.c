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
const static char sign[]="[LCDSwtich]";
extern char outName[OUTNAME_NUM][INOUTNAME_LEN];
int switchInterfaceIndex=0;
int SwitchPortIndex=0;




//向接口框显示接口名称
void dispInterFaceIndexFun()
{
		rt_kprintf("%\n ");
		for(int i=0;i<INOUTNAME_LEN;i++){//ACUID_LEN
			 	rt_kprintf("%0x ",outName[switchInterfaceIndex][i]);
		}
		rt_kprintf("%\n ");
		LCDWtite(DISP_SWITCH_INTERFACE_ADDR,(uint8_t *)outName[switchInterfaceIndex],INOUTNAME_LEN);
		LCDWtite(DISP_SWITCHINTERF_INTERFACE_ADDR,(uint8_t *)outName[switchInterfaceIndex],INOUTNAME_LEN);
}
//接口名称增加
void dispInterFaceIndexAdd()
{
	
	if(switchInterfaceIndex<OUTNAME_NUM){
		switchInterfaceIndex++;
	}
  else
		switchInterfaceIndex=0;
}
//接口名称减少
void dispInterFaceIndexReduc()
{
	if(switchInterfaceIndex==0){
		switchInterfaceIndex=OUTNAME_NUM-1;
	}
  else
		switchInterfaceIndex--;
}