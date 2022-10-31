#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////����������Ҫ�޸Ĵ˴�/////////////////////////////////////////
#define   UART_NUM        4//һ��4·����
typedef enum{
		USE_UART2=0,
	  USE_UART3,
	  USE_UART6,
	  USE_UART4,
	  USE_DIS_UART=0XFF
}uartEnum;
const static char     UartName[UART_NUM][6] ={"port1", "port2",  "port3",  "port4"};//��ӳ��һ������
const static uartEnum UartNum[UART_NUM]     ={USE_UART2,USE_UART3,USE_UART6,USE_UART4};//��ӳ��һ������
/////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct{
	  uint32_t bps;
	  rt_bool_t offline;
	  rt_mutex_t  uartMutex;
	  struct  rt_messagequeue* uartMessque;
}uartConfStru;


typedef struct{
	 rt_bool_t 	workFlag;
	 uint16_t  	slaveAddr; //0-255
	 uartEnum  	useUartNum;
	 uint32_t   colTime;
	 void (* modbusRead)(void );
}modbusFlashStru;



//////////////////////////////���Ӵ�������Ҫ�޸��±� һһ��Ӧ////////////////////////////////////////
#define   MODBUS_NUM      4
extern modbusFlashStru  modbusFlash[MODBUS_NUM];//��Ҫ�洢��flash���豸����
typedef enum{
     CIRCULA=0,
     PARTDISCHAG,//partDischag,
     PRESSSETTL,//pressSettl,
     THREEAXIS //threeAxis
}modbNumEnum;

typedef enum{

		CIRCULA_TIME=0,
	  PARTDISCHAG_TIME,
		PRESSSETTL_TIME,
		THREEAXIS_TIME,
		HEART_TIME=10,
		REG_TIME=11
}upDataTimEnum;//��Ҫ��modbusName ����һһ��Ӧ ��ʵ�ִ��뾫���Ч
const static char  modbusName[MODBUS_NUM][20] ={"�ӵػ���","�ַ�","������","���������"};
const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600};
const static int   modbusType[MODBUS_NUM]     ={1,        1,       2,      2};//��ͬ���͵�modbus�豸������ͬ
/////////////////////////////////////////////////////////////////////////////////////////////////////

extern uartConfStru  uartDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartMutexQueueCfg(void);
extern rt_err_t uartDataRec(uartEnum uartNum,uint8_t dat);















#endif

