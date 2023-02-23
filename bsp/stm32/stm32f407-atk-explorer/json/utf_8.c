#include "board.h"
#include "uartReconf.h"
//utf8格式传感器名称 上传时候用
#ifdef USE_4GAS 
//const  char  modbusName_utf8[MODBUS_NUM][30] ={"接地环流","局放","沉降仪","防外破","甲烷","氧气","硫化氢","一氧化碳","温湿度","水位"};

#else

const  char  modbusName_utf8[MODBUS_NUM][30] ={"接地环流","局放","沉降仪","防外破","温湿度","水位"};

#endif
