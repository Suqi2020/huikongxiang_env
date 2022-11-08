#ifndef   __RS485_PRESS_SETTLE_H__
#define   __RS485_PRESS_SETTLE_H__

#include <board.h>
typedef struct{
	  uint16_t temp;//除以100 传输float类型  单位0C
	  uint16_t height;//除以10 传输float类型  单位mm
}pressSettlStru;

extern pressSettlStru pressSettle[PRESSSETTL_485_NUM];

#endif
