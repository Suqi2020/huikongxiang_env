#include <board.h>


const static char sign[]="[digInput]";



#define IOIN1_READ       HAL_GPIO_ReadPin(IO_IN1_GPIO_Port, IO_IN1_Pin)
#define IOIN2_READ       HAL_GPIO_ReadPin(IO_IN2_GPIO_Port, IO_IN2_Pin)
#define IOIN3_READ       HAL_GPIO_ReadPin(IO_IN3_GPIO_Port, IO_IN3_Pin)
#define IOIN4_READ       HAL_GPIO_ReadPin(IO_IN4_GPIO_Port, IO_IN4_Pin)
#define IOIN5_READ       HAL_GPIO_ReadPin(IO_IN5_GPIO_Port, IO_IN5_Pin)
#define IOIN6_READ       HAL_GPIO_ReadPin(IO_IN6_GPIO_Port, IO_IN6_Pin)
#define IOIN7_READ       HAL_GPIO_ReadPin(IO_IN7_GPIO_Port, IO_IN7_Pin)
#define IOIN8_READ       HAL_GPIO_ReadPin(IO_IN8_GPIO_Port, IO_IN8_Pin)

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
				if(packFlash.input[i].workFlag==true){
						if(diReadFun(i)==true){
								inpoutpFlag.digInput[i].upFlag =true;
							  inpoutpFlag.digInput[i].lowFlag =false; 
							  rt_kprintf("%s diIOSet upFlag %d\n",sign,i);
						}
						else {
							  inpoutpFlag.digInput[i].upFlag =false;
								inpoutpFlag.digInput[i].lowFlag=true;
							  rt_kprintf("%s diIOSet lowFlag %d\n",sign,i);
						}
				}
		}
}


void printfDIList()
{
		for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(packFlash.input[j].workFlag==RT_TRUE){//打开
						rt_kprintf("%s digInput ",sign);

						rt_kprintf("%s ",packFlash.input[j].name);
						rt_kprintf("%s ",packFlash.input[j].devID);
						
						rt_kprintf("%s ",packFlash.input[j].model);
						rt_kprintf("%d \n",packFlash.input[j].port);



				}
		}
}

//digInput 水泵 GYNJLXSD000000162 GY281 1
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
				packFlash.input[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFlash.input[port-1].name, argv[1]);
				rt_strcpy(packFlash.input[port-1].devID,argv[2]);
				rt_strcpy(packFlash.input[port-1].model,argv[3]);
				packFlash.input[port-1].port=port;
				rt_kprintf("%s add diginput chanl %d\n",sign,port);
        rt_kprintf("%s digInput OK\n",sign);
		}
		else{//删除
			 for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFlash.input[j].devID,argv[2])==0){
							packFlash.input[j].workFlag=RT_FALSE;
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


