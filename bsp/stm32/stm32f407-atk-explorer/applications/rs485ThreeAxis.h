#ifndef   __RS485_THREEAXIS_H__
#define   __RS485_THREEAXIS_H__

#include <board.h>

typedef struct{
		uint16_t temp;
	  uint16_t acclrationX;
		uint16_t acclrationY;
		uint16_t acclrationZ;
}threeAxisStru;
extern threeAxisStru threeAxis;
#endif
