#ifndef __MODBUSCONFIG_H__
#define __MODBUSCONFIG_H__
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
//modbus��������ʪ��
typedef struct{
		float tempUpLimit;
		float tempLowLimit;
		float humUpLimit;
		float humLowLimit;
}tempHumStru_p;

//modbus������������
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
//modbus���� ������
typedef struct{
	  float tempUpLimit;
		float tempLowLimit;
	  float heightUpLimit;
	  float heightLowLimit;
}pressSettlStru_p;
typedef struct
{
	  //����ֵ �Ŵ���100��
		float cirCurAUpLimit;
	  float cirCurALowLimit;
		float cirCurBUpLimit;
	  float cirCurBLowLimit;
		float cirCurCUpLimit;
	  float cirCurCLowLimit;
}circuStru_p;

//modbus������co
typedef struct{
		float coUpLimit;
		float coLowLimit;
}coStru_p;
//modbus������O2
typedef struct{
		float o2UpLimit;
		float o2LowLimit;
}o2Stru_p;
//modbus������ch4
typedef struct{
		float ch4UpLimit;
		float ch4LowLimit;
}ch4Stru_p;
//modbus������h2s
typedef struct{
		float h2sUpLimit;
		float h2sLowLimit;
}h2sStru_p;
//modbus������waterLev
typedef struct{
		float depthUpLimit;
		float depthLowLimit;
}depthStru_p;

//modbus������partDischarg
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

//modbus+�豸����(������)+�˿�(port1-port4)+�豸��ַ(0-�ر��豸)+�ɼ����(��)
#ifdef  USE_4GAS
#define   MODBUS_NUM      10
#else
#define   MODBUS_NUM      6
#endif
typedef struct{
	 void (* modbusRead)(void );
}modbusFunStru;
extern modbusFunStru modbusFun[MODBUS_NUM];
//////////////////////////////���Ӵ�������Ҫ�޸��±� һһ��Ӧ////////////////////////////////////////

//extern deviceFlashStru  modbusFlash[MODBUS_NUM];//��Ҫ�洢��flash���豸����
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
}upDataTimEnum;//��Ҫ��modbusName ����һһ��Ӧ ��ʵ�ִ��뾫���Ч
#else
typedef enum{
		CIRCULA_TIME=0,PARTDISCHAG_TIME,PRESSSETTL_TIME,THREEAXIS_TIME,\
	  TEMPHUM_TIME,WATERDEPTH_TIME,\
	  ANA_TEMPHUM_TIME,HEART_TIME,REG_TIME
}upDataTimEnum;//��Ҫ��modbusName ����һһ��Ӧ ��ʵ�ִ��뾫���Ч
#endif
#ifdef  USE_4GAS

#define GAS_TIME    CO_TIME  //4�����崫����ʹ�� ͬһ����ʱ��
#else
#define GAS_TIME    22      //ʹ��һ�ֻ�����崫����ʱ����˴�
#endif

#ifdef  USE_4GAS
const static char  modbusName[MODBUS_NUM][20] ={"�ӵػ���","�ַ�","������","������","����","����","����","һ����̼","��ʪ��","ˮλ"};
const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,   9600,   9600,   9600,   9600,   9600,   9600};
#else
const static char  modbusName[MODBUS_NUM][20] ={"�ӵػ���","�ַ�","������","������","��ʪ��","ˮλ"};
const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,     9600,   9600};

#endif
//const static int   modbusType[MODBUS_NUM]     ={1,        1,       2,      2, 		 3,  			3,  		3,  		3,  		3,  		3};//��ͬ���͵�modbus�豸������ͬ
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
			uint32_t  gasColTime;//��co�Ķ�ʱ�����ɼ���Ϣ  ȥ�������������嶨ʱ��  �ϲ�����ϴ�
			uint32_t  tempHumColTime;
			uint32_t  waterDepthColTime;
///////////////////////////////////////////////////
			analogStru analog[ANALOG_NUM];
///////////////////////////////////////////////////
//������ִ�������������ֵ start

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
			
			tempHumStru_p    analogTempHum;//��֧�ֶ�·ģ���¶ȴ�����
//������ִ�������������ֵ end
}deviceFlashStru;


extern rt_err_t uartDataRec( uartEnum uartNum,uint8_t dat);
extern deviceFlashStru sheet;



//extern rt_err_t uartDataRec( uartEnum uartNum,uint8_t dat);


















#endif




