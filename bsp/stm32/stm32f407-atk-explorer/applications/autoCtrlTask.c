#include   "NetJsonDataTask.h"
#include   "board.h"

const static char sign[]="[autoCtrlTask]";
//char *sign={[]};
//自动控制逻辑的处理
//1/阈值的判断 以及置位标记  仅仅对于输入设备
void  thresholdOutSetInputFlag()
{
}
//2/根据标记来执行的输出控制标记
void  autoCtrlRun()
{
		int i,j,k;
		for(int i=0;i<CRTL_TOTAL_NUM;i++){
				if(sheet.autoctrl[i].workFlag==true){
						for(int j=0;j<CRTL_IN_NUM;j++){
								if(sheet.autoctrl[i].input[j].flag!=NULL){//指针全部不为空
										if(*sheet.autoctrl[i].input[j].flag==false){//如果有一个标记不成立 跳出（因为所有标记的关系是与）
												rt_kprintf("%s input flag not ok\n",sign);
												break;
										}
								}
						}
						if(j==CRTL_IN_NUM){//输入条件全部满足了  执行输出 此处仅仅是置位标记位  需要在另一个函数中调用
							  for(k=0;k<CRTL_OUT_NUM;k++){
										if(sheet.autoctrl[i].output[k].flag!=NULL){//指针不为空
												*sheet.autoctrl[i].output[k].flag=true;//执行  调用指针执行的flag标记
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




//根据设置的outputIO的高低电平有效来设置上电初始化IO电平		
void  outIOInit()
{
		int i;
		for(int j=0;j<CRTL_TOTAL_NUM;j++){
				for(i=0;i<DO_NUM;i++){
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.digOutput[i].lowFlag){
								digOutputONFun(i);//默认低有效  初始化高的
							  rt_kprintf("%s digoutput on %d\n",i);
						}
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.digOutput[i].upFlag){
								digOutputOFFFun(i);//默认高有效  初始化低的
							  rt_kprintf("%s digoutput off %d\n",i);
						}
				}
				for(i=0;i<V33O_NUM;i++){
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v33Output[i].lowFlag){
								v33OutputONFun(i);//默认低有效  初始化高的
							  rt_kprintf("%s v33output on %d\n",i);
						}
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v33Output[i].upFlag){
								v33OutputOFFFun(i);//默认高有效  初始化低的
							  rt_kprintf("%s v33output off %d\n",i);
						}
				}
				for(i=0;i<V5O_NUM;i++){
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v5Output[i].lowFlag){
								v5OutputONFun(i);//默认低有效  初始化高的
							  rt_kprintf("%s v5output on %d\n",i);
						}
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v5Output[i].upFlag){
								v5OutputOFFFun(i);//默认高有效  初始化低的
							  rt_kprintf("%s v5output off %d\n",i);
						}
				}
				for(i=0;i<V12O_NUM;i++){
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v12Output[i].lowFlag){
								v12OutputONFun(i);//默认低有效  初始化高的
								rt_kprintf("%s v12output on %d\n",i);
						}
						if(sheet.autoctrl[j].output[i].flag==&inpoutpFlag.v12Output[i].upFlag){
								v12OutputOFFFun(i);//默认高有效  初始化低的
							  rt_kprintf("%s v12output off %d\n",i);
						}
				}
		}
}
//输出电平执行
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





