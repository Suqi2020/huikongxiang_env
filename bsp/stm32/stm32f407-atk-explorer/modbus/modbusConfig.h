#ifndef __MODBUSCONFIG_H__
#define __MODBUSCONFIG_H__
//#include "uartReconf.h"
#include  "board.h"


#define  MODL_LEN     8

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
//modbus传感器温湿度
typedef struct{
		float tempUpLimit;
		float tempLowLimit;
		float humUpLimit;
		float humLowLimit;

}tempHumStru_p;
typedef struct{
		
	  uint32_t  tempUpFlag;//阈值超限的标记
	  uint32_t  tempLowFlag;
	  uint32_t  humUpFlag;
	  uint32_t  humLowFlag;
}tempHumFlagStru;
//modbus传感器防外破
typedef struct{
		float tempUpLimit;
	  float tempLowLimit;
	  uint16_t accXUpLimit;
	  uint16_t accXLowLimit;
	  uint16_t accYUpLimit;
	  uint16_t accYLowLimit;
	  uint16_t accZUpLimit;
	  uint16_t accZLowLimit;
}threeAxisStru_p;
typedef struct{
		uint32_t  tempUpFlag;//阈值超限的标记
		uint32_t  tempLowFlag;
		uint32_t  accXUpFlag;
		uint32_t  accXLowFlag;
		uint32_t  accYUpFlag;
		uint32_t  accYLowFlag;
		uint32_t  accZUpFlag;
		uint32_t  accZLowFlag;
}threeAxisFlagStru;
//modbus传感 沉降仪
typedef struct{
	  float tempUpLimit;
		float tempLowLimit;
	  float heightUpLimit;
	  float heightLowLimit;
}pressSettlStru_p;
//modbus传感裂缝仪
typedef struct{
	  float tempUpLimit;
		float tempLowLimit;
	  float distancUpLimit;
	  float distancLowLimit;
}crackMeterStru_p;
typedef struct{
		uint32_t  tempUpFlag;//阈值超限的标记
		uint32_t  tempLowFlag;
		uint32_t  heightUpFlag;
		uint32_t  heightLowFlag;
}pressSettlFlagStru;


typedef struct{
		uint32_t  tempUpFlag;//阈值超限的标记
		uint32_t  tempLowFlag;
		uint32_t  distancUpFlag;
		uint32_t  distancLowFlag;
}crackMeterFlagStru;
typedef struct
{
	  //环流值 放大了100倍
		float cirCurAUpLimit;
	  float cirCurALowLimit;
		float cirCurBUpLimit;
	  float cirCurBLowLimit;
		float cirCurCUpLimit;
	  float cirCurCLowLimit;
}circuStru_p;
typedef struct{
		uint32_t  cirCurAUpFlag;//阈值超限的标记
		uint32_t  cirCurALowFlag;
		uint32_t  cirCurBUpFlag;
		uint32_t  cirCurBLowFlag;
		uint32_t  cirCurCUpFlag;
		uint32_t  cirCurCLowFlag;
}circuFlagStru;

//modbus传感器co
typedef struct{
		float coUpLimit;
		float coLowLimit;
}coStru_p;
typedef struct{
		uint32_t  coUpFlag;//阈值超限的标记
		uint32_t  coLowFlag;
}coFlagStru;
//modbus传感器O2
typedef struct{
		float o2UpLimit;
		float o2LowLimit;
}o2Stru_p;
typedef struct{
	  uint32_t  o2UpFlag;//阈值超限的标记
		uint32_t  o2LowFlag;
}o2FlagStru;
//modbus传感器ch4
typedef struct{
		float ch4UpLimit;
		float ch4LowLimit;
}ch4Stru_p;
typedef struct{
	  uint32_t  ch4UpFlag;//阈值超限的标记
		uint32_t  ch4LowFlag;
}ch4FlagStru;
//modbus传感器h2s
typedef struct{
		float h2sUpLimit;
		float h2sLowLimit;
}h2sStru_p;
typedef  struct{
	  uint32_t  h2sUpFlag;//阈值超限的标记
		uint32_t  h2sLowFlag;
}h2sFlagStru;
//modbus传感器waterLev
typedef struct{
		float depthUpLimit;
		float depthLowLimit;
}depthStru_p;
typedef struct{
	  uint32_t  depthUpFlag;//阈值超限的标记
		uint32_t  depthLowFlag;
}depthFlagStru;

