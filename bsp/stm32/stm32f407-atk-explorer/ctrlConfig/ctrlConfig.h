#ifndef __CRTL_CONFIG_H__
#define __CRTL_CONFIG_H__


#include "board.h"








//typedef struct{
//	
//}thresholdStru;

#define CRTL_IN_NUM      10   //ÿ���߼�����֧��10������
#define CRTL_OUT_NUM     5   //ÿ���߼�����֧��5���������
#define CRTL_TOTAL_NUM   10  //�ܹ�10���߼�����
typedef struct{
	bool *inputFlag[CRTL_IN_NUM];
	bool *outputFlag[CRTL_OUT_NUM];
	uint8_t workFlag; //1 ���óɹ�  ����-ʧ��
}autoCtrl_stru;












#endif
