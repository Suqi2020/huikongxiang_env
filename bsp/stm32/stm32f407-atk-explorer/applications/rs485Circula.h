#ifndef   __RS485_CIRCULA_H__
#define   __RS485_CIRCULA_H__

#include <board.h>
//1路485 公众环流  目前测试只采集环流值
//  1、读取环流值   
//  2、读取报警标志 
//  3、配置报警阈值 
//  4、读取报警阈值
//  5、设置采集间隔
//  6、读取采集间隔



typedef struct
{
	  //环流值 放大了100倍
		uint32_t circlCurA;
		uint32_t circlCurB;
	  uint32_t circlCurC;
	  uint32_t circlCurD;//备用
	  //阈值
	  uint32_t thresholdVal;

	  
	  //报警状态
	  uint8_t warningA;
	  uint8_t warningB;
	  uint8_t warningC;
	  uint8_t warningD;
	  //采集间隔 单位秒
		uint16_t AcqInterv;
    uint16_t rev;
} CIRCURStru;

extern CIRCURStru  cirCurStru_p;

void readCirCurrAndWaring(void);
rt_bool_t waringcheck(void);
uint16_t 	cirCulaDataPack(void);
void  waringEventPack(void);


#endif
