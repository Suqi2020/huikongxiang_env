/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief  		配置MCU，移植W5500程序需要修改的文件，配置W5500的MAC和IP地址
**************************************************************************************************
*/
#include <stdio.h> 
#include <string.h>

#include "w5500_conf.h"
#include "utility.h"
#include "w5500.h"
#include "dhcp.h"
#include "board.h"
/*IP配置方法选择，请自行选择*/
const uint8	ip_from = IP_FROM_DEFINE;//IP_FROM_DEFINE;//IP_FROM_DEFINE;				///IP_FROM_DHCP


const static char sign[]="[w55Conf]";
extern SPI_HandleTypeDef hspi1;
CONFIG_MSG  		ConfigMsg;																	/*配置结构体*/
EEPROM_MSG_STR  EEPROM_MSG;															/*EEPROM存储信息结构体*/

/*定义MAC地址,如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
#if 1
/*定义默认IP信息*/
//uint8 local_ip[4]  ={192,168,16,88};											/*定义W5500默认IP地址*/
uint8 subnet[4]    ={255,255,255,0};										/*定义W5500默认子网掩码*/
//uint8 gateway[4]   ={192,168,16,1};											/*定义W5500默认网关*/
uint8 dns_server[4]={114,114,114,114};									/*定义W5500默认DNS*/
uint16 local_port=5000;	                       					/*定义本地端口*/

/*定义远端IP信息*/
//uint8  remote_ip[4]={192,168,1,109};											/*远端IP地址*/
//uint16 remote_port=8080;																/*远端端口号*/
//uint8  remote_ip[4]={192,168,16,6};											/*远端IP地址*/

#else

//uint8 local_ip[4]  ={192,168,1,88};											/*定义W5500默认IP地址*/
uint8 subnet[4]    ={255,255,255,0};										/*定义W5500默认子网掩码*/
//uint8 gateway[4]   ={192,168,1,1};											/*定义W5500默认网关*/
uint8 dns_server[4]={114,114,114,114};									/*定义W5500默认DNS*/

uint16 local_port=5000;	                       					/*定义本地端口*/

//uint16 remote_port=11001;																
//uint8  remote_ip[4]={192,168,1,102};											/*????ip*/
#endif



uint8   dhcp_ok   = 0;													   			/*dhcp成功获取IP*/
uint32	ms        = 0;															  	/*毫秒计数*/
uint32	dhcp_time = 0;															  	/*DHCP运行计数*/
volatile uint8_t	    ntptimer  = 0;															  	/*NPT秒计数*/

/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
void set_w5500_ip(void)
{	
	mac[5]=packFlash.netIpFlash.macaddr;
		
   /*复制定义的配置信息到配置结构体*/
	memcpy(ConfigMsg.mac, mac, 6);
	memcpy(ConfigMsg.lip,packFlash.netIpFlash.localIp,4);
	memcpy(ConfigMsg.sub,subnet,4);
	memcpy(ConfigMsg.gw,packFlash.netIpFlash.gateway,4);
	memcpy(ConfigMsg.dns,dns_server,4);
	if(ip_from==IP_FROM_DEFINE)	
		printf("%s使用定义的IP信息配置W5500\r\n",sign);
	
	

	/*使用DHCP获取IP参数，需调用DHCP子函数*/		
	if(ip_from==IP_FROM_DHCP)								
	{
		/*复制DHCP获取的配置信息到配置结构体*/
		if(dhcp_ok==1)
		{
			printf("%sIP from DHCP\r\n",sign);	 
			memcpy(ConfigMsg.lip,DHCP_GET.lip, 4);
			memcpy(ConfigMsg.sub,DHCP_GET.sub, 4);
			memcpy(ConfigMsg.gw,DHCP_GET.gw, 4);
			memcpy(ConfigMsg.dns,DHCP_GET.dns,4);
		}
		else
		{
			printf("%sDHCP子程序未运行,或者不成功\r\n",sign);
			printf("%s使用定义的IP信息配置W5500\r\n",sign);
		}
	}
		
	/*以下配置信息，根据需要选用*/	
	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*将IP配置信息写入W5500相应寄存器*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	getSIPR (packFlash.netIpFlash.localIp);			
	printf("%sW5500 IP地址   : %d.%d.%d.%d\r\n", sign,packFlash.netIpFlash.localIp[0],packFlash.netIpFlash.localIp[1],\
												packFlash.netIpFlash.localIp[2],packFlash.netIpFlash.localIp[3]);
	getSUBR(subnet);
	printf("%sW5500 子网掩码 : %d.%d.%d.%d\r\n",sign, subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(packFlash.netIpFlash.gateway);
	printf("%sW5500 网关     : %d.%d.%d.%d\r\n",sign, packFlash.netIpFlash.gateway[0],packFlash.netIpFlash.gateway[1],\
													packFlash.netIpFlash.gateway[2],packFlash.netIpFlash.gateway[3]);
	printf("%sW5500 物理地址 : %02X %02X %02X %02X %02X %02X\r\n",sign, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}

/**
*@brief		配置W5500的MAC地址
*@param		无
*@return	无
*/
void set_w5500_mac(void)
{
	memcpy(ConfigMsg.mac, mac, 6);
	setSHAR(ConfigMsg.mac);	/**/
	memcpy(DHCP_GET.mac, mac, 6);
}

/**
*@brief		W5500片选信号设置函数
*@param		val: 为“0”表示片选端口为低，为“1”表示片选端口为高
*@return	无
*/
void wiz_cs(uint8_t val)
{
	if (val == LOW) 
	{
	  //GPIO_ResetBits(WIZ_SPIx_SCS_PORT, WIZ_SPIx_SCS); 		
		HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_RESET);
	}
	else if (val == HIGH)
	{

		HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_SET);
	}
}

