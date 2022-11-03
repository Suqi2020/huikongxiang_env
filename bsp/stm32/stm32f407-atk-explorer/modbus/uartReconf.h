#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__
#include "flashsave.h"
#include "board.h"
#include "dataPack.h"



typedef enum{
		HEART_TIME=0,
		REG_TIME
}upDataTimEnum;//需要与modbusName 名称一一对应 来实现代码精简高效

/////////////////////////////////////////////////////////////////////////////////////////////////////

extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartMutexQueueCfg(void);

//extern rt_err_t uartDataRec(uartEnum uartNum,uint8_t dat);
rt_bool_t errConfigCheck(void);













#endif

