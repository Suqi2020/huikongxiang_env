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
//??????
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
#define		MODBUS_CFG_NAME_ADDR  		0x1360
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

//netoffline ??ʾ??????ַ
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




#define   KEY_SAVE_ADDR             0X1700
#define   KEY_RESET_ADDR            0X1702
//??????
#define   KEY_NETERROR_ADDR         0X1A00
#define   KEY_MODBUSERR_ADDR        0X1A02
#define   NET_ERR_DISPLAY_ADDR      0X1A0C
#define   MODBUS_ERR_DISPLAY_ADDR   0X1A0E

//kenal lcd command
#define   LCD_HEAD       0X5AA5
#define   LCD_FILL_TAIL  0XFFFF
#define   LCD_WRITE      0X82
#define   LCD_READ       0X83
#define   LCD_RUN        0X4F4B
#define   LCD_BUF_LEN    50


#define  KEY_ADDR  			0X0000





















#endif

