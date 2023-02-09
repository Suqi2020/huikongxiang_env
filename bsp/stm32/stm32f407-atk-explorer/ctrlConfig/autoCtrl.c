#include <board.h>
//阈值设置参考 《汇控箱modbus串口配置V0.4.doc》
/*
a、当输入多个设备命令时候，设备间的关系为逻辑与（即输入条件全部满足），
b、输出多个设备命令时候，设备间的关系为逻辑与（即输出的设备都需要执行）
c、当输入多个设备，并且输入设备间的关系是逻辑或的时候，需要多次单独设置即可
d、以上逻辑配置时候如需要删除单个输入或者输出设备，选项为0即可（逻辑控制列
表的指针为空）或者重启设备
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
const static char sign[]="[autoCtrl]";
char *inName[]={"AI","DI","MB",};
char *outName[]={"DO","V3O","V5O","V12O"};
uint8_t inputIndex=0;//本次输入坐标配置
uint8_t outputIndex=0;//本次输出的坐标配置

uint8_t pindex=0;//总共配置过这么多控制条数
//检查一下没有设置过的Index 
uint8_t findCtrlIndex()
{
		for(int i=0;i<CRTL_TOTAL_NUM;i++){
				if(sheet.autoctrl[i].workFlag!=1){
						return i;//找到没有用过的 
				}
		}
		return 255;
}
//查找当前的输入index
uint8_t findInputIndex()
{
}
//查找当前的输出index
uint8_t findOutputIndex()
{
}

char testa=100;
static void autoctrl(char argc,char*argv[])
{
		if(rt_strcmp("list",argv[1])==0){
			
				return;
		}//打印
		if(rt_strcmp("sure",argv[1])==0){
			
				return;
		}//确定按钮
		if(rt_strcmp("cancel",argv[1])==0){
			
				return;
		}//取消按钮
		if(rt_strcmp("delete",argv[1])==0){
			
				return;
		}//打印删除按钮
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
															  if(subname==1){//温度
																		if(limit==0){//下限
																				sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.tempLowFlag;
																		}
																		else{//上限
																			  sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.tempUpFlag;
																		}
																		inputIndex++;
																}
																if(subname==2){//湿度
																		if(limit==0){//下限
																				sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.humDownFlag;
																		}
																		else{//上限
																			  sheet.autoctrl[pindex].inputFlag[inputIndex]=&sheet.analogTempHum.humUpFlag;
																		}
																		inputIndex++;
																}
																if(subname==0){//删除上边设置的温度湿度值  按照图形显示必须配置过才能显示取消的按钮  图形界面需要二值显示
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