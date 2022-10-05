#include   "NetJsonDataTask.h"
#include   "board.h"


struct rt_mailbox mbNetRecData;
extern void AllDownPhrase(char *data,int lenth);
//网络数据接收的处理 
void   netDataRecTask(void *para)
{
	
	  char *str=RT_NULL;
		while(1){
			 if (rt_mb_recv(&mbNetRecData, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
			 {
					rt_kprintf("thread1: get a mail:%s  %d\r\n", str,strlen(str));
          AllDownPhrase(str,strlen(str));
					
			 }

		}
}
