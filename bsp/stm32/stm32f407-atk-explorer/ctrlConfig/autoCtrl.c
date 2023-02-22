#include <board.h>
//阈值设置参考 《汇控箱modbus串口配置V0.4.doc》
/*
a、当输入多个设备命令时候，设备间的关系为逻辑与（即输入条件全部满足），
b、输出多个设备命令时候，设备间的关系为逻辑与（即输出的设备都需要执行）
c、当输入多个设备，并且输入设备间的关系是逻辑或的时候，需要多次单独设置即可
d、不支持删除正在配置中的单个设备，需要设置cancel键或者重启设备
e、sure cancel确定或取消本次(a/b/c/d)配置
f、list显示最多10组配置生效的逻辑列表
g、delete(1-10)删除配置过的逻辑列表
h、10组逻辑控制用指针形式指向阈值执行的flag
{ char *inputFlag[10];
	Char *outputFlag[10];
	Char workFlag;
}total[10];
//////////////////////////////////////////////////
autoctrl AI 温湿度  GYNJLXSD000000499 1 
autoctrl AI 温湿度  GYNJLXSD000000499 2 
autoctrl DI 温湿度  GYNJLXSD000000498 1
autoctrl DI 温湿度  GYNJLXSD000000498 2 
autoctrl MB 甲烷  GYNJLXSD000000491 1 
autoctrl MB 硫化氢  GYNJLXSD000000491 1 
autoctrl MB 环流  GYNJLXSD000000492 1 
autoctrl MB 环流  GYNJLXSD000000492 2 
autoctrl MB 环流  GYNJLXSD000000492 3 
autoctrl DO 水泵  GYNJLXSD000000499 1 
autoctrl DO 水泵  GYNJLXSD000000499 2
autoctrl V3O 水泵  GYNJLXSD000000419 1
autoctrl V5O 水泵  GYNJLXSD000000429 1
autoctrl V12O 水泵  GYNJLXSD000000439 1
//////////////////////////////////////////////////
*/

inoutDevStru inpoutpFlag __attribute__((at(0x2000D000)))={0};//输入输出超值的标记位  指定内存地址  防止更改代码后内存地址移位
//sheet.autoctrl[pindex].output[outputIndex].flag中指针指向了inpoutpFlag
const static char sign[]="[autoCtrl]";
char *inName[INOUTNAME_LEN]={"AI","DI","MB",};
char *outName[INOUTNAME_LEN]={"DO","V3O","V5O","V12O"};
uint8_t inputIndex=0; //本次配置控制逻辑的输入坐标配置   被inputFlag 指针调用
uint8_t outputIndex=0;//本次配置控制逻辑的输出的坐标配置 被outputFlag指针调用

uint8_t pindex=0;//总共配置过这么多控制条数 每配置或者删除完一条以及上电时候 需要重新读取pindex 
//如果是正在配置界面 并没有配置完成  点击删除 此时不能更新pindex
bool configFlag=false;//做个标记来判断是不是正在配置
//检查一下没有设置过的Index 
static uint8_t findCtrlIndex()
{
		for(int i=0;i<CRTL_TOTAL_NUM;i++){
				if(sheet.autoctrl[i].workFlag!=1){
					  
						return i;//找到没有用过的 
				}
		}
		return 255;
}
bool firstReadFlag=false;
//查找当前的空闲index  每次调用index前都需要调用此函数
//uint8_t findInputIndex()
//{
//	  for(int i=0;i>CRTL_IN_NUM;i++){
//				if(sheet.autoctrl[pindex].inputFlag[i]==NULL){
//						return i;
//				}
//		}
//		return 255;
//}
//查找当前的输出index
//uint8_t findOutputIndex()
//{
//}

//char testa=100;

