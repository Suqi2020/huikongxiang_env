#ifndef   __RS485_PRESS_SETTLE_H__
#define   __RS485_PRESS_SETTLE_H__

#include <board.h>
typedef struct{
	  uint16_t temp;//����100 ����float����  ��λ0C
	  uint16_t height;//����10 ����float����  ��λmm
}pressSettlStru;

extern pressSettlStru pressSettle[PRESSSETTL_485_NUM];

#endif
