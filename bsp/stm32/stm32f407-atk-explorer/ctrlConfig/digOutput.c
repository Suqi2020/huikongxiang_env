#include <board.h>
//����8·ttl��ƽ�����2·3V3��ƽ���  2·5V��ƽ��� 4·12V���
const static char sign[]="[output]";



void printfDOList()
{
		for(int j=0;j<DO_NUM;j++){//��һ�� �ҵ� GYNJLXSD000000499  ���
				if(packFLash.digoutput[j].workFlag==RT_TRUE){//��
						rt_kprintf("%s digOutput ",sign);
						rt_kprintf("%s ",packFLash.digoutput[j].name);
						rt_kprintf("%s ",packFLash.digoutput[j].devID);
						rt_kprintf("%s ",packFLash.digoutput[j].model);
						rt_kprintf("%d \n",packFLash.digoutput[j].port);
				}
		}
}

//����port����� 
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
		if((port<=DO_NUM)&&(port>0)){//���
				packFLash.digoutput[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.digoutput[port-1].name, argv[1]);
				rt_strcpy(packFLash.digoutput[port-1].devID,argv[2]);
				rt_strcpy(packFLash.digoutput[port-1].model,argv[3]);
				packFLash.digoutput[port-1].port=port;
				rt_kprintf("%s add digOutput chanl %d\n",sign,port);
			  rt_kprintf("%s digOutput OK\n",sign);
		}
		else{//ɾ��
			 for(int j=0;j<DO_NUM;j++){//��һ�� �ҵ� GYNJLXSD000000499  ���
					if(rt_strcmp(packFLash.digoutput[j].devID,argv[2])==0){
							packFLash.digoutput[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete digOutput channel %d\n",sign,j+1);
					}
			 }
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[digOutput ˮ�� GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-8 ֮�������ӦID�����в���]\n",sign);

}
MSH_CMD_EXPORT(digOutput,digOutput config);//FINSH_FUNCTION_EXPORT_CMD





void printfPower3V3List()
{
		for(int j=0;j<V33O_NUM;j++){//��һ�� �ҵ� GYNJLXSD000000499  ���
				if(packFLash.v33output[j].workFlag==RT_TRUE){//��
						rt_kprintf("%s power3V3 ",sign);
						rt_kprintf("%s ",packFLash.v33output[j].name);
						rt_kprintf("%s ",packFLash.v33output[j].devID);
						rt_kprintf("%s ",packFLash.v33output[j].model);
						rt_kprintf("%d \n",packFLash.v33output[j].port);
				}
		}
}

//����port����� 
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
		if((port<=V33O_NUM)&&(port>0)){//���
				packFLash.v33output[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.v33output[port-1].name, argv[1]);
				rt_strcpy(packFLash.v33output[port-1].devID,argv[2]);
				rt_strcpy(packFLash.v33output[port-1].model,argv[3]);
				packFLash.v33output[port-1].port=port;
				rt_kprintf("%s add 3V3Output chanl %d\n",sign,port);
			  rt_kprintf("%s 3V3Output OK\n",sign);
		}
		else{//ɾ��
			 for(int j=0;j<V33O_NUM;j++){//��һ�� �ҵ� GYNJLXSD000000499  ���
					if(rt_strcmp(packFLash.v33output[j].devID,argv[2])==0){
							packFLash.v33output[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete 3V3Output channel %d\n",sign,j+1);
					}
			 }
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[power3V3 ˮ�� GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-8 ֮�������ӦID�����в���]\n",sign);

}
MSH_CMD_EXPORT(power3V3,power3V3 config);//FINSH_FUNCTION_EXPORT_CMD




void printfPower5VList()
{
		for(int j=0;j<V5O_NUM;j++){//��һ�� �ҵ� GYNJLXSD000000499  ���
				if(packFLash.v5output[j].workFlag==RT_TRUE){//��
						rt_kprintf("%s power5V ",sign);
						rt_kprintf("%s ",packFLash.v5output[j].name);
						rt_kprintf("%s ",packFLash.v5output[j].devID);
						rt_kprintf("%s ",packFLash.v5output[j].model);
						rt_kprintf("%d \n",packFLash.v5output[j].port);
				}
		}
}

//����port����� 
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
		if((port<=V5O_NUM)&&(port>0)){//���
				packFLash.v5output[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.v5output[port-1].name, argv[1]);
				rt_strcpy(packFLash.v5output[port-1].devID,argv[2]);
				rt_strcpy(packFLash.v5output[port-1].model,argv[3]);
				packFLash.v5output[port-1].port=port;
				rt_kprintf("%s add 5VOutput chanl %d\n",sign,port);
			  rt_kprintf("%s 5VOutput OK\n",sign);
		}
		else{//ɾ��
			 for(int j=0;j<V5O_NUM;j++){//��һ�� �ҵ� GYNJLXSD000000499  ���
					if(rt_strcmp(packFLash.v5output[j].devID,argv[2])==0){
							packFLash.v5output[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete 5VOutput channel %d\n",sign,j+1);
					}
			 }
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[power5V ˮ�� GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-8 ֮�������ӦID�����в���]\n",sign);

}
MSH_CMD_EXPORT(power5V,power5V config);//FINSH_FUNCTION_EXPORT_CMD

void printfPower12VList()
{
		for(int j=0;j<V12O_NUM;j++){//��һ�� �ҵ� GYNJLXSD000000499  ���
				if(packFLash.v5output[j].workFlag==RT_TRUE){//��
						rt_kprintf("%s power12V ",sign);
						rt_kprintf("%s ",packFLash.v5output[j].name);
						rt_kprintf("%s ",packFLash.v5output[j].devID);
						rt_kprintf("%s ",packFLash.v5output[j].model);
						rt_kprintf("%d \n",packFLash.v5output[j].port);
				}
		}
}

//����port����� 
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
		if((port<=V12O_NUM)&&(port>0)){//���
				packFLash.v5output[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.v5output[port-1].name, argv[1]);
				rt_strcpy(packFLash.v5output[port-1].devID,argv[2]);
				rt_strcpy(packFLash.v5output[port-1].model,argv[3]);
				packFLash.v5output[port-1].port=port;
				rt_kprintf("%s add 12VOutput chanl %d\n",sign,port);
			  rt_kprintf("%s 12VOutput OK\n",sign);
		}
		else{//ɾ��
			 for(int j=0;j<V12O_NUM;j++){//��һ�� �ҵ� GYNJLXSD000000499  ���
					if(rt_strcmp(packFLash.v5output[j].devID,argv[2])==0){
							packFLash.v5output[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete 12VOutput channel %d\n",sign,j+1);
					}
			 }
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[power12V ˮ�� GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-4 ֮�������ӦID�����в���]\n",sign);

}
MSH_CMD_EXPORT(power12V,power12V config);//FINSH_FUNCTION_EXPORT_CMD



void printfOutputList()
{
		printfDOList();
	  printfPower3V3List();
	  printfPower5VList();
	  printfPower12VList();
}