#ifndef  	__MODBUS_H__
#define  	__MODBUS_H__
#include  "board.h"


#define READ_03       0x03
#define READ_04       0x04
#define READM_BIT  	  0X01
#define WRITE      		0x06
#define WRITE_MUL  		0x10


extern  rt_mutex_t read485_mutex;

uint8_t modbusReadReg(uint16_t slavAddr,uint16_t regAddr,uint8_t cmd,uint16_t len,uint8_t * out);
uint8_t modbusReadBitReg(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out);
//写一个寄存器
uint8_t modbusWriteOneReg(uint16_t slavAddr,uint16_t regAddr,uint16_t value,uint8_t *out);
//写多个寄存器  len  数据长度 len/2寄存器个数
uint8_t modbusWriteMultReg(uint16_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t *in,uint8_t *out);


//modbus回复数据校验 
//rt_bool_t  modbusReadRespCheck(uint16_t slavAddr,uint8_t *buf,uint16_t len);

int  modbusRespCheck(uint16_t slavAddr,uint8_t *buf,uint16_t len,rt_bool_t readFlag);


#endif
