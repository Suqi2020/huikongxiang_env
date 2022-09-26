#include "W5500Task.h"
#include "board.h"

//w5500联网状态的维护 


rt_sem_t  w5500Iqr_semp = RT_NULL;//w5500有数据时候中断来临

/***************----- 网络参数变量定义 -----***************/
unsigned char Gateway_IP[4];//网关IP地址 
unsigned char Sub_Mask[4];	//子网掩码 
unsigned char Phy_Addr[6];	//物理地址(MAC) 
unsigned char IP_Addr[4];	//本机IP地址 

unsigned char S0_Port[2];	//端口0的端口号(5000) 
unsigned char S0_DIP[4];	//端口0目的IP地址 
unsigned char S0_DPort[2];	//端口0目的端口号(6000) 

unsigned char UDP_DIPR[4];	//UDP(广播)模式,目的主机IP地址
unsigned char UDP_DPORT[2];	//UDP(广播)模式,目的主机端口号

/***************----- 端口的运行模式 -----***************/
unsigned char S0_Mode =3;	//端口0的运行模式,0:TCP服务器模式,1:TCP客户端模式,2:UDP(广播)模式
#define TCP_SERVER	0x00	//TCP服务器模式
#define TCP_CLIENT	0x01	//TCP客户端模式 
#define UDP_MODE	0x02	//UDP(广播)模式 

/***************----- 端口的运行状态 -----***************/
unsigned char S0_State =0;	//端口0状态记录,1:端口完成初始化,2端口完成连接(可以正常传输数据) 
#define S_INIT		0x01	//端口完成初始化 
#define S_CONN		0x02	//端口完成连接,可以正常传输数据 

/***************----- 端口收发数据的状态 -----***************/
unsigned char S0_Data;		//端口0接收和发送数据的状态,1:端口接收到数据,2:端口发送数据完成 
#define S_RECEIVE	 0x01	//端口接收到一个数据包 
#define S_TRANSMITOK 0x02	//端口发送一个数据包完成 

/***************----- 端口数据缓冲区 -----***************/
unsigned char Rx_Buffer[2048];	//端口接收数据缓冲区 
unsigned char Tx_Buffer[2048];	//端口发送数据缓冲区 

unsigned char W5500_Interrupt;	//W5500中断标志(0:无中断,1:有中断)

/*******************************************************************************
* 函数名  : Load_Net_Parameters
* 描述    : 装载网络参数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
*******************************************************************************/
void Load_Net_Parameters(void)
{
	Gateway_IP[0] = 192;//加载网关参数
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 1;
	Gateway_IP[3] = 1;

	Sub_Mask[0]=255;//加载子网掩码
	Sub_Mask[1]=255;
	Sub_Mask[2]=255;
	Sub_Mask[3]=0;

	Phy_Addr[0]=0x0c;//加载物理地址
	Phy_Addr[1]=0x29;
	Phy_Addr[2]=0xab;
	Phy_Addr[3]=0x7c;
	Phy_Addr[4]=0x00;
	Phy_Addr[5]=0x01;

	IP_Addr[0]=192;//加载本机IP地址
	IP_Addr[1]=168;
	IP_Addr[2]=1;
	IP_Addr[3]=199;

	S0_Port[0] = 0x13;//加载端口0的端口号5000 
	S0_Port[1] = 0x88;

	S0_DIP[0]=192;//加载端口0的目的IP地址
	S0_DIP[1]=168;
	S0_DIP[2]=1;
	S0_DIP[3]=190;
	
	S0_DPort[0] = 0x17;//加载端口0的目的端口号6000
	S0_DPort[1] = 0x70;

	S0_Mode=TCP_CLIENT;//加载端口0的工作模式,TCP客户端模式
}

void  w5500Task(void *parameter)
{

		#if 0
  reset_w5500();											/*硬复位W5500*/
	set_w5500_mac();										/*配置MAC地址*/
	set_w5500_ip();											/*配置IP地址*/
	
	socket_buf_init(txsize, rxsize);		/*初始化8个Socket的发送接收缓存大小*/
	
  rt_kprintf(" 电脑作为TCP服务器,让W5500作为 TCP客户端去连接 \r\n");
	rt_kprintf(" 服务器IP:%d.%d.%d.%d\r\n",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
	rt_kprintf(" 监听端口:%d \r\n",remote_port);
	rt_kprintf(" 连接成功后，服务器发送数据给W5500，W5500将返回对应数据 \r\n");
	rt_kprintf(" 应用程序执行中……\r\n"); 
	
	while(1)                            /*循环执行的函数*/ 
	{
		  do_tcp_client();                  /*TCP_Client 数据回环测试程序*/ 
		  rt_thread_mdelay(500);
	}
	#else
	
//	ip_from=IP_FROM_DHCP;				///IP_FROM_DHCP
//	
//	reset_w5500();											/*硬复位W5500*/
//	set_w5500_mac();										/*配置MAC地址*/
//	
//	socket_buf_init(txsize, rxsize);		/*初始化8个Socket的发送接收缓存大小*/

//	dhcp_timer_init();																 /*初始化DHCP定时器*/
//	init_dhcp_client();				                       /*初始化DHCP客户端*/ 
//  rt_kprintf(" 网络已完成初始化……\r\n");
//  rt_kprintf(" 野火网络适配板作为DHCP客户端，尝试从DHCP服务器获取IP地址 \r\n");
//	uint32_t test111=0;
//  while(1) 														/*循环执行的函数*/ 
//  {
//    do_dhcp();                        /*DHCP测试程序*/
//		if(test111++>=10000){
//			test111=0;
//			dhcp_time++;
//			rt_kprintf("test\n");
//		}

//		//dhcp_time++;
//  }


//////////////////////////////////////////////////////
ip_from=IP_FROM_DHCP;
  reset_w5500();											/*硬复位W5500*/
  set_w5500_mac();										/*配置MAC地址*/
		set_w5500_ip();											/*配置IP地址*/
  socket_buf_init(txsize, rxsize);		/*初始化8个Socket的发送接收缓存大小*/
	
  printf(" 网络已完成初始化……\r\n");
  printf(" 野火网络适配板作为DHCP客户端，尝试从DHCP服务器获取IP地址 \r\n");
	uint32_t test111=0;
  while(1) 														/*循环执行的函数*/ 
  {
    do_dhcp();                        /*DHCP测试程序*/
		if(test111++>=100){
			test111=0;
			dhcp_time++;
			rt_kprintf("test\n");
		}  
		rt_thread_mdelay(100);
	}
	#endif

}	
