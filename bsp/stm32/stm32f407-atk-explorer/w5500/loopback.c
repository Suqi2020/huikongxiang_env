#include "loopback.h"
#include "board.h"

uint8 I_STATUS[MAX_SOCK_NUM];
uint8 ch_status[MAX_SOCK_NUM] = {0};/** 0:close, 1:ready, 2:connected */

//uint8_t  NetTxBuffer[TX_RX_MAX_BUF_SIZE]={0};
uint8_t  NetRxBuffer[TX_RX_MAX_BUF_SIZE]={0};
uint16_t netRxBufLen=0;
const static char sign[]="[lookback]";
void rstCh_status()
{
		rt_memset(ch_status,0,MAX_SOCK_NUM);
}
//					len = recvfrom(s, data_buf, len,(uint8*)&destip,&destport);/* read the received data */
//					sendto(s, data_buf, len,(uint8*)&destip,destport) ;

void W5500ISR(void)
{
	uint8 IR_val = 0, SIR_val = 0;
	uint8 tmp,s;
	
	IINCHIP_WRITE(IMR, 0x00);
	IINCHIP_WRITE(SIMR, 0x00);
    IINCHIP_ISR_DISABLE();

	IR_val = IINCHIP_READ(IR);
	SIR_val = IINCHIP_READ(SIR);


#ifdef DEBUG
      rt_kprintf("\r\nIR_val : %02x", IR_val);
      rt_kprintf("\r\nSIR_val : %02x\r\n", SIR_val);
      //printf("\r\nSn_MR(0): %02x", IINCHIP_READ(Sn_MR(0)));
      //printf("\r\nSn_SR(0): %02x\r\n", IINCHIP_READ(Sn_SR(0)));
#endif


	if (IR_val > 0) {
	   	if (IR_val & IR_CONFLICT)
	   	{
#ifdef DEBUG
			rt_kprintf("IP conflict : %.2x\r\n", IR_val);
#endif
	   	}
	   	if (IR_val & IR_MAGIC)
	   	{
#ifdef DEBUG
	   		rt_kprintf("Magic packet: %.2x\r\n",IR_val);
#endif
	   	}
		if (IR_val & IR_PPPoE)
	   	{
#ifdef DEBUG
	   		rt_kprintf("PPPoE closed: %.2x\r\n",IR_val);
#endif
	   	}
		if (IR_val & IR_UNREACH)
	   	{
#ifdef DEBUG
	   		rt_kprintf("Destination unreached: %.2x\r\n",IR_val);
#endif
	   	}
    /* interrupt clear */
    IINCHIP_WRITE(IR, IR_val);
	}
	for(s = 0;s < 8;s ++)
	{
		tmp = 0;
		if (SIR_val & IR_SOCK(s))
		{
			/* save interrupt value*/
			tmp = IINCHIP_READ(Sn_IR(s)); // can be come to over two times.
			I_STATUS[s] |= tmp;			
			tmp &= 0x0F; 
			//I_STATUS_FLAG[0]++;
			IINCHIP_WRITE(Sn_IR(s), tmp);	
#ifdef DEBUG
			rt_kprintf("Sn_IR(s): %.2x\r\n",tmp);
#endif
			/*---*/
		}
	}


    IINCHIP_ISR_ENABLE();
	IINCHIP_WRITE(IMR, 0xF0);
	IINCHIP_WRITE(SIMR, 0xFF); 

//#ifdef DEBUG
//        printf("IR2: %02x, IMR2: %02x, Sn_IR(0): %02x, Sn_IMR(0): %02x\r\n", IINCHIP_READ(IR2), IINCHIP_READ(IMR2), IINCHIP_READ(Sn_IR(0)), IINCHIP_READ(Sn_IMR(0)));
//#endif
}

void SOCK_DISCON(SOCKET s)
{
	disconnect(s);
	ch_status[s] = 0;
	I_STATUS[s] &= ~(0x02);
}

		



extern rt_bool_t gbNetState;
 rt_bool_t  offLineTimesGet=RT_TRUE;


uint32_t gu32RecDataLen=0;

extern int  netPhraseRecData(uint8_t *recBuf ,int len);



//extern int mqttRet;
void loopback_tcp(uint16 port)
{
	// uint16 len=0;	
	
	switch(getSn_SR(SOCK_TCPC))								  				         /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:											        		         /*socket���ڹر�״̬*/
			socket(SOCK_TCPC,Sn_MR_TCP,port,Sn_MR_ND);
		  rt_kprintf("SOCK_CLOSED\n");
		  break;
		case SOCK_INIT:													        	         /*socket���ڳ�ʼ��״̬*/
			connect(SOCK_TCPC,packFlash.netIpFlash.remoteIp,packFlash.netIpFlash.remotePort);               /*socket���ӷ�����*/ 
		  rt_kprintf("SOCK_INIT\n");
		  break;
		case SOCK_ESTABLISHED: 												             /*socket�������ӽ���״̬*/
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*��������жϱ�־λ*/
			}
			netRxBufLen=getSn_RX_RSR(SOCK_TCPC); 								  	         /*����lenΪ�ѽ������ݵĳ���*/
			if(netRxBufLen>0)
			{
				recv(SOCK_TCPC,NetRxBuffer,netRxBufLen); 							   		         /*��������Server������*/
				rt_kprintf("rec:");
				for(int i=0;i<netRxBufLen ;i++)
				rt_kprintf("%02x ",NetRxBuffer[i]);
				rt_kprintf("\n");
				void netRecSendEvent();		
        netRecSendEvent();				//mqttLoopData();						     	         /*��Server��������*/
			}		  
			if(gbNetState!=RT_TRUE){
					gbNetState =RT_TRUE;	
					rt_kprintf("SOCK_ESTABLISHED\n");
			}
		  break;
		case SOCK_CLOSE_WAIT: 											    	         /*socket���ڵȴ��ر�״̬*/
			close(SOCK_TCPC);
		  rt_kprintf("SOCK_CLOSE_WAIT\n");
		  break;
	}
}




void w5500Init()
{

	  SOCK_DISCON(SOCK_TCPC);
		reset_w5500();											/*Ӳ��λW5500*/
		set_w5500_mac();										/*����MAC��ַ*/
		setRTR(2000);/*�������ʱ��ֵ*/
		setRCR(3);/*����������·��ʹ���*/
		socket_buf_init(txsize, rxsize);		/*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
		IINCHIP_WRITE(Sn_MR(7), 0x20);//TCPģʽ�¿�������ʱACK
		IINCHIP_WRITE(Sn_IMR(7), 0x0F);
		IINCHIP_WRITE(IMR, 0xF0);
		IINCHIP_WRITE(SIMR, 0xFE);
	 
	  extern void rstDhcp();
	  rstDhcp();

		rstCh_status();
}
