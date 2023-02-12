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


inoutDevStru inpoutpFlag={0};
const static char sign[]="[autoCtrl]";
char *inName[]={"AI","DI","MB",};
char *outName[]={"DO","V3O","V5O","V12O"};
uint8_t inputIndex=0; //�������ÿ����߼���������������   ��inputFlag ָ�����
uint8_t outputIndex=0;//�������ÿ����߼���������������� ��outputFlagָ�����

uint8_t pindex=0;//�ܹ����ù���ô��������� ÿ���û���ɾ����һ���Լ��ϵ�ʱ�� ��Ҫ���¶�ȡpindex 
//������������ý��� ��û���������  ���ɾ�� ��ʱ���ܸ���pindex
bool configFlag=false;//����������ж��ǲ�����������
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
bool firstReadFlag=false;
//���ҵ�ǰ�Ŀ���index  ÿ�ε���indexǰ����Ҫ���ô˺���
//uint8_t findInputIndex()
//{
//	  for(int i=0;i>CRTL_IN_NUM;i++){
//				if(sheet.autoctrl[pindex].inputFlag[i]==NULL){
//						return i;
//				}
//		}
//		return 255;
//}
//���ҵ�ǰ�����index
uint8_t findOutputIndex()
{
}

char testa=100;

//�����߼�����ʱ������Ҫɾ�����������������豸��ѡ��Ϊ0���ɣ�
//����Ӻ���ǰ��ͼ�ν�����ʾ��һ��һ��ɾ�����߼������б��ָ��Ϊ�գ����������豸��cancel����Ҳ����ȡ��������������
//
void crtlCirculaConf(char **argv)
{
		uint8_t subname = atoi16(argv[4],10);
		uint8_t limit   = atoi16(argv[5],10);
		int i;
		if((subname==0)||(subname>3)){
				rt_kprintf("%sERR:autoCirculaConf argv[4] subname %s should be <4 and not 0\n",sign);
				return ;
		}
		if(!((limit==0)||(limit==1))){
				rt_kprintf("%sERR:autoCirculaConf argv[5] limit %s should be 0 1\n",sign);
				return ;
		}
		
		
		for(i=0;i<CIRCULA_485_NUM;i++){
					if(rt_strcmp(sheet.cirCula[i].ID,argv[2])==0){//�ҵ���ͬID��
							if(subname==1){//A
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusCircul[i].cirCurALowFlag;
											inputIndex++;//
											return;// true;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusCircul[i].cirCurAUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
							else if(subname==2){//A
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusCircul[i].cirCurBLowFlag;
											inputIndex++;//return true;
											return;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusCircul[i].cirCurBUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
							else if(subname==3){//A
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusCircul[i].cirCurCLowFlag;
											inputIndex++;//return true;
											return;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusCircul[i].cirCurCUpFlag;
											inputIndex++;
											return;
											//return true;
									}
							}
					}
			}
}



void crtlPartDischagConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>9)){
					rt_kprintf("%sERR:partDischagConf argv[4] subname %s should be <10 and not 0\n",sign);
				  return ;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:partDischagConf argv[5] limit %s should be 0 1\n",sign);
				  return ;
			}
		
			for(i=0;i<PARTDISCHAG_485_NUM;i++){
					if(rt_strcmp(sheet.partDischag[i].ID,argv[2])==0){//�ҵ���ͬID��
							if(subname==1){//A
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].amplitudeALowFlag;
											inputIndex++;//
											return;// true;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].amplitudeAUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
							else if(subname==2){//B
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].amplitudeBLowFlag;
											inputIndex++;//return true;
											return;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].amplitudeBUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
							else if(subname==3){//C
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].amplitudeCLowFlag;
											inputIndex++;//return true;
											return;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].amplitudeCUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
							else if(subname==4){//A
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].freqALowFlag;
											inputIndex++;//
											return;// true;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].freqAUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
							else if(subname==5){//B
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].freqBLowFlag;
											inputIndex++;//return true;
											return;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].freqBUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
							else if(subname==6){//C
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].freqCLowFlag;
											inputIndex++;//return true;
											return;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].freqCUpFlag;
											inputIndex++;//return true;
											return;
									}
							}	
							else if(subname==7){//A
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].dischargeALowFlag;
											inputIndex++;//
											return;// true;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].dischargeAUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
							else if(subname==8){//B
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].dischargeBLowFlag;
											inputIndex++;//return true;
											return;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].dischargeBUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
							else if(subname==9){//C
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].dischargeCLowFlag;
											inputIndex++;//return true;
											return;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPartDisChg[i].dischargeCUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
					}
			}
}

void ctrlPressSettlConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>2)){
					rt_kprintf("%sERR:ctrlPressSettlConf argv[4] subname %s should be <=2 and not 0\n",sign);
				  return ;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlPressSettlConf argv[5] limit %s should be 0 1\n",sign);
				  return ;
			}
			
			
			for(i=0;i<PRESSSETTL_485_NUM;i++){
						if(rt_strcmp(sheet.pressSetl[i].ID,argv[2])==0){//�ҵ���ͬID��
								if(subname==1){//A
									  if(limit==0){//����
												sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPreSettl[i].tempLowFlag;
											  inputIndex++;//
											  return;// true;
										}
										else{
											  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPreSettl[i].tempUpFlag;
												inputIndex++;//return true;
											  return;
										}
								}
								else if(subname==2){//A
									  if(limit==0){//����
												sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPreSettl[i].heightLowFlag;
											  inputIndex++;//return true;
											  return;
										}
										else{
											  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusPreSettl[i].heightUpFlag;
												inputIndex++;//return true;
											  return;
										}
								}
						}
			}
}



void ctrlThreeAxisConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>4)){
					rt_kprintf("%sERR:ctrlThreeAxisConf argv[4] subname %s should be <=4 and not 0\n",sign);
				  return ;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlThreeAxisConf argv[5] limit %s should be 0 1\n",sign);
				  return ;
			}
			for(i=0;i<THREEAXIS_485_NUM;i++){
						if(rt_strcmp(sheet.threeAxiss[i].ID,argv[2])==0){//�ҵ���ͬID��
								if(subname==1){//A
									  if(limit==0){//����
												sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusThreAxis[i].tempLowFlag;
											  inputIndex++;//
											  return;// true;
										}
										else{
											  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusThreAxis[i].tempUpFlag;
												inputIndex++;//return true;
											  return;
										}
								}
								else if(subname==2){//A
									  if(limit==0){//����
												sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusThreAxis[i].accXLowFlag;
											  inputIndex++;//return true;
											  return;
										}
										else{
											  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusThreAxis[i].accXUpFlag;
												inputIndex++;//return true;
											  return;
										}
								}
								else if(subname==3){//B
									  if(limit==0){//����
												sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusThreAxis[i].accYLowFlag;
											  inputIndex++;//return true;
											  return;
										}
										else{
											  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusThreAxis[i].accYUpFlag;
												inputIndex++;//return true;
											  return;
										}
								}							
								else if(subname==4){//C
									  if(limit==0){//����
												sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusThreAxis[i].accZLowFlag;
											  inputIndex++;//return true;
											  return;
										}
										else{
											  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusThreAxis[i].accZUpFlag;
												inputIndex++;//return true;
											  return;
										}
								}
						}
			}
}
			
						
#ifdef USE_4GAS

void ctrlCh4Conf(char **argv)
{

			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>1)){
					rt_kprintf("%sERR:ctrlCh4Conf argv[4] subname %s should be <=1 and not 0\n",sign);
				  return ;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlCh4Conf argv[5] limit %s should be 0 1\n",sign);
				  return ;
			}
			
			
			for(i=0;i<CH4_485_NUM;i++){
						if(rt_strcmp(sheet.ch4[i].ID,argv[2])==0){//�ҵ���ͬID��
								if(subname==1){//A
									  if(limit==0){//����
												sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusCh4[i].ch4LowFlag;
											  inputIndex++;//
											  return;// true;
										}
										else{
											  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusCh4[i].ch4UpFlag;
												inputIndex++;//return true;
											  return;
										}
								}
						}
			}
}
void ctrlO2Conf(char **argv)
{

			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>1)){
					rt_kprintf("%sERR:ctrlO2Conf argv[4] subname %s should be <=1 and not 0\n",sign);
				  return ;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlO2Conf argv[5] limit %s should be 0 1\n",sign);
				  return ;
			}
			for(i=0;i<O2_485_NUM;i++){
						if(rt_strcmp(sheet.o2[i].ID,argv[2])==0){//�ҵ���ͬID��
								if(subname==1){//A
									  if(limit==0){//����
												sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusO2[i].o2LowFlag;
											  inputIndex++;//
											  return;// true;
										}
										else{
											  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusO2[i].o2UpFlag;
												inputIndex++;//return true;
											  return;
										}
								}
						}
			}
}
void ctrlH2sConf(char **argv)
{

			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>1)){
					rt_kprintf("%sERR:ctrlH2sConf argv[4] subname %s should be <=1 and not 0\n",sign);
				  return ;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlH2sConf argv[5] limit %s should be 0 1\n",sign);
				  return ;
			}
			
			
			for(i=0;i<H2S_485_NUM;i++){
						if(rt_strcmp(sheet.h2s[i].ID,argv[2])==0){//�ҵ���ͬID��
								if(subname==1){//A
									  if(limit==0){//����
												sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusH2s[i].h2sLowFlag;
											  inputIndex++;//
											  return;// true;
										}
										else{
											  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusH2s[i].h2sUpFlag;
												inputIndex++;//return true;
											  return;
										}
								}
						}
			}
}
void ctrlCoConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>1)){
					rt_kprintf("%sERR:ctrlCoConf argv[4] subname %s should be <=1 and not 0\n",sign);
				  return ;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlCoConf argv[5] limit %s should be 0 1\n",sign);
				  return ;
			}
			for(i=0;i<CO_485_NUM;i++){
					if(rt_strcmp(sheet.co[i].ID,argv[2])==0){//�ҵ���ͬID��
							if(subname==1){//A
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusCo[i].coLowFlag;
											inputIndex++;//
											return;// true;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusCo[i].coUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
					}
			}
}					

