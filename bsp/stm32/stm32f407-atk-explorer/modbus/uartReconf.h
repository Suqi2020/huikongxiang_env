#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"



#define   UART_NUM        4//一共4路串口
#define   ACUID_LEN       18
#define 	DEVID_LEN       20
#define   MODEL_LEN       8
#define   NAME_LEN        20


 

//typedef struct{
////		uint32_t bps;
////    uint16_t calTime;	
////    uint8_t  delayTime;//100ms的整数倍  端口下边挂载的传感器采集间隔时间  局放2秒
////	  uint8_t rev;
//}uartSaveStru;

//extern uartSaveStru  port[UART_NUM];

typedef struct{
		
	  uint8_t  localIp[4];
	  uint8_t  gateway[4];
	  uint8_t  remoteIp[4];
    uint16_t remotePort; 
	  uint8_t  macaddr;
	  uint8_t  rev;
}netIpFlashStru;

#define  ANALOG_NUM   8//8路模拟量

typedef struct{
		char  devID[DEVID_LEN];
	  char  model[MODEL_LEN];
		char  name[NAME_LEN];
	
    uint8_t port; //1-8
	  uint8_t rev;//
	  uint16_t calTime; //采集间隔时间
	

}analogDevStru;

typedef struct{
//		uartSaveStru   port[UART_NUM];
		netIpFlashStru netIpFlash;
	  analogDevStru  analog[ANALOG_NUM];
	  uint32_t    uartBps[UART_NUM];
	  char     acuId[ACUID_LEN+1];
	  char rev;
}packIpUartStru;
extern packIpUartStru packFLash;
#define  MODBID_LEN   20
#define  MODL_LEN     8
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////串口设置需要修改此处/////////////////////////////////////////

typedef enum{
		USE_UART2=0,
	  USE_UART3,
	  USE_UART6,
	  USE_UART4,
	  USE_DIS_UART=0XFF
}uartEnum;
typedef struct{
		bool 	workFlag;
		uartEnum  	useUartNum;
		uint16_t  	slaveAddr; //0-255
		char  ID[MODBID_LEN];
		char  model[MODL_LEN];

}modbusStru;
typedef struct{
		char  ID[MODBID_LEN];
		char  model[8];
	  char  name[20];
	  char  funName[16];
	  uint8_t   	port;
	  rt_bool_t 	workFlag;
	  uint32_t   	colTime;
}analogStru;
#define THREEAXIS_485_NUM     40
#define PRESSSETTL_485_NUM    40
#define CIRCULA_485_NUM   	  5
#define PARTDISCHAG_485_NUM   5
#define CH4_485_NUM   			  2
#define CO_485_NUM   				  2
#define H2S_485_NUM   			  2
#define O2_485_NUM   				  2
#define WATERDEPTH_485_NUM   	2
#define TEMPHUM_485_NUM   	  2
#define TOTOLA_485_NUM     (THREEAXIS_485_NUM+\
														PRESSSETTL_485_NUM+\
														CIRCULA_485_NUM+\
														PARTDISCHAG_485_NUM+\
														CH4_485_NUM+\
														CO_485_NUM+\
														H2S_485_NUM+\
														O2_485_NUM+\
														WATERDEPTH_485_NUM+\
														TEMPHUM_485_NUM)
														
#define ANALOG_NUM   	        8
/////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct{
	 // uint32_t bps;
	//rt_bool_t offline;
	  rt_mutex_t  uartMutex;
	  struct  rt_messagequeue* uartMessque;
}uartConfStru;
//modbus+设备名称(波特率)+端口(port1-port4)+设备地址(0-关闭设备)+采集间隔(秒)
#define   MODBUS_NUM      10
typedef struct{
	 void (* modbusRead)(void );
}modbusFunStru;
extern modbusFunStru modbusFun[MODBUS_NUM];
//////////////////////////////增加传感器需要修改下边 一一对应////////////////////////////////////////

//extern deviceFlashStru  modbusFlash[MODBUS_NUM];//需要存储到flash的设备配置
typedef enum{
     CIRCULA=0, 	PARTDISCHAG,			PRESSSETTL, 			THREEAXIS,			CH4,		O2		,H2S,			CO,			TEMPHUM,			WATERDEPTH
}modbNumEnum;

typedef enum{
		CIRCULA_TIME=0,PARTDISCHAG_TIME,PRESSSETTL_TIME,THREEAXIS_TIME,CH4_TIME,O2_TIME,H2S_TIME,CO_TIME,TEMPHUM_TIME,WATERDEPTH_TIME,
	  ANA_TEMPHUM_TIME,
		HEART_TIME,REG_TIME
}upDataTimEnum;//需要与modbusName 名称一一对应 来实现代码精简高效
const static char  modbusName[MODBUS_NUM][20] ={"接地环流","局放","防沉降","防外破","甲烷","氧气","硫化氢","一氧化碳","温湿度","水位"};
const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,   9600,   9600,   9600,   9600,   9600,   9600};
//const static int   modbusType[MODBUS_NUM]     ={1,        1,       2,      2, 		 3,  			3,  		3,  		3,  		3,  		3};//想同类型的modbus设备名称相同

typedef struct{
			
			modbusStru  cirCula[CIRCULA_485_NUM];
			modbusStru  partDischag[PARTDISCHAG_485_NUM];
			modbusStru  pressSetl[PRESSSETTL_485_NUM];
	    modbusStru  threeAxiss[THREEAXIS_485_NUM];
			modbusStru  ch4[CH4_485_NUM];
	    modbusStru  o2[O2_485_NUM];
			
			modbusStru  h2s[H2S_485_NUM];
			modbusStru  co[CO_485_NUM];
			
			modbusStru  tempHum[TEMPHUM_485_NUM];
			modbusStru  waterDepth[WATERDEPTH_485_NUM];
			
			uint32_t  cirCulaColTime;
			uint32_t  partDischagColTime;
			uint32_t  pressSetlColTime;
	    uint32_t  threeAxissColTime;
			uint32_t  ch4ColTime;
	    uint32_t  o2ColTime;
			
			uint32_t  h2sColTime;
			uint32_t  coColTime;
			uint32_t  tempHumColTime;
			uint32_t  waterDepthColTime;
			
			
			analogStru analog[ANALOG_NUM];
			//uint32_t   analogColTime;
	
}deviceFlashStru;
/////////////////////////////////////////////////////////////////////////////////////////////////////
extern deviceFlashStru sheet;
extern uartConfStru  uartDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartMutexQueueCfg(void);

extern rt_err_t uartDataRec(uartEnum uartNum,uint8_t dat);
rt_bool_t errConfigCheck(void);













#endif

