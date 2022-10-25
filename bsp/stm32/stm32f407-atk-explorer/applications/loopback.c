#include "loopback.h"
#include "board.h"

uint8 I_STATUS[MAX_SOCK_NUM];
uint8 ch_status[MAX_SOCK_NUM] = {0};/** 0:close, 1:ready, 2:connected */

uint8_t  NetTxBuffer[TX_RX_MAX_BUF_SIZE]={0};
uint8_t  NetRxBuffer[TX_RX_MAX_BUF_SIZE]={0};
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
#if 0
void loopback_tcps(SOCKET s, uint16 port)
{	
	uint16 len = 0;//, cnt = 0;
	uint8 * data_buf = (uint8*) TX_BUF;
	uint8 tmp = 0;

		if(I_STATUS[s] == SOCK_CLOSED)
		{// cloes
			if(!ch_status[s]) 
			{
				ch_status[s] = 1;
				if(socket(s,Sn_MR_TCP,port,0x00) == 0)	  /* reinitialize the socket */
				{
					ch_status[s] = 0;
				}
				else	
				{
					listen(s);
				}
			}
		}  
		if(I_STATUS[s]&Sn_IR_CON)
		{// connected
			ch_status[s] = 2;
			I_STATUS[s] &= ~(0x01);
			//break;
		}
		if(I_STATUS[s]&Sn_IR_DISCON)
		{// discon			
			if ((getSn_RX_RSR(s)) > 0)		/* check Rx data */
			{				
				if (len > TX_RX_MAX_BUF_SIZE) 
				len = TX_RX_MAX_BUF_SIZE; /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
			    recv(s, data_buf, len);		/* read the received data */
						data_buf[len]=0x00;
				rt_kprintf("%s\r\n",data_buf);
			}
			SOCK_DISCON(s);
			//break;
		}
		if(I_STATUS[s]&Sn_IR_RECV)
		{
			IINCHIP_WRITE(IMR, 0x00);//开中断屏蔽IP,UNREACH,PPPoE,Magic Packet
			IINCHIP_WRITE(SIMR, 0x00);//开socket中断屏蔽  
			IINCHIP_ISR_DISABLE();
			tmp = I_STATUS[s];
			I_STATUS[s] &= ~(0x04); // RECV
			IINCHIP_ISR_ENABLE();
 
			IINCHIP_WRITE(IMR, 0xF0);
			IINCHIP_WRITE(SIMR, 0xFF);
		// recv
			if (tmp & Sn_IR_RECV)
			{
				if((len = getSn_RX_RSR(s)) > 0)		/* check Rx data */
				{
					if (len > TX_RX_MAX_BUF_SIZE) 
					len = TX_RX_MAX_BUF_SIZE;	/* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					len = recv(s, data_buf, len); /* read the received data */
					data_buf[len]=0;  //防止多打印
					rt_kprintf("%s\r\n",data_buf);
					send(s,data_buf,len);
					tmp = I_STATUS[s];
				}
			 }
			//break;
		}
		if(I_STATUS[s]&Sn_IR_SEND_OK)
		{
			// send ok
			I_STATUS[s] &= ~(0x10);
			//break;
		}
#if 0
#if (__MCU_TYPE__ == __MCU_STM32F103__)
		/**/
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) == 0x00) EXTI_GenerateSWInterrupt(EXTI_Line6);

#endif


#endif
		
}

