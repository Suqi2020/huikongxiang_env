#include  "flashsave.h"
#include "utility.h"
const static char sign[]="[flashSave]";
extern  uint8 mac[6];



modbusDevSaveStru  modbusDevSave[DEV_NUM]  __attribute__ ((aligned (4)));

packIpUartStru packFLash __attribute__ ((aligned (4)));


static void net(int argc, char *argv[])
{
	  if(ip_from != IP_FROM_DEFINE){
				printf("%sERR:use IP_FROM_DHCP\r\n",sign);
				return;
		}
		if(argc==1){
				goto ERR;
		}
		if(argc==2){
			if(0==rt_strcmp((char *)"save", argv[1])){
				rt_kprintf("%snet save OK\n",sign);
				stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFLash));
				stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFLash,sizeof(packFLash));
		  }
			else
				goto ERR;
		}
		int i;
		if(0==rt_strcmp((char *)"macaddr", argv[1])){
				if(argc!=3){
					goto ERR;
				}
				mac[5]=atoi16(argv[2],10);
				packFLash.netIpFlash.macaddr=mac[5];
				rt_kprintf("%sfor macaddr OK\n",sign);
		
		}
		else 	if(0==rt_strcmp((char *)"localIp", argv[1])){
				if(argc!=6){
					goto ERR;
				}
				packFLash.netIpFlash.localIp[0] =atoi16(argv[2],10);
				packFLash.netIpFlash.localIp[1] =atoi16(argv[3],10);
				packFLash.netIpFlash.localIp[2] =atoi16(argv[4],10);
				packFLash.netIpFlash.localIp[3] =atoi16(argv[5],10);

				rt_kprintf("%sfor localIp OK\n",sign);

		}
		else 	if(0==rt_strcmp((char *)"gateway", argv[1])){
				if(argc!=6){
					goto ERR;
				}
			  packFLash.netIpFlash.gateway[0] =atoi16(argv[2],10);
				packFLash.netIpFlash.gateway[1] =atoi16(argv[3],10);
				packFLash.netIpFlash.gateway[2] =atoi16(argv[4],10);
				packFLash.netIpFlash.gateway[3] =atoi16(argv[5],10); 
	
				rt_kprintf("%sfor gateway OK\n",sign);

		}
		else 	if(0==rt_strcmp((char *)"remoteIp", argv[1])){
				if(argc!=6){
					goto ERR;
				}
			  packFLash.netIpFlash.remoteIp[0] =atoi16(argv[2],10);
				packFLash.netIpFlash.remoteIp[1] =atoi16(argv[3],10);
				packFLash.netIpFlash.remoteIp[2] =atoi16(argv[4],10);
				packFLash.netIpFlash.remoteIp[3] =atoi16(argv[5],10);
		
				rt_kprintf("%sfor remoteIp OK\n",sign);
	
		}
		else 	if(0==rt_strcmp((char *)"remotePort", argv[1])){
				if(argc!=3){
					goto ERR;
				}
				packFLash.netIpFlash.remotePort=atoi32(argv[2],10);

		
				rt_kprintf("%sfor remotePort OK\n",sign);

		}

		
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[net macaddr 100]\n",sign);
		rt_kprintf("%s[net localIp 192 168 1 122]\n",sign);
		rt_kprintf("%s[net gateway 192 168 1 1]\n",sign);
		rt_kprintf("%s[net remoteIp 192 168 1 100]\n",sign);
		rt_kprintf("%s[net remotePort 8080]\n",sign);
		rt_kprintf("%s[net save]\n",sign);

}
MSH_CMD_EXPORT(net,ip port config);//FINSH_FUNCTION_EXPORT_CMD

static void uart(int argc, char *argv[])
{

		if(argc==1){
				goto ERR;
		}
		if(argc==2){
			if(0==rt_strcmp((char *)"save", argv[1])){
				rt_kprintf("%suart save OK\n",sign);
				stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFLash));
				stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFLash,sizeof(packFLash));
		  }
			else
				goto ERR;
		}
		if(0==rt_strcmp((char *)"port1", argv[1])){
				if(argc!=3){
					goto ERR;
				}
				packFLash.port[0].bps     =atoi32(argv[2],10);
				packFLash.port[0].calTime =atoi32(argv[3],10);
				rt_kprintf("%sport1 config\n",sign);
		}
		else if(0==rt_strcmp((char *)"port2", argv[1])){
				if(argc!=3){
					goto ERR;
				}
				packFLash.port[1].bps     =atoi32(argv[2],10);
				packFLash.port[1].calTime =atoi32(argv[3],10);
				rt_kprintf("%sport2 config\n",sign);
		}
		else if(0==rt_strcmp((char *)"port3", argv[1])){
				if(argc!=3){
					goto ERR;
				}
				packFLash.port[2].bps     =atoi32(argv[2],10);
				packFLash.port[2].calTime =atoi32(argv[3],10);
				rt_kprintf("%sport3 config\n",sign);
		}
		else if(0==rt_strcmp((char *)"port4", argv[1])){
				if(argc!=3){
					goto ERR;
				}
				packFLash.port[3].bps     =atoi32(argv[2],10);
				packFLash.port[3].calTime =atoi32(argv[3],10);
				rt_kprintf("%sport4 config\n",sign);
		}
		
		return;
		ERR:
		rt_kprintf("%s[uart 端口(1-4) 波特率 串口采集时间]\n",sign);
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[uart port1 9600 120]\n",sign);

		rt_kprintf("%s[uart save]\n",sign);

}
MSH_CMD_EXPORT(uart,uart config);//FINSH_FUNCTION_EXPORT_CMD



