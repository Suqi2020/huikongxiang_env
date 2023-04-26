#ifndef  __UART_RECONFIG_H__
#define  __UART_RECONFIG_H__

#include "board.h"

#include "dataPack.h"
//存储分为3个部分  1/modbus传感器存储
//								 2/ip 串口 acuid 模拟传感器 输入 输出设备
//								 3/数字,模拟传感器阈值存储


#define  LENTH          500
 
#define  MSGPOOL_LEN   LENTH 
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

//#define  ANALOG_NUM   8//8路模拟量

typedef struct{
		char  devID[DEVID_LEN];
	  char  model[MODEL_LEN];
		char  name[NAME_LEN];
	
    uint8_t port; //1-8
	  uint8_t rev;//
	  uint16_t calTime; //采集间隔时间
	

}analogDevStru;

//typedef struct{
//		bool lowFlag;
//	  bool upFlag;
//}digitStru_p;
typedef struct{
	
		char  devID[DEVID_LEN];
	  char  model[MODEL_LEN];
		char  name[NAME_LEN];
    uint8_t port; //1-8
	  uint8_t workFlag;//RT_TRUE-
//		bool levelFlag;//给个值
	  uint8_t rev[2];
	//	bool upFlag;
}digitStru;//数字输入输出
typedef struct{
//		uartSaveStru   port[UART_NUM];
		netIpFlashStru netIpFlash;        //存储IP数据
//	  analogDevStru  analog[ANALOG_NUM];//存储模拟数据
	  uint32_t       uartBps[UART_NUM];    //存储串口配置
	  char     acuId[ACUID_LEN+1];      //存储acuID
	  char rev;
		digitStru diginput[DI_NUM];   //数字输入
	  digitStru digoutput[DO_NUM];   //数字输出
		digitStru v33output[V33O_NUM]; //3V3电源输出
		digitStru v5output[V5O_NUM];   //5V电源输出
		digitStru v12output[V12O_NUM]; //12V电源输出
	
//			//配置的输入输出高低电平有效需要存储
//		digitStru_p        digInput[DI_NUM];
//		digitStru_p        digOutput[DO_NUM];
//		digitStru_p        digV33Out[V33O_NUM];
//		digitStru_p        digV5out[V5O_NUM];
//		digitStru_p        digV12out[V12O_NUM];
}packIpUartStru;
extern packIpUartStru packFlash;

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////串口设置需要修改此处/////////////////////////////////////////



//	
//typedef struct{
//	 // uint32_t bps;
//	//rt_bool_t offline;
//	  rt_mutex_t  uartMutex;
//	  struct  rt_messagequeue* uartMessque;
//}uartConfStru;


/////////////////////////////////////////////////////////////////////////////////////////////////////

//extern uartConfStru  .uartMessque[];
extern struct  rt_messagequeue uartmque[UART_NUM];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartMutexQueueCfg(void);


rt_bool_t errConfigCheck(void);













#endif

