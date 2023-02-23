#include <board.h>


const static char sign[]="[digInput]";


//digital input 读取电平值 高电平返回1   其他返回0
bool  diReadFun(char num)
{
	  int ret=0;
		switch(num)
		{
			case 0: ret=IOIN1_READ;break;
			case 1: ret=IOIN2_READ;break;
			case 2: ret=IOIN3_READ;break;
			case 3: ret=IOIN4_READ;break;
			case 4: ret=IOIN5_READ;break;
			case 5: ret=IOIN6_READ;break;
			case 6: ret=IOIN7_READ;break;
			case 7: ret=IOIN8_READ;break;
		}
		return ret;
}
//digit input检测到符合逻辑的电平 置位相应的flag
void diIOSetFlag()
{
		for(int i=0;i<DI_NUM;i++){
				if(packFlash.diginput[i].workFlag==true){
						if(diReadFun(i)==true){
								inpoutpFlag.digInput[i].upFlag =true;
							  inpoutpFlag.digInput[i].lowFlag =false; 
							  //rt_kprintf("%s diIOSet upFlag %d\n",sign,i);
						}
						else {
							  inpoutpFlag.digInput[i].upFlag =false;
								inpoutpFlag.digInput[i].lowFlag=true;
							  //rt_kprintf("%s diIOSet lowFlag %d\n",sign,i);
						}
				}
		}
}

//打印digit input 列表
void printfDIList()
{
		for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(packFlash.diginput[j].workFlag==RT_TRUE){//打开
						rt_kprintf("%s digInput ",sign);

						rt_kprintf("%s ",packFlash.diginput[j].name);
						rt_kprintf("%s ",packFlash.diginput[j].devID);
						
						rt_kprintf("%s ",packFlash.diginput[j].model);
						rt_kprintf("%d \n",packFlash.diginput[j].port);



				}
		}
}

//digInput 水泵 GYNJLXSD000000162 GY281 1
//digital input 输入配置
static void digInput(int argc, char *argv[])
{
	 int port;
	 if(0==rt_strcmp((char *)"list", argv[1])){
				printfDIList();
				return;
		}
		if(argc!=5){
				goto ERR;
		}
		port = atoi16(argv[4],10);

		if((port<=DI_NUM)&&(port>0)){//添加
				packFlash.diginput[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFlash.diginput[port-1].name, argv[1]);
				rt_strcpy(packFlash.diginput[port-1].devID,argv[2]);
				rt_strcpy(packFlash.diginput[port-1].model,argv[3]);
				packFlash.diginput[port-1].port=port;
				rt_kprintf("%s add diginput chanl %d\n",sign,port);
        rt_kprintf("%s digInput OK\n",sign);
		}
		else{//删除
			 for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFlash.diginput[j].devID,argv[2])==0){
							packFlash.diginput[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete diginput channel %d\n",sign,j+1);
					}
			 }
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[digInput 水泵 GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-8 之外清除对应ID的所有参数]\n",sign);

}
MSH_CMD_EXPORT(digInput,digInput config);//FINSH_FUNCTION_EXPORT_CMD