#endif


void ctrlTemphumConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>2)){
					rt_kprintf("%sERR:ctrlTempHumConf argv[4] subname %s should be <=2 and not 0\n",sign);
				  return ;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlTempHumConf argv[5] limit %s should be 0 1\n",sign);
				  return ;
			}
			for(i=0;i<TEMPHUM_485_NUM;i++){
					if(rt_strcmp(sheet.tempHum[i].ID,argv[2])==0){//�ҵ���ͬID��
							if(subname==1){//A
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusTempHum[i].tempLowFlag;
											inputIndex++;//
											return;// true;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusTempHum[i].tempUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
							else if(subname==2){//A
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusTempHum[i].humLowFlag;
											inputIndex++;//
											return;// true;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusTempHum[i].humUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
					}
			}
}					
void ctrlWaterConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>1)){
					rt_kprintf("%sERR:ctrlWaterConf argv[4] subname %s should be <=1 and not 0\n",sign);
				  return ;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlWaterConf argv[5] limit %s should be 0 1\n",sign);
				  return ;
			}
			for(i=0;i<WATERDEPTH_485_NUM;i++){
					if(rt_strcmp(sheet.waterDepth[i].ID,argv[2])==0){//�ҵ���ͬID��
							if(subname==1){//A
									if(limit==0){//����
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusWaterDepth[i].depthLowFlag;
											inputIndex++;//
											return;// true;
									}
									else{
											sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.modbusWaterDepth[i].depthUpFlag;
											inputIndex++;//return true;
											return;
									}
							}
					}
			}
}													

