#include   "NetJsonDataTask.h"
#include   "board.h"




struct rt_mailbox mbNetData;

void   NetJsonTask(void *para)
{
	
	  char *str;
		while(1){
			
			
			
			 if (rt_mb_recv(&mbNetData, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("thread1: get a mail from mailbox, the content:%s  %d\n", str,strlen(str));

            /* ??100ms */
            rt_thread_mdelay(100);
        }
			  rt_thread_mdelay(1000);
		}
}


void   NetJsonTask2(void *para)
{
	  
	  volatile uint8_t i=30;
	  uint8_t  *mb_str1;

					rt_thread_mdelay(500);
	char *test="1234567890qwertyuio\n";
		while(1){
				while(i--){
						//rt_thread_mdelay(500);
						mb_str1=(uint8_t  *)rt_malloc(100);
						memcpy(mb_str1,test,strlen(test)+1);
						rt_mb_send_wait(&mbNetData, (rt_uint32_t)&mb_str1,0XFFFFFFFF);
						rt_kprintf("send %s %d\n",mb_str1,i);
				}
				while(1){
				rt_thread_mdelay(500);
				}
		}
}