#include <rtthread.h>
#include <board.h>
#ifdef   USE_WDT

extern IWDG_HandleTypeDef hiwdg;


//看门狗最大32秒复位 这里10秒钟喂狗一次
void   WDTTask(void *parameter)
{
	  uint32_t e;
		while(1){
				if(rt_event_recv(&WDTEvent,\
					(EVENT_WDT_AUTOCTRL|EVENT_WDT_W5500|EVENT_WDT_UPTASK|EVENT_WDT_MQTTTASK|EVENT_WDT_LCDTASK|EVENT_WDT_RECTASK|EVENT_WDT_SENDTASK),\
					RT_EVENT_FLAG_AND|RT_EVENT_FLAG_CLEAR,10000, &e) == RT_EOK){
						HAL_IWDG_Refresh(&hiwdg);
						rt_kprintf("feed dog 0x%x\n ",e);
					}
		}
}

#endif
