#include <board.h>

//<<�ַ����߼�� --GY-JF100-C01>>  GZPD-1000���¾ַ�

#define  MSGPOOL_LEN   1024 //485���������  ����1k��Ҫ�޸Ĵ˴�
rt_mutex_t partDischagMutex = RT_NULL;

//���еĶ���

uint8_t partDischagQuePool[MSGPOOL_LEN];  //

struct  rt_messagequeue partDischagmque;

#define   SLAVE_ADDR     0X01
#define   LENTH          1024  //���������õ���������buf����
//#define   LARGE_TIMES    100 //�Ŵ���  ���������Ҫ ��ȡ�Ĵ���0x000b �п��ܷŴ�10��
extern uint8_t packBuf[TX_RX_MAX_BUF_SIZE];


void partDischagUartSend(uint8_t *buf,int len)
{
		UART3_485_SEND;
	  HAL_UART_Transmit(&huart3,(uint8_t *)buf,len,1000);
		UART3_485_REC;
}

//�����ַ��õ��Ļ���������Ϣ����
void  partDischagMutexQueueCreat()
{
	  partDischagMutex = rt_mutex_create("partDischagMutex", RT_IPC_FLAG_FIFO);
    if (partDischagMutex == RT_NULL)
    {
        rt_kprintf("create partDischagMutex failed.\n");
        return ;
    }
		
//////////////////////////////////��Ϣ����//////////////////////////////////
		
		int result = rt_mq_init(&partDischagmque,"partDischagmque",&partDischagQuePool[0],1,sizeof(partDischagQuePool),RT_IPC_FLAG_FIFO);       
		if (result != RT_EOK)
    {
        rt_kprintf("init partDischagmque failed.\n");
        return ;
    }
}