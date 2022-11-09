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
/*
读取软硬件	# SEND HEX>
01 03 00 00 00 02 C4 0B	# RECV HEX>
01 03 04 11 10 13 00 F3 FA
读取485ID	# SEND HEX>
01 03 00 02 00 01 25 CA	# RECV HEX>
01 03 02 00 01 79 84
配置485ID	# SEND HEX>
01 06 00 02 00 01 E9 CA	# RECV HEX>
01 06 00 02 00 01 E9 CA
读取波特率	# SEND HEX>
01 03 00 03 00 01 74 0A	# RECV HEX>
01 03 02 00 02 39 85
配置波特率	# SEND HEX>
01 06 00 03 00 02 F8 0B	# RECV HEX>
01 06 00 03 00 02 F8 0B
读取采集间隔	# SEND HEX>
01 03 00 04 00 01 C5 CB	# RECV HEX>
01 03 02 00 06 38 46
配置采集间隔	# SEND HEX>
01 06 00 04 00 05 08 08	# RECV HEX>
01 06 00 04 00 05 08 08
读取环流传感器增益	# SEND HEX>
01 03 00 05 00 04 54 08	# RECV HEX>
01 03 08 01 2C 01 2C 01 2C 01 2C E9 B6
配置环流传感器增益	# SEND HEX>
01 06 00 05 01 2D 58 46	# RECV HEX>
01 06 00 05 01 2D 58 46
读取报警阈值	# SEND HEX>
01 03 00 09 00 02 14 09	# RECV HEX>
01 03 04 00 00 00 32 7B E6
配置报警阈值	# SEND HEX>
01 10 00 09 00 02 04 00 00 00 33 73 D0	# RECV HEX>
01 10 00 09 00 02 91 CA 
读取小数处理方式	# SEND HEX>
01 03 00 0B 00 01 F5 C8	# RECV HEX>
01 03 02 00 00 B8 44
配置小数处理方式	# SEND HEX>
01 06 00 0B 00 01 39 C8	# RECV HEX>
01 06 00 0B 00 01 39 C8
读取时间戳	# SEND HEX>
01 03 00 20 00 02 C5 C1	# RECV HEX>
01 03 04 5E 1D EC 3B 74 CE
配置时间戳	# SEND HEX>
01 10 00 20 00 02 04 5E 1D EC 00 3E 99	# RECV HEX>
01 10 00 20 00 02 40 02
执行设备复位	# SEND HEX>
01 06 00 22 00 01 E8 00	# RECV HEX>
01 06 00 22 00 01 E8 00
读取环流值	# SEND HEX>
01 03 00 23 00 08 B5 C6	# RECV HEX>
01 03 10 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E4 59
读取报警标志	# SEND HEX>
01 03 00 2B 00 04 34 01	# RECV HEX>
01 03 08 00 00 00 00 00 00 00 00 95 D7
*/

typedef struct
{
	  //环流值 放大了100倍
		float circlCurA;
		float circlCurB;
	  float circlCurC;
	  float circlCurD;//备用
	  //阈值
	  //uint32_t thresholdVal;

	  
	  //报警状态  一个字节足够 方便对接modbus回应
	  uint16_t warningA;
	  uint16_t warningB;
	  uint16_t warningC;
	  uint16_t warningD;
	  //采集间隔 单位秒
		//uint16_t AcqInterv;
	//小数点计算数值
    uint16_t point; //非modbus真实值  此处读取modbus后经过了转换便于直接计算  0-值为100  1-2 值为10
} CIRCURStru;

//extern CIRCURStru  cirCurStru_p[];

void readCirCurrAndWaring(int num);
rt_bool_t cirCurrWaringcheck(int num);
uint16_t 	cirCulaDataPack(void);
void  cirCurrWaringEventPack(void);
void readCirCurrAndWaring(int num);

#endif
