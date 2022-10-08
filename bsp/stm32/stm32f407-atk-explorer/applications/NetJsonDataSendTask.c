#include   "NetJsonDataTask.h"
#include   "board.h"


struct rt_mailbox mbNetSendData;

extern uint16_t netSend(uint8_t *data,int len);
extern rt_bool_t gbNetState;
//网络数据的发送
void   netDataSendTask(void *para)
{
		uint8_t *str=RT_NULL;
		while(1){
			  if (rt_mb_recv(&mbNetSendData, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
        {
						if((str[0]==(uint8_t)((uint16_t)HEAD>>8))&&(str[1]==(uint8_t)(HEAD))){
								int lenth= (str[2]<<8)+str[3]+HEAD_LEN+LENTH_LEN+CRC_LEN+TAIL_LEN;  
								if(lenth<=2048){
										
									  if(netSend(str,lenth)==0){
												gbNetState=RT_FALSE;
											  rt_kprintf("net send fail\n");
										}
									  else{
											  rt_kprintf("net send fail\n");
										}
//									  for(int i=0;i<lenth;i++)
//									    rt_kprintf("%02x",str[i]);
//										rt_kprintf("\r\nnet send  %d\r\n",lenth);
								}
								else{
									rt_kprintf("lenth  err：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",lenth,str[0],str[1],str[2],str[3]);
										//rt_kprintf("lenth  err %d str[2]:%d  str[3]:%d\r\n",lenth,str[2],str[3]);
								}
						}
						else{
							   rt_kprintf("head  err\r\n");
						}
        }

		}
}