//�����ָ������
static void autoctrlInputcfg(char*argv[])
{
	  int i,j;
	  uint8_t limit ;
		for(i=0;i<sizeof(inName)/sizeof(inName[0]);i++){
				if(rt_strcmp(inName[i],argv[1])==0){//find input
						switch(i){
							case 0://AI
								for(j=0;j<ANALOG_NUM;j++){
										if(rt_strcmp(argv[2],sheet.analog[j].name)==0){//find  name
												if(rt_strcmp(argv[3],sheet.analog[j].ID)==0){//find ID
													  uint8_t subname = atoi16(argv[4],10);
													  uint8_t limit   = atoi16(argv[5],10);
													  if(!((subname==0)||(subname==1)||(subname==2))){
																rt_kprintf("%sERR:argv[4] subname %s should be 0 1 2\n",sign,inName[i]);
														}
														if(!((limit==0)||(limit==1))){
																rt_kprintf("%sERR:argv[5] limit %s should be 0 1\n",sign,inName[i]);
														}
														if(sheet.analog[j].subName==subname){//find subname
															  if(subname==1){//�¶�
																		if(limit==0){//����
																				sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.analogTempHum.tempLowFlag;
																		}
																		else{//����
																			  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.analogTempHum.tempUpFlag;
																		}
																		inputIndex++;
																}
																else if(subname==2){//ʪ��
																		if(limit==0){//����
																				sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.analogTempHum.humLowFlag;
																		}
																		else{//����
																			  sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.analogTempHum.humUpFlag;
																		}
																		inputIndex++;
																}
//																else if(subname==0){//ɾ���ϱ����õ��¶�ʪ��ֵ  ����ͼ����ʾ�������ù�������ʾȡ���İ�ť  ͼ�ν�����Ҫ��ֵ��ʾ
//																	  if(inputIndex==0){
//																			  rt_kprintf("%sERR:inpudIndex is 0\n",sign);
//																				return;
//																		}
//																		inputIndex--;
//																		sheet.autoctrl[pindex].inputFlag[inputIndex]=NULL;
//																}
																configFlag=true;
														}
												}
										}
								}
								break;
							case 1://DI
								limit   = atoi16(argv[5],10);
								if(!((limit==0)||(limit==1))){
										rt_kprintf("%sERR:di conf argv[5] limit %s should be 0 1\n",sign);
										return ;
								}
								for(i=0;i<DI_NUM;i++){
										if(0==rt_strcmp((char *)packFLash.input[i].devID,argv[2])){
												//packFLash.input[i].levelFlag=limit;//ͨ��levelFlag����λ lowFlag upFlag
											  if(limit==0)
														sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.digInput[i].lowFlag;
												else
														sheet.autoctrl[pindex].inputFlag[inputIndex]=&inpoutpFlag.digInput[i].upFlag;
										}
								}
								break;
							case 2://MB
								for(i=0;i<MODBUS_NUM;i++){
										if(0==rt_strcmp((char *)modbusName[i], argv[2])){
												switch(i){
													case CIRCULA:
															// if(true==autoCirculaConf(argv))
															crtlCirculaConf(argv);	
													break;
													case PARTDISCHAG:
														  crtlPartDischagConf(argv);
													break;
													case PRESSSETTL:
															ctrlPressSettlConf(argv);
													break;
													case THREEAXIS:
															ctrlThreeAxisConf(argv);
													break;
										#ifdef USE_4GAS
													case CH4:
														  ctrlCh4Conf(argv);
													break;
													case O2:
														  ctrlO2Conf(argv);
													break;
													case H2S:
														  ctrlH2sConf(argv);
													break;
													case CO:
															ctrlCoConf(argv);
													break;
										#endif
													case TEMPHUM:
														ctrlTemphumConf(argv);
													break;
													case WATERDEPTH:
														ctrlWaterConf(argv);
													break;
													default:
														rt_kprintf("%sERR:argv[2]>%d\n",sign,WATERDEPTH);
													break;
												}
												break;
										}
								}
								break;
						}
//						for(j=0;j<ANALOG_NUM;j++)
						return;
				}
		}	
}

