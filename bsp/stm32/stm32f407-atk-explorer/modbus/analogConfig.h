#ifndef   _ANALOGCONFIG_H_
#define   _ANALOGCONFIG_H_

#include  "board.h"

#ifndef     ANA_MASK
#define   ANALOGNAME_NUM      1
////////////////////////////////////////////////////////////////////////////
const static char  analogName[ANALOGNAME_NUM][NAME_LEN]={"温湿度"};
const static char  anaSubName1Val[ANA_SUBNAME_LEN][NAME_LEN]={"温度","湿度"};
//typedef enum{
//		enumAnaTemp=1,
//	  enumAnaHum=2
//}anaTempHumEnum;

///////////////////////////////////////////////////////////////////////////




typedef struct{
		char  ID[MODBID_LEN];
		char  model[8];
	  char  name[20];
//	  char  funName[16];
	  uint8_t   	port;
	  rt_bool_t 	workFlag;
	  uint32_t   	colTime;
	  char     subName;//子选项  比如温湿度中的 温度 湿度
	  uint8_t     rev[3];
}analogStru;



extern uint32_t chanl[8];
extern uint16_t Get_Adc_Average(uint16_t ch,uint16_t times);





#endif





#endif

