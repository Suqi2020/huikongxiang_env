#include <board.h>
//包括8路ttl电平输出和2路3V3电平输出  2路5V电平输出 4路12V输出
const static char sign[]="[output]";



void printfDOList()
{
		for(int j=0;j<DO_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(packFLash.digoutput[j].workFlag==RT_TRUE){//打开
						rt_kprintf("%s digOutput ",sign);
						rt_kprintf("%s ",packFLash.digoutput[j].name);
						rt_kprintf("%s ",packFLash.digoutput[j].devID);
						rt_kprintf("%s ",packFLash.digoutput[j].model);
						rt_kprintf("%d \n",packFLash.digoutput[j].port);
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
				packFLash.digoutput[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.digoutput[port-1].name, argv[1]);
				rt_strcpy(packFLash.digoutput[port-1].devID,argv[2]);
				rt_strcpy(packFLash.digoutput[port-1].model,argv[3]);
				packFLash.digoutput[port-1].port=port;
				rt_kprintf("%s add digOutput chanl %d\n",sign,port);
			  rt_kprintf("%s digOutput OK\n",sign);
		}
		else{//删除
			 for(int j=0;j<DO_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFLash.digoutput[j].devID,argv[2])==0){
							packFLash.digoutput[j].workFlag=RT_FALSE;
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





void printfPower3V3List()
{
		for(int j=0;j<V33O_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(packFLash.v33output[j].workFlag==RT_TRUE){//打开
						rt_kprintf("%s power3V3 ",sign);
						rt_kprintf("%s ",packFLash.v33output[j].name);
						rt_kprintf("%s ",packFLash.v33output[j].devID);
						rt_kprintf("%s ",packFLash.v33output[j].model);
						rt_kprintf("%d \n",packFLash.v33output[j].port);
				}
		}
}

//按照port来添加 
static void power3V3(int argc, char *argv[])
{
	 
	  if(0==rt_strcmp((char *)"list", argv[1])){
				printfPower3V3List();
				return;
		}
		if(argc!=5){
				goto ERR;
		}
    int port = atoi16(argv[4],10);
		if((port<=V33O_NUM)&&(port>0)){//添加
				packFLash.v33output[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.v33output[port-1].name, argv[1]);
				rt_strcpy(packFLash.v33output[port-1].devID,argv[2]);
				rt_strcpy(packFLash.v33output[port-1].model,argv[3]);
				packFLash.v33output[port-1].port=port;
				rt_kprintf("%s add 3V3Output chanl %d\n",sign,port);
			  rt_kprintf("%s 3V3Output OK\n",sign);
		}
		else{//删除
			 for(int j=0;j<V33O_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFLash.v33output[j].devID,argv[2])==0){
							packFLash.v33output[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete 3V3Output channel %d\n",sign,j+1);
					}
			 }
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[power3V3 水泵 GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-8 之外清除对应ID的所有参数]\n",sign);

}
MSH_CMD_EXPORT(power3V3,power3V3 config);//FINSH_FUNCTION_EXPORT_CMD




void printfPower5VList()
{
		for(int j=0;j<V5O_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(packFLash.v5output[j].workFlag==RT_TRUE){//打开
						rt_kprintf("%s power5V ",sign);
						rt_kprintf("%s ",packFLash.v5output[j].name);
						rt_kprintf("%s ",packFLash.v5output[j].devID);
						rt_kprintf("%s ",packFLash.v5output[j].model);
						rt_kprintf("%d \n",packFLash.v5output[j].port);
				}
		}
}

//按照port来添加 
static void power5V(int argc, char *argv[])
{
	 
	  if(0==rt_strcmp((char *)"list", argv[1])){
				printfDOList();
				return;
		}
		if(argc!=5){
				goto ERR;
		}
    int port = atoi16(argv[4],10);
		if((port<=V5O_NUM)&&(port>0)){//添加
				packFLash.v5output[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.v5output[port-1].name, argv[1]);
				rt_strcpy(packFLash.v5output[port-1].devID,argv[2]);
				rt_strcpy(packFLash.v5output[port-1].model,argv[3]);
				packFLash.v5output[port-1].port=port;
				rt_kprintf("%s add 5VOutput chanl %d\n",sign,port);
			  rt_kprintf("%s 5VOutput OK\n",sign);
		}
		else{//删除
			 for(int j=0;j<V5O_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFLash.v5output[j].devID,argv[2])==0){
							packFLash.v5output[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete 5VOutput channel %d\n",sign,j+1);
					}
			 }
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[power5V 水泵 GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-8 之外清除对应ID的所有参数]\n",sign);

}
MSH_CMD_EXPORT(power5V,power5V config);//FINSH_FUNCTION_EXPORT_CMD

void printfPower12VList()
{
		for(int j=0;j<V12O_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(packFLash.v5output[j].workFlag==RT_TRUE){//打开
						rt_kprintf("%s power12V ",sign);
						rt_kprintf("%s ",packFLash.v5output[j].name);
						rt_kprintf("%s ",packFLash.v5output[j].devID);
						rt_kprintf("%s ",packFLash.v5output[j].model);
						rt_kprintf("%d \n",packFLash.v5output[j].port);
				}
		}
}

//按照port来添加 
static void power12V(int argc, char *argv[])
{
	 
	  if(0==rt_strcmp((char *)"list", argv[1])){
				printfPower12VList();
				return;
		}
		if(argc!=5){
				goto ERR;
		}
    int port = atoi16(argv[4],10);
		if((port<=V12O_NUM)&&(port>0)){//添加
				packFLash.v5output[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.v5output[port-1].name, argv[1]);
				rt_strcpy(packFLash.v5output[port-1].devID,argv[2]);
				rt_strcpy(packFLash.v5output[port-1].model,argv[3]);
				packFLash.v5output[port-1].port=port;
				rt_kprintf("%s add 12VOutput chanl %d\n",sign,port);
			  rt_kprintf("%s 12VOutput OK\n",sign);
		}
		else{//删除
			 for(int j=0;j<V12O_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFLash.v5output[j].devID,argv[2])==0){
							packFLash.v5output[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete 12VOutput channel %d\n",sign,j+1);
					}
			 }
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[power12V 水泵 GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-4 之外清除对应ID的所有参数]\n",sign);

}
MSH_CMD_EXPORT(power12V,power12V config);//FINSH_FUNCTION_EXPORT_CMD



void printfOutputList()
{
		printfDOList();
	  printfPower3V3List();
	  printfPower5VList();
	  printfPower12VList();
}