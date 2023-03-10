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
const static char sign[]="[LCDKeyEvent]";


int modbusConfIDCheck(char *inputID);
void dispInterFaceIndexFun(void);
//接口名称增加
void dispInterFaceIndexAdd(void);
//接口名称减少
void dispInterFaceIndexReduc(void);


void	LCDDispIP(void);
void	LCDDispUart(void);
void	LCDDispMCUID(void);
void  LCDDispNetOffline(void);
void  offLineIndexAdd(void);
void  dispCinaName(uint8_t *buf);
void  dispChinaNameIndexLow(void);
void  dispChinaNameIndexAdd(void);
void  LDCDispMosbusInfo(void);
void  keyModbusCfgSure(void);
void  modbDevReadIndexLow(void);
void  modbDevReadIndexAdd(void);
void	modbErrDevReadIndexLow(void);
void	LDCDispErrMosbusInfo(void);
void  modbErrDevReadIndexAdd(void);
void	delModbusDevbyID_p(void);
void	LCDDispModbusGet(void);
void	LCDDispErrModbusGet(void);
void	offLineIndexLow(void);
void  dispPortIndexLow(void);
void  dispPortIndexFun(void);
void  rstPortIndexFun(void);
void  dispPortIndexAdd(void);
void  dispOutputNameIDType(void);
void  dispLevelState(char level);
//按键触发总接口
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
				dispChinaNameIndexLow();
				dispCinaName(buf);
				break;
			case	KEY_MODBUS_NEXTNAME_ADDR:
				dispChinaNameIndexAdd();
				dispCinaName(buf);
				break;
			case	KEY_MODBUS_SURENAME_ADDR:// 显示到1360
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
				stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFlash));//每次擦除128k字节数据 存储时候需要一起存储
				stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFlash,sizeof(packFlash));
				stm32_flash_write(FLASH_MODBUS_SAVE_ADDR,(uint8_t*)&sheet,sizeof(sheet));
				break;
			case KEY_RESET_ADDR://复位
				rt_hw_cpu_reset();
				break;
			case KEY_MODBUS_CFG_WATCH_ADDR:

				LDCDispMosbusInfo();
				break;
			case KEY_MODBUS_CFG_SURE_ADDR:
				keyModbusCfgSure();
				break;
			case KEY_MODBUSDISP_LAST_ADDR:
				modbDevReadIndexLow();
				LDCDispMosbusInfo();
				break;

			case  KEY_MODBUSDISP_NEXT_ADDR:
				modbDevReadIndexAdd();
				LDCDispMosbusInfo();
			  break;
			

//////////////////////////////////////////////////

			case KEY_MODBUSDISP_ERRLAST_ADDR:
				modbErrDevReadIndexLow();
				LDCDispErrMosbusInfo();
				break;
			case KEY_MODBUSDISP_ERRNEXT_ADDR:
				modbErrDevReadIndexAdd();
				LDCDispErrMosbusInfo();
				break;
			case  KEY_MODBUSDISP_DEL_ADDR:
				delModbusDevbyID_p();
			  LCDDispModbusGet();
			  LDCDispMosbusInfo();
				break;
			case  KEY_NETERROR_ADDR:
				//rt_kprintf("%s按键按下\n",sign);
				LCDDispNetOffline();
				break;
			case  KEY_MODBUSERR_ADDR:
				LCDDispErrModbusGet();
				LDCDispErrMosbusInfo();
				break;
			case	NET_OFFLINE_LAST_ADDR:
				offLineIndexLow();
				LCDDispNetOffline();
				break;
			case  NET_OFFLINE_NEXT_ADDR:

				offLineIndexAdd();
				LCDDispNetOffline();
				break;
			case  KEY_SWITCH_INTERFACE_ADDR:
				rt_kprintf("%sKEY_SWITCH_INTERFACE_ADDR \n",sign);
				dispInterFaceIndexFun();
			  dispPortIndexFun();
				break;
			case KEY_SWITCH_PORT_ADDR:
				break;
			case KEY_SWITCH_LEVEL_ADDR:
				break;
			
			case KEY_SWITCH_SURE_ADDR:
				break;
			
			case KEY_SWITCH_RETURN_ADDR:
				break;
			

			case KEY_SWITCHINTERF_SURE_ADDR://do nothing
				break;
			case KEY_SWITCHINTERF_NEXT_ADDR:
				rt_kprintf("%sKEY_SWITCHINTERF_NEXT_ADDR \n",sign);
				dispInterFaceIndexAdd();
			  dispInterFaceIndexFun();
				rstPortIndexFun();
			  dispPortIndexFun();
			  dispOutputNameIDType();
				break;
			case KEY_SWITCHINTERF_LAST_ADDR:
				rt_kprintf("%sKEY_SWITCHINTERF_LAST_ADDR \n",sign);
				dispInterFaceIndexReduc();
			  dispInterFaceIndexFun();
			  rstPortIndexFun();
			  dispPortIndexFun();
			  dispOutputNameIDType();
				break;
			case KEY_SWITCHINTERF_RETURN_ADDR://
				
				break;
			case KEY_SWITCHPORT_SURE_ADDR:
				break;
			case KEY_SWITCHPORT_NEXT_ADDR:
				dispPortIndexAdd();
			  dispPortIndexFun();
			  dispOutputNameIDType();
				break;
			case KEY_SWITCHPORT_LAST_ADDR:
				dispPortIndexLow();
			  dispPortIndexFun();
			  dispOutputNameIDType();
				break;
			case KEY_SWITCHPORT_RETURN_ADDR:
				//dispOutputNameIDType();
				break;
			case KEY_SWITCHLEVEL_ON_ADDR:
				rt_kprintf("%s ON\n",sign);
			  dispLevelState(1);
			  levelSet(1);
				break;
			case KEY_SWITCHLEVEL_OFF_ADDR:
				rt_kprintf("%s OFF\n",sign);
			  dispLevelState(0);
			  levelSet(0);
				break;
			case KEY_SWITCHLEVEL_SURE_ADDR:
				break;
			case KEY_SWITCHLEVEL_RETURN_ADDR:
				break;

		//开关控制end
		}
		rt_free(buf);
		buf=RT_NULL;
		
}
