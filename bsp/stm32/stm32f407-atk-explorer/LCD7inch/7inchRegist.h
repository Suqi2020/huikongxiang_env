/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-5      SummerGift   first version
 */

#ifndef __7INCHREGIST_H__
#define __7INCHREGIST_H__


#include "board.h"
//NET
#define   LOCAL_IP1_ADDR         0X1000
#define   LOCAL_IP2_ADDR         0X1002
#define   LOCAL_IP3_ADDR         0X1004
#define   LOCAL_IP4_ADDR         0X1006
#define   PHY_PORT_ADDR          0X1008
#define   REMOTE_IP1_ADDR        0X100A
#define   REMOTE_IP2_ADDR        0X100C
#define   REMOTE_IP3_ADDR        0X100E
#define   REMOTE_IP4_ADDR    		 0X1010
#define   REMOTE_PORT_ADDR   		 0X1012
#define   KEY_IP_SURE_ADDR			 0X1014
#define   KEY_IP_READ_ADDR			 0X1016
//波特率
#define   PORT1_ADDR     		 			0X1100
#define   PORT2_ADDR     		 			0X1102
#define   PORT3_ADDR     		 			0X1104
#define   PORT4_ADDR     		 			0X1106
#define   KEY_PORT_SURE_ADDR		  0X1108
#define   KEY_PORT_READ_ADDR		  0X110A
//MCUID
#define   MCUID_ADDR         			0X1210
#define		KEY_MCUID_SURE_ADDR		  0X1202
#define		KEY_MCUID_READ_ADDR		  0X1204
//MODBUS
//#define		MODBUS_CFG_NAME_ADDR  		0x1360
#define		MODBUS_CFG_ID_ADDR    		0x1320
#define		MODBUS_CFG_SORT_ADDR  		0X1300
#define		MODBUS_CFG_TYPE_ADDR  		0x1350
#define		MODBUS_CFG_PORT_ADDR  		0x1302
#define		MODBUS_CFG_ADDR_ADDR  		0x1304
#define		MODBUS_CFG_TIME_ADDR  		0x1306
#define		KEY_MODBUS_CFG_SURE_ADDR  0x1308
#define		KEY_MODBUS_CFG_DEL_ADDR   0x130A
#define		KEY_MODBUS_CFG_WATCH_ADDR 0x130C
#define		KEY_MODBUS_CFG_NAME_ADDR  0x130E
#define		MODBUS_CFG_NAME2_ADDR  		0x1380

#define		KEY_MODBUS_LASTNAME_ADDR  0x1310
#define		KEY_MODBUS_NEXTNAME_ADDR  0x1312
#define		KEY_MODBUS_SURENAME_ADDR  0x1314
//modbusDisp
#define   MODBUSDISP_NAME_ADDR      0X1460
#define   MODBUSDISP_ID_ADDR        0X1420
#define   MODBUSDISP_SORT_ADDR      0X1400
#define   MODBUSDISP_TYPE_ADDR      0X1450
#define   MODBUSDISP_PORT_ADDR      0X1402
#define   MODBUSDISP_ADDR_ADDR      0X1404
#define   MODBUSDISP_TIME_ADDR      0X1406
#define   MODBUSDISP_TOTALNUM_ADDR  0X1408
#define   MODBUSDISP_NOWNUM_ADDR    0X140A

#define   KEY_MODBUSDISP_LAST_ADDR  0X140C
#define   KEY_MODBUSDISP_NEXT_ADDR  0X140E
#define   KEY_MODBUSDISP_DEL_ADDR   0X1410

//netoffline 显示界面地址
#define		NET_OFFLINE_TOTALTIMES_ADDR  0X1600
#define		NET_OFFLINE_TIMES_ADDR       0X1602
#define   NET_OFFLINE_RELAYTIME_ADDR   0X1604
#define		NET_OFFLINE_LAST_ADDR        0X1606
#define   NET_OFFLINE_NEXT_ADDR        0X1608



//troublemodbusDisp
#define   MODBUSDISP_ERRNAME_ADDR      0X1560
#define   MODBUSDISP_ERRID_ADDR        0X1520
#define   MODBUSDISP_ERRSORT_ADDR      0X1500
#define   MODBUSDISP_ERRTYPE_ADDR      0X1550
#define   MODBUSDISP_ERRPORT_ADDR      0X1502
#define   MODBUSDISP_ERRADDR_ADDR      0X1504
#define   MODBUSDISP_ERRTIME_ADDR      0X1506
#define   MODBUSDISP_ERRTOTALNUM_ADDR  0X1508
#define   MODBUSDISP_ERRNOWNUM_ADDR    0X150A

