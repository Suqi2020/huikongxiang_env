#include <board.h>

const static char sign[]="[digOutput]";



void printfDOList()
{
		for(int j=0;j<DO_NUM;j++){//��һ�� �ҵ� GYNJLXSD000000499  ���
				if(packFLash.output[j].workFlag==RT_TRUE){//��
						rt_kprintf("%s digOutput ",sign);
						rt_kprintf("%s ",packFLash.output[j].name);
						rt_kprintf("%s ",packFLash.output[j].devID);
						rt_kprintf("%s ",packFLash.output[j].model);
						rt_kprintf("%d \n",packFLash.output[j].port);
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
				packFLash.output[port-1].workFlag=RT_TRUE;
				rt_strcpy(packFLash.output[port-1].name, argv[1]);
				rt_strcpy(packFLash.output[port-1].devID,argv[2]);
				rt_strcpy(packFLash.output[port-1].model,argv[3]);
				packFLash.output[port-1].port=port;
				rt_kprintf("%s add digOutput chanl %d\n",sign,port);
			  rt_kprintf("%s digOutput OK\n",sign);
		}
		else{//ɾ��
			 for(int j=0;j<DO_NUM;j++){//��һ�� �ҵ� GYNJLXSD000000499  ���
					if(rt_strcmp(packFLash.output[j].devID,argv[2])==0){
							packFLash.output[j].workFlag=RT_FALSE;
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

