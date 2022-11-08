#ifndef   __RS485_GAS_H__
#define   __RS485_GAS_H__

#include <board.h>
//typedef struct{
//	  uint16_t ch4;//气体浓度值 扩大了1000倍
//	  uint16_t co;
//	  uint16_t o2;
//	  uint16_t h2s;
//}concentrationStru;

//extern concentrationStru gas;
extern  void readCH4(int num);
extern  void readCO(int num);
extern  void readH2S(int num);
extern  void readO2(int num);
#endif


