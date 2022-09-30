#ifndef   __DATAPACK_H__
#define   __DATAPACK_H__

#include   "board.h"

#define   HEAD_LEN   2
#define   LENTH_LEN  2
#define   CRC_LEN    2
#define   TAIL_LEN   2
#define   HEAD      0XAACC
#define   TIAL      0XCCAA
/**************************/
//心跳上行定义
typedef  struct
{
	  char identifier[25];
		char id[16];
}heartParaStru;
typedef  struct
{
		uint64_t mid;
	  char packetype[25];
	  char timestamp[15];
	  heartParaStru para;
}heartUpStru;

extern heartUpStru  heartUp;
//typedef  struct
//{
//	
//}heartDownStru;
//心跳下行先不定义结构体 节约ram flash
/**************************/




























#endif


