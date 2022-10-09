#include   "NetJsonDataTask.h"
#include   "board.h"


struct rt_mailbox mbNetSendData;

extern uint16_t netSend(uint8_t *data,int len);
extern rt_bool_t gbNetState;
uint16_t netDataSendCheck(uint8_t *str)
{
	  int lenth=0;
		if((str[0]==(uint8_t)((uint16_t)HEAD>>8))&&(str[1]==(uint8_t)(HEAD))){
			  lenth= (str[2]<<8)+str[3]+HEAD_LEN+LENTH_LEN+CRC_LEN+TAIL_LEN;  
				if(lenth<=2048){
				}
				else{
					rt_kprintf("lenth  err：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",lenth,str[0],str[1],str[2],str[3]);
						//rt_kprintf("lenth  err %d str[2]:%d  str[3]:%d\r\n",lenth,str[2],str[3]);
				}
		}
		else{
				 rt_kprintf("head  err\r\n");
		}
		return lenth;
}
//网络数据的发送
void   netDataSendTask(void *para)
{
		uint8_t *str=RT_NULL;
		while(1){
			  if (rt_mb_recv(&mbNetSendData, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
        { 
						int lenth = netDataSendCheck(str);
						if(lenth!=0){
								if(netSend(str,lenth)==0){//启动个定时器来实现重发  2s内收不到回复
										gbNetState=RT_FALSE;//发送身边 重新联网
										rt_kprintf("net send fail\n");
								}
								else{
										rt_kprintf("net send succ\n");
								}	
						}
				}
		}
}

