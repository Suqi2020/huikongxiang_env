#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"

typedef struct{
		//char name[20];
		//uint8_t	chanl;//串口通道  0 1 2 3 需要小于NUM
	  uint32_t bps;
	  rt_bool_t offline;
		//void  (*UartSend)(uint8_t *buf,int len);
		rt_err_t  (*UartRec)(uint8_t dat);
}modbusConfStru;

typedef enum{
		USE_UART2=0,
	  USE_UART3,
	  USE_UART6,
	  USE_UART4
}uartEnum;


typedef struct{
		uartEnum cirCula;
	  uartEnum partDischag;
		uartEnum pressSettl;
		uartEnum threeAxis;
}uartChanlStru;// 使用的对应的哪个串口
extern uartChanlStru chanl;
extern modbusConfStru  modDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartConfFlashRead(void);

















#endif