void printfWorkModbus()
{
		rt_kprintf("%smodbus 启动《名称   类型   设备ID 	 端口      设备地址   寄存器命令   寄存器地址 寄存器长度》\n",sign);
	  for(int i=0;i<DEV_NUM;i++){
				if(modbusCheckWork(modbusDevSave[i].port)==RT_TRUE){
						rt_kprintf("%s ",modbusDevSave[i].name);
						rt_kprintf("%s ",modbusDevSave[i].model);
						rt_kprintf("%s ",modbusDevSave[i].devID);
						rt_kprintf("%d ",modbusDevSave[i].port);
						rt_kprintf("%d ",modbusDevSave[i].devAddr);
						rt_kprintf("%d ",sign,modbusDevSave[i].regCmd);
						rt_kprintf("%d ",sign,modbusDevSave[i].regAddr);
						rt_kprintf("%d ",sign,modbusDevSave[i].regLen);			
						rt_kprintf("\n");
				}
		}
}

//使用前提条件必须启用upKeepStateTask中参数以及回调函数初始化才能使用
static void modbus(int argc, char *argv[])
{
	  int i;
		if (argc != 9)
		{
			  if(argc==2){
						if(0==rt_strcmp((char *)"save", argv[1])){
							rt_kprintf("%smodbus save OK\n",sign);
							stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFLash));
							stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFLash,sizeof(packFLash));
						}
				}
				else{
						rt_kprintf("%sERR input argc\n",sign);
						goto ERR;
				}
		}
	  for(i=0;i<DEV_NUM;i++){
				if(0==rt_strcmp((char *)modbusDevSave[i].devID, argv[3])){//找到了设置过的设备 基于唯一ID
					  modbusDevSave[i].port = atoi16(argv[4],10); 
						if(modbusCheckWork(modbusDevSave[i].port)==RT_TRUE){//0xff 停用  1-4启动
								rt_strcpy(modbusDevSave[i].name,argv[1]);
							  rt_strcpy(modbusDevSave[i].model,argv[2]);
			
							  modbusDevSave[i].regAddr=atoi16(argv[5],10); 
							  modbusDevSave[i].regCmd =atoi16(argv[6],10); 
							  modbusDevSave[i].regAddr=atoi16(argv[7],10);
								modbusDevSave[i].regLen =atoi16(argv[8],10); 				
								rt_kprintf("%s%s on\n",sign,modbusDevSave[i].devID);
						}
						else{
							  rt_kprintf("%s%s off\n",sign,modbusDevSave[i].devID);
						}
						return;
				}
		}
	  for(i=0;i<DEV_NUM;i++){
					  modbusDevSave[i].port=  atoi16(argv[4],10); 
						if(modbusCheckWork(modbusDevSave[i].port)==RT_FALSE){//0xff 停用  1-4启动
								rt_strcpy(modbusDevSave[i].name,argv[1]);
							  rt_strcpy(modbusDevSave[i].model,argv[2]);
			
							  modbusDevSave[i].regAddr=atoi16(argv[5],10); 
							  modbusDevSave[i].regCmd =atoi16(argv[6],10); 
							  modbusDevSave[i].regAddr=atoi16(argv[7],10);
								modbusDevSave[i].regLen =atoi16(argv[8],10); 				
								rt_kprintf("%s%s on\n",sign,modbusDevSave[i].devID);
						}
						else{
							  rt_kprintf("%s%s off\n",sign,modbusDevSave[i].devID);
						}
		}
	
		return;//正确跳出
		ERR:
		rt_kprintf("%sfor example:《modbus  局放   GY280   devid     port      devaddr    regcmd        egaddr    reglen》\
									modbus   名称   类型   设备ID 	 端口      设备地址   寄存器命令   寄存器地址 寄存器长度\
									modbus  局放   GY280  GYNJLXSD000000458  3    1        3    	 2     	2\n",sign);
		rt_kprintf("%smodbus save\n",sign);
		rt_kprintf("%sport=0xff  close modbus device\n",sign);

}
//FINSH_FUNCTION_EXPORT(modbus, offline finsh);//FINSH_FUNCTION_EXPORT_CMD
MSH_CMD_EXPORT(modbus,port slaveaddr config);//FINSH_FUNCTION_EXPORT_CMD


//1-4启用  其它关闭
rt_bool_t  modbusCheckWork(uint8_t port)
{
		if((port==1)||(port==2)||(port==3)||(port==4)){
			 return RT_TRUE;
		}
		return RT_FALSE;
}



extern  void modbusCommRead(uartEnum uartNum,uint8_t cmd,uint16_t slavAddr,uint16_t regAddr,uint16_t reglen);
void  modbusReadData(int count)
{
	  for(int i=0;i<UART_NUM;i++){
			if(count%(packFLash.port[i].calTime)==0){
					rt_kprintf("port%d time is up\n",sign);
				  for(int j=0;j<DEV_NUM;j++){
						 if(modbusDevSave[i].port==(i+1)){
							 rt_kprintf("%s read\n",sign,modbusDevSave[j].devID);
							 modbusCommRead(i,modbusDevSave[j].regCmd,modbusDevSave[j].devAddr,modbusDevSave[j].regAddr,modbusDevSave[j].regLen);
						 }
					}
			}
		}
}

