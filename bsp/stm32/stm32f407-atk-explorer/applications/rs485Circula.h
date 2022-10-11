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



typedef struct
{
	  //����ֵ �Ŵ���100��
		uint32_t circlCurA;
		uint32_t circlCurB;
	  uint32_t circlCurC;
	  uint32_t circlCurD;//����
	  //��ֵ
	  uint32_t thresholdVal;

	  
	  //����״̬
	  uint8_t warningA;
	  uint8_t warningB;
	  uint8_t warningC;
	  uint8_t warningD;
	  //�ɼ���� ��λ��
		uint16_t AcqInterv;
    uint16_t rev;
} CIRCURStru;

extern CIRCURStru  cirCurStru_p;

void readCirCurrAndWaring(void);
rt_bool_t waringcheck(void);
uint16_t 	cirCulaDataPack(void);
void  waringEventPack(void);


#endif
