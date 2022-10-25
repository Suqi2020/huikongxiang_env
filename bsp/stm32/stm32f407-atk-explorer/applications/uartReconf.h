#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"

typedef struct{
		//char name[20];
		//uint8_t	chanl;//����ͨ��  0 1 2 3 ��ҪС��NUM
	  uint32_t bps;
	  rt_bool_t offline;
		//void  (*UartSend)(uint8_t *buf,int len);
		rt_err_t  (*UartRec)(uint8_t dat);
}modbusConfStru;

typedef enum{
		USE_UART2=0,
	  USE_UART3,
	  USE_UART6,
	  USE_UART4,
	  USE_DIS_UART=0XFF
}uartEnum;


typedef struct{
		uartEnum cirCula;
	  uartEnum partDischag;
		uartEnum pressSettl;
		uartEnum threeAxis;
}uartChanlStru;// ʹ�õĶ�Ӧ���ĸ�����
extern uartChanlStru chanl;
extern modbusConfStru  modDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartConfFlashRead(void);

















#endif

