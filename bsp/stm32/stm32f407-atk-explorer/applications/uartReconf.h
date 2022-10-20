#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"

typedef struct{
		//char name[20];
		//uint8_t	chanl;//串口通道  0 1 2 3 需要小于NUM
	  uint32_t bps;
		//void  (*UartSend)(uint8_t *buf,int len);
		rt_err_t  (*UartRec)(uint8_t dat);
}modbusConfStru;
typedef struct{
		uint8_t cirCula;
	  uint8_t partDischag;
		uint8_t pressSettl;
		uint8_t threeAxis;
}uartChanlStru;
typedef enum{
		USE_UART2=0,
	  USE_UART3,
	  USE_UART6,
	  USE_UART4
}uartEnum;
extern uartChanlStru chanl;
extern modbusConfStru  modDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartConfRead(void);

















#endif