/**
*@brief		设置W5500的片选端口SCSn为低
*@param		无
*@return	无
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		设置W5500的片选端口SCSn为高
*@param		无
*@return	无
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}

/**
*@brief		W5500复位设置函数
*@param		无
*@return	无
*/
void reset_w5500(void)
{
	HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
	rt_thread_mdelay(200);
  HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET);	
	rt_thread_mdelay(200);
	HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
	rt_thread_mdelay(1600);
}


uint8_t SPI_SendByte(uint8_t byte)
{
 uint8_t rxbyte=0;
 uint8_t SPITimeout =200;
 while(__HAL_SPI_GET_FLAG(&hspi1,SPI_FLAG_TXE)==RESET)
 {
  if((SPITimeout--)==0){
		HAL_SPI_ErrorCallback(&hspi1);
		return 0;
	}
 }
 HAL_SPI_TransmitReceive(&hspi1,&byte,&rxbyte,1,100);
 return rxbyte;
}

/**
*@brief		STM32 SPI1读写8位数据
*@param		dat：写入的8位数据
*@return	无
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI_SendByte(dat));
}



/**
*@brief		写入一个8位数据到W5500
*@param		addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return	无
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
   iinchip_csoff();                              		
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
   IINCHIP_SpiSendData(data);                   
   iinchip_cson();                            
}

/**
*@brief		从W5500读出一个8位数据
*@param		addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return	无
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
   uint8 data = 0;
   iinchip_csoff();                            
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
   data = IINCHIP_SpiSendData(0x00);            
   iinchip_cson();                               
   return data;    
}

/**
*@brief		向W5500写入len字节数据
*@param		addrbsb: 写入数据的地址
*@param   buf：写入字符串
*@param   len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
   uint16 idx = 0;
   if(len == 0) printf("%sUnexpected2 length 0\r\n",sign);
   iinchip_csoff();                               
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
   for(idx = 0; idx < len; idx++)
   {
     IINCHIP_SpiSendData(buf[idx]);
   }
   iinchip_cson();                           
   return len;  
}

/**
*@brief		从W5500读出len字节数据
*@param		addrbsb: 读取数据的地址
*@param 	buf：存放读取数据
*@param		len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint16 idx = 0;
  if(len == 0)
  {
    printf("%sUnexpected2 length 0\r\n",sign);
  }
  iinchip_csoff();                                
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
  for(idx = 0; idx < len; idx++)                   
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();                                  
  return len;
}



//netIpFlashStru netIpFlash  __attribute__ ((aligned (4)));;

static void net(int argc, char *argv[])
{
	  if(ip_from != IP_FROM_DEFINE){
				printf("%sERR:use IP_FROM_DHCP\r\n",sign);
				return;
		}
		if(argc==1){
				goto ERR;
		}
//		int i;
		if(0==rt_strcmp((char *)"macaddr", argv[1])){
				if(argc!=3){
					goto ERR;
				}
				mac[5]=atoi16(argv[2],10);
				packFlash.netIpFlash.macaddr=mac[5];
				rt_kprintf("%smacaddr OK\n",sign);

		}
		else 	if(0==rt_strcmp((char *)"localIp", argv[1])){
				if(argc!=6){
					goto ERR;
				}
				packFlash.netIpFlash.localIp[0] =atoi16(argv[2],10);
				packFlash.netIpFlash.localIp[1] =atoi16(argv[3],10);
				packFlash.netIpFlash.localIp[2] =atoi16(argv[4],10);
				packFlash.netIpFlash.localIp[3] =atoi16(argv[5],10);

				rt_kprintf("%slocalIp OK\n",sign);
		}
		else 	if(0==rt_strcmp((char *)"gateway", argv[1])){
				if(argc!=6){
					goto ERR;
				}
			  packFlash.netIpFlash.gateway[0] =atoi16(argv[2],10);
				packFlash.netIpFlash.gateway[1] =atoi16(argv[3],10);
				packFlash.netIpFlash.gateway[2] =atoi16(argv[4],10);
				packFlash.netIpFlash.gateway[3] =atoi16(argv[5],10); 
	
				rt_kprintf("%sgateway OK\n",sign);

		}
		else 	if(0==rt_strcmp((char *)"remoteIp", argv[1])){
				if(argc!=6){
					goto ERR;
				}
			  packFlash.netIpFlash.remoteIp[0] =atoi16(argv[2],10);
				packFlash.netIpFlash.remoteIp[1] =atoi16(argv[3],10);
				packFlash.netIpFlash.remoteIp[2] =atoi16(argv[4],10);
				packFlash.netIpFlash.remoteIp[3] =atoi16(argv[5],10);
		
				rt_kprintf("%sremoteIp OK\n",sign);

				//STMFLASH_Write(FLASH_IP_SAVE_ADDR,(uint32_t*)&netIpFlash,sizeof(netIpFlash));
		}
		else 	if(0==rt_strcmp((char *)"remotePort", argv[1])){
				if(argc!=3){
					goto ERR;
				}
				packFlash.netIpFlash.remotePort=atoi32(argv[2],10);

		
				rt_kprintf("%sremotePort OK\n",sign);
		}
		
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[net macaddr 100]\n",sign);
		rt_kprintf("%s[net localIp 192 168 1 122]\n",sign);
		rt_kprintf("%s[net gateway 192 168 1 1]\n",sign);
		rt_kprintf("%s[net remoteIp 192 168 1 100]\n",sign);
		rt_kprintf("%s[net remotePort 8080]\n",sign);

}
MSH_CMD_EXPORT(net,ip port config);//FINSH_FUNCTION_EXPORT_CMD

