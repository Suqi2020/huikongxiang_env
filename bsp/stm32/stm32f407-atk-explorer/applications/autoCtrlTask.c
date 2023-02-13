#include   "NetJsonDataTask.h"
#include   "board.h"

const static char sign[]="[autoCtrlTask]";
//char *sign={[]};
//�Զ������߼��Ĵ���
//1/��ֵ���ж� �Լ���λ���  �������������豸
void  thresholdOutSetInputFlag()
{
}
//2/���ݱ����ִ�е�������Ʊ��
void  autoCtrlRun()
{
		int i,j,k;
		for(int i=0;i<CRTL_TOTAL_NUM;i++){
				if(sheet.autoctrl[i].workFlag==true){
						for(int j=0;j<CRTL_IN_NUM;j++){
								if(sheet.autoctrl[i].input[j].flag!=NULL){//ָ��ȫ����Ϊ��
										if(*sheet.autoctrl[i].input[j].flag==false){//�����һ����ǲ����� ��������Ϊ���б�ǵĹ�ϵ���룩
												rt_kprintf("%s input flag not ok\n",sign);
												break;
										}
								}
						}
						if(j==CRTL_IN_NUM){//��������ȫ��������  ִ����� �˴���������λ���λ  ��Ҫ����һ�������е���
							  for(k=0;k<CRTL_OUT_NUM;k++){
										if(sheet.autoctrl[i].output[k].flag!=NULL){//ָ�벻Ϊ��
												*sheet.autoctrl[i].output[k].flag=true;//ִ��  ����ָ��ִ�е�flag���
											  rt_kprintf("%s output flag is set\n",sign);
										}
								}
						}
				}
		}
}


static void digOutputONFun(char num)
{
	switch(num)
	{
			case 0:IOOUT1_ON;break;
			case 1:IOOUT2_ON;break;
			case 2:IOOUT3_ON;break;
			case 3:IOOUT4_ON;break;
			case 4:IOOUT5_ON;break;
			case 5:IOOUT6_ON;break;
			case 6:IOOUT7_ON;break;
			case 7:IOOUT8_ON;break;
	}
}

static void digOutputOFFFun(char num)
{
	switch(num)
	{
			case 0:IOOUT1_OFF;break;
			case 1:IOOUT2_OFF;break;
			case 2:IOOUT3_OFF;break;
			case 3:IOOUT4_OFF;break;
			case 4:IOOUT5_OFF;break;
			case 5:IOOUT6_OFF;break;
			case 6:IOOUT7_OFF;break;
			case 7:IOOUT8_OFF;break;
	}
}

static void v33OutputONFun(char num)
{
	switch(num)
	{
			case 0:SPAKER1_ON;break;
			case 1:SPAKER3_ON;break;
	}
}

static void v33OutputOFFFun(char num)
{
	switch(num)
	{
			case 0:SPAKER1_OFF;break;
			case 1:SPAKER3_OFF;break;
	}
}
static void v5OutputONFun(char num)
{
	switch(num)
	{
			case 0:SPAKER2_ON;break;
			case 1:SPAKER4_ON;break;
	}
}

static void v5OutputOFFFun(char num)
{
	switch(num)
	{
			case 0:SPAKER2_OFF;break;
			case 1:SPAKER4_OFF;break;
	}
}



static void v12OutputONFun(char num)
{
	switch(num)
	{
			case 0:SPAKER1_ON;break;
			case 1:SPAKER2_ON;break;
			case 2:SPAKER3_ON;break;
			case 3:SPAKER4_ON;break;

	}
}

static void v12OutputOFFFun(char num)
{
	switch(num)
	{
			case 0:SPAKER1_OFF;break;
			case 1:SPAKER2_OFF;break;
			case 2:SPAKER3_OFF;break;
			case 3:SPAKER4_OFF;break;

	}
}




