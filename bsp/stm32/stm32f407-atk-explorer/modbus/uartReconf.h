#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"


#define   MODBUS_NUM      5
#define   UART_NUM        4//һ��4·����


typedef enum{
		USE_UART2=0,
	  USE_UART3,
	  USE_UART6,
	  USE_UART4,
	  USE_DIS_UART=0XFF
}uartEnum;

typedef struct{
	  uint32_t bps;
	  rt_bool_t offline;
	  rt_mutex_t  uartMutex;
	  struct  rt_messagequeue* uartMessque;
	 // uint8_t uartMessquePool[MSGPOOL_LEN];
}uartConfStru;


typedef struct{
	 rt_bool_t 	workFlag;
	 uint16_t  	slaveAddr; //0-255
	 uartEnum  	useUartNum;
}modbusFlashStru;


extern modbusFlashStru  modbusFlash[];//��Ҫ�洢��flash���豸����


typedef enum{
     huanLiu=0,
     juFang,
     chenJiangYi,
     sanZhou
}modbNumEnum;

typedef struct{
		uartEnum cirCula;
	  uartEnum partDischag;
		uartEnum pressSettl;
		uartEnum threeAxis;
}uartChanlStru;// ʹ�õĶ�Ӧ���ĸ�����
extern uartChanlStru chanl;
extern uartConfStru  uartDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartConfFlashRead(void);

extern rt_err_t uartDataRec(uartEnum uartNum,uint8_t dat);















#endif

