#ifndef __CRTL_CONFIG_H__
#define __CRTL_CONFIG_H__


#include "board.h"








//typedef struct{
//	
//}thresholdStru;

#define CRTL_IN_NUM      10   //每条逻辑控制支持10个输入
#define CRTL_OUT_NUM     5   //每条逻辑控制支持5个输出控制
#define CRTL_TOTAL_NUM   10  //总共10条逻辑控制
typedef struct{
	bool *inputFlag[CRTL_IN_NUM];
	bool *outputFlag[CRTL_OUT_NUM];
	uint8_t workFlag; //1 设置成功  其他-失败
}autoCtrl_stru;












#endif
