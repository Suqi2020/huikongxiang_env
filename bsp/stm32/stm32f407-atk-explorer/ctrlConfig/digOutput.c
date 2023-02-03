#include <board.h>

const static char sign[]="[digOutput]";



void printfDOList()
{
		for(int j=0;j<DO_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(packFLash.output[j].workFlag==RT_TRUE){//打开
						rt_kprintf("%s digOutput ",sign);
						rt_kprintf("%s ",packFLash.output[j].name);
						rt_kprintf("%s ",packFLash.output[j].devID);
						rt_kprintf("%s ",packFLash.output[j].model);
						rt_kprintf("%d \n",packFLash.output[j].port);
				}
		}
}

//按照port来添加 
static void digOutput(int argc, char *argv[])
{
	 
	  if(0==rt_strcmp((char *)"list", argv[1])){
				printfDOList();
				return;
		}
		if(argc!=5){
				goto ERR;
		}
    int port = atoi16(argv[4],10);
		if((port<=DO_NUM)&&(port>0)){//添加
				packFLash.output[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.output[port-1].name, argv[1]);
				rt_strcpy(packFLash.output[port-1].devID,argv[2]);
				rt_strcpy(packFLash.output[port-1].model,argv[3]);
				packFLash.output[port-1].port=port;
				rt_kprintf("%s add digOutput chanl %d\n",sign,port);
			  rt_kprintf("%s digOutput OK\n",sign);
		}
		else{//删除
			 for(int j=0;j<DO_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFLash.output[j].devID,argv[2])==0){
							packFLash.output[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete digOutput channel %d\n",sign,j+1);
					}
			 }
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[digOutput 水泵 GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-8 之外清除对应ID的所有参数]\n",sign);

}
MSH_CMD_EXPORT(digOutput,digOutput config);//FINSH_FUNCTION_EXPORT_CMD

