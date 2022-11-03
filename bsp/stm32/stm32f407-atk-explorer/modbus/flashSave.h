#ifndef   __FLASH_SACE_H__
#define   __FLASH_SACE_H__


#include  "board.h"

#define  DEV_NUM         100   //最大支持100个传感器

#define  STM32_FLASH_BASE     0x08000000//STM32 FLASH的起始地址
#define  STM_FLASH_SIZE       0X00100000//总大小
#define  USE_FLASH_SIZE       0X4000  // 开启10kflash存储

#define  FLASH_IP_SAVE_ADDR  (STM32_FLASH_BASE+STM_FLASH_SIZE-USE_FLASH_SIZE)  
#define  FLASH_MODBUS_SAVE_ADDR     (FLASH_IP_SAVE_ADDR+0X400)
//#define  FLASH_SAVE_ADDR     (FLASH_IP_SAVE_ADDR+0X400)
//////////////////////////////////////////////
//需要保存到flash的数据
typedef struct{
		char  devID[16];
	  char  model[8];
		char  name[20];
	
    uint8_t port; //1-4
	  uint8_t devAddr;//1-254  0是关闭此设备  255flash默认读出
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
////////////////////////////////////////串口设置需要修改此处/////////////////////////////////////////
#define   UART_NUM        4//一共4路串口

const static char     UartName[UART_NUM][6] ={"port1", "port2",  "port3",  "port4"};//重映射一个名称
const static uartEnum UartNum[UART_NUM]     ={USE_UART2,USE_UART3,USE_UART6,USE_UART4};//重映射一个名称
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
命令《modbus  局放   GY280     devid           port      devaddr   regcmd        regaddr     reglen》
      modbus   名称   类型      设备ID 	       端口      设备地址   寄存器命令   寄存器地址 寄存器长度
      modbus  局放   GY280  GYNJLXSD000000458  port3     1             3    		 2     	2
      modbus  save//存储
      modbus  局放   GY280  GYNJLXSD000000458  port3     0            3    		 2     	2//停用设备 并清除已经设置过的地址和所有寄存器

*/

#endif



