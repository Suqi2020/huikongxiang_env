#include  "flashsave.h"
#include "utility.h"
const static char sign[]="[flashSave]";
extern  uint8 mac[6];
rt_bool_t errNumFlag[DEV_NUM]={0};
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
				stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFLash));//每次擦除128k字节数据 存储时候需要一起存储
				stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFLash,sizeof(packFLash));
				stm32_flash_write(FLASH_MODBUS_SAVE_ADDR,(uint8_t*)modbusDevSave,sizeof(modbusDevSave));
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
static const uint8_t portStr[UART_NUM][6]={"port1","port2","port3","port4"};
static void uart(int argc, char *argv[])
{
	  
		if(argc==1){
				goto ERR;
		}
		if(argc==2){
			if(0==rt_strcmp((char *)"save", argv[1])){
				rt_kprintf("%suart save OK\n",sign);
				stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFLash));//每次擦除128k字节数据 存储时候需要一起存储
				stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFLash,sizeof(packFLash));
				stm32_flash_write(FLASH_MODBUS_SAVE_ADDR,(uint8_t*)modbusDevSave,sizeof(modbusDevSave));
		  }
			else
				goto ERR;
		}
		for(int i=0;i<UART_NUM;i++){
				if(0==rt_strcmp((char *)portStr[i], argv[1])){
						if(argc!=5){
							goto ERR;
						}
						packFLash.port[i].bps     =atoi32(argv[2],10);
						packFLash.port[i].calTime =atoi32(argv[3],10);
						packFLash.port[i].delayTime=atoi32(argv[4],10);
						MX_USART2_UART_Init(packFLash.port[i].bps);
						rt_kprintf("%sport1 config\n",sign);
				}
		}
//		if(0==rt_strcmp((char *)"port1", argv[1])){
//				if(argc!=4){
//					goto ERR;
//				}
//				packFLash.port[0].bps     =atoi32(argv[2],10);
//				packFLash.port[0].calTime =atoi32(argv[3],10);
//				MX_USART2_UART_Init(packFLash.port[0].bps);
//				rt_kprintf("%sport1 config\n",sign);
//		}
//		else if(0==rt_strcmp((char *)"port2", argv[1])){
//				if(argc!=4){
//					goto ERR;
//				}
//				packFLash.port[1].bps     =atoi32(argv[2],10);
//				packFLash.port[1].calTime =atoi32(argv[3],10);
//				MX_USART3_UART_Init(packFLash.port[1].bps);
//				rt_kprintf("%sport2 config\n",sign);
//		}
//		else if(0==rt_strcmp((char *)"port3", argv[1])){
//				if(argc!=4){
//					goto ERR;
//				}
//				packFLash.port[2].bps     =atoi32(argv[2],10);
//				packFLash.port[2].calTime =atoi32(argv[3],10);
//				MX_USART6_UART_Init(packFLash.port[2].bps);
//				rt_kprintf("%sport3 config\n",sign);
//		}
//		else if(0==rt_strcmp((char *)"port4", argv[1])){
//				if(argc!=4){
//					goto ERR;
//				}
//				packFLash.port[3].bps     =atoi32(argv[2],10);
//				packFLash.port[3].calTime =atoi32(argv[3],10);
//				MX_UART4_Init(packFLash.port[3].bps);	
//				rt_kprintf("%sport4 config\n",sign);
//		}
		return;
		ERR:
		rt_kprintf("%s[uart 端口(1-4) 波特率 串口采集时间(s) 串口下modbus采集间隔(ms)]\n",sign);
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[uart port1 9600 120 1]\n",sign);

		rt_kprintf("%s[uart save]\n",sign);

}
MSH_CMD_EXPORT(uart,uart config);//FINSH_FUNCTION_EXPORT_CMD
//打印配置的modbus参数
static void printModbusDevList(int i)
{
		rt_kprintf("%-10s ",modbusDevSave[i].name);
		rt_kprintf("%s ",modbusDevSave[i].model);
		rt_kprintf("%s ",modbusDevSave[i].devID);
		rt_kprintf("[%x] ",modbusDevSave[i].port);
		rt_kprintf("[%x] ",modbusDevSave[i].devAddr);
		rt_kprintf("%x ",modbusDevSave[i].regCmd);
		rt_kprintf("%x ",modbusDevSave[i].regAddr);
		rt_kprintf("%x ",modbusDevSave[i].regLen);			
		rt_kprintf("\n");
}
//检查挂在同一个串口下边的modbus设备有同样的设备地址
void modbusDevErrCheck()
{
		for(int i=0;i<DEV_NUM;i++){
			  for(int j=i+1;j<DEV_NUM;j++){
						if(modbusCheckWork(modbusDevSave[i].port)==RT_TRUE){//port合法
								if(modbusCheckWork(modbusDevSave[j].port)==RT_TRUE){//port合法
										if(modbusDevSave[i].devAddr==modbusDevSave[j].devAddr){
												if(modbusDevSave[i].port==modbusDevSave[j].port){
													rt_kprintf("%sERR:modbus device config err\n",sign);
													printModbusDevList(i);
													printModbusDevList(j);
												}
										}
										if(modbusDevSave[i].devID==modbusDevSave[j].devID){//devid的唯一性
												rt_kprintf("%sERR:modbus have the same devid\n",sign);
												printModbusDevList(i);
												printModbusDevList(j);
										}
								}
						}
				}
		}
}