//modbus传感器partDischarg
typedef struct{
		uint32_t amplitudeAUpLimit;
		uint32_t amplitudeALowLimit;
		uint32_t freqAUpLimit;
		uint32_t freqALowLimit;
		uint32_t dischargeAUpLimit;
		uint32_t dischargeALowLimit;
	
		uint32_t amplitudeBUpLimit;
		uint32_t amplitudeBLowLimit;
		uint32_t freqBUpLimit;
		uint32_t freqBLowLimit;
		uint32_t dischargeBUpLimit;
		uint32_t dischargeBLowLimit;

		uint32_t amplitudeCUpLimit;
		uint32_t amplitudeCLowLimit;
		uint32_t freqCUpLimit;
		uint32_t freqCLowLimit;
		uint32_t dischargeCUpLimit;
		uint32_t dischargeCLowLimit;
}partDisChgStru_p;
typedef struct{
	  uint32_t  amplitudeAUpFlag;//阈值超限的标记
		uint32_t  amplitudeALowFlag;
	  uint32_t  amplitudeBUpFlag;
		uint32_t  amplitudeBLowFlag;
		uint32_t  amplitudeCUpFlag;
		uint32_t  amplitudeCLowFlag;
		
	  uint32_t  freqAUpFlag;//阈值超限的标记
		uint32_t  freqALowFlag;
	  uint32_t  freqBUpFlag;
		uint32_t  freqBLowFlag;
		uint32_t  freqCUpFlag;
		uint32_t  freqCLowFlag;

	  uint32_t  dischargeAUpFlag;//阈值超限的标记
		uint32_t  dischargeALowFlag;
	  uint32_t  dischargeBUpFlag;
		uint32_t  dischargeBLowFlag;
		uint32_t  dischargeCUpFlag;
		uint32_t  dischargeCLowFlag;
}partDisChgFlagStru;



#define GAS_NUM               2
#define THREEAXIS_485_NUM     40
#define PRESSSETTL_485_NUM    40
#define CIRCULA_485_NUM   	  5
#define PARTDISCHAG_485_NUM   5
#define CRACKMETER_485_NUM    20
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
														TEMPHUM_485_NUM+\
														CRACKMETER_485_NUM)
#else
	#define TOTOLA_485_NUM     (THREEAXIS_485_NUM+\
														PRESSSETTL_485_NUM+\
														CIRCULA_485_NUM+\
														PARTDISCHAG_485_NUM+\
														WATERDEPTH_485_NUM+\
														TEMPHUM_485_NUM+\
														CRACKMETER_485_NUM)
#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

//modbus+设备名称(波特率)+端口(port1-port4)+设备地址(0-关闭设备)+采集间隔(秒)
#ifdef  USE_4GAS
#define   MODBUS_NUM      11
#else
#define   MODBUS_NUM      7
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
	  TEMPHUM,			WATERDEPTH,CRACKMETER
}modbNumEnum;
#else
typedef enum{
     CIRCULA=0, 	PARTDISCHAG,			PRESSSETTL, 			THREEAXIS,		\
	  TEMPHUM,			WATERDEPTH,CRACKMETER
}modbNumEnum;
#endif
#ifdef  USE_4GAS

#ifndef     ANA_MASK
typedef enum{
		CIRCULA_TIME=0,PARTDISCHAG_TIME,PRESSSETTL_TIME,THREEAXIS_TIME,\
	  CH4_TIME,O2_TIME,H2S_TIME,CO_TIME,\
	  TEMPHUM_TIME,WATERDEPTH_TIME,CRACKMETER_TIME,\
	  ANA_TEMPHUM_TIME,\
	HEART_TIME,REG_TIME
}upDataTimEnum;//需要与modbusName 名称一一对应 来实现代码精简高效
#else
typedef enum{
		CIRCULA_TIME=0,PARTDISCHAG_TIME,PRESSSETTL_TIME,THREEAXIS_TIME,\
	  CH4_TIME,O2_TIME,H2S_TIME,CO_TIME,\
	  TEMPHUM_TIME,WATERDEPTH_TIME,CRACKMETER_TIME,\
	HEART_TIME,REG_TIME
}upDataTimEnum;//需要与modbusName 名称一一对应 来实现代码精简高效
#endif
#else

#ifndef     ANA_MASK
typedef enum{
		CIRCULA_TIME=0,PARTDISCHAG_TIME,PRESSSETTL_TIME,THREEAXIS_TIME,\
	  TEMPHUM_TIME,WATERDEPTH_TIME,\
	  ANA_TEMPHUM_TIME,HEART_TIME,REG_TIME
}upDataTimEnum;//需要与modbusName 名称一一对应 来实现代码精简高效
#else
typedef enum{
		CIRCULA_TIME=0,PARTDISCHAG_TIME,PRESSSETTL_TIME,THREEAXIS_TIME,\
	  TEMPHUM_TIME,WATERDEPTH_TIME,\
	  HEART_TIME,REG_TIME
}upDataTimEnum;//需要与modbusName 名称一一对应 来实现代码精简高效
#endif
#endif
#ifdef  USE_4GAS

#define GAS_TIME    CO_TIME  //4种气体传感器使用 同一个定时器
#else
#define GAS_TIME    22      //使用一种混合气体传感器时候定义此处
#endif