#endif
void loopback_tcpc(SOCKET s, uint16 port)
{
	static rt_bool_t  offLineTimesGet=RT_FALSE;
	uint16 len; 						
	//uint8 * data_buf = (uint8*) NetRxBuffer;
	uint8	tmp = 0;
	switch (I_STATUS[s])
	{
		case SOCK_CLOSED://无中断
			// cloes
			if(!ch_status[s])
			{				
				ch_status[s] = 1;
				socket(s, Sn_MR_TCP, port, 0x00);  			//connect(SOCK_TCPC,remote_ip,remote_port);                /*socket连接服务器*/ 
				connect(s, remote_ip, remote_port);
				rt_kprintf("%sw5500 coning\r\n",sign);
			}			
//			rt_kprintf("w5500 cloes %d\r\n",I_STATUS[s]);
			break;
		case Sn_IR_CON: 
			// connected
			ch_status[s] = 2;
			I_STATUS[s] &= ~(0x01);
		  rt_kprintf("%sw5500 connected\r\n",sign);
		  extern rt_bool_t  gbNetState;
			gbNetState =RT_TRUE;	
		  offLineTimesGet =RT_FALSE;
			offLine.relayTimer[offLine.times]=(rt_tick_get()-offLine.relayTimer[offLine.times])/1000;
		
			heartUpPack();
		  extern struct rt_mailbox mbNetSendData;
		  extern uint8_t   packBuf[TX_RX_MAX_BUF_SIZE];
			rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER); 
			break;
		case Sn_IR_DISCON: 
			// discon
			if ((len = getSn_RX_RSR(s)) > 0)		/* check Rx data */
			{
				if (len > TX_RX_MAX_BUF_SIZE) len = TX_RX_MAX_BUF_SIZE; /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
				//uint8_t *data_buf = (uint8_t *)rt_malloc(len);
				len = recv(s, NetRxBuffer, len);		/* read the received data */
				
				NetRxBuffer[len]=0;  //防止多打印
				
				
				extern struct rt_mailbox mbNetRecData;
				rt_mb_send_wait(&mbNetRecData, (rt_ubase_t)&NetRxBuffer,RT_WAITING_FOREVER);  
//				rt_kprintf("%s\r\n",data_buf);
//				rt_free(data_buf);
			}
			SOCK_DISCON(s);
			rt_kprintf("w5500 discon\r\n");
			extern rt_bool_t gbNetState;
			
			if(offLineTimesGet==RT_FALSE){//只获取一次
				  offLineTimesGet =RT_TRUE;
					gbNetState = RT_FALSE;
					offLine.times++;
					offLine.relayTimer[offLine.times]=rt_tick_get();
			}
			rt_kprintf("%sw5500 discon\r\n",sign);
			break;
		case Sn_IR_RECV: 
			IINCHIP_WRITE(IMR, 0x00);//开中断屏蔽IP,UNREACH,PPPoE,Magic Packet
			IINCHIP_WRITE(SIMR, 0x00);//开socket中断屏蔽  
			IINCHIP_ISR_DISABLE();
			tmp = I_STATUS[s];
			I_STATUS[s] &= ~(0x04); // RECV
			IINCHIP_ISR_ENABLE();
 
			IINCHIP_WRITE(IMR, 0xF0);
			IINCHIP_WRITE(SIMR, 0xFF);
			if (tmp & Sn_IR_RECV)
			{
				if((len = getSn_RX_RSR(s)) > 0)		/* check Rx data */
				{
					if (len > TX_RX_MAX_BUF_SIZE) 
					len = TX_RX_MAX_BUF_SIZE;	/* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
				

					len = recv(s, NetRxBuffer, len); /* read the received data */
					NetRxBuffer[len]=0;  //防止多打印
					extern struct rt_mailbox mbNetRecData;
					rt_mb_send_wait(&mbNetRecData, (rt_ubase_t)&NetRxBuffer,RT_WAITING_FOREVER);  
					//send(s,data_buf,len);//
//					rt_thread_mdelay(500); //延时后边再打印
					rt_kprintf("%snet rec len:%d \r\n",sign,len);
//					rt_kprintf("[");
//					for(int i=0;i<len;i++)
//					   rt_kprintf("%02x",NetRxBuffer[i]);
//					rt_kprintf("]\r\n");

					tmp = I_STATUS[s];
				}
			 }
			break;
		case Sn_IR_SEND_OK: 
			// send ok
			I_STATUS[s] &= ~(0x10);
		  //rt_kprintf("w5500 send ok\r\n"); //发送接口 send(s,data_buf,len);
			break;
		default:
			I_STATUS[s] =0;
		  ch_status[s] = 0;
		  break;
	}
	//if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) == 0x00) EXTI_GenerateSWInterrupt(EXTI_Line6);
}
#if 0
void loopback_udp(SOCKET s, uint16 port)
{
	uint16 len;
	uint8 * data_buf = (u8*) TX_BUF;
	uint32_t destip = 0;
	uint16 destport;
  uint8 tmp;
	
	switch (I_STATUS[s])
	{
		case SOCK_CLOSED:                                               /* if a socket is closed */
			//printf("\r\n%d : Loop-Back UDP Started. port :%d", s, port);
			if(socket(s,Sn_MR_UDP,port,0x00)== 0)    /* reinitialize the socket */
				rt_kprintf("\a%d : Fail to create socket.",s);
			break;
		case Sn_IR_SEND_OK: 
			// send ok
			I_STATUS[s] &= ~(0x10);
			break;
		case Sn_IR_RECV: 			
			IINCHIP_WRITE(IMR, 0x00);//开中断屏蔽IP,UNREACH,PPPoE,Magic Packet
			IINCHIP_WRITE(SIMR, 0x00);//开socket中断屏蔽  
			IINCHIP_ISR_DISABLE();
			tmp = I_STATUS[s];
			I_STATUS[s] &= ~(0x04); // RECV
			IINCHIP_ISR_ENABLE();
 
			IINCHIP_WRITE(IMR, 0xF0);
			IINCHIP_WRITE(SIMR, 0xFF);
			if (tmp & Sn_IR_RECV)
			{
				if((len = getSn_RX_RSR(s)) > 0)		/* check Rx data */
				{
					if (len > TX_RX_MAX_BUF_SIZE) 
					len = TX_RX_MAX_BUF_SIZE;	/* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					len = recvfrom(s, data_buf, len,(uint8*)&destip,&destport);/* read the received data */
					data_buf[len]=0;  //防止多打印
					rt_kprintf("%s\r\n",data_buf);
					sendto(s, data_buf, len,(uint8*)&destip,destport) ;
					tmp = I_STATUS[s];
				}
			 }
			break;
	}
}
#endif



void w5500Init()
{

	  SOCK_DISCON(SOCK_TCPC);
		reset_w5500();											/*硬复位W5500*/
		set_w5500_mac();										/*配置MAC地址*/
		setRTR(2000);/*设置溢出时间值*/
		setRCR(3);/*设置最大重新发送次数*/
		socket_buf_init(txsize, rxsize);		/*初始化8个Socket的发送接收缓存大小*/
		IINCHIP_WRITE(Sn_MR(7), 0x20);//TCP模式下开启无延时ACK
		IINCHIP_WRITE(Sn_IMR(7), 0x0F);
		IINCHIP_WRITE(IMR, 0xF0);
		IINCHIP_WRITE(SIMR, 0xFE);
	 
	  extern void rstDhcp();
	  rstDhcp();

		rstCh_status();
}