//�����ָ������
//{"DO","V3O","V5O","V12O"};
static void autoctrlOutputcfg(char*argv[])
{
		int i;
	
		int  limit   = atoi16(argv[5],10);
		if(!((limit==0)||(limit==1))){
				rt_kprintf("%sERR:autoctrlOutputcfg argv[5] limit %s should be 0 1\n",sign);
				return ;
		}
		for(i=0;i<sizeof(outName)/sizeof(outName[0]);i++){
				if(rt_strcmp(outName[i],argv[i])==0){//find output
						switch(i)
						{
							case 0:
								for(i=0;i<DO_NUM;i++){
										if(0==rt_strcmp((char *)packFLash.digoutput[i].devID,argv[2])){
												//packFLash.digoutput[i].levelFlag=limit;//ͨ��levelFlag����λ lowFlag upFlag
											  if(limit==0)
														sheet.autoctrl[pindex].outputFlag[outputIndex]=&inpoutpFlag.digInput[i].lowFlag;
												else
														sheet.autoctrl[pindex].outputFlag[outputIndex]=&inpoutpFlag.digInput[i].upFlag;
										}
								}
								break;
							case 1:
								for(i=0;i<V33O_NUM;i++){
										if(0==rt_strcmp((char *)packFLash.v33output[i].devID,argv[2])){
												//packFLash.digoutput[i].levelFlag=limit;//ͨ��levelFlag����λ lowFlag upFlag
											  if(limit==0)
														sheet.autoctrl[pindex].outputFlag[outputIndex]=&inpoutpFlag.v33Output[i].lowFlag;
												else
														sheet.autoctrl[pindex].outputFlag[outputIndex]=&inpoutpFlag.v33Output[i].upFlag;
										}
								}
								break;
							case 2:
								for(i=0;i<V5O_NUM;i++){
										if(0==rt_strcmp((char *)packFLash.v5output[i].devID,argv[2])){
												//packFLash.digoutput[i].levelFlag=limit;//ͨ��levelFlag����λ lowFlag upFlag
											  if(limit==0)
														sheet.autoctrl[pindex].outputFlag[outputIndex]=&inpoutpFlag.v5Output[i].lowFlag;
												else
														sheet.autoctrl[pindex].outputFlag[outputIndex]=&inpoutpFlag.v5Output[i].upFlag;
										}
								}
								break;
							case 3:
								for(i=0;i<V12O_NUM;i++){
										if(0==rt_strcmp((char *)packFLash.v12output[i].devID,argv[2])){
												//packFLash.digoutput[i].levelFlag=limit;//ͨ��levelFlag����λ lowFlag upFlag
											  if(limit==0)
														sheet.autoctrl[pindex].outputFlag[outputIndex]=&inpoutpFlag.v12Output[i].lowFlag;
												else
														sheet.autoctrl[pindex].outputFlag[outputIndex]=&inpoutpFlag.v12Output[i].upFlag;
										}
								}
								break;
						}
						return;
				}
		}
	
}
static void autoctrl(char argc,char*argv[])
{
	  if(firstReadFlag==false){
				firstReadFlag=true;
				pindex=findCtrlIndex();//ÿ���ϵ���ȡһ�μ���
		}
	  if(pindex==255){
				rt_kprintf("%s,ERR:aotuctrl is full,the total number is %d\n",sign,CRTL_TOTAL_NUM);
			  return;
		}
		if(rt_strcmp("list",argv[1])==0){
			
				return;
		}//��ӡ
		if(rt_strcmp("sure",argv[1])==0){//ȷ���������ð�ť  ȷ������ֻ��ɾ�� ����ȡ��
			  sheet.autoctrl[pindex].workFlag=true;//����������Ч
				pindex =findCtrlIndex();//ȷ����ˢ������
			  configFlag=false;
				inputIndex=0; 
				outputIndex=0;
				return;
		}
		int i,j,k;
		if(rt_strcmp("cancel",argv[1])==0){//ȡ���������ð�ť
			  for( k=0;k<CRTL_IN_NUM;k++){
						sheet.autoctrl[pindex].inputFlag[inputIndex]=NULL;
				}
				for( k=0;k<CRTL_OUT_NUM;k++){
						sheet.autoctrl[pindex].outputFlag[outputIndex]=NULL;
				}
				pindex =findCtrlIndex();//ȷ����ˢ������
			  configFlag=false;
				inputIndex=0; 
				outputIndex=0;
				return;
		}
		if(rt_strcmp("delete",argv[1])==0){
			  sheet.autoctrl[pindex].workFlag=false;//��������ʧЧ
			  if(configFlag==false)
						pindex =findCtrlIndex();//ɾ����ˢ������
				return;
		}//��ӡɾ����ť
		if(pindex==255){
				rt_kprintf("%saotuCtrl num<=8 ,delete it first\n",sign);
			  return;
		}
		autoctrlInputcfg(argv);
//		for(i=0;i<sizeof(inName)/sizeof(inName[0]);i++){
//				if(rt_strcmp(inName[i],argv[1])==0){//find input
//						switch(i){
//							case 0://AI
//								for(j=0;j<ANALOG_NUM;j++){
//										if(rt_strcmp(argv[2],sheet.analog[j].name)==0){//find  name
//												if(rt_strcmp(argv[3],sheet.analog[j].ID)==0){//find ID
//													  uint8_t subname = atoi16(argv[4],10);
//													  uint8_t limit   = atoi16(argv[5],10);
//													  if(!((subname==0)||(subname==1)||(subname==2))){
//																rt_kprintf("%sERR:argv[4] subname %s should be 0 1 2\n",sign,inName[i]);
//														}
//														if(!((limit==0)||(limit==1))){
//																rt_kprintf("%sERR:argv[5] limit %s should be 0 1\n",sign,inName[i]);
//														}
//														if(sheet.analog[j].subName==subname){//find subname
//															  if(subname==1){//�¶�
//																		if(limit==0){//����
//																				sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.tempLowFlag;
//																		}
//																		else{//����
//																			  sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.tempUpFlag;
//																		}
//																		inputIndex++;
//																}
//																else if(subname==2){//ʪ��
//																		if(limit==0){//����
//																				sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.humDownFlag;
//																		}
//																		else{//����
//																			  sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.humUpFlag;
//																		}
//																		inputIndex++;
//																}
//																else if(subname==0){//ɾ���ϱ����õ��¶�ʪ��ֵ  ����ͼ����ʾ�������ù�������ʾȡ���İ�ť  ͼ�ν�����Ҫ��ֵ��ʾ
//																	  if(inputIndex==0){
//																			  rt_kprintf("%sERR:inpudIndex is 0\n",sign);
//																				return;
//																		}
//																		inputIndex--;
//																		sheet.autoctrl[pindex].inputFlag[inputIndex]=NULL;
//																}
//																configFlag=true;
//																
//														}
//												}
//										}
//								}
//								break;
//							case 1://DI
//								break;
//							case 2://MB
//								break;
//						}
//						for(j=0;j<ANALOG_NUM;j++)
//						return;
//				}
//		}

}
MSH_CMD_EXPORT(autoctrl,autoctrl config);