void printfWorkModbus()
{
		rt_kprintf("%smodbus 启动\n<<名称 类型 设备ID 端口[hex] 设备地址[hex] 寄存器命令[hex] 寄存器地址[hex] 寄存器长度[hex]>>\n",sign);
	  for(int i=0;i<DEV_NUM;i++){
				if(modbusCheckWork(modbusDevSave[i].port)==RT_TRUE){
						printModbusDevList(i);
				}
		}
}
void printfNorespModbus(char *sign)
{
	   rt_bool_t noResp=RT_TRUE;
	   
		 for(int i=0;i<DEV_NUM;i++){
				if(modbusCheckWork(modbusDevSave[i].port)==RT_TRUE){
					  if(errNumFlag[i]==RT_TRUE){
							  if(noResp==RT_TRUE){
										noResp=RT_FALSE;
									  rt_kprintf("%smodbus设备不回应\n",sign);
								}
								printModbusDevList(i);
						}
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
							stm32_flash_erase(FLASH_MODBUS_SAVE_ADDR, sizeof(modbusDevSave));//每次擦除128k字节数据 存储时候需要一起存储
							stm32_flash_write(FLASH_MODBUS_SAVE_ADDR,(uint8_t*)modbusDevSave,sizeof(modbusDevSave));
							stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFLash,sizeof(packFLash));
							return;
						}
				}
				else{
						rt_kprintf("%sERR input argc\n",sign);
						goto ERR;
				}
		}
	  for(i=0;i<DEV_NUM;i++){
				if(0==rt_strcmp((char *)modbusDevSave[i].devID, argv[3])){//找到了设置过的设备 基于唯一ID
						rt_strcpy(modbusDevSave[i].name,argv[1]);
						rt_strcpy(modbusDevSave[i].model,argv[2]);
						modbusDevSave[i].port = (uint8_t)atoi16(argv[4],16); 
						modbusDevSave[i].devAddr=(uint8_t)atoi16(argv[5],16); 
						modbusDevSave[i].regCmd =atoi16(argv[6],16); 
						modbusDevSave[i].regAddr=atoi16(argv[7],16);
						modbusDevSave[i].regLen =atoi16(argv[8],16);
				  	if(modbusCheckWork(modbusDevSave[i].port)==RT_TRUE){//0xff 停用  1-4启动
								rt_kprintf("%s%s reon\n",sign,modbusDevSave[i].devID);
								rt_kprintf("test 3 NO[%d]\n",i);
						}
						else{
							  rt_kprintf("test 4\n");
							  rt_kprintf("%s%s off\n",sign,modbusDevSave[i].devID);
						}
						return;
				}
		}
	  for(i=0;i<DEV_NUM;i++){
				if(modbusCheckWork(modbusDevSave[i].port)==RT_FALSE){//0xff 停用  1-4启动
						rt_strcpy(modbusDevSave[i].name,argv[1]);
						rt_strcpy(modbusDevSave[i].model,argv[2]);
						rt_strcpy(modbusDevSave[i].devID,argv[3]);
						modbusDevSave[i].port =  (uint8_t)atoi16(argv[4],16); 
						modbusDevSave[i].devAddr=(uint8_t)atoi16(argv[5],16); 
						modbusDevSave[i].regCmd =atoi16(argv[6],16); 
						modbusDevSave[i].regAddr=atoi16(argv[7],16);
						modbusDevSave[i].regLen =atoi16(argv[8],16); 		
						rt_kprintf("test 1 NO[%d] %d %d %d %d\n",i,modbusDevSave[i].devAddr,modbusDevSave[i].regCmd,modbusDevSave[i].regAddr,modbusDevSave[i].regLen);							
						rt_kprintf("%s%s on\n",sign,modbusDevSave[i].devID);
						return;
				}
		}
		return;//正确跳出
		ERR:
		rt_kprintf("%sexample:《modbus  局放   GY280   devid     port[hex]    devaddr[hex]  regcmd[hex]   regaddr[hex]  reglen[hex]》\n",sign);
		rt_kprintf("                    modbus   名称   类型   设备ID 	 端口      设备地址   寄存器命令   寄存器地址 寄存器长度\n");
		rt_kprintf("                    modbus  局放   GY280  GYNJLXSD000000458  3      1        3    	   2       	2\n");
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



