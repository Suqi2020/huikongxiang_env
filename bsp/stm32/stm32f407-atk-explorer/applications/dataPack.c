#include  "dataPack.h"


heartUpStru  heartUp;
//�������ݴ��
void heartUpPack()
{
	
		heartUp.mid=0x0001;
		strcpy(heartUp.packetype,"CMD_HEARTBEAT");
		strcpy(heartUp.timestamp,"123456789099");
	  strcpy(heartUp.para.identifier,"area_control_unit");
	  strcpy(heartUp.para.id,"10000000000001");
}
//���������� ע����Ϣ����һ��task  ����ά��
//1������������������ÿ�η��ͺ�������ʱ�� ����ղ�����Ӧ��һֱ�ط�  ���5��
//2�������÷������� 
//3�����ͷ�2�� �������͵�  �ͻظ��ģ���һ�Σ�
//  1��task
           
//�������ݽ���
void AllDownPhrase(char *data,int lenth)
{
//	1������ͷβУ�� ���Զ���
//	2����ȡpackettype���ֱ�У��
}