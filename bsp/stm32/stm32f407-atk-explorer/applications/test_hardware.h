#ifndef  __TEST_HARDWARE_H__
#define  __TEST_HARDWARE_H__
#include  "board.h"

//#define TEST_ADC
//#define TEST_UART
//#define  TEST_IOIN
//#define  TEST_IOOUT
//#define  TEST_SPAKER
//#define   TEST_RELAY
//#define  TEST_W5500


#ifdef TEST_UART

extern void uart2SendRecTest(void);
extern void uart3SendRecTest(void);
extern void uart4SendRecTest(void);
extern void uart5SendRecTest(void);
extern void uart6SendRecTest(void);

#endif
typedef struct{
	 uint32_t times;
	 uint32_t relayTimer[100];
}offLineStru;

extern uint32_t offLTimes;
extern uint32_t onLTimes;
extern offLineStru offLine;



































#endif

