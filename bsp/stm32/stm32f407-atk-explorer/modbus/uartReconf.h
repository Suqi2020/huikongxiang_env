#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"


#define  SAME_DEV_NUM         40

#define   UART_NUM        4//һ��4·����




 

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
		char  devID[20];
	  char  model[8];
		char  name[20];
	
    uint8_t port; //1-8
	  uint8_t rev;//
	  uint16_t calTime; //�ɼ����ʱ��
	

}analogDevStru;

typedef struct{
//		uartSaveStru   port[UART_NUM];
		netIpFlashStru netIpFlash;
	  analogDevStru  analog[ANALOG_NUM];
	  uint32_t    uartBps[UART_NUM];
	  char     acuId[20];
}packIpUartStru;
extern packIpUartStru packFLash;

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////����������Ҫ�޸Ĵ˴�/////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct{
	 // uint32_t bps;
	//rt_bool_t offline;
	  rt_mutex_t  uartMutex;
	  struct  rt_messagequeue* uartMessque;
}uartConfStru;
//modbus+�豸����(������)+�˿�(port1-port4)+�豸��ַ(0-�ر��豸)+�ɼ����(��)
#define   MODBUS_NUM      10
typedef struct{
	 void (* modbusRead)(void );
}modbusFunStru;
extern modbusFunStru modbusFun[MODBUS_NUM];
//////////////////////////////���Ӵ�������Ҫ�޸��±� һһ��Ӧ////////////////////////////////////////

//extern deviceFlashStru  modbusFlash[MODBUS_NUM];//��Ҫ�洢��flash���豸����
typedef enum{
     CIRCULA=0, 	PARTDISCHAG,			PRESSSETTL, 			THREEAXIS,			CH4,		O2		,H2S,			CO,			TEMPHUM,			WATERDEPTH
}modbNumEnum;

typedef enum{
		CIRCULA_TIME=0,PARTDISCHAG_TIME,PRESSSETTL_TIME,THREEAXIS_TIME,CH4_TIME,O2_TIME,H2S_TIME,CO_TIME,TEMPHUM_TIME,WATERDEPTH_TIME,
	  ANA_TEMPHUM_TIME,
		HEART_TIME,REG_TIME
}upDataTimEnum;//��Ҫ��modbusName ����һһ��Ӧ ��ʵ�ִ��뾫���Ч
const static char  modbusName[MODBUS_NUM][20] ={"�ӵػ���","�ַ�","������","������","����","����","����","һ����̼","��ʪ��","ˮλ"};
const static int   modbusBps[MODBUS_NUM]      ={115200,   115200  ,9600,   9600,   9600,   9600,   9600,   9600,   9600,   9600};
const static int   modbusType[MODBUS_NUM]     ={1,        1,       2,      2, 		 3,  			3,  		3,  		3,  		3,  		3};//��ͬ���͵�modbus�豸������ͬ
/////////////////////////////////////////////////////////////////////////////////////////////////////

extern uartConfStru  uartDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartMutexQueueCfg(void);


rt_bool_t errConfigCheck(void);













#endif

