#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////串口设置需要修改此处/////////////////////////////////////////
#define   UART_NUM        4//一共4路串口
typedef enum{
		USE_UART2=0,
	  USE_UART3,
	  USE_UART6,
	  USE_UART4,
	  USE_DIS_UART=0XFF
}uartEnum;
const static char     UartName[UART_NUM][6] ={"port1", "port2",  "port3",  "port4"};//重映射一个名称
const static uartEnum UartNum[UART_NUM]     ={USE_UART2,USE_UART3,USE_UART6,USE_UART4};//重映射一个名称
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



//////////////////////////////增加传感器需要修改下边 一一对应////////////////////////////////////////
#define   MODBUS_NUM      10
extern modbusFlashStru  modbusFlash[MODBUS_NUM];//需要存储到flash的设备配置
typedef enum{
     CIRCULA=0, 	PARTDISCHAG,			PRESSSETTL, 			THREEAXIS,			CH4,		O2		,H2S,			CO,			TEMPHUM,			WATERLEVEL
}modbNumEnum;

typedef enum{
		CIRCULA_TIME=0,PARTDISCHAG_TIME,PRESSSETTL_TIME,THREEAXIS_TIME,CH4_TIME,O2_TIME,H2S_TIME,CO_TIME,TEMPHUM_TIME,WATERLEVEL_TIME,
		HEART_TIME=20,
		REG_TIME
}upDataTimEnum;//需要与modbusName 名称一一对应 来实现代码精简高效
const static char  modbusName[MODBUS_NUM][20] ={"接地环流","局放","沉降仪","三轴测振仪","甲烷","氧气","硫化氢","一氧化碳","温湿度","水位"};
const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,   9600,   9600,   9600,   9600,   9600,   9600};
const static int   modbusType[MODBUS_NUM]     ={1,        1,       2,      2, 		 3,  			3,  		3,  		3,  		3,  		3};//想同类型的modbus设备名称相同
/////////////////////////////////////////////////////////////////////////////////////////////////////

extern uartConfStru  uartDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartMutexQueueCfg(void);
extern rt_err_t uartDataRec(uartEnum uartNum,uint8_t dat);















#endif