//指针指向局放的阈值
static bool crtlCirculaConf(char **argv)
{
		uint8_t subname = atoi16(argv[4],10);
		uint8_t limit   = atoi16(argv[5],10);
		int i;
		if((subname==0)||(subname>3)){
				rt_kprintf("%sERR:autoCirculaConf argv[4] subname %s should be <4 and not 0\n",sign);
				return false;
		}
		if(!((limit==0)||(limit==1))){
				rt_kprintf("%sERR:autoCirculaConf argv[5] limit %s should be 0 1\n",sign);
				return false;
		}

		for(i=0;i<CIRCULA_485_NUM;i++){
					if(rt_strcmp(sheet.cirCula[i].ID,argv[3])==0){//找到相同ID的
//							rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].typeName,argv[1]);
//							rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].senseName,argv[2]);
//							rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].ID,argv[3]);
//							sheet.autoctrl[pindex].input[inputIndex].subName=subname;
							if(subname==1){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusCircul[i].cirCurALowFlag;
											//inputIndex++;//
											return true;// true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusCircul[i].cirCurAUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
							else if(subname==2){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusCircul[i].cirCurBLowFlag;
											//inputIndex++;//return true;
											return true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusCircul[i].cirCurBUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
							else if(subname==3){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusCircul[i].cirCurCLowFlag;
											//inputIndex++;//return true;
											return true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusCircul[i].cirCurCUpFlag;
											//inputIndex++;
											return true;
											//return true;
									}
							}
					}
			}
		return false;
}


//指针指向局放的阈值
static bool crtlPartDischagConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>9)){
					rt_kprintf("%sERR:partDischagConf argv[4] subname %s should be <10 and not 0\n",sign);
				  return false;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:partDischagConf argv[5] limit %s should be 0 1\n",sign);
				  return false;
			}
		
			for(i=0;i<PARTDISCHAG_485_NUM;i++){
					if(rt_strcmp(sheet.partDischag[i].ID,argv[3])==0){//找到相同ID的
//						  rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].typeName,argv[1]);
//						  rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].senseName,argv[2]);
//						  rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].ID,argv[3]);
//						  sheet.autoctrl[pindex].input[inputIndex].subName=subname;

							if(subname==1){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].amplitudeALowFlag;
											//inputIndex++;//
											return true;// true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].amplitudeAUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
							else if(subname==2){//B
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].amplitudeBLowFlag;
											//inputIndex++;//return true;
											return true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].amplitudeBUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
							else if(subname==3){//C
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].amplitudeCLowFlag;
											//inputIndex++;//return true;
											return true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].amplitudeCUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
							else if(subname==4){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].freqALowFlag;
											//inputIndex++;//
											return true;// true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].freqAUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
							else if(subname==5){//B
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].freqBLowFlag;
											//inputIndex++;//return true;
											return true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].freqBUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
							else if(subname==6){//C
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].freqCLowFlag;
											//inputIndex++;//return true;
											return true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].freqCUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}	
							else if(subname==7){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].dischargeALowFlag;
											//inputIndex++;//
											return true;// true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].dischargeAUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
							else if(subname==8){//B
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].dischargeBLowFlag;
											//inputIndex++;//return true;
											return true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].dischargeBUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
							else if(subname==9){//C
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].dischargeCLowFlag;
											//inputIndex++;//return true;
											return true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPartDisChg[i].dischargeCUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
					}
			}
			return false;
}
//指针指向防沉降的阈值
static bool ctrlPressSettlConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>2)){
					rt_kprintf("%sERR:ctrlPressSettlConf argv[4] subname %s should be <=2 and not 0\n",sign);
				  return false;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlPressSettlConf argv[5] limit %s should be 0 1\n",sign);
				  return false;
			}
			
			
			for(i=0;i<PRESSSETTL_485_NUM;i++){
						if(rt_strcmp(sheet.pressSetl[i].ID,argv[3])==0){//找到相同ID的
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].typeName,argv[1]);
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].senseName,argv[2]);
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].ID,argv[3]);
//								sheet.autoctrl[pindex].input[inputIndex].subName=subname;
								if(subname==1){//A
									  if(limit==0){//下限
												sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPreSettl[i].tempLowFlag;
											  //inputIndex++;//
											  return true;// true;
										}
										else{
											  sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPreSettl[i].tempUpFlag;
												//inputIndex++;//return true;
											  return true;
										}
								}
								else if(subname==2){//A
									  if(limit==0){//下限
												sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPreSettl[i].heightLowFlag;
											  //inputIndex++;//return true;
											  return true;
										}
										else{
											  sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusPreSettl[i].heightUpFlag;
												//inputIndex++;//return true;
											  return true;
										}
								}
						}
			}
			return false;
}

