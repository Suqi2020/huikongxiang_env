#include "W5500Task.h"
#include "board.h"

//w5500����״̬��ά�� 


rt_sem_t  w5500Iqr_semp = RT_NULL;//w5500������ʱ���ж�����

/***************----- ��������������� -----***************/
unsigned char Gateway_IP[4];//����IP��ַ 
unsigned char Sub_Mask[4];	//�������� 
unsigned char Phy_Addr[6];	//�����ַ(MAC) 
unsigned char IP_Addr[4];	//����IP��ַ 

unsigned char S0_Port[2];	//�˿�0�Ķ˿ں�(5000) 
unsigned char S0_DIP[4];	//�˿�0Ŀ��IP��ַ 
unsigned char S0_DPort[2];	//�˿�0Ŀ�Ķ˿ں�(6000) 

unsigned char UDP_DIPR[4];	//UDP(�㲥)ģʽ,Ŀ������IP��ַ
unsigned char UDP_DPORT[2];	//UDP(�㲥)ģʽ,Ŀ�������˿ں�

/***************----- �˿ڵ�����ģʽ -----***************/
unsigned char S0_Mode =3;	//�˿�0������ģʽ,0:TCP������ģʽ,1:TCP�ͻ���ģʽ,2:UDP(�㲥)ģʽ
#define TCP_SERVER	0x00	//TCP������ģʽ
#define TCP_CLIENT	0x01	//TCP�ͻ���ģʽ 
#define UDP_MODE	0x02	//UDP(�㲥)ģʽ 

/***************----- �˿ڵ�����״̬ -----***************/
unsigned char S0_State =0;	//�˿�0״̬��¼,1:�˿���ɳ�ʼ��,2�˿��������(����������������) 
#define S_INIT		0x01	//�˿���ɳ�ʼ�� 
#define S_CONN		0x02	//�˿��������,���������������� 

/***************----- �˿��շ����ݵ�״̬ -----***************/
unsigned char S0_Data;		//�˿�0���պͷ������ݵ�״̬,1:�˿ڽ��յ�����,2:�˿ڷ���������� 
#define S_RECEIVE	 0x01	//�˿ڽ��յ�һ�����ݰ� 
#define S_TRANSMITOK 0x02	//�˿ڷ���һ�����ݰ���� 

/***************----- �˿����ݻ����� -----***************/
unsigned char Rx_Buffer[2048];	//�˿ڽ������ݻ����� 
unsigned char Tx_Buffer[2048];	//�˿ڷ������ݻ����� 

unsigned char W5500_Interrupt;	//W5500�жϱ�־(0:���ж�,1:���ж�)

/*******************************************************************************
* ������  : Load_Net_Parameters
* ����    : װ���������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢�����ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/
void Load_Net_Parameters(void)
{
	Gateway_IP[0] = 192;//�������ز���
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 1;
	Gateway_IP[3] = 1;

	Sub_Mask[0]=255;//������������
	Sub_Mask[1]=255;
	Sub_Mask[2]=255;
	Sub_Mask[3]=0;

	Phy_Addr[0]=0x0c;//���������ַ
	Phy_Addr[1]=0x29;
	Phy_Addr[2]=0xab;
	Phy_Addr[3]=0x7c;
	Phy_Addr[4]=0x00;
	Phy_Addr[5]=0x01;

	IP_Addr[0]=192;//���ر���IP��ַ
	IP_Addr[1]=168;
	IP_Addr[2]=1;
	IP_Addr[3]=199;

	S0_Port[0] = 0x13;//���ض˿�0�Ķ˿ں�5000 
	S0_Port[1] = 0x88;

	S0_DIP[0]=192;//���ض˿�0��Ŀ��IP��ַ
	S0_DIP[1]=168;
	S0_DIP[2]=1;
	S0_DIP[3]=190;
	
	S0_DPort[0] = 0x17;//���ض˿�0��Ŀ�Ķ˿ں�6000
	S0_DPort[1] = 0x70;

	S0_Mode=TCP_CLIENT;//���ض˿�0�Ĺ���ģʽ,TCP�ͻ���ģʽ
}

void  w5500Task(void *parameter)
{

		#if 0
  reset_w5500();											/*Ӳ��λW5500*/
	set_w5500_mac();										/*����MAC��ַ*/
	set_w5500_ip();											/*����IP��ַ*/
	
	socket_buf_init(txsize, rxsize);		/*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
	
  rt_kprintf(" ������ΪTCP������,��W5500��Ϊ TCP�ͻ���ȥ���� \r\n");
	rt_kprintf(" ������IP:%d.%d.%d.%d\r\n",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
	rt_kprintf(" �����˿�:%d \r\n",remote_port);
	rt_kprintf(" ���ӳɹ��󣬷������������ݸ�W5500��W5500�����ض�Ӧ���� \r\n");
	rt_kprintf(" Ӧ�ó���ִ���С���\r\n"); 
	
	while(1)                            /*ѭ��ִ�еĺ���*/ 
	{
		  do_tcp_client();                  /*TCP_Client ���ݻػ����Գ���*/ 
		  rt_thread_mdelay(500);
	}
	#else
	
//	ip_from=IP_FROM_DHCP;				///IP_FROM_DHCP
//	
//	reset_w5500();											/*Ӳ��λW5500*/
//	set_w5500_mac();										/*����MAC��ַ*/
//	
//	socket_buf_init(txsize, rxsize);		/*��ʼ��8��Socket�ķ��ͽ��ջ����С*/

//	dhcp_timer_init();																 /*��ʼ��DHCP��ʱ��*/
//	init_dhcp_client();				                       /*��ʼ��DHCP�ͻ���*/ 
//  rt_kprintf(" ��������ɳ�ʼ������\r\n");
//  rt_kprintf(" Ұ�������������ΪDHCP�ͻ��ˣ����Դ�DHCP��������ȡIP��ַ \r\n");
//	uint32_t test111=0;
//  while(1) 														/*ѭ��ִ�еĺ���*/ 
//  {
//    do_dhcp();                        /*DHCP���Գ���*/
//		if(test111++>=10000){
//			test111=0;
//			dhcp_time++;
//			rt_kprintf("test\n");
//		}

//		//dhcp_time++;
//  }


//////////////////////////////////////////////////////
ip_from=IP_FROM_DHCP;
  reset_w5500();											/*Ӳ��λW5500*/
  set_w5500_mac();										/*����MAC��ַ*/
		set_w5500_ip();											/*����IP��ַ*/
  socket_buf_init(txsize, rxsize);		/*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
	
  printf(" ��������ɳ�ʼ������\r\n");
  printf(" Ұ�������������ΪDHCP�ͻ��ˣ����Դ�DHCP��������ȡIP��ַ \r\n");
	uint32_t test111=0;
  while(1) 														/*ѭ��ִ�еĺ���*/ 
  {
    do_dhcp();                        /*DHCP���Գ���*/
		if(test111++>=100){
			test111=0;
			dhcp_time++;
			rt_kprintf("test\n");
		}  
		rt_thread_mdelay(100);
	}
	#endif

}	
