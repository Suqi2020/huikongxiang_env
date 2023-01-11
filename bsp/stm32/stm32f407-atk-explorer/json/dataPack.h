#ifndef   __DATAPACK_H__
#define   __DATAPACK_H__

#include   "board.h"
//#define   ACUID_LEN  18
#define   HEAD_LEN   2
#define   LENTH_LEN  2
#define   CRC_LEN    2
#define   TAIL_LEN   2
#define   HEAD      0XAACC
#define   TAIL      0XCCAA

//需要记录上行的messID用于下行回应的判断
typedef  struct
{
		uint32_t upMessID;
	  uint32_t upHeartMessID;
	  uint32_t devRegMessID;
	  uint32_t repDataMessID;
	  //char  devID[16];
    	
}mcuParazStru;

extern mcuParazStru mcu;


//typedef  struct
//{

//	  char  ID[16];
//	  char  model[16];
//		char  name[20];
//		char  ip[20];
//		char  port[8];
//	  char  type[4];
//    	
//}rs485ParaStru;

extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];
//extern  rs485ParaStru devi[];
extern  uint64_t  utcTime(void);
extern  uint32_t upMessIdAdd(void);
/**************************/
//心跳上行定义
//typedef  struct
//{
//	  char identifier[25];
//		char id[16];
//}heartParaStru;
//typedef  struct
//{
//		uint64_t mid;
//	  char packetype[25];
//	  char timestamp[15];
//	  heartParaStru para;
//}heartUpStru;

//extern heartUpStru  heartUp;
//typedef  struct
//{
//	
//}heartDownStru;
//心跳下行先不定义结构体 节约ram flash
/**************************/























extern uint16_t heartUpPack(void);
extern uint16_t devRegPack(void);
extern uint16_t rs485DataPack(void);


#endif


