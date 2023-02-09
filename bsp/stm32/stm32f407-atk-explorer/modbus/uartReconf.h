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
#define   V33O_NUM        2
#define   V5O_NUM         2
#define   V12O_NUM        4
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

//#define  ANALOG_NUM   8//8·ģ����

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
//	  analogDevStru  analog[ANALOG_NUM];//�洢ģ������
	  uint32_t       uartBps[UART_NUM];    //�洢��������
	  char     acuId[ACUID_LEN+1];      //�洢acuID
	  char rev;
		digitStru input[DI_NUM];   //��������
	  digitStru digoutput[DO_NUM];   //�������
		digitStru v33output[V33O_NUM]; //3V3��Դ���
		digitStru v5output[V5O_NUM];   //5V��Դ���
		digitStru v12output[V12O_NUM]; //12V��Դ���
}packIpUartStru;
extern packIpUartStru packFLash;

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////����������Ҫ�޸Ĵ˴�/////////////////////////////////////////



	
typedef struct{
	 // uint32_t bps;
	//rt_bool_t offline;
	  rt_mutex_t  uartMutex;
	  struct  rt_messagequeue* uartMessque;
}uartConfStru;


/////////////////////////////////////////////////////////////////////////////////////////////////////

extern uartConfStru  uartDev[];
extern void rs485UartSend(uint8_t chanl,uint8_t *buf,int len);
extern void uartMutexQueueCfg(void);


rt_bool_t errConfigCheck(void);













#endif

