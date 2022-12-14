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






const static char  analogName[20]="��ʪ��";
const static char  analogName1Val[2][20]={"temperature","humidity"};


//uint32_t STMFLASH_ReadWord(uint32_t faddr);		  	//������  
//void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
//void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
////����д��
//void Test_Write(uint32_t WriteAddr,uint32_t WriteData);	
#endif
