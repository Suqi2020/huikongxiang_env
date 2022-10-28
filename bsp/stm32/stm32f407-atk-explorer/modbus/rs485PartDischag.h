#ifndef   __RS485PARTSISCHARGE_H__
#define   __RS485PARTSISCHARGE_H__
#include <board.h>


 typedef struct  
 { 
  unsigned a:1; 
  unsigned b:3; 
  unsigned c:4; 
 } bs; 
 
typedef struct{
		uint32_t amplitudeA;
	  uint32_t freqA;
	  uint32_t dischargeA;
	
		uint32_t amplitudeB;
		uint32_t freqB;
		uint32_t dischargeB;

		uint32_t amplitudeC;
		uint32_t freqC;
		uint32_t dischargeC;
	
	  bs alarm;  //bit0 A bit1 B bit2 C
	  uint8_t rev[3];
}partDischargeStru;

extern partDischargeStru partDisc;

void readPdFreqDischarge(void);
//extern rt_bool_t  PartDischgWarningCheck(void);
extern rt_bool_t readPartDischgWarning(void);
extern void  partDisWaringEventPack(void);
void  partDisDataPack(void);
#endif




