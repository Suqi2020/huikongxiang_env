#ifndef   _ANALOGCONFIG_H_
#define   _ANALOGCONFIG_H_

#include  "board.h"
#define   ANALOGNAME_NUM      1
////////////////////////////////////////////////////////////////////////////
const static char  analogName[ANALOGNAME_NUM][20]={"��ʪ��"};
//const static char  analogName1Val[2][20]={"temperature","humidity"};
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
	  uint8_t     subName;//��ѡ��  ������ʪ���е� �¶� ʪ��
	  uint8_t     rev[3];
}analogStru;



extern uint32_t chanl[8];
extern uint16_t Get_Adc_Average(uint16_t ch,uint16_t times);











#endif