#ifdef  USE_4GAS
const static char  modbusName[MODBUS_NUM][NAME_LEN] ={"HuanLiu","JuFang","FangChenJiang","FangWaiPo","JiaWan","YangQi","LiuHuaQing","YiYangHuaTan","WenShiDu","ShuiWei","LieFengYi"};
const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,   9600,   9600,   9600,   9600,   9600,   9600,9600};
#else
const static char  modbusName[MODBUS_NUM][NAME_LEN] ={"HuanLiu","JuFang","FangChenJiang","FangWaiPo","WenShiDu","ShuiWei","LieFengYi"};
const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,     9600,   9600,9600};

//const static char  modbusName[MODBUS_NUM][NAME_LEN] ={"接地环流","局放","防沉降","防外破","甲烷","氧气","硫化氢","一氧化碳","温湿度","水位","裂缝仪"};
//const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,   9600,   9600,   9600,   9600,   9600,   9600,9600};
//#else
//const static char  modbusName[MODBUS_NUM][NAME_LEN] ={"接地环流","局放","防沉降","防外破","温湿度","水位","裂缝仪"};
//const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,     9600,   9600,960};

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
			modbusStru  crackMeter[CRACKMETER_485_NUM];
///////////////////////////////////////////////////
			uint32_t  cirCulaColTime;
			uint32_t  partDischagColTime;
			uint32_t  pressSetlColTime;
	    uint32_t  threeAxissColTime;
#ifdef  USE_4GAS
	    uint32_t  REV[3]; //此处不能省略 为了lcd显示指针自增来配置
//			uint32_t  ch4ColTime;
//	    uint32_t  o2ColTime;
//			uint32_t  h2sColTime;
			uint32_t  gasColTime;//用co的定时器来采集信息  去掉其他三个气体定时器  合并打包上传
#endif
			uint32_t  tempHumColTime;
			uint32_t  waterDepthColTime;
			uint32_t  crackMeterColTime;
///////////////////////////////////////////////////
#ifndef     ANA_MASK
			analogStru analog[ANALOG_NUM];
#endif
///////////////////////////////////////////////////
//放入各种传感器上限下限值 start  跟modbusStru 一一对应
			circuStru_p      modbusCircul[CIRCULA_485_NUM];
			partDisChgStru_p modbusPartDisChg[PARTDISCHAG_485_NUM];
			pressSettlStru_p modbusPreSettl[PRESSSETTL_485_NUM];
			threeAxisStru_p  modbusThreAxis[THREEAXIS_485_NUM];
#ifdef  USE_4GAS
			ch4Stru_p        modbusCh4[CH4_485_NUM];
			o2Stru_p         modbusO2[O2_485_NUM];
			h2sStru_p        modbusH2s[H2S_485_NUM];
			coStru_p         modbusCo[CO_485_NUM];
#endif
			tempHumStru_p	   modbusTempHum[TEMPHUM_485_NUM];
			depthStru_p			 modbusWaterDepth[WATERDEPTH_485_NUM];
			crackMeterStru_p modbusCrackMeter[CRACKMETER_485_NUM];
		#ifndef     ANA_MASK
			tempHumStru_p    analogTempHum;//不支持多路模拟温度传感器
		#endif
//放入各种传感器上限下限值 end
  
			autoCtrl_stru		 autoctrl[CRTL_TOTAL_NUM];
			//uint32_t         autoCrtl
			




	    
}deviceFlashStru;//存储到flash中

typedef struct{
		uint32_t lowFlag;//低电平
	  uint32_t upFlag;//高电平
}digputFlagStru;


typedef struct{
			circuFlagStru      modbusCircul[CIRCULA_485_NUM];
			partDisChgFlagStru modbusPartDisChg[PARTDISCHAG_485_NUM];
			pressSettlFlagStru modbusPreSettl[PRESSSETTL_485_NUM];
			threeAxisFlagStru  modbusThreAxis[THREEAXIS_485_NUM];
#ifdef  USE_4GAS
			ch4FlagStru        modbusCh4[CH4_485_NUM];
			o2FlagStru         modbusO2[O2_485_NUM];
			h2sFlagStru        modbusH2s[H2S_485_NUM];
			coFlagStru         modbusCo[CO_485_NUM];
#endif
			tempHumFlagStru	   modbusTempHum[TEMPHUM_485_NUM];
			depthFlagStru			 modbusWaterDepth[WATERDEPTH_485_NUM];
	    crackMeterFlagStru modbusCrackMeter[CRACKMETER_485_NUM];
			tempHumFlagStru    analogTempHum;//不支持多路模拟温度传感器
		  digputFlagStru     digInput[DI_NUM];
	    digputFlagStru     digOutput[DO_NUM];
	    digputFlagStru     v33Output[V33O_NUM];
	    digputFlagStru     v5Output[V5O_NUM];
	    digputFlagStru     v12Output[V12O_NUM];
}inoutDevStru;
//DI配置高有效
extern rt_err_t uartDataRec( uartEnum uartNum,uint8_t dat);
extern deviceFlashStru sheet;
extern inoutDevStru inpoutpFlag;


//extern rt_err_t uartDataRec( uartEnum uartNum,uint8_t dat);


















#endif




