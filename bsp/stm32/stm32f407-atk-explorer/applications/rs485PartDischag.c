#include <board.h>

//<<局放在线检测 --GY-JF100-C01>>  GZPD-1000电缆局放

#define  MSGPOOL_LEN   1024 //485数据最大量  大于1k需要修改此处
rt_mutex_t partDischagMutex = RT_NULL;

//队列的定义

uint8_t partDischagQuePool[MSGPOOL_LEN];  //

struct  rt_messagequeue partDischagmque;

#define   SLAVE_ADDR     0X01
#define   LENTH          1024  //工作环流用到的最大接收buf长度
//#define   LARGE_TIMES    100 //放大倍数  后期如果需要 读取寄存器0x000b 有可能放大10倍
extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];


void partDischagUartSend(uint8_t *buf,int len)
{
		UART3_485_SEND;
	  HAL_UART_Transmit(&huart3,(uint8_t *)buf,len,1000);
		UART3_485_REC;
}

//创建局放用到的互斥量和消息队列
void  partDischagMutexQueueCreat()
{
	  partDischagMutex = rt_mutex_create("partDischagMutex", RT_IPC_FLAG_FIFO);
    if (partDischagMutex == RT_NULL)
    {
        rt_kprintf("create partDischagMutex failed.\n");
        return ;
    }
		
//////////////////////////////////消息队列//////////////////////////////////
		
		int result = rt_mq_init(&partDischagmque,"partDischagmque",&partDischagQuePool[0],1,sizeof(partDischagQuePool),RT_IPC_FLAG_FIFO);       
		if (result != RT_EOK)
    {
        rt_kprintf("init partDischagmque failed.\n");
        return ;
    }
}