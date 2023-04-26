#include   "NetJsonDataTask.h"
#include   "board.h"

const static char task[]="[dataSend]";
//mqtt数据包重新封装为头部4字节+头部+mqtttopic+mqttjson格式

struct rt_mailbox mbNetSendData;
extern void netSend(uint8_t *data,int len);
void   netDataSendTask(void *para)
{
	  extern rt_bool_t gbNetState;
		uint8_t *str=RT_NULL;
		while(1){
			  if (rt_mb_recv(&mbNetSendData, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
        { 
						uint32_t lenth = (str[0]<<24)+(str[1]<<16)+(str[2]<<8)+str[3];
						if((lenth!=0)&&(gbNetState ==RT_TRUE)){
							netSend(str+PACK_HEAD_LEN,lenth);//头部不发送
						}
						else
							rt_kprintf("%sERR:net offline drop data\r\n",task);
				}
		}
}