#define   KEY_MODBUSDISP_ERRLAST_ADDR  0X150C
#define   KEY_MODBUSDISP_ERRNEXT_ADDR  0X150E

//////////////////////////////////////////////////////////////
//开关控制界面
#define   KEY_SWITCH_INTERFACE_ADDR    0x5400
#define   KEY_SWITCH_PORT_ADDR         0x5402
#define   KEY_SWITCH_LEVEL_ADDR        0x5404
#define   KEY_SWITCH_SURE_ADDR         0x5406
#define   KEY_SWITCH_RETURN_ADDR       0x5408

//#define   DISP_SWITCH_INTERFACE_ADDR   0x5440
#define   DISP_SWITCH_PORT_ADDR        0x540C
#define   DISP_SWITCH_NAME_ADDR        0x5450
#define   DISP_SWITCH_ID_ADDR       	 0x5460
#define   DISP_SWITCH_TYPE_ADDR        0x5480
#define   DISP_SWITCH_LEVEL_ADDR       0x5414

//开关控制调出接口显示界面
#define   DISP_SWITCH_INTERFACE_ADDR  0x5410

#define   KEY_SWITCHINTERF_SURE_ADDR    		0x5418
#define   KEY_SWITCHINTERF_NEXT_ADDR        0x541A
#define   KEY_SWITCHINTERF_LAST_ADDR        0x541C
#define   KEY_SWITCHINTERF_RETURN_ADDR      0x541E
//开关控制调出port显示界面
#define   DISP_SWITCHPORT_PORT_ADDR         0x5420

#define   KEY_SWITCHPORT_SURE_ADDR    			0x5422
#define   KEY_SWITCHPORT_NEXT_ADDR        	0x5424
#define   KEY_SWITCHPORT_LAST_ADDR        	0x5426
#define   KEY_SWITCHPORT_RETURN_ADDR      	0x5428
//开关控制调出电平显示界面
#define   KEY_SWITCHLEVEL_ON_ADDR    				0x542A
#define   KEY_SWITCHLEVEL_OFF_ADDR    			0x542C
#define   KEY_SWITCHLEVEL_SURE_ADDR    			0x542E
#define   KEY_SWITCHLEVEL_RETURN_ADDR    		0x5430
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//输入界面

#define   KEY_INPUTCFG_LOOK_ADDR      	    0X5000
#define   KEY_INPUTCFG_SURE_ADDR        	  0X5002
#define   DISP_INPUTCFG_NAME_ADDR        	 	0X5004
#define   DISP_INPUTCFG_ID_ADDR        	 	  0X5010
#define   DISP_INPUTCFG_TYPE_ADDR        	  0X5030
#define   DISP_INPUTCFG_PORT_ADDR        	  0X5038

//输入查看界面
#define   DISP_INPUT_NAME_ADDR             0x5040
#define   DISP_INPUT_ID_ADDR               0x5050
#define   DISP_INPUT_TYPE_ADDR             0x5064
#define   DISP_INPUT_PORT_ADDR             0x5070
#define   DISP_INPUT_TOTALNUM_ADDR         0x5072
#define   DISP_INPUT_THENUM_ADDR           0x5074

#define   KEY_INPUT_LAST_ADDR              0x5076
#define   KEY_INPUT_NEXT_ADDR              0x5078
#define   KEY_INPUT_DEL_ADDR               0x507A
#define   KEY_INPUT_RETURN_ADDR            0x507C
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//输出设置
//#define        DISP_OUTPUT_INTERFACE_ADDR     0x5110
#define        DISP_OUTPUT_NAME_ADDR     			0x5118
#define        DISP_OUTPUT_ID_ADDR     				0x5128
#define        DISP_OUTPUT_TYPE_ADDR     			0x5138
#define        DISP_OUTPUT_PORT_ADDR     			0x5140
#define        KEY_OUTPUT_INTERFACE_ADDR     	0x5142
#define        KEY_OUTPUT_SURE_ADDR     		0x5144
#define        KEY_OUTPUT_LOOK_ADDR     			0x5146
#define        KEY_OUTPUT_RETURN_ADDR     	0x5148

#define        DISP_OUTPUT_INTERFACE_ADDR   0x514A
//#define        KEY_OUTPUT_SURE_P_ADDR     		0x5150
#define        KEY_OUTPUT_LAST_ADDR     		0x5152
#define        KEY_OUTPUT_NEXT_ADDR     		0x5154
#define        KEY_OUTPUT_RETURN_P_ADDR     	0x5156