//指针指向三轴加速度的参数
static bool ctrlThreeAxisConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>4)){
					rt_kprintf("%sERR:ctrlThreeAxisConf argv[4] subname %s should be <=4 and not 0\n",sign);
				  return false;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlThreeAxisConf argv[5] limit %s should be 0 1\n",sign);
				  return false;
			}
			for(i=0;i<THREEAXIS_485_NUM;i++){
						if(rt_strcmp(sheet.threeAxiss[i].ID,argv[3])==0){//找到相同ID的
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].typeName,argv[1]);
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].senseName,argv[2]);
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].ID,argv[3]);
//								sheet.autoctrl[pindex].input[inputIndex].subName=subname;
								if(subname==1){//A
									  if(limit==0){//下限
												sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusThreAxis[i].tempLowFlag;
											  //inputIndex++;//
											  return true;// true;
										}
										else{
											  sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusThreAxis[i].tempUpFlag;
												//inputIndex++;//return true;
											  return true; 
										}
								}
								else if(subname==2){//A
									  if(limit==0){//下限
												sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusThreAxis[i].accXLowFlag;
											  //inputIndex++;//return true;
											  return true;
										}
										else{
											  sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusThreAxis[i].accXUpFlag;
												//inputIndex++;//return true;
											  return true;
										}
								}
								else if(subname==3){//B
									  if(limit==0){//下限
												sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusThreAxis[i].accYLowFlag;
											  //inputIndex++;//return true;
											  return true;
										}
										else{
											  sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusThreAxis[i].accYUpFlag;
												//inputIndex++;//return true;
											  return true;
										}
								}							
								else if(subname==4){//C
									  if(limit==0){//下限
												sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusThreAxis[i].accZLowFlag;
											  //inputIndex++;//return true;
											  return true;
										}
										else{
											  sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusThreAxis[i].accZUpFlag;
												//inputIndex++;//return true;
											  return true;
										}
								}
						}
			}
			return false;
}
			
						
#ifdef USE_4GAS
//指针指向甲烷的阈值
static bool ctrlCh4Conf(char **argv)
{

			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>1)){
					rt_kprintf("%sERR:ctrlCh4Conf argv[4] subname %s should be <=1 and not 0\n",sign);
				  return false;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlCh4Conf argv[5] limit %s should be 0 1\n",sign);
				  return false;
			}
			
			
			for(i=0;i<CH4_485_NUM;i++){
						if(rt_strcmp(sheet.ch4[i].ID,argv[3])==0){//找到相同ID的
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].typeName,argv[1]);
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].senseName,argv[2]);
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].ID,argv[3]);
//								sheet.autoctrl[pindex].input[inputIndex].subName=subname;
								if(subname==1){//A
									  if(limit==0){//下限
												sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusCh4[i].ch4LowFlag;
											  //inputIndex++;//
											  return true;// true;
										}
										else{
											  sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusCh4[i].ch4UpFlag;
												//inputIndex++;//return true;
											  return true;
										}
								}
						}
			}
			return false;
}
//指针指向氧气的阈值
static bool ctrlO2Conf(char **argv)
{

			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>1)){
					rt_kprintf("%sERR:ctrlO2Conf argv[4] subname %s should be <=1 and not 0\n",sign);
				  return false;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlO2Conf argv[5] limit %s should be 0 1\n",sign);
				  return false;
			}
			for(i=0;i<O2_485_NUM;i++){
					if(rt_strcmp(sheet.o2[i].ID,argv[3])==0){//找到相同ID的
							if(subname==1){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusO2[i].o2LowFlag;
											//inputIndex++;//
											return true;// true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusO2[i].o2UpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
					}
			}
			return false;
}
//指针指向硫化氢的阈值
static bool ctrlH2sConf(char **argv)
{

			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>1)){
					rt_kprintf("%sERR:ctrlH2sConf argv[4] subname %s should be <=1 and not 0\n",sign);
				  return false;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlH2sConf argv[5] limit %s should be 0 1\n",sign);
				  return false;
			}
			
			
			for(i=0;i<H2S_485_NUM;i++){
					if(rt_strcmp(sheet.h2s[i].ID,argv[3])==0){//找到相同ID的
							if(subname==1){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusH2s[i].h2sLowFlag;
											//inputIndex++;//
											return true;// true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusH2s[i].h2sUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
					}
			}
			return false;
}
//指针指向CO的阈值
static bool ctrlCoConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>1)){
					rt_kprintf("%sERR:ctrlCoConf argv[4] subname %s should be <=1 and not 0\n",sign);
				  return false;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlCoConf argv[5] limit %s should be 0 1\n",sign);
				  return false;
			}
			for(i=0;i<CO_485_NUM;i++){
					if(rt_strcmp(sheet.co[i].ID,argv[3])==0){//找到相同ID的
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].typeName,argv[1]);
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].senseName,argv[2]);
//								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].ID,argv[3]);
//								sheet.autoctrl[pindex].input[inputIndex].subName=subname;

							if(subname==1){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusCo[i].coLowFlag;
											//inputIndex++;//
											return true;// true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusCo[i].coUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
					}
			}
			return false;
}					

