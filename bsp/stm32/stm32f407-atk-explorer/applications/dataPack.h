#ifndef   __DATAPACK_H__
#define   __DATAPACK_H__

#include   "board.h"

#define   HEAD_LEN   2
#define   LENTH_LEN  2
#define   CRC_LEN    2
#define   TAIL_LEN   2
#define   HEAD      0XAACC
#define   TAIL      0XCCAA

//��Ҫ��¼���е�messID�������л�Ӧ���ж�
typedef  struct
{
		uint32_t upMessID;
	  uint32_t upHeartMessID;
	  uint32_t devRegMessID;
	  uint32_t repDataMessID;
	  char  devID[15];
    	
}deviceParazStru;
extern deviceParazStru device;
/**************************/
//�������ж���
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
//���������Ȳ�����ṹ�� ��Լram flash
/**************************/























extern uint16_t heartUpPack(void);




#endif