//�������õ�outputIO�ĸߵ͵�ƽ��Ч�������ϵ��ʼ��IO��ƽ		
void  outIOInit()
{
		int i;
		for(int j=0;j<CRTL_TOTAL_NUM;j++){
				for(i=0;i<DO_NUM;i++){
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.digOutput[i].lowFlag){
								digOutputONFun(i);//Ĭ�ϵ���Ч  ��ʼ���ߵ�
							  rt_kprintf("%s digoutput on %d\n",i);
						}
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.digOutput[i].upFlag){
								digOutputOFFFun(i);//Ĭ�ϸ���Ч  ��ʼ���͵�
							  rt_kprintf("%s digoutput off %d\n",i);
						}
				}
				for(i=0;i<V33O_NUM;i++){
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v33Output[i].lowFlag){
								v33OutputONFun(i);//Ĭ�ϵ���Ч  ��ʼ���ߵ�
							  rt_kprintf("%s v33output on %d\n",i);
						}
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v33Output[i].upFlag){
								v33OutputOFFFun(i);//Ĭ�ϸ���Ч  ��ʼ���͵�
							  rt_kprintf("%s v33output off %d\n",i);
						}
				}
				for(i=0;i<V5O_NUM;i++){
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v5Output[i].lowFlag){
								v5OutputONFun(i);//Ĭ�ϵ���Ч  ��ʼ���ߵ�
							  rt_kprintf("%s v5output on %d\n",i);
						}
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v5Output[i].upFlag){
								v5OutputOFFFun(i);//Ĭ�ϸ���Ч  ��ʼ���͵�
							  rt_kprintf("%s v5output off %d\n",i);
						}
				}
				for(i=0;i<V12O_NUM;i++){
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v12Output[i].lowFlag){
								v12OutputONFun(i);//Ĭ�ϵ���Ч  ��ʼ���ߵ�
								rt_kprintf("%s v12output on %d\n",i);
						}
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v12Output[i].upFlag){
								v12OutputOFFFun(i);//Ĭ�ϸ���Ч  ��ʼ���͵�
							  rt_kprintf("%s v12output off %d\n",i);
						}
				}
		}
}
//�����ƽִ��
void  ctrlOutSetIO()
{
			int i;
			for(i=0;i<DO_NUM;i++){
					if(inpoutpFlag.digOutput[i].lowFlag==true){
							digOutputOFFFun(i);
						  rt_kprintf("%s ctrlOutSetIO DO off %d\n",i);
					}
					if(inpoutpFlag.digOutput[i].upFlag==true){
							digOutputONFun(i);
						  rt_kprintf("%s ctrlOutSetIO DO on %d\n",i);
					}
			}

			for(i=0;i<V33O_NUM;i++){
					if(inpoutpFlag.v33Output[i].lowFlag==true){
							v33OutputOFFFun(i);
						  rt_kprintf("%s ctrlOutSetIO v33O off %d\n",i);
					}
					if(inpoutpFlag.v33Output[i].upFlag==true){
							v33OutputONFun(i);
						  rt_kprintf("%s ctrlOutSetIO v33O on %d\n",i);
					}
			}

			for(i=0;i<V5O_NUM;i++){
					if(inpoutpFlag.v5Output[i].lowFlag==true){
							v5OutputOFFFun(i);
						  rt_kprintf("%s ctrlOutSetIO v5O off %d\n",i);
					}
					if(inpoutpFlag.v5Output[i].upFlag==true){
							v5OutputONFun(i);
						  rt_kprintf("%s ctrlOutSetIO v5O on %d\n",i);
					}
			}

			for(i=0;i<V12O_NUM;i++){
					if(inpoutpFlag.v12Output[i].lowFlag==true){
							v12OutputOFFFun(i);
						  rt_kprintf("%s ctrlOutSetIO v12O off %d\n",i);
					}
					if(inpoutpFlag.v12Output[i].upFlag==true){
							v12OutputONFun(i);
						  rt_kprintf("%s ctrlOutSetIO v12O on %d\n",i);
					}
			}
}
void  autoCtrlTask(void *para)
{
	
		while(1){	
			
			  autoCtrlRun();
			  ctrlOutSetIO();
				rt_thread_mdelay(1000);
		}
}





