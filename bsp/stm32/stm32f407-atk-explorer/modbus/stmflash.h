#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "uartReconf.h"
#include "board.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//STM32�ڲ�FLASH��д ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//FLASH��ʼ��ַ

#define  ADDR_FLASH_SECTOR_11       ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */
#define  FLASH_IP_SAVE_ADDR    			 ADDR_FLASH_SECTOR_11  //���һ������ 128K�ֽ� (STM32_FLASH_BASE+STM_FLASH_SIZE-USE_FLASH_SIZE)  
#define  FLASH_MODBUS_SAVE_ADDR     (FLASH_IP_SAVE_ADDR+0X400)

typedef enum{
		USE_UART2=0,
	  USE_UART3,
	  USE_UART6,
	  USE_UART4,
	  USE_DIS_UART=0XFF
}uartEnum;

typedef struct{
		rt_bool_t 	workFlag;
		uartEnum  	useUartNum;
		uint16_t  	slaveAddr; //0-255
		char  ID[20];
		char  model[10];
//		char  name[20];

}modbusStru;
//
#define THREEAXIS_485_NUM    40
#define PRESSSETTL_485_NUM   40
#define CIRCULA_485_NUM   	  5
#define PARTDISCHAG_485_NUM   5
#define CH4_485_NUM   			  2
#define CO_485_NUM   				  2
#define H2S_485_NUM   			  2
#define O2_485_NUM   				  2
#define WATERDEPTH_485_NUM   	  2
#define TEMPHUM_485_NUM   	  2
typedef struct{
			modbusStru  threeAxiss[THREEAXIS_485_NUM];
			modbusStru  cirCula[CIRCULA_485_NUM];
			modbusStru  partDischag[PARTDISCHAG_485_NUM];
			modbusStru  pressSetl[PRESSSETTL_485_NUM];
			modbusStru  ch4[CH4_485_NUM];
			modbusStru  co[CO_485_NUM];
			modbusStru  h2s[H2S_485_NUM];
			modbusStru  o2[O2_485_NUM];
			modbusStru  waterLev[WATERDEPTH_485_NUM];
			modbusStru  tempHum[TEMPHUM_485_NUM];
			
			uint32_t  threeAxissColTime;
			uint32_t  cirCulaColTime;
			uint32_t  partDischagColTime;
			uint32_t  pressSetlColTime;
			uint32_t  ch4ColTime;
			uint32_t  coColTime;
			uint32_t  h2sColTime;
			uint32_t  o2ColTime;
			uint32_t  waterLevColTime;
			uint32_t  tempHumColTime;
	
}modbusFlashStru;
extern modbusFlashStru sheet;

 
extern rt_err_t uartDataRec(uartEnum uartNum,uint8_t dat);

//uint32_t STMFLASH_ReadWord(uint32_t faddr);		  	//������  
//void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
//void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
////����д��
//void Test_Write(uint32_t WriteAddr,uint32_t WriteData);	
#endif
