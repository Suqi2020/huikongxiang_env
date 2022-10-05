#include   "NetJsonDataTask.h"
#include   "board.h"


struct rt_mailbox mbNetSendData;



//网络数据的发送
void   netDataSednTask(void *para)
{
		uint8_t *str=RT_NULL;
		while(1){
			  if (rt_mb_recv(&mbNetSendData, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
        {
						if((str[0]==(uint8_t)((uint16_t)HEAD>>8))&&(str[1]==(uint8_t)(HEAD))){
								int lenth= (str[2]<<8)+str[3]+HEAD_LEN+LENTH_LEN+CRC_LEN+TAIL_LEN;  
								if(lenth<=2048){
										extern void netSend(uint8_t *data,int len);
										netSend(str,lenth);
										rt_kprintf("net send\r\n");
								}
								else{
										rt_kprintf("lenth  err %d\r\n",lenth);
								}
						}
						else{
							   rt_kprintf("head  err\r\n");
						}
        }

		}
}

