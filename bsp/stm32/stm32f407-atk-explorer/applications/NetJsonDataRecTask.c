#include   "NetJsonDataTask.h"
#include   "board.h"


struct rt_mailbox mbNetRecData;
extern void AllDownPhrase(char *data,int lenth);
//网络数据接收的处理 
void   netDataRecTask(void *para)
{
	
	  uint8_t *str=RT_NULL;
		while(1){
			 if (rt_mb_recv(&mbNetRecData, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
			 {
						if((str[0]==(uint8_t)((uint16_t)HEAD>>8))&&(str[1]==(uint8_t)(HEAD))){
								int lenth= (str[2]<<8)+str[3]+HEAD_LEN+LENTH_LEN+CRC_LEN+TAIL_LEN;  
								if(lenth<=2048){
										
									  for(int i=0;i<lenth;i++)
									    rt_kprintf("%02x",str[i]);
										rt_kprintf("\r\n net rec  %d\r\n",lenth);
									
									  AllDownPhrase((char *)str,lenth);
								}
								else{
									  rt_kprintf("lenth  err：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",lenth,str[0],str[1],str[2],str[3]);
										//rt_kprintf("lenth  err %d str[2]:%d  str[3]:%d\r\n",lenth,str[2],str[3]);
								}
						}
						else{
							   rt_kprintf("head  err\r\n");
						}	 
				 
//	rt_kprintf("netrec:%d\r\n",strlen(str));

//	for(int i=0;i<strlen(str);i++)
//	rt_kprintf("%02x",str[i]);
//	rt_kprintf("\r\n");
//				 
//				 
//					rt_kprintf("thread1: get a mail:%s  %d\r\n", str,strlen(str));
          
					
			 }

		}
}
