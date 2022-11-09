#ifndef   __RS485_CIRCULA_H__
#define   __RS485_CIRCULA_H__

#include <board.h>
//1·485 ���ڻ���  Ŀǰ����ֻ�ɼ�����ֵ
//  1����ȡ����ֵ   
//  2����ȡ������־ 
//  3�����ñ�����ֵ 
//  4����ȡ������ֵ
//  5�����òɼ����
//  6����ȡ�ɼ����
/*
��ȡ��Ӳ��	# SEND HEX>
01 03 00 00 00 02 C4 0B	# RECV HEX>
01 03 04 11 10 13 00 F3 FA
��ȡ485ID	# SEND HEX>
01 03 00 02 00 01 25 CA	# RECV HEX>
01 03 02 00 01 79 84
����485ID	# SEND HEX>
01 06 00 02 00 01 E9 CA	# RECV HEX>
01 06 00 02 00 01 E9 CA
��ȡ������	# SEND HEX>
01 03 00 03 00 01 74 0A	# RECV HEX>
01 03 02 00 02 39 85
���ò�����	# SEND HEX>
01 06 00 03 00 02 F8 0B	# RECV HEX>
01 06 00 03 00 02 F8 0B
��ȡ�ɼ����	# SEND HEX>
01 03 00 04 00 01 C5 CB	# RECV HEX>
01 03 02 00 06 38 46
���òɼ����	# SEND HEX>
01 06 00 04 00 05 08 08	# RECV HEX>
01 06 00 04 00 05 08 08
��ȡ��������������	# SEND HEX>
01 03 00 05 00 04 54 08	# RECV HEX>
01 03 08 01 2C 01 2C 01 2C 01 2C E9 B6
���û�������������	# SEND HEX>
01 06 00 05 01 2D 58 46	# RECV HEX>
01 06 00 05 01 2D 58 46
��ȡ������ֵ	# SEND HEX>
01 03 00 09 00 02 14 09	# RECV HEX>
01 03 04 00 00 00 32 7B E6
���ñ�����ֵ	# SEND HEX>
01 10 00 09 00 02 04 00 00 00 33 73 D0	# RECV HEX>
01 10 00 09 00 02 91 CA 
��ȡС������ʽ	# SEND HEX>
01 03 00 0B 00 01 F5 C8	# RECV HEX>
01 03 02 00 00 B8 44
����С������ʽ	# SEND HEX>
01 06 00 0B 00 01 39 C8	# RECV HEX>
01 06 00 0B 00 01 39 C8
��ȡʱ���	# SEND HEX>
01 03 00 20 00 02 C5 C1	# RECV HEX>
01 03 04 5E 1D EC 3B 74 CE
����ʱ���	# SEND HEX>
01 10 00 20 00 02 04 5E 1D EC 00 3E 99	# RECV HEX>
01 10 00 20 00 02 40 02
ִ���豸��λ	# SEND HEX>
01 06 00 22 00 01 E8 00	# RECV HEX>
01 06 00 22 00 01 E8 00
��ȡ����ֵ	# SEND HEX>
01 03 00 23 00 08 B5 C6	# RECV HEX>
01 03 10 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E4 59
��ȡ������־	# SEND HEX>
01 03 00 2B 00 04 34 01	# RECV HEX>
01 03 08 00 00 00 00 00 00 00 00 95 D7
*/

typedef struct
{
	  //����ֵ �Ŵ���100��
		float circlCurA;
		float circlCurB;
	  float circlCurC;
	  float circlCurD;//����
	  //��ֵ
	  //uint32_t thresholdVal;

	  
	  //����״̬  һ���ֽ��㹻 ����Խ�modbus��Ӧ
	  uint16_t warningA;
	  uint16_t warningB;
	  uint16_t warningC;
	  uint16_t warningD;
	  //�ɼ���� ��λ��
		//uint16_t AcqInterv;
	//С���������ֵ
    uint16_t point; //��modbus��ʵֵ  �˴���ȡmodbus�󾭹���ת������ֱ�Ӽ���  0-ֵΪ100  1-2 ֵΪ10
} CIRCURStru;

//extern CIRCURStru  cirCurStru_p[];

void readCirCurrAndWaring(int num);
rt_bool_t cirCurrWaringcheck(int num);
uint16_t 	cirCulaDataPack(void);
void  cirCurrWaringEventPack(void);
void readCirCurrAndWaring(int num);

#endif