#endif

//指针指向温湿度的阈值
static bool ctrlTemphumConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>2)){
					rt_kprintf("%sERR:ctrlTempHumConf argv[4] subname %s should be <=2 and not 0\n",sign);
				  return false;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlTempHumConf argv[5] limit %s should be 0 1\n",sign);
				  return false;
			}
			for(i=0;i<TEMPHUM_485_NUM;i++){
					if(rt_strcmp(sheet.tempHum[i].ID,argv[3])==0){//找到相同ID的
							if(subname==1){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusTempHum[i].tempLowFlag;
											//inputIndex++;//
											return true;// true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusTempHum[i].tempUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
							else if(subname==2){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusTempHum[i].humLowFlag;
											//inputIndex++;//
											return true;// true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusTempHum[i].humUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
					}
			}
			return false;
}					
//指针指向水位传感器的阈值
static bool  ctrlWaterConf(char **argv)
{
			uint8_t subname = atoi16(argv[4],10);
			uint8_t limit   = atoi16(argv[5],10);
	    int i;
			if((subname==0)||(subname>1)){
					rt_kprintf("%sERR:ctrlWaterConf argv[4] subname %s should be <=1 and not 0\n",sign);
				  return false;
			}
			if(!((limit==0)||(limit==1))){
					rt_kprintf("%sERR:ctrlWaterConf argv[5] limit %s should be 0 1\n",sign);
				  return false;
			}
			for(i=0;i<WATERDEPTH_485_NUM;i++){
					if(rt_strcmp(sheet.waterDepth[i].ID,argv[3])==0){//找到相同ID的
//							rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].typeName,argv[1]);
//							rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].senseName,argv[2]);
//							rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].ID,argv[3]);
//							sheet.autoctrl[pindex].input[inputIndex].subName=subname;
//              sheet.autoctrl[pindex].input[inputIndex].limit=limit;
							if(subname==1){//A
									if(limit==0){//下限
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusWaterDepth[i].depthLowFlag;
											//inputIndex++;//
											return true;// true;
									}
									else{
											sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.modbusWaterDepth[i].depthUpFlag;
											//inputIndex++;//return true;
											return true;
									}
							}
					}
			}
			return false;
}													

//输入的指针配置
static void autoctrlInputcfg(char*argv[])
{
	  int i,j;
	  uint8_t limit ,subName;
	  bool ret=false;
		for(i=0;i<sizeof(inName)/sizeof(inName[0]);i++){
				if(rt_strcmp(inName[i],argv[1])==0){//find input
						if(inputIndex>=CRTL_IN_NUM){
								rt_kprintf("%serr:autoctrl input num is %d\n",sign,CRTL_IN_NUM);
							  return;
						}
						switch(i){
							case 0://AI
								for(j=0;j<ANALOG_NUM;j++){
										if(rt_strcmp(argv[2],sheet.analog[j].name)==0){//find  name
												if(rt_strcmp(argv[3],sheet.analog[j].ID)==0){//find ID
													  uint8_t subname = atoi16(argv[4],10);
													  uint8_t limit   = atoi16(argv[5],10);

														rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].typeName,argv[1]);
														rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].senseName,argv[2]);
														rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].ID,argv[3]);
														sheet.autoctrl[pindex].input[inputIndex].subName=subname;
														sheet.autoctrl[pindex].input[inputIndex].limit=limit;
													  if(!((subname==0)||(subname==1)||(subname==2))){
																rt_kprintf("%sERR:argv[4] subname %s should be 0 1 2\n",sign,inName[i]);
														}
														if(!((limit==0)||(limit==1))){
																rt_kprintf("%sERR:argv[5] limit %s should be 0 1\n",sign,inName[i]);
														}
														if(sheet.analog[j].subName==subname){//find subname
															  if(subname==1){//温度
																		if(limit==0){//下限
																				sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.analogTempHum.tempLowFlag;
																		}
																		else{//上限
																			  sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.analogTempHum.tempUpFlag;
																		}
																		inputIndex++;
																}
																else if(subname==2){//湿度
																		if(limit==0){//下限
																				sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.analogTempHum.humLowFlag;
																		}
																		else{//上限
																			  sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.analogTempHum.humUpFlag;
																		}
																		inputIndex++;
																}
																configFlag=true;
																rt_kprintf("%saotuctrl AI OK\n",sign);
																break;
														}
												}
										}
								}
								break;
							case 1://DI
								limit   = atoi16(argv[5],10);
								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].typeName,argv[1]);
								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].senseName,argv[2]);
								rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].ID,argv[3]);
								sheet.autoctrl[pindex].input[inputIndex].subName=0;
								sheet.autoctrl[pindex].input[inputIndex].limit=limit;
								if(!((limit==0)||(limit==1))){
										rt_kprintf("%sERR:di conf argv[5] limit %s should be 0 1\n",sign);
										return ;
								}
								for(i=0;i<DI_NUM;i++){
										if(0==rt_strcmp((char *)packFlash.input[i].devID,argv[3])){
												//packFlash.input[i].levelFlag=limit;//通过levelFlag来置位 lowFlag upFlag
											  if(limit==0)
														sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.digInput[i].lowFlag;
												else
														sheet.autoctrl[pindex].input[inputIndex].flag=&inpoutpFlag.digInput[i].upFlag;
												inputIndex++;
												configFlag=true;
												rt_kprintf("%saotuctrl DI OK\n",sign);
										}
								}
								break;
							case 2://MB
								for(i=0;i<MODBUS_NUM;i++){
										if(0==rt_strcmp((char *)modbusName[i], argv[2])){
												limit   = atoi16(argv[5],10);
												subName = atoi16(argv[4],10);
												rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].typeName,argv[1]);
												rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].senseName,argv[2]);
												rt_strcpy(sheet.autoctrl[pindex].input[inputIndex].ID,argv[3]);
												sheet.autoctrl[pindex].input[inputIndex].subName=subName;
												sheet.autoctrl[pindex].input[inputIndex].limit=limit;
												switch(i){
													case CIRCULA:
															// if(true==autoCirculaConf(argv))
															ret=crtlCirculaConf(argv);	
													break;
													case PARTDISCHAG:
														  ret=crtlPartDischagConf(argv);
													break;
													case PRESSSETTL:
															ret=ctrlPressSettlConf(argv);
													break;
													case THREEAXIS:
															ret=ctrlThreeAxisConf(argv);
													break;
										#ifdef USE_4GAS
													case CH4:
														  ret=ctrlCh4Conf(argv);
													break;
													case O2:
														  ret=ctrlO2Conf(argv);
													break;
													case H2S:
														  ret=ctrlH2sConf(argv);
													break;
													case CO:
															ret=ctrlCoConf(argv);
													break;
										#endif
													case TEMPHUM:
														ret=ctrlTemphumConf(argv);
													break;
													case WATERDEPTH:
														ret=ctrlWaterConf(argv);
													break;
													default:
														rt_kprintf("%sERR:argv[2]>%d\n",sign,WATERDEPTH);
													break;
												}
												
												if(ret==true){
														configFlag=true;
													  inputIndex++;
														rt_kprintf("%saotuctrl MB OK\n",sign);
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

//输出的指针配置
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
				if(rt_strcmp(outName[i],argv[1])==0){//find output

						if(outputIndex>=CRTL_OUT_NUM){
								rt_kprintf("%serr:autoctrl output num is %d\n",sign,CRTL_OUT_NUM);
							  return;
						}
						rt_strcpy(sheet.autoctrl[pindex].output[outputIndex].typeName,argv[1]);
						rt_strcpy(sheet.autoctrl[pindex].output[outputIndex].senseName,argv[2]);
						rt_strcpy(sheet.autoctrl[pindex].output[outputIndex].ID,argv[3]);
						sheet.autoctrl[pindex].output[outputIndex].subName=0;
					  sheet.autoctrl[pindex].output[outputIndex].limit=limit;
						switch(i)
						{
							case 0:
								for(i=0;i<DO_NUM;i++){
										if(0==rt_strcmp((char *)packFlash.digoutput[i].devID,argv[3])){
												//packFlash.digoutput[i].levelFlag=limit;//通过levelFlag来置位 lowFlag upFlag
											  if(limit==0)
														sheet.autoctrl[pindex].output[outputIndex].flag=&inpoutpFlag.digOutput[i].lowFlag;
												else
														sheet.autoctrl[pindex].output[outputIndex].flag=&inpoutpFlag.digOutput[i].upFlag;
												rt_kprintf("%saotuctrl DO OK addr=0x%x\n",sign,sheet.autoctrl[pindex].output[outputIndex].flag);
												outputIndex++;
												configFlag=true;
												
										}
								}
								break;
							case 1:							
								for(i=0;i<V33O_NUM;i++){
										if(0==rt_strcmp((char *)packFlash.v33output[i].devID,argv[3])){
												//packFlash.digoutput[i].levelFlag=limit;//通过levelFlag来置位 lowFlag upFlag
											  if(limit==0)
														sheet.autoctrl[pindex].output[outputIndex].flag=&inpoutpFlag.v33Output[i].lowFlag;
												else
														sheet.autoctrl[pindex].output[outputIndex].flag=&inpoutpFlag.v33Output[i].upFlag;
												rt_kprintf("%saotuctrl V33O OK addr=0x%x\n",sign,sheet.autoctrl[pindex].output[outputIndex].flag);
												outputIndex++;
												configFlag=true;
												
										}
								}
								break;
							case 2:
								for(i=0;i<V5O_NUM;i++){
										if(0==rt_strcmp((char *)packFlash.v5output[i].devID,argv[3])){
												
											  if(limit==0)
														sheet.autoctrl[pindex].output[outputIndex].flag=&inpoutpFlag.v5Output[i].lowFlag;
												else
														sheet.autoctrl[pindex].output[outputIndex].flag=&inpoutpFlag.v5Output[i].upFlag;
												rt_kprintf("%saotuctrl V5O OK  addr=0x%x\n",sign,sheet.autoctrl[pindex].output[outputIndex].flag);
												outputIndex++;
												configFlag=true;
												
										}
								}
								break;
							case 3:
								for(i=0;i<V12O_NUM;i++){
										if(0==rt_strcmp((char *)packFlash.v12output[i].devID,argv[3])){
												//packFlash.digoutput[i].levelFlag=limit;//通过levelFlag来置位 lowFlag upFlag
											  if(limit==0)
														sheet.autoctrl[pindex].output[outputIndex].flag=&inpoutpFlag.v12Output[i].lowFlag;
												else
														sheet.autoctrl[pindex].output[outputIndex].flag=&inpoutpFlag.v12Output[i].upFlag;
												rt_kprintf("%saotuctrl V12O OK  addr=0x%x\n",sign,sheet.autoctrl[pindex].output[outputIndex].flag);
												outputIndex++;
												configFlag=true;
												
										}
								}
								break;
								default:

								break;
						}
						return;
				}
		}
	
}
//打印自动控制的配置列表
void printfCtrl()
{
		rt_kprintf("%sauto ctrl list\n",sign);
		for(int i=0;i<CRTL_TOTAL_NUM;i++){
				//rt_kprintf("%s,autoctrl list",sign);
			  if(sheet.autoctrl[i].workFlag==true){
					  rt_kprintf("%sautoctrl the [%d] inctrl\n",sign,i+1);
						for(int k=0;k<CRTL_IN_NUM;k++){
								if((sheet.autoctrl[i].input[k].flag!=NULL)&&((uint32_t)(uint32_t*)(sheet.autoctrl[i].input[k].flag)!=0xFFFFFFFF)){
										rt_kprintf("In:autoctrl ");
										rt_kprintf("%s ",sheet.autoctrl[i].input[k].typeName);
										rt_kprintf("%s ",sheet.autoctrl[i].input[k].senseName);
										rt_kprintf("%s ",sheet.autoctrl[i].input[k].ID);
										rt_kprintf("%d ",sheet.autoctrl[i].input[k].subName);
										rt_kprintf("%d \n",sheet.autoctrl[i].input[k].limit);
								}
						}
						for(int j=0;j<CRTL_OUT_NUM;j++){
							 							//	if((sheet.autoctrl[i].input[k].flag!=NULL)&&((uint32_t)(uint32_t*)(sheet.autoctrl[i].input[k].flag)!=0xFFFFFFFF)){
								if((sheet.autoctrl[i].output[j].flag!=NULL)&&((uint32_t)(uint32_t*)sheet.autoctrl[i].output[j].flag!=0xFFFFFFFF)){
										rt_kprintf("Out:autoctrl ");
										rt_kprintf("%s ",sheet.autoctrl[i].output[j].typeName);
										rt_kprintf("%s ",sheet.autoctrl[i].output[j].senseName);
										rt_kprintf("%s ",sheet.autoctrl[i].output[j].ID);
										rt_kprintf("%d ",sheet.autoctrl[i].output[j].subName);
										rt_kprintf("%d \n",sheet.autoctrl[i].output[j].limit);
								}
						}	
						rt_kprintf("\n");
				}
		}
}
//自动控制输入函数  串口输出命令首先调用此函数解析
//以上逻辑配置时候如需要删除单个输入或者输出设备，选项为0即可，
//必须从后往前（图形界面显示）一个一个删除（逻辑控制列表的指针为空）或者重启设备，cancel命令也可以取消本次整体配置
static void autoctrl(char argc,char*argv[])
{
	  if(firstReadFlag==false){
				firstReadFlag=true;
				pindex=findCtrlIndex();//每次上电后读取一次即可
		}

		if(rt_strcmp("list",argv[1])==0){
				printfCtrl();
				return;
		}//打印
		if(rt_strcmp("delete",argv[1])==0){
			  
			  int num=atoi16(argv[2],10);
			  if( num==0){
						rt_kprintf("%sargv[2] should not 0\n",sign);
				}
			  rt_kprintf("%s %d %d\n",sign,num,pindex);
			  
			  sheet.autoctrl[num-1].workFlag=false;//本条配置失效
				for(int j=0;j<CRTL_IN_NUM;j++)
					 sheet.autoctrl[num-1].input[j].flag=NULL;
				for(int j=0;j<CRTL_OUT_NUM;j++)
					 sheet.autoctrl[num-1].output[j].flag=NULL;
			  if(configFlag==false)
						pindex =findCtrlIndex();//删除后刷新坐标
				rt_kprintf("%saotuctrl delete OK\n",sign);
				return;
		}//打印删除按钮

	  if(pindex==255){
				rt_kprintf("%sERR:aotuctrl is full,the total number is %d\n",sign,CRTL_TOTAL_NUM);
			  return;
		}
		if(rt_strcmp("sure",argv[1])==0){//确定本次配置按钮  确定完了只能删除 不能取消
			  sheet.autoctrl[pindex].workFlag=true;//本条配置生效
				pindex =findCtrlIndex();//确定后刷新坐标
			  configFlag=false;
				inputIndex=0; 
				outputIndex=0;
			  rt_kprintf("%saotuctrl sure OK\n",sign);
				return;
		}
		int k;
		if(rt_strcmp("cancel",argv[1])==0){//取消本次配置按钮
			  for( k=0;k<CRTL_IN_NUM;k++){
						sheet.autoctrl[pindex].input[inputIndex].flag=NULL;
				}
				for( k=0;k<CRTL_OUT_NUM;k++){
						sheet.autoctrl[pindex].output[outputIndex].flag=NULL;
				}
				pindex =findCtrlIndex();//确定后刷新坐标
			  configFlag=false;
				inputIndex=0; 
				outputIndex=0;
				rt_kprintf("%saotuctrl cancel OK\n",sign);
				return;
		}

		if(pindex>=CRTL_TOTAL_NUM){
				rt_kprintf("%serr:autoctrl totoal num is %d\n",sign,CRTL_TOTAL_NUM);
		}
		autoctrlInputcfg(argv); //检查是不是输入配置
		autoctrlOutputcfg(argv);//检查是不是输出配置
}
MSH_CMD_EXPORT(autoctrl,autoctrl config);