#define        DISP_OUTPUT_READ_INTERFACE_ADDR   0x5158
#define        DISP_OUTPUT_READ_NAME_ADDR        0x5160
#define        DISP_OUTPUT_READ_ID_ADDR          0x5168
#define        DISP_OUTPUT_READ_TYPE_ADDR        0x5178
#define        DISP_OUTPUT_READ_PORT_ADDR        0x5180
#define        DISP_OUTPUT_READ_TOTALNUM_ADDR    0x5182
#define        DISP_OUTPUT_READ_THENUM_ADDR      0x5184
#define        KEY_OUTPUT_READ_INTERFACE_ADDR    0x5186
#define        KEY_OUTPUT_READ_LAST_ADDR    		 0x5188
#define        KEY_OUTPUT_READ_NEXT_ADDR     		 0x518A
#define        KEY_OUTPUT_READ_DELETE_ADDR     	 0x518C
#define        KEY_OUTPUT_READ_RETURN_ADDR     	 0x518E

#define        DISP_OUTPUT_READ_INTERFACE_P_ADDR     0x5190
#define        KEY_OUTPUT_READ_LAST_P_ADDR     0x5198
#define        KEY_OUTPUT_READ_NEXT_P_ADDR     0x519A
#define        KEY_OUTPUT_READ_RETURN_P_ADDR     0x519C

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//模拟传感器配置显示
#define        DISP_ANA_NAME_ADDR     	 0x5200
#define        DISP_ANA_SUBNAME_ADDR     0x5208
#define        DISP_ANA_ID_ADDR     		0x5210
#define        DISP_ANA_TYPE_ADDR     0x5220
#define        DISP_ANA_PORT_ADDR     0x5228
#define        DISP_ANA_TIME_ADDR     0x522A

#define        KEY_ANA_SUBNAME_INTERFACE_ADDR     0x522C
#define        KEY_ANA_SURE_ADDR     0x522E
#define        KEY_ANA_LOOK_ADDR     0x5230
//#define        KEY_ANA_RETURN_ADDR     0x5232

//#define             0x5208
#define        KEY_ANA_SURE_ADDR     0x5236
#define        KEY_ANA_LAST_ADDR     0x5238
#define        KEY_ANA_NEXT_ADDR     0x523A


#define        DISP_ANAREAD_NAME_ADDR    0x5240
#define        DISP_ANAREAD_SUBNAME_ADDR 0x5248
#define        DISP_ANAREAD_ID_ADDR     0x5250
#define        DISP_ANAREAD_TYPE_ADDR     0x5260
#define        DISP_ANAREAD_PORT_ADDR     0x5268
#define        DISP_ANAREAD_TIME_ADDR     0x526A
#define        DISP_ANAREAD_TOTALNUM_ADDR     0x526C
#define        DISP_ANAREAD_THENUM_ADDR     0x526E
#define        KEY_ANAREAD_LAST_ADDR     0x5270
#define        KEY_ANAREAD_NEXT_ADDR     0x5272
#define        KEY_ANAREAD_DEL_ADDR     0x5274
//#define        KEY_ANAREAD_RETURN_ADDR     0x5276
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//保存界面

#define   KEY_SAVE_ADDR             0X1700
#define   KEY_RESET_ADDR            0X1702
//主界面
#define   KEY_NETERROR_ADDR         0X1A00
#define   KEY_MODBUSERR_ADDR        0X1A02
#define   NET_ERR_DISPLAY_ADDR      0X1A0C
#define   MODBUS_ERR_DISPLAY_ADDR   0X1A0E
//////////////////////////////////////////////////////////////
//kenal lcd command
///#define   LCD_HEAD       0XAEA5
#define   LCD_HEAD       0X5AA5
#define   LCD_FILL_TAIL  0XFFFF
#define   LCD_WRITE      0X82
#define   LCD_READ       0X83
#define   LCD_RUN        0X4F4B
#define   LCD_BUF_LEN    50


#define  KEY_ADDR  			0X0000






extern int LCDWtite(uint16_t addr,uint8_t *data,uint8_t dataLen);


//typedef struct{
//		char  name[25];
//		char  ID[20];
//		char  model[8];
//		uint8_t   port;
//		uint8_t   addr;
//		uint32_t  colTime;
//}LCDDispModInfoStru;
//typedef struct{
//	  uint8_t X;//modbus设备的种类
//	  uint8_t Y;//每种modbus的个数
//	  uint8_t flag;//存在标记为1
//}modbusPositStru;

//extern LCDDispModInfoStru  modbusLCDRead;
//extern modbusPositStru  modPosit[TOTOLA_485_NUM]; 
//extern modbusPositStru  modPositErr[TOTOLA_485_NUM];








#endif

