#ifndef  __TEST_HARDWARE_H__
#define  __TEST_HARDWARE_H__


//#define TEST_ADC
#define TEST_UART
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




































#endif

