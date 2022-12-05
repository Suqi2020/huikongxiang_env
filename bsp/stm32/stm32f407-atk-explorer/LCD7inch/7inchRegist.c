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



uint8_t  sendLCDBuf[LCD_BUF_LEN];
uint8_t  recLCDBuf[LCD_BUF_LEN];



//if (rt_mq_recv(&mq, recLCDBuf+i, 1, RT_WAITING_FOREVER) == RT_EOK){
//	i++
//}
//结果 return 0 无响应  1 正常响应
int LCDWtite(uint16_t addr,uint8_t *data,uint8_t dataLen,uint8_t repTimes)
{
	  int len=0;
		rt_memset(sendLCDBuf,0,LCD_BUF_LEN);
		rt_memset(recLCDBuf,0,LCD_BUF_LEN);
		sendLCDBuf[len]=(uint8_t)(LCD_HEAD>>8);len++;
	  sendLCDBuf[len]=(uint8_t)LCD_HEAD;len++;
	  sendLCDBuf[len]=dataLen+2+1    ;len++;//长度暂时填充0  2-headlen 1-writelen
}

