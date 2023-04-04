#ifndef __CRTL_CONFIG_H__
#define __CRTL_CONFIG_H__


#include "board.h"






#define INOUTNAME_LEN    8

//typedef struct{
//	
//}thresholdStru;

#define CRTL_IN_NUM      10   //每条逻辑控制支持10个输入
#define CRTL_OUT_NUM     5   //每条逻辑控制支持5个输出控制
#define CRTL_TOTAL_NUM   20  //总共10条逻辑控制

typedef struct{
	  char  typeName[INOUTNAME_LEN];//如果长度大于  8
	  char  senseName[20]; 
	  char  ID[MODBID_LEN];//20
	  char  subName[20];
	  int  limit;
	  //char  rev[2];
	  uint32_t  *flag;
}ctrlInOutStru;//本结构体存储为了查看已经设置成功的逻辑控制列表

typedef struct{
		ctrlInOutStru input[CRTL_IN_NUM];
		ctrlInOutStru output[CRTL_OUT_NUM];
		uint8_t workFlag; //1 设置成功  其他-失败
}autoCtrl_stru;












#endif
