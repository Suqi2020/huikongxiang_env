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


/*
uint8_t  modbTotalIndex=0;
uint8_t  modbDevReadIndex=0;
static int chinaNameIndex=0;//当前用到的名字标记  根据modbNumEnum对应起来
extern LCDDispModInfoStru  modbusLCDRead;
extern int offLineIndex;
extern uint8_t  modbErrTotalIndex;
extern uint8_t  modbErrDevReadIndex;
extern modbusStru LCDInput;

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
				stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFlash));//每次擦除128k字节数据 存储时候需要一起存储
				stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFlash,sizeof(packFlash));
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
*/