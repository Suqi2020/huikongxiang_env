#include <board.h>
//��ֵ���òο� �������modbus��������V0.4.doc��
/*
a�����������豸����ʱ���豸��Ĺ�ϵΪ�߼��루����������ȫ�����㣩��
b���������豸����ʱ���豸��Ĺ�ϵΪ�߼��루��������豸����Ҫִ�У�
c�����������豸�����������豸��Ĺ�ϵ���߼����ʱ����Ҫ��ε������ü���
d�������߼�����ʱ������Ҫɾ�����������������豸��ѡ��Ϊ0���ɣ��߼�������
���ָ��Ϊ�գ����������豸
e��sure cancelȷ����ȡ������(a/b/c/d)����
f��list��ʾ���10��������Ч���߼��б�
g��delete(1-10)ɾ�����ù����߼��б�
h��10���߼�������ָ����ʽָ����ִֵ�е�flag
{ char *inputFlag[10];
	Char *outputFlag[10];
	Char workFlag;
}total[10];
//////////////////////////////////////////////////
autoctrl AI ��ʪ��  GYNJLXSD000000499 1 
autoctrl AI ��ʪ��  GYNJLXSD000000499 2 
autoctrl DI ��ʪ��  GYNJLXSD000000498 1
autoctrl DI ��ʪ��  GYNJLXSD000000498 2 
autoctrl MB ����  GYNJLXSD000000491 1 
autoctrl MB ����  GYNJLXSD000000491 1 
autoctrl MB ����  GYNJLXSD000000492 1 
autoctrl MB ����  GYNJLXSD000000492 2 
autoctrl MB ����  GYNJLXSD000000492 3 
autoctrl DO ˮ��  GYNJLXSD000000499 1 
autoctrl DO ˮ��  GYNJLXSD000000499 2
autoctrl V3O ˮ��  GYNJLXSD000000419 1
autoctrl V5O ˮ��  GYNJLXSD000000429 1
autoctrl V12O ˮ��  GYNJLXSD000000439 1
//////////////////////////////////////////////////
*/
const static char sign[]="[autoCtrl]";
char *inName[]={"AI","DI","MB",};
char *outName[]={"DO","V3O","V5O","V12O"};
uint8_t inputIndex=0;//����������������
uint8_t outputIndex=0;//�����������������

uint8_t pindex=0;//�ܹ����ù���ô���������
//���һ��û�����ù���Index 
uint8_t findCtrlIndex()
{
		for(int i=0;i<CRTL_TOTAL_NUM;i++){
				if(sheet.autoctrl[i].workFlag!=1){
						return i;//�ҵ�û���ù��� 
				}
		}
		return 255;
}
//���ҵ�ǰ������index
uint8_t findInputIndex()
{
}
//���ҵ�ǰ�����index
uint8_t findOutputIndex()
{
}

char testa=100;
static void autoctrl(char argc,char*argv[])
{
		if(rt_strcmp("list",argv[1])==0){
			
				return;
		}//��ӡ
		if(rt_strcmp("sure",argv[1])==0){
			
				return;
		}//ȷ����ť
		if(rt_strcmp("cancel",argv[1])==0){
			
				return;
		}//ȡ����ť
		if(rt_strcmp("delete",argv[1])==0){
			
				return;
		}//��ӡɾ����ť
		int i,j;
		pindex =findCtrlIndex();
		if(pindex==255){
				rt_kprintf("%saotuCtrl num<=8 ,delete it first\n",sign);
			  return;
		}
		for(i=0;i<sizeof(inName)/sizeof(inName[0]);i++){
				if(rt_strcmp(inName[i],argv[1])==0){//find input
						switch(i){
							case 0://AI
								for(j=0;j<ANALOG_NUM;j++){
										if(rt_strcmp(argv[2],sheet.analog[j].name)==0){//find  name
												if(rt_strcmp(argv[3],sheet.analog[j].ID)==0){//find ID
													  uint8_t subname = atoi16(argv[4],10);
													  uint8_t limit   = atoi16(argv[5],10);
														if(sheet.analog[j].subName==subname){//find subname
															  if(subname==1){//�¶�
																		if(limit==0){//����
																				sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.tempLowFlag;
																		}
																		else{//����
																			  sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.tempUpFlag;
																		}
																		inputIndex++;
																}
																if(subname==2){//ʪ��
																		if(limit==0){//����
																				sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.humDownFlag;
																		}
																		else{//����
																			  sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.humUpFlag;
																		}
																		inputIndex++;
																}
																if(subname==0){//ɾ���ϱ����õ��¶�ʪ��ֵ  ����ͼ����ʾ�������ù�������ʾȡ���İ�ť  ͼ�ν�����Ҫ��ֵ��ʾ
																	  if(inputIndex==0)
																				return;
																		inputIndex--;
																		sheet.autoctrl[pindex].inputFlag[inputIndex]=NULL;
																}
														}
												}
										}
								}
								break;
							case 1://DI
								break;
							case 2://MB
								break;
						}
						for(j=0;j<ANALOG_NUM;j++)
						return;
				}
		}
		for(i=0;i<sizeof(outName)/sizeof(outName[0]);i++){
				if(rt_strcmp(outName[i],argv[i])==0){//find output
						
						return;
				}
		}
}
MSH_CMD_EXPORT(autoctrl,autoctrl config);