#include <board.h>


const static char sign[]="[digInput]";




void printfDIList()
{
		for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(packFLash.input[j].workFlag==RT_TRUE){//打开
						rt_kprintf("%s digInput ",sign);

						rt_kprintf("%s ",packFLash.input[j].name);
						rt_kprintf("%s ",packFLash.input[j].devID);
						
						rt_kprintf("%s ",packFLash.input[j].model);
						rt_kprintf("%d \n",packFLash.input[j].port);



				}
		}
}

//digInput 水泵 GYNJLXSD000000162 GY281 1
static void digInput(int argc, char *argv[])
{
	 if(0==rt_strcmp((char *)"list", argv[1])){
				printfDIList();
				return;
		}
		if(argc!=5){
				goto ERR;
		}
	 int port = atoi16(argv[4],10);

	 if((port<=DI_NUM)&&(port>0)){//添加
				packFLash.input[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.input[port-1].name, argv[1]);
				rt_strcpy(packFLash.input[port-1].devID,argv[2]);
				rt_strcpy(packFLash.input[port-1].model,argv[3]);
				packFLash.input[port-1].port=port;
				rt_kprintf("%s add diginput chanl %d\n",sign,port);

	 }
	 else{//删除
			 for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFLash.input[j].devID,argv[2])==0){
							packFLash.input[j].workFlag=RT_FALSE;
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