extern  rt_bool_t modbusCommRead(modbusDevSaveStru modbus,uint8_t *out);
//void  modbusReadData(int count)
//{
//	  for(int i=0;i<UART_NUM;i++){
//				if((count+i*5)%(packFLash.port[i].calTime)==0){//错开5秒 避免4路同时到达
//						rt_kprintf("%sport%d time is up\n",sign,i+1);
//					    uint8_t *readBuf=rt_malloc(200);
//					
//					
//						for(int j=0;j<DEV_NUM;j++){
//							 if(modbusDevSave[j].port==(i+1)){//存在合法的port 就打包头部 头部只打包一次
//								 rt_kprintf("%s read%s %s\n",sign,modbusDevSave[j].devID,modbusDevSave[j].name);
//								 if(RT_FALSE== modbusCommRead(modbusDevSave[j],readBuf))//执行到此处打包param和data
//										errNumFlag[j]=RT_TRUE;//modbus 不回应
//								 else{
//										errNumFlag[j]=RT_FALSE;
//								rt_kprintf("%scheck data:",sign);//
//								
//									 for(int k=0;k<modbusDevSave[j].regLen*2;k++)
//											rt_kprintf("%02x ",readBuf[k]);
//									 rt_kprintf("\n");
//								 }
//								 rt_thread_mdelay(packFLash.port[i].delayTime*100);// 局放GY-JF100-C01和别的设备挂载在同一个串口的情况下连续读取设备会导致局放读取失败 此处需要延时2秒
//							 
//							 }
//						}
//					rt_free(readBuf);
//					readBuf=RT_NULL;
//						
//						
//				}
//		}
//}

extern uint16_t uartModbusJsonPack(int num);
void  modbusReadData(int count)
{
	  for(int i=0;i<UART_NUM;i++){
				if((count+i*5)%(packFLash.port[i].calTime)==0){//错开5秒 避免4路同时到达
						rt_kprintf("%sport%d time is up\n",sign,i+1);
					  uartModbusJsonPack(i);
						
						
				}
		}
}



