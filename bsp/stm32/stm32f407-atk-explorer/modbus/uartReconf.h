#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"
//存储分为3个部分  1/modbus传感器存储
//								 2/ip 串口 acuid 模拟传感器 输入 输出设备
//								 3/数字,模拟传感器阈值存储


#define   UART_NUM        4//一共4路串口
#define   ACUID_LEN       18
#define 	DEVID_LEN       20
#define   MODEL_LEN       8
#define   NAME_LEN        20
#define   DI_NUM          8
#define   DO_NUM          8
#define   USE_4GAS
 

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
	
		char  devID[DEVID_LEN];
	  char  model[MODEL_LEN];
		char  name[NAME_LEN];
    uint8_t port; //1-8
	  uint8_t workFlag;//RT_TRUE-
	  uint8_t rev[2];
}digitStru;//数字输入输出
typedef struct{
//		uartSaveStru   port[UART_NUM];
		netIpFlashStru netIpFlash;        //存储IP数据
	  analogDevStru  analog[ANALOG_NUM];//存储模拟数据
	  uint32_t       uartBps[UART_NUM];    //存储串口配置
	  char     acuId[ACUID_LEN+1];      //存储acuID
	  char rev;
		digitStru input[DI_NUM];   //数字输入
	  digitStru output[DO_NUM];   //数字输出
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
}uartEnum;////
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


#define GAS_NUM               2
#define THREEAXIS_485_NUM     40
#define PRESSSETTL_485_NUM    40
#define CIRCULA_485_NUM   	  5
#define PARTDISCHAG_485_NUM   5
#ifdef  USE_4GAS
#define CH4_485_NUM   			  GAS_NUM
#define CO_485_NUM   				  GAS_NUM
#define H2S_485_NUM   			  GAS_NUM
#define O2_485_NUM   				  GAS_NUM
#endif
#define WATERDEPTH_485_NUM   	2
#define TEMPHUM_485_NUM   	  2
#ifdef  USE_4GAS
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
#else
	#define TOTOLA_485_NUM     (THREEAXIS_485_NUM+\
														PRESSSETTL_485_NUM+\
														CIRCULA_485_NUM+\
														PARTDISCHAG_485_NUM+\
														WATERDEPTH_485_NUM+\
														TEMPHUM_485_NUM)
#endif
#define ANALOG_NUM   	        8
/////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct{
	 // uint32_t bps;
	//rt_bool_t offline;
	  rt_mutex_t  uartMutex;
	  struct  rt_messagequeue* uartMessque;
}uartConfStru;
//modbus+设备名称(波特率)+端口(port1-port4)+设备地址(0-关闭设备)+采集间隔(秒)
#ifdef  USE_4GAS
#define   MODBUS_NUM      10
#else
#define   MODBUS_NUM      6
#endif
typedef struct{
	 void (* modbusRead)(void );
}modbusFunStru;
extern modbusFunStru modbusFun[MODBUS_NUM];
//////////////////////////////增加传感器需要修改下边 一一对应////////////////////////////////////////

//extern deviceFlashStru  modbusFlash[MODBUS_NUM];//需要存储到flash的设备配置
#ifdef  USE_4GAS
typedef enum{
     CIRCULA=0, 	PARTDISCHAG,			PRESSSETTL, 			THREEAXIS,		\
   	CH4,		O2		,H2S,			CO,			\
	  TEMPHUM,			WATERDEPTH
}modbNumEnum;
#else
typedef enum{
     CIRCULA=0, 	PARTDISCHAG,			PRESSSETTL, 			THREEAXIS,		\
	  TEMPHUM,			WATERDEPTH
}modbNumEnum;
#endif
#ifdef  USE_4GAS
typedef enum{
		CIRCULA_TIME=0,PARTDISCHAG_TIME,PRESSSETTL_TIME,THREEAXIS_TIME,\
	  CH4_TIME,O2_TIME,H2S_TIME,CO_TIME,\
	  TEMPHUM_TIME,WATERDEPTH_TIME,\
	  ANA_TEMPHUM_TIME,HEART_TIME,REG_TIME
}upDataTimEnum;//需要与modbusName 名称一一对应 来实现代码精简高效
#else
typedef enum{
		CIRCULA_TIME=0,PARTDISCHAG_TIME,PRESSSETTL_TIME,THREEAXIS_TIME,\
	  TEMPHUM_TIME,WATERDEPTH_TIME,\
	  ANA_TEMPHUM_TIME,HEART_TIME,REG_TIME
}upDataTimEnum;//需要与modbusName 名称一一对应 来实现代码精简高效
#endif
#ifdef  USE_4GAS

#define GAS_TIME    CO_TIME  //4种气体传感器使用 同一个定时器
#else
#define GAS_TIME    22      //使用一种混合气体传感器时候定义此处
#endif

#ifdef  USE_4GAS
const static char  modbusName[MODBUS_NUM][20] ={"接地环流","局放","防沉降","防外破","甲烷","氧气","硫化氢","一氧化碳","温湿度","水位"};
const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,   9600,   9600,   9600,   9600,   9600,   9600};
#else
const static char  modbusName[MODBUS_NUM][20] ={"接地环流","局放","防沉降","防外破","温湿度","水位"};
const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,     9600,   9600};

#endif
//const static int   modbusType[MODBUS_NUM]     ={1,        1,       2,      2, 		 3,  			3,  		3,  		3,  		3,  		3};//想同类型的modbus设备名称相同
//extern char modbusName_UTF8[MODBUS_NUM][30];
extern const  char  modbusName_utf8[MODBUS_NUM][30];
typedef struct{
///////////////////////////////////////////////////
			modbusStru  cirCula[CIRCULA_485_NUM];
			modbusStru  partDischag[PARTDISCHAG_485_NUM];
			modbusStru  pressSetl[PRESSSETTL_485_NUM];
	    modbusStru  threeAxiss[THREEAXIS_485_NUM];
#ifdef  USE_4GAS
			modbusStru  ch4[CH4_485_NUM];
	    modbusStru  o2[O2_485_NUM];
			
			modbusStru  h2s[H2S_485_NUM];
			modbusStru  co[CO_485_NUM];
#endif
			modbusStru  tempHum[TEMPHUM_485_NUM];
			modbusStru  waterDepth[WATERDEPTH_485_NUM];
///////////////////////////////////////////////////
			uint32_t  cirCulaColTime;
			uint32_t  partDischagColTime;
			uint32_t  pressSetlColTime;
	    uint32_t  threeAxissColTime;
//			uint32_t  ch4ColTime;
//	    uint32_t  o2ColTime;
//			uint32_t  h2sColTime;
			uint32_t  gasColTime;//用co的定时器来采集信息  去掉其他三个气体定时器  合并打包上传
			uint32_t  tempHumColTime;
			uint32_t  waterDepthColTime;
///////////////////////////////////////////////////
			analogStru analog[ANALOG_NUM];
///////////////////////////////////////////////////
			


}deviceFlashStru;
/////////////////////////////////////////////////////////////////////////////////////////////////////
extern deviceFlashStru sheet;
extern uartConfStru  uartDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartMutexQueueCfg(void);

extern rt_err_t uartDataRec(uartEnum uartNum,uint8_t dat);
rt_bool_t errConfigCheck(void);













#endif

