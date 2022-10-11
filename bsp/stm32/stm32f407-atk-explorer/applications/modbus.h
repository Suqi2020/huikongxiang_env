#ifndef  	__MODBUS_H__
#define  	__MODBUS_H__
#include  "board.h"


//typedef struct{
//	int len;
//	char* data;
//} MQTTLenString;

//extern MQTTLenString test;
uint8_t modbusReadReg(uint8_t slavAddr,uint16_t regAddr,uint16_t len,uint8_t * out);
uint8_t modbusWriteOneReg(uint8_t slavAddr,uint8_t regAddr,uint8_t *out);
uint8_t modbusWriteMultReg(uint8_t slavAddr,uint8_t regAddr,uint16_t len,uint8_t *out);
rt_bool_t  modbusReadRespCheck(uint8_t slavAddr,uint8_t *buf,uint16_t len);






#endif
