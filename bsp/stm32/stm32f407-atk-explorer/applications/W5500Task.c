#include "W5500Task.h"
#include "board.h"

//w5500联网状态的维护 
const static char task[]="[w55task]";

rt_sem_t  w5500Iqr_semp = RT_NULL;//w5500有数据时候中断来临

/***************----- 网络参数变量定义 -----***************/
unsigned char Gateway_IP[4];//网关IP地址 
unsigned char Sub_Mask[4];	//子网掩码 
unsigned char Phy_Addr[6];	//物理地址(MAC) 
unsigned char IP_Addr[4];	//本机IP地址 

unsigned char S0_Port[2];	//端口0的端口号(5000) 
unsigned char S0_DIP[4];	//端口0目的IP地址 
unsigned char S0_DPort[2];	//端口0目的端口号(6000) 



/***************----- 端口的运行模式 -----***************/
unsigned char S0_Mode =3;	//端口0的运行模式,0:TCP服务器模式,1:TCP客户端模式,2:UDP(广播)模式
#define TCP_SERVER	0x00	//TCP服务器模式
#define TCP_CLIENT	0x01	//TCP客户端模式 

rt_bool_t  gbNetState =RT_FALSE;   //联网状态  false 断网  true联网




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
	static rt_err_t ret=0;
	W5500_enum W5500State=W5500InitEnum;
  static uint8_t dhcpTick=0;
	static uint8_t rstW5500Ct=0;
//	while(1){
//			rt_thread_mdelay(500);
//			  
//	}
  while(1) 														/*循环执行的函数*/ 
  {
		switch(W5500State)
		{
			case W5500InitEnum:
						w5500Init();
			      W5500State=W5500DHCPEnum;
			      rt_kprintf("%s init……\r\n",task);
				break;
			case W5500DHCPEnum:
				    if(ip_from==IP_FROM_DHCP){//IP_FROM_DHCP)
							if(RT_TRUE == do_dhcp()){                        /*DHCP测试程序*/
									W5500State=W5500NetOKEnum;
									break;
							}
						}
						else{
							  set_w5500_ip();
								rt_kprintf("%sW5500 服务器IP:%d.%d.%d.%d\r\n",task,remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
								rt_kprintf("%sW5500 监听端口:%d \r\n",task,remote_port);
								W5500State=W5500NetOKEnum;
							  rt_sem_release(w5500Iqr_semp);
								break;
						}
						if(dhcpTick++>=5){//1秒基准  200*5
								dhcpTick=0;
								dhcp_time++;//dhcp计时 模拟定时器
							  rstW5500Ct++;
						}  
						if(rstW5500Ct>=30){//没联网的话 每隔30秒去重新初始化w5500
							 rstW5500Ct=0;
							 W5500State=W5500InitEnum;
						}
						rt_thread_mdelay(200);//不要修改 
				break;
			case W5500NetOKEnum:
			      ret=rt_sem_take(w5500Iqr_semp,1000);//阻塞1秒 查询中断状态 等中断来//RT_WAITING_FOREVER
			      static int count=0;      
						if(ret==RT_EOK){
								W5500ISR();//w5500
							 // rt_kprintf("w5500 test\r\n");
								loopback_tcpc(SOCK_TCPC, local_port);//W5500内部自动维护网络连接 此处只读寄存器
						}

					  if(gbNetState ==RT_FALSE){//没联网  重新初始化
								if(count++>5){ //5秒或者连续中断5次 还没联网 重启
										W5500State=W5500InitEnum;
									  count=0;
								}
								
						}
				break;
		}
	}

}	
//封装外部调用发送 函数接口
void netSend(uint8_t *data,int len)
{
		//return send(SOCK_TCPC,	data,len);
	
	
		if(send(SOCK_TCPC,	data,len)==0){//启动个定时器来实现重发  2s内收不到回复
				gbNetState=RT_FALSE;//发送身边 重新联网
				offLine.times++;
				offLine.relayTimer[offLine.times]=rt_tick_get();
				rt_kprintf("%snet send fail\n",task);
		}
		else{
				rt_kprintf("%snet send succ\n",task);
		}	
}


	
