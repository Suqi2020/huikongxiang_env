#ifndef   __FLASH_SACE_H__
#define   __FLASH_SACE_H__


#include  "board.h"

#define  DEV_NUM         100   //���֧��100��������

#define  STM32_FLASH_BASE     0x08000000//STM32 FLASH����ʼ��ַ
#define  STM_FLASH_SIZE       0X00100000//�ܴ�С
#define  USE_FLASH_SIZE       0X4000  // ����10kflash�洢

#define  FLASH_IP_SAVE_ADDR  (STM32_FLASH_BASE+STM_FLASH_SIZE-USE_FLASH_SIZE)  
#define  FLASH_MODBUS_SAVE_ADDR     (FLASH_IP_SAVE_ADDR+0X400)
//#define  FLASH_SAVE_ADDR     (FLASH_IP_SAVE_ADDR+0X400)
//////////////////////////////////////////////
//��Ҫ���浽flash������
typedef struct{
		char  devID[16];
	  char  model[8];
		char  name[20];
	
    uint8_t port; //1-4
	  uint8_t devAddr;//1-254  0�ǹرմ��豸  255flashĬ�϶���
	  uint16_t regCmd;
	
	  uint16_t regAddr;
	  uint16_t regLen;
}modbusDevSaveStru;
extern modbusDevSaveStru  modbusDevSave[DEV_NUM];

typedef enum{
		USE_UART2=0,
	  USE_UART3,
	  USE_UART6,
	  USE_UART4,
	  USE_DIS_UART=0XFF
}uartEnum;



/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////����������Ҫ�޸Ĵ˴�/////////////////////////////////////////
#define   UART_NUM        4//һ��4·����

const static char     UartName[UART_NUM][6] ={"port1", "port2",  "port3",  "port4"};//��ӳ��һ������
const static uartEnum UartNum[UART_NUM]     ={USE_UART2,USE_UART3,USE_UART6,USE_UART4};//��ӳ��һ������
/////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct{
	  rt_mutex_t  uartMutex;
	  struct  rt_messagequeue* uartMessque;
}uartConfStru;


typedef struct{
		uint32_t bps;
    uint16_t calTime;	

	  uint8_t rev[2];
}uartSaveStru;
extern uartSaveStru  port[UART_NUM];

typedef struct{
		
	  uint8_t  localIp[4];
	  uint8_t  gateway[4];
	  uint8_t  remoteIp[4];
    uint16_t   remotePort; 
	  uint8_t  macaddr;
	  uint8_t  rev;
}netIpFlashStru;

typedef struct{
		uartSaveStru  port[UART_NUM];
		netIpFlashStru netIpFlash;
}packIpUartStru;
extern packIpUartStru packFLash;
extern rt_bool_t  modbusCheckWork(uint8_t port);
//extern  netIpFlashStru netIpFlash;
/////////////////////////////////////////////
/*
���modbus  �ַ�   GY280     devid           port      devaddr   regcmd        regaddr     reglen��
      modbus   ����   ����      �豸ID 	       �˿�      �豸��ַ   �Ĵ�������   �Ĵ�����ַ �Ĵ�������
      modbus  �ַ�   GY280  GYNJLXSD000000458  port3     1             3    		 2     	2
      modbus  save//�洢
      modbus  �ַ�   GY280  GYNJLXSD000000458  port3     0            3    		 2     	2//ͣ���豸 ������Ѿ����ù��ĵ�ַ�����мĴ���

*/

#endif



