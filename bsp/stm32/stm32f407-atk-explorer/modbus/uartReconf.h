#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"
//�洢��Ϊ3������  1/modbus�������洢
//								 2/ip ���� acuid ģ�⴫���� ���� ����豸
//								 3/����,ģ�⴫������ֵ�洢


#define   UART_NUM        4//һ��4·����
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
////    uint8_t  delayTime;//100ms��������  �˿��±߹��صĴ������ɼ����ʱ��  �ַ�2��
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

#define  ANALOG_NUM   8//8·ģ����

typedef struct{
		char  devID[DEVID_LEN];
	  char  model[MODEL_LEN];
		char  name[NAME_LEN];
	
    uint8_t port; //1-8
	  uint8_t rev;//
	  uint16_t calTime; //�ɼ����ʱ��
	

}analogDevStru;


typedef struct{
	
		char  devID[DEVID_LEN];
	  char  model[MODEL_LEN];
		char  name[NAME_LEN];
    uint8_t port; //1-8
	  uint8_t workFlag;//RT_TRUE-
	  uint8_t rev[2];
}digitStru;//�����������
typedef struct{
//		uartSaveStru   port[UART_NUM];
		netIpFlashStru netIpFlash;        //�洢IP����
	  analogDevStru  analog[ANALOG_NUM];//�洢ģ������
	  uint32_t       uartBps[UART_NUM];    //�洢��������
	  char     acuId[ACUID_LEN+1];      //�洢acuID
	  char rev;
		digitStru input[DI_NUM];   //��������
	  digitStru output[DO_NUM];   //�������
}packIpUartStru;
extern packIpUartStru packFLash;
#define  MODBID_LEN   20
#define  MODL_LEN     8
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////����������Ҫ�޸Ĵ˴�/////////////////////////////////////////

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
			


}deviceFlashStru;
/////////////////////////////////////////////////////////////////////////////////////////////////////
extern deviceFlashStru sheet;
extern uartConfStru  uartDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartMutexQueueCfg(void);

extern rt_err_t uartDataRec(uartEnum uartNum,uint8_t dat);
rt_bool_t errConfigCheck(void);













#endif

