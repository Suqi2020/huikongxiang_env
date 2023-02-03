#ifndef   _ANALOGCONFIG_H_
#define   _ANALOGCONFIG_H_

#include  "board.h"
#define   ANALOGNAME_NUM      1
////////////////////////////////////////////////////////////////////////////
const static char  analogName[ANALOGNAME_NUM][20]={"温湿度"};
//const static char  analogName1Val[2][20]={"temperature","humidity"};
//typedef enum{
//		enumAnaTemp=1,
//	  enumAnaHum=2
//}anaTempHumEnum;

///////////////////////////////////////////////////////////////////////////
#define  MODBID_LEN   20



typedef struct{
		char  ID[MODBID_LEN];
		char  model[8];
	  char  name[20];
//	  char  funName[16];
	  uint8_t   	port;
	  rt_bool_t 	workFlag;
	  uint32_t   	colTime;
	  uint8_t     subName;//子选项  比如温湿度中的 温度 湿度
	  uint8_t     rev[3];
}analogStru;
















#endif

