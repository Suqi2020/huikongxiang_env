#include "board.h"


//#include "stmflash.h"

const static char sign[]="[flash]";////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//STM32内部FLASH读写 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
packIpUartStru packFLash   __attribute__ ((aligned (4)));
deviceFlashStru sheet      __attribute__ ((aligned (4)));
const static char     UartName[UART_NUM][6] ={"port1", "port2",  "port3",  "port4"};//重映射一个名称
const  uartEnum UartNum[UART_NUM]     ={USE_UART2,USE_UART3,USE_UART6,USE_UART4};//重映射一个名称

extern void uartSingConf(int num,int bps);

static void flash(int argc, char *argv[])
{
		if(argc==1){
				goto ERR;
		}
		if(argc==2){
			if(0==rt_strcmp((char *)"save", argv[1])){
				rt_kprintf("%sflash save OK\n",sign);
				stm32_flash_erase(FLASH_IP_SAVE_ADDR, sizeof(packFLash));//每次擦除128k字节数据 存储时候需要一起存储
				stm32_flash_write(FLASH_IP_SAVE_ADDR,(uint8_t*)&packFLash,sizeof(packFLash));
				stm32_flash_write(FLASH_MODBUS_SAVE_ADDR,(uint8_t*)&sheet,sizeof(sheet));
		  }
			else
				goto ERR;
		}
		return;
		ERR:
		//rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[flash save]\n",sign);
}
MSH_CMD_EXPORT(flash,flash save);//FINSH_FUNCTION_EXPORT_CMD



//modbus+设备名称(波特率)+ID+model+端口(port1-port4)+设备地址(0/255-关闭设备)+采集间隔(秒)
		
		
		
		
void printModbusDevList()
{
	  rt_kprintf("%s printf modbus device list\n",sign);
		for(int i=0;i<MODBUS_NUM;i++){
			switch(i)
			{
				case CIRCULA:
					for(int j=0;j<CIRCULA_485_NUM;j++){//核对有没有配置过
							if(sheet.cirCula[j].workFlag==RT_TRUE){
									rt_kprintf("%s modbus ",sign);
									rt_kprintf("%10s ",modbusName[i]);
								  rt_kprintf("(%d)",modbusBps[i]);
									rt_kprintf("%s ",sheet.cirCula[j].ID);
									rt_kprintf("%s ",sheet.cirCula[j].model);
									rt_kprintf("%s ",UartName[sheet.cirCula[j].useUartNum]);
									rt_kprintf("%d ",sheet.cirCula[j].slaveAddr);
									rt_kprintf("%d \n",sheet.cirCulaColTime);
							}
					}
				break;
				case PARTDISCHAG:
					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//核对有没有配置过
							if(sheet.partDischag[j].workFlag==RT_TRUE){
									rt_kprintf("%s modbus ",sign);
									rt_kprintf("%10s ",modbusName[i]);
									rt_kprintf("(%d)",modbusBps[i]);
									rt_kprintf("%s ",sheet.partDischag[j].ID);
									rt_kprintf("%s ",sheet.partDischag[j].model);
									rt_kprintf("%s ",UartName[sheet.partDischag[j].useUartNum]);
									rt_kprintf("%d ",sheet.partDischag[j].slaveAddr);
									rt_kprintf("%d \n",sheet.partDischagColTime);
							}
					}
				break;
				case PRESSSETTL:
					for(int j=0;j<PRESSSETTL_485_NUM;j++){//核对有没有配置过
							if(sheet.pressSetl[j].workFlag==RT_TRUE){
									rt_kprintf("%s modbus ",sign);
									rt_kprintf("%10s ",modbusName[i]);
								  rt_kprintf("(%d)",modbusBps[i]);
									rt_kprintf("%s ",sheet.pressSetl[j].ID);
									rt_kprintf("%s ",sheet.pressSetl[j].model);
									rt_kprintf("%s ",UartName[sheet.pressSetl[j].useUartNum]);
									rt_kprintf("%d ",sheet.pressSetl[j].slaveAddr);
									rt_kprintf("%d \n",sheet.pressSetlColTime);
							}
					}
				break;
				case THREEAXIS:
					for(int j=0;j<THREEAXIS_485_NUM;j++){//核对有没有配置过
							if(sheet.threeAxiss[j].workFlag==RT_TRUE){
									rt_kprintf("%s modbus ",sign);
									rt_kprintf("%10s ",modbusName[i]);
								  rt_kprintf("(%d)",modbusBps[i]);
									rt_kprintf("%s ",sheet.threeAxiss[j].ID);
									rt_kprintf("%s ",sheet.threeAxiss[j].model);
									rt_kprintf("%s ",UartName[sheet.threeAxiss[j].useUartNum]);
									rt_kprintf("%d ",sheet.threeAxiss[j].slaveAddr);
									rt_kprintf("%d \n",sheet.threeAxissColTime);
							}
					}
				break;
	#ifdef USE_4GAS 
				case CH4:
					for(int j=0;j<CH4_485_NUM;j++){//核对有没有配置过
							if(sheet.ch4[j].workFlag==RT_TRUE){
									rt_kprintf("%s modbus ",sign);
									rt_kprintf("%10s ",modbusName[i]);
								  rt_kprintf("(%d)",modbusBps[i]);
									rt_kprintf("%s ",sheet.ch4[j].ID);
									rt_kprintf("%s ",sheet.ch4[j].model);
									rt_kprintf("%s ",UartName[sheet.ch4[j].useUartNum]);
									rt_kprintf("%d ",sheet.ch4[j].slaveAddr);
									rt_kprintf("%d \n",sheet.gasColTime);//sheet.ch4ColTime);
							}
					}
				break;
				case O2:
					for(int j=0;j<O2_485_NUM;j++){//核对有没有配置过
							if(sheet.o2[j].workFlag==RT_TRUE){
									rt_kprintf("%s modbus ",sign);
									rt_kprintf("%10s ",modbusName[i]);
								  rt_kprintf("(%d)",modbusBps[i]);
									rt_kprintf("%s ",sheet.o2[j].ID);
									rt_kprintf("%s ",sheet.o2[j].model);
									rt_kprintf("%s ",UartName[sheet.o2[j].useUartNum]);
									rt_kprintf("%d ",sheet.o2[j].slaveAddr);
									rt_kprintf("%d \n",sheet.gasColTime);//sheet.o2ColTime);
							}
					}
				break;
				case H2S:
					for(int j=0;j<H2S_485_NUM;j++){//核对有没有配置过
							if(sheet.h2s[j].workFlag==RT_TRUE){
									rt_kprintf("%s modbus ",sign);
									rt_kprintf("%10s ",modbusName[i]);
								  rt_kprintf("(%d)",modbusBps[i]);
									rt_kprintf("%s ",sheet.h2s[j].ID);
									rt_kprintf("%s ",sheet.h2s[j].model);
									rt_kprintf("%s ",UartName[sheet.h2s[j].useUartNum]);
									rt_kprintf("%d ",sheet.h2s[j].slaveAddr);
									rt_kprintf("%d \n",sheet.gasColTime);//sheet.h2sColTime);
							}
					}
				break;
				case CO:
					for(int j=0;j<CO_485_NUM;j++){//核对有没有配置过
							if(sheet.co[j].workFlag==RT_TRUE){
									rt_kprintf("%s modbus ",sign);
									rt_kprintf("%10s ",modbusName[i]);
								  rt_kprintf("(%d)",modbusBps[i]);
									rt_kprintf("%s ",sheet.co[j].ID);
									rt_kprintf("%s ",sheet.co[j].model);
									rt_kprintf("%s ",UartName[sheet.co[j].useUartNum]);
									rt_kprintf("%d ",sheet.co[j].slaveAddr);
									rt_kprintf("%d \n",sheet.gasColTime);
							}
					}
				break;
	#endif
				case TEMPHUM:
					for(int j=0;j<TEMPHUM_485_NUM;j++){//核对有没有配置过
							if(sheet.tempHum[j].workFlag==RT_TRUE){
									rt_kprintf("%s modbus ",sign);
									rt_kprintf("%10s ",modbusName[i]);
								  rt_kprintf("(%d)",modbusBps[i]);
									rt_kprintf("%s ",sheet.tempHum[j].ID);
									rt_kprintf("%s ",sheet.tempHum[j].model);
									rt_kprintf("%s ",UartName[sheet.tempHum[j].useUartNum]);
									rt_kprintf("%d ",sheet.tempHum[j].slaveAddr);
									rt_kprintf("%d \n",sheet.tempHumColTime);
							}
					}
				break;
				case WATERDEPTH:
					for(int j=0;j<WATERDEPTH_485_NUM;j++){//核对有没有配置过
							if(sheet.waterDepth[j].workFlag==RT_TRUE){
									rt_kprintf("%s modbus ",sign);
									rt_kprintf("%10s",modbusName[i]);
								  rt_kprintf("(%d)",modbusBps[i]);
			
									rt_kprintf("%s ",sheet.waterDepth[j].ID);
									rt_kprintf("%s ",sheet.waterDepth[j].model);
									rt_kprintf("%s ",UartName[sheet.waterDepth[j].useUartNum]);
									rt_kprintf("%d ",sheet.waterDepth[j].slaveAddr);
									rt_kprintf("%d \n",sheet.waterDepthColTime);
							}
					}
				break;
				default:
				break;
			}
		}
}

static int circulaConf(int uartnum,char *argv[])
{
	int i=0;
	int ret=0;
	int slaveAddr=atoi32(argv[5],10);
	sheet.cirCulaColTime=atoi32(argv[6],10);
	for( i=0;i<CIRCULA_485_NUM;i++){//核对有没有配置过
			if(rt_strcmp(sheet.cirCula[i].ID,argv[2])==0){//配置过
					if((slaveAddr==0)||(slaveAddr==255)){//关闭
							sheet.cirCula[i].workFlag=RT_FALSE;//关闭
						  rt_kprintf("%s del circula\n",sign);
					}
					else{
							sheet.cirCula[i].workFlag=RT_TRUE;//打开
					}
					sheet.cirCula[i].slaveAddr=slaveAddr;	
					sheet.cirCula[i].useUartNum=UartNum[uartnum];
					rt_strcpy(sheet.cirCula[i].model,argv[3]);
					rt_kprintf("%s circula reconfig %d\n",sign,i);
					rt_kprintf("%s circula OK\n",sign);
					ret =1;
					break;
			}
	}
	if(i==CIRCULA_485_NUM){//没有配置过
			for(int j=0;j<CIRCULA_485_NUM;j++){
					if(sheet.cirCula[j].workFlag!=RT_TRUE){
							sheet.cirCula[j].workFlag=RT_TRUE;//打开
							sheet.cirCula[j].slaveAddr=slaveAddr;	
							sheet.cirCula[j].useUartNum=UartNum[uartnum];
							rt_strcpy(sheet.cirCula[j].model,argv[3]);
							rt_strcpy(sheet.cirCula[j].ID,argv[2]);
							rt_kprintf("%s circula config %d\n",sign,j);
						  rt_kprintf("%s circula OK\n",sign);
							ret =1;
							break;
					}
			}
	}
	return ret;
}

static int partDischagConf(int uartnum,char *argv[])
{
	int i=0;
	int ret=0;
	int slaveAddr=atoi32(argv[5],10);
	sheet.partDischagColTime=atoi32(argv[6],10);
	for( i=0;i<PARTDISCHAG_485_NUM;i++){//核对有没有配置过
			if(rt_strcmp(sheet.partDischag[i].ID,argv[2])==0){//配置过
					if((slaveAddr==0)||(slaveAddr==255)){//关闭
							sheet.partDischag[i].workFlag=RT_FALSE;//关闭
						  rt_kprintf("%s del partDischarge\n",sign);
					}
					else{
							sheet.partDischag[i].workFlag=RT_TRUE;//打开
					}
					sheet.partDischag[i].slaveAddr=slaveAddr;	
					sheet.partDischag[i].useUartNum=UartNum[uartnum];
					rt_strcpy(sheet.partDischag[i].model,argv[3]);
					rt_kprintf("%s partDischag reconfig %d\n",sign,i);
					rt_kprintf("%s partDischag OK\n",sign);
					ret =1;
					break;
			}
	}
	if(i==PARTDISCHAG_485_NUM){//没有配置过
			for(int j=0;j<PARTDISCHAG_485_NUM;j++){
					if(sheet.partDischag[j].workFlag!=RT_TRUE){
							sheet.partDischag[j].workFlag=RT_TRUE;//打开
							sheet.partDischag[j].slaveAddr=slaveAddr;	
							sheet.partDischag[j].useUartNum=UartNum[uartnum];
							rt_strcpy(sheet.partDischag[j].model,argv[3]);
							rt_strcpy(sheet.partDischag[j].ID,argv[2]);
							rt_kprintf("%s partDischag config %d\n",sign,j);
						  rt_kprintf("%s partDischag OK\n",sign);
							ret =1;
							break;
					}
			}
	}
	return ret;
}

static int pressSettlConf(int uartnum,char *argv[])
{
	int i=0;
	int ret=0;
	int slaveAddr=atoi32(argv[5],10);
	sheet.pressSetlColTime=atoi32(argv[6],10);
	for( i=0;i<PRESSSETTL_485_NUM;i++){//核对有没有配置过
			if(rt_strcmp(sheet.pressSetl[i].ID,argv[2])==0){//配置过
					if((slaveAddr==0)||(slaveAddr==255)){//关闭
							sheet.pressSetl[i].workFlag=RT_FALSE;//关闭
						  rt_kprintf("%s del pressSettle\n",sign);
					}
					else{
							sheet.pressSetl[i].workFlag=RT_TRUE;//打开
					}
					sheet.pressSetl[i].slaveAddr=slaveAddr;	
					sheet.pressSetl[i].useUartNum=UartNum[uartnum];
					rt_strcpy(sheet.pressSetl[i].model,argv[3]);
					rt_kprintf("%s pressSettl reconfig %d\n",sign,i);
					rt_kprintf("%s pressSettl OK\n",sign);
					ret =1;
					break;
			}
	}
	if(i==PRESSSETTL_485_NUM){//没有配置过
			for(int j=0;j<PRESSSETTL_485_NUM;j++){
					if(sheet.pressSetl[j].workFlag!=RT_TRUE){
							sheet.pressSetl[j].workFlag=RT_TRUE;//打开
							sheet.pressSetl[j].slaveAddr=slaveAddr;	
							sheet.pressSetl[j].useUartNum=UartNum[uartnum];
							rt_strcpy(sheet.pressSetl[j].model,argv[3]);
							rt_strcpy(sheet.pressSetl[j].ID,argv[2]);
							rt_kprintf("%s pressSettl config %d\n",sign,j);
						  rt_kprintf("%s pressSettl OK\n",sign);
							ret =1;
							break;
					}
			}
	}
	return ret;
}


static int threeAxisConf(int uartnum,char *argv[])
{
	int i=0;
	int ret=0;
	int slaveAddr=atoi32(argv[5],10);
	sheet.threeAxissColTime =atoi32(argv[6],10);
	for( i=0;i<THREEAXIS_485_NUM;i++){//核对有没有配置过
			if(rt_strcmp(sheet.threeAxiss[i].ID,argv[2])==0){//配置过
					if((slaveAddr==0)||(slaveAddr==255)){//关闭
							sheet.threeAxiss[i].workFlag=RT_FALSE;//关闭
						  rt_kprintf("%s del threeAxis\n",sign);
					}
					else{
							sheet.threeAxiss[i].workFlag=RT_TRUE;//打开
					}
					sheet.threeAxiss[i].slaveAddr=slaveAddr;	
					sheet.threeAxiss[i].useUartNum=UartNum[uartnum];
					rt_strcpy(sheet.threeAxiss[i].model,argv[3]);
					rt_kprintf("%s threeaxis reconfig %d\n",sign,i);
					rt_kprintf("%s threeaxis OK\n",sign);
					ret =1;
					break;
			}
	}
	if(i==THREEAXIS_485_NUM){//没有配置过
			for(int j=0;j<THREEAXIS_485_NUM;j++){
					if(sheet.threeAxiss[j].workFlag!=RT_TRUE){
							sheet.threeAxiss[j].workFlag=RT_TRUE;//打开
							sheet.threeAxiss[j].slaveAddr=slaveAddr;	
							sheet.threeAxiss[j].useUartNum=UartNum[uartnum];
							rt_strcpy(sheet.threeAxiss[j].model,argv[3]);
							rt_strcpy(sheet.threeAxiss[j].ID,argv[2]);
							rt_kprintf("%s threeaxis config %d\n",sign,j);
						  rt_kprintf("%s threeaxis OK\n",sign);
							ret =1;
							break;
					}
			}
	}
	return ret;
}
#ifdef USE_4GAS
static int ch4Conf(int uartnum,char *argv[])
{
	int i=0;
	int ret=0;
	int slaveAddr=atoi32(argv[5],10);
	sheet.gasColTime=atoi32(argv[6],10);
	for( i=0;i<CH4_485_NUM;i++){//核对有没有配置过
			if(rt_strcmp(sheet.ch4[i].ID,argv[2])==0){//配置过
					if((slaveAddr==0)||(slaveAddr==255)){//关闭
							sheet.ch4[i].workFlag=RT_FALSE;//关闭
							rt_kprintf("%s del ch4\n",sign);
					}
					else{
							sheet.ch4[i].workFlag=RT_TRUE;//打开
					}
					sheet.ch4[i].slaveAddr=slaveAddr;	
					sheet.ch4[i].useUartNum=UartNum[uartnum];
					rt_strcpy(sheet.ch4[i].model,argv[3]);
					rt_kprintf("%s ch4 reconfig %d\n",sign,i);
					rt_kprintf("%s ch4 OK\n",sign);
					ret =1;
					break;
			}
	}
	if(i==CH4_485_NUM){//没有配置过
			for(int j=0;j<CH4_485_NUM;j++){
					if(sheet.ch4[j].workFlag!=RT_TRUE){
							sheet.ch4[j].workFlag=RT_TRUE;//打开
							sheet.ch4[j].slaveAddr=slaveAddr;	
							sheet.ch4[j].useUartNum=UartNum[uartnum];
							rt_strcpy(sheet.ch4[j].model,argv[3]);
							rt_strcpy(sheet.ch4[j].ID,argv[2]);
							rt_kprintf("%s ch4 config %d\n",sign,j);
						  rt_kprintf("%s ch4 OK\n",sign);
							ret =1;
							break;
					}
			}
	}
	return ret;
}
static int coConf(int uartnum,char *argv[])
{
	int i=0;
	int ret=0;
	int slaveAddr=atoi32(argv[5],10);
	sheet.gasColTime=atoi32(argv[6],10);
	for( i=0;i<CO_485_NUM;i++){//核对有没有配置过
			if(rt_strcmp(sheet.co[i].ID,argv[2])==0){//配置过
					if((slaveAddr==0)||(slaveAddr==255)){//关闭
							sheet.co[i].workFlag=RT_FALSE;//关闭
						  rt_kprintf("%s del co\n",sign);
					}
					else{
							sheet.co[i].workFlag=RT_TRUE;//打开
					}
					sheet.co[i].slaveAddr=slaveAddr;	
					sheet.co[i].useUartNum=UartNum[uartnum];
					rt_strcpy(sheet.co[i].model,argv[3]);
					rt_kprintf("%s co reconfig %d\n",sign,i);
					rt_kprintf("%s co OK\n",sign);
					ret =1;
					break;
			}
	}
	if(i==CO_485_NUM){//没有配置过
			for(int j=0;j<CO_485_NUM;j++){
					if(sheet.co[j].workFlag!=RT_TRUE){
							sheet.co[j].workFlag=RT_TRUE;//打开
							sheet.co[j].slaveAddr=slaveAddr;	
							sheet.co[j].useUartNum=UartNum[uartnum];
							rt_strcpy(sheet.co[j].model,argv[3]);
							rt_strcpy(sheet.co[j].ID,argv[2]);
							rt_kprintf("%s co config %d\n",sign,j);
						  rt_kprintf("%s co OK\n",sign);
							ret =1;
							break;
					}
			}
	}
	return ret;
}
static int h2sConf(int uartnum,char *argv[])
{
	int i=0;
	int ret=0;
	int slaveAddr=atoi32(argv[5],10);
	sheet.gasColTime=atoi32(argv[6],10);
	for( i=0;i<H2S_485_NUM;i++){//核对有没有配置过
			if(rt_strcmp(sheet.h2s[i].ID,argv[2])==0){//配置过
					if((slaveAddr==0)||(slaveAddr==255)){//关闭
							sheet.h2s[i].workFlag=RT_FALSE;//关闭
						  rt_kprintf("%s del h2s\n",sign);
					}
					else{
							sheet.h2s[i].workFlag=RT_TRUE;//打开
					}
					sheet.h2s[i].slaveAddr=slaveAddr;	
					sheet.h2s[i].useUartNum=UartNum[uartnum];
					rt_strcpy(sheet.h2s[i].model,argv[3]);
					rt_kprintf("%s h2s reconfig %d\n",sign,i);
					rt_kprintf("%s h2s OK\n",sign);
					ret =1;
					break;
			}
	}
	if(i==H2S_485_NUM){//没有配置过
			for(int j=0;j<H2S_485_NUM;j++){
					if(sheet.h2s[j].workFlag!=RT_TRUE){
							sheet.h2s[j].workFlag=RT_TRUE;//打开
							sheet.h2s[j].slaveAddr=slaveAddr;	
							sheet.h2s[j].useUartNum=UartNum[uartnum];
							rt_strcpy(sheet.h2s[j].model,argv[3]);
							rt_strcpy(sheet.h2s[j].ID,argv[2]);
							rt_kprintf("%s h2s config %d\n",sign,j);
						  rt_kprintf("%s h2s OK\n",sign);
							ret =1;
							break;
					}
			}
	}
	return ret;
}
static int o2Conf(int uartnum,char *argv[])
{
	int i=0;
	int ret=0;
	int slaveAddr=atoi32(argv[5],10);
	sheet.cirCulaColTime=atoi32(argv[6],10);
	sheet.gasColTime=atoi32(argv[6],10);
	for( i=0;i<O2_485_NUM;i++){//核对有没有配置过
			if(rt_strcmp(sheet.o2[i].ID,argv[2])==0){//配置过
					if((slaveAddr==0)||(slaveAddr==255)){//关闭
							sheet.o2[i].workFlag=RT_FALSE;//关闭
						  rt_kprintf("%s del o2\n",sign);
					}
					else{
							sheet.o2[i].workFlag=RT_TRUE;//打开
					}
					sheet.o2[i].slaveAddr=slaveAddr;	
					sheet.o2[i].useUartNum=UartNum[uartnum];
					rt_strcpy(sheet.o2[i].model,argv[3]);
					rt_kprintf("%s o2 reconfig %d\n",sign,i);
					rt_kprintf("%s o2 OK\n",sign);
					ret =1;
					break;
			}
	}
	if(i==O2_485_NUM){//没有配置过
			for(int j=0;j<O2_485_NUM;j++){
					if(sheet.o2[j].workFlag!=RT_TRUE){
							sheet.o2[j].workFlag=RT_TRUE;//打开
							sheet.o2[j].slaveAddr=slaveAddr;	
							sheet.o2[j].useUartNum=UartNum[uartnum];
							rt_strcpy(sheet.o2[j].model,argv[3]);
							rt_strcpy(sheet.o2[j].ID,argv[2]);
							rt_kprintf("%s o2 config %d\n",sign,j);
						  rt_kprintf("%s o2 OK\n",sign);
							ret =1;
							break;
					}
			}
	}
	return ret;
}
#endif
static int waterDepthConf(int uartnum,char *argv[])
{
	int i=0;
	int ret=0;
	int slaveAddr=atoi32(argv[5],10);
	sheet.waterDepthColTime=atoi32(argv[6],10);
	for( i=0;i<WATERDEPTH_485_NUM;i++){//核对有没有配置过
			if(rt_strcmp(sheet.waterDepth[i].ID,argv[2])==0){//配置过
					if((slaveAddr==0)||(slaveAddr==255)){//关闭
							sheet.waterDepth[i].workFlag=RT_FALSE;//关闭
							rt_kprintf("%s del waterdepth\n",sign);
					}
					else{
							sheet.waterDepth[i].workFlag=RT_TRUE;//打开
					}
					sheet.waterDepth[i].slaveAddr=slaveAddr;	
					sheet.waterDepth[i].useUartNum=UartNum[uartnum];
					rt_strcpy(sheet.waterDepth[i].model,argv[3]);
					rt_kprintf("%s waterDepth reconfig %d\n",sign,i);
					rt_kprintf("%s waterDepth OK\n",sign);
					ret =1;
					break;
			}
	}
	if(i==WATERDEPTH_485_NUM){//没有配置过
			for(int j=0;j<WATERDEPTH_485_NUM;j++){
					if(sheet.waterDepth[j].workFlag!=RT_TRUE){
							sheet.waterDepth[j].workFlag=RT_TRUE;//打开
							sheet.waterDepth[j].slaveAddr=slaveAddr;	
							sheet.waterDepth[j].useUartNum=UartNum[uartnum];
							rt_strcpy(sheet.waterDepth[j].model,argv[3]);
							rt_strcpy(sheet.waterDepth[j].ID,argv[2]);
							rt_kprintf("%s waterDepth config %d\n",sign,j);
						  rt_kprintf("%s waterDepth OK\n",sign);
							ret =1;
							break;
					}
			}
	}
	return ret;
}
static int tempHumConf(int uartnum,char *argv[])
{
	int i=0;
	int ret=0;
	int slaveAddr=atoi32(argv[5],10);
	sheet.tempHumColTime=atoi32(argv[6],10);
	for( i=0;i<TEMPHUM_485_NUM;i++){//核对有没有配置过
			if(rt_strcmp(sheet.tempHum[i].ID,argv[2])==0){//配置过
					if((slaveAddr==0)||(slaveAddr==255)){//关闭
							sheet.tempHum[i].workFlag=RT_FALSE;//关闭
							rt_kprintf("%s del temp humi\n",sign);
					}
					else{
							sheet.tempHum[i].workFlag=RT_TRUE;//打开
					}
					sheet.tempHum[i].slaveAddr=slaveAddr;	
					sheet.tempHum[i].useUartNum=UartNum[uartnum];
					rt_strcpy(sheet.tempHum[i].model,argv[3]);
					rt_kprintf("%s tempHum reconfig %d\n",sign,i);
					rt_kprintf("%s tempHum OK\n",sign);
					ret =1;
					break;
			}
	}
	if(i==TEMPHUM_485_NUM){//没有配置过
			for(int j=0;j<TEMPHUM_485_NUM;j++){
					if(sheet.tempHum[j].workFlag!=RT_TRUE){
							sheet.tempHum[j].workFlag=RT_TRUE;//打开
							sheet.tempHum[j].slaveAddr=slaveAddr;	
							sheet.tempHum[j].useUartNum=UartNum[uartnum];
							rt_strcpy(sheet.tempHum[j].model,argv[3]);
							rt_strcpy(sheet.tempHum[j].ID,argv[2]);
							rt_kprintf("%s tempHum config %d\n",sign,j);
						  rt_kprintf("%s tempHum OK\n",sign);
							ret =1;
							break;
					}
			}
	}
	return ret;
}
static int modbusConf(int modbusnum,int uartnum,char *argv[])
{
	  int ret=0;
	  //int slaveAddr=atoi32(argv[5],10);//提取设备地址 前面已经核对过
		rt_kprintf("conf:%d %d\n",modbusnum,uartnum);
		switch(modbusnum)
		{
			case CIRCULA:
				ret =circulaConf(uartnum,argv);
			break;
			case PARTDISCHAG:
				ret=partDischagConf(uartnum,argv);
			break;
			case PRESSSETTL:
				ret=pressSettlConf(uartnum,argv);
			break;
			case THREEAXIS:
				ret=threeAxisConf(uartnum,argv);
			break;
#ifdef USE_4GAS
			case CH4:
				ret=ch4Conf(uartnum,argv);
			break;
			case O2:
				ret=o2Conf(uartnum,argv);
			break;
			case H2S:
				ret=h2sConf(uartnum,argv);
      break;
      case CO:
				ret=coConf(uartnum,argv);
      break;
#endif
      case TEMPHUM:
				ret=tempHumConf(uartnum,argv);
      break;
      case WATERDEPTH:
				ret=waterDepthConf(uartnum,argv);
			break;
			default:
			break;
		}
		return ret;
}
//配置前检查ID有没有重复的 有的话删除重复的ID
int modbusConfIDCheck(char *inputID)
{
		for(int i=0;i<MODBUS_NUM;i++){
			switch(i)
			{
				case CIRCULA:
					for(int j=0;j<CIRCULA_485_NUM;j++){//核对有没有配置过
							if(sheet.cirCula[j].workFlag==RT_TRUE){
									if(rt_strcmp(sheet.cirCula[j].ID,inputID)==0){
											rt_kprintf("del cirCula same ID\n");
											sheet.cirCula[j].workFlag=RT_FALSE;
										  return 1;
									}
							}
					}
				break;
				case PARTDISCHAG:
					for(int j=0;j<PARTDISCHAG_485_NUM;j++){//核对有没有配置过
							if(sheet.partDischag[j].workFlag==RT_TRUE){
									if(rt_strcmp(sheet.partDischag[j].ID,inputID)==0){
											rt_kprintf("del partDischag same ID\n");
											sheet.partDischag[j].workFlag=RT_FALSE;
										  return 1;
									}
							}
					}
				break;
				case PRESSSETTL:
					for(int j=0;j<PRESSSETTL_485_NUM;j++){//核对有没有配置过
							if(sheet.pressSetl[j].workFlag==RT_TRUE){
									if(rt_strcmp(sheet.pressSetl[j].ID,inputID)==0){
											rt_kprintf("del pressSetl same ID\n");
											sheet.pressSetl[j].workFlag=RT_FALSE;
										  return 1;
									}
							}
					}
				break;
				case THREEAXIS:
					for(int j=0;j<THREEAXIS_485_NUM;j++){//核对有没有配置过
							if(sheet.threeAxiss[j].workFlag==RT_TRUE){
									if(rt_strcmp(sheet.threeAxiss[j].ID,inputID)==0){
											rt_kprintf("del threeAxiss same ID\n");
											sheet.threeAxiss[j].workFlag=RT_FALSE;
										  return 1;
									}
							}
					}
				break;
	#ifdef 	 USE_4GAS 	
				case CH4:
//					for(int j=0;j<CH4_485_NUM;j++){//核对有没有配置过
//							if(sheet.ch4[j].workFlag==RT_TRUE){
//									if(rt_strcmp(sheet.ch4[j].ID,inputID)==0){
//											rt_kprintf("del ch4 same ID\n");
//											sheet.ch4[j].workFlag=RT_FALSE;
//										  return 1;
//									}
//								}
//					}
				break;
				case O2:
//					for(int j=0;j<O2_485_NUM;j++){//核对有没有配置过
//							if(sheet.o2[j].workFlag==RT_TRUE){
//									if(rt_strcmp(sheet.o2[j].ID,inputID)==0){
//											rt_kprintf("del o2 same ID\n");
//											sheet.o2[j].workFlag=RT_FALSE;
//										  return 1;
//									}
//							}
//					}
				break;
				case H2S:
//					for(int j=0;j<H2S_485_NUM;j++){//核对有没有配置过
//							if(sheet.h2s[j].workFlag==RT_TRUE){
//									if(rt_strcmp(sheet.h2s[j].ID,inputID)==0){
//											rt_kprintf("del h2s same ID\n");
//											sheet.h2s[j].workFlag=RT_FALSE;
//										  return 1;
//									}
//							}
//					}
				break;
				case CO:
//					for(int j=0;j<CO_485_NUM;j++){//核对有没有配置过
//							if(sheet.co[j].workFlag==RT_TRUE){
//									if(rt_strcmp(sheet.co[j].ID,inputID)==0){
//											rt_kprintf("del co same ID\n");
//											sheet.co[j].workFlag=RT_FALSE;
//										  return 1;
//									}
//							}
//					}
				break;
	#endif
				case TEMPHUM:
					for(int j=0;j<TEMPHUM_485_NUM;j++){//核对有没有配置过
							if(sheet.tempHum[j].workFlag==RT_TRUE){
									if(rt_strcmp(sheet.tempHum[j].ID,inputID)==0){
											rt_kprintf("del TEMPHUM same ID\n");
											sheet.tempHum[j].workFlag=RT_FALSE;
										  return 1;
									}
							}
					}
				break;
				case WATERDEPTH:
					for(int j=0;j<WATERDEPTH_485_NUM;j++){//核对有没有配置过
							if(sheet.waterDepth[j].workFlag==RT_TRUE){
									if(rt_strcmp(sheet.waterDepth[j].ID,inputID)==0){
											rt_kprintf("del waterDepth same ID\n");
											sheet.waterDepth[j].workFlag=RT_FALSE;
										  return 1;
									}
							}
					}
				break;
				default:
				break;
			}
		}
		return 0;
}
//相同种类的传感器必须挂到同一个port下边
//每种传感器最后一次输入的采集时间为最终采集时间
//modbus   局放 port2 2 60
//使用前提条件必须启用upKeepStateTask中参数以及回调函数初始化才能使用
//核对名称 核对地址 核对port才能写入
//modbus+设备名称(波特率)+ID+model+端口(port1-port4)+设备地址(0/255-关闭设备)+采集间隔(秒)
static void modbus(int argc, char *argv[])
{
	  extern void timeStop(upDataTimEnum num);
	  extern void timeInit(upDataTimEnum num,int value,int firstCnt);
	  int i,j;
		if ((argc != 7)&&(argc != 2))
		{
				rt_kprintf("%sERR input argc\n",sign);
				goto ERR;
		}
		if(argc==2){
					if(0==rt_strcmp((char *)"list", argv[1])){
							printModbusDevList();
						  return;
					}
		}
		else{
			uint8_t slaveaddr=atoi16(argv[5],10);
			int setTime =atoi16(argv[6],10);
			if(setTime<60){
					rt_kprintf("%serr:argv[6] 采集时间>60 now is%d\n",sign,setTime);
					goto ERR;
			}
			if(slaveaddr>255){
					rt_kprintf("%serr:argv[5] between 0 and 255 %d\n",sign,argv[5]);
					goto ERR;
			}
			for(i=0;i<MODBUS_NUM;i++){
					if(0==rt_strcmp((char *)modbusName[i], argv[1])){
							for(j=0;j<UART_NUM;j++){
									if(0==rt_strcmp((char *)UartName[j], argv[4])){//串口 合法
											break;
									}
							}
							break;
					}
			}
			if(i>=MODBUS_NUM){
					rt_kprintf("%serr:argv[1]\n",sign);
					for(int j=0;j<MODBUS_NUM;j++)
						rt_kprintf("%s ",modbusName[j]);
					rt_kprintf("\n ");
					goto ERR;
			}
			if(j>=UART_NUM){
					rt_kprintf("%serr:argv[2]\n",sign);
					for(int j=0;j<UART_NUM;j++)
						rt_kprintf("%s ",UartName[j]);
					rt_kprintf("\n ");
					goto ERR;
			}
			if(1==modbusConf(i,j,argv)){//配置modbus参数到表中
					return;//正确跳出
			}
			else{
					rt_kprintf("%smodbus exit delete first\n",sign);
			}
	
		}
		
		ERR:
		rt_kprintf("%sfor example:modbus+设备名称(波特率)+ID+model+端口(port1-port4)+设备地址(0/255-关闭设备)+采集间隔(秒)\n",sign);
		rt_kprintf("%sNOTE:括号内对参数进行解释,不需要输入\n",sign);
		rt_kprintf("%smodebus list\n",sign);
}
//FINSH_FUNCTION_EXPORT(modbus, offline finsh);//FINSH_FUNCTION_EXPORT_CMD
MSH_CMD_EXPORT(modbus,port slaveaddr config);//FINSH_FUNCTION_EXPORT_CMD



//static const uint8_t portStr[UART_NUM][6]={"port1","port2","port3","port4"};

//static void uart(int argc, char *argv[])
//{
//	  
//		if(argc!=3){
//				goto ERR;
//		}
//		rt_strcpy((char *)packFLash.acuId,argv[1]);
//		ERR:
//		rt_kprintf("%s[uart 端口(1-4) 波特率]\n",sign);
//		rt_kprintf("%sfor example\n",sign);
//		rt_kprintf("%s[uart port1 9600]\n",sign);

//}
//MSH_CMD_EXPORT(uart,uart config);//FINSH_FUNCTION_EXPORT_CMD
static const uint8_t portStr[UART_NUM][6]={"port1","port2","port3","port4"};
////////////////////串口配置/////////////////////////////
static void uart(int argc, char *argv[])
{
	  
		if(argc!=3){
				goto ERR;
		}

		for(int i=0;i<UART_NUM;i++){
				if(0==rt_strcmp((char *)portStr[i], argv[1])){
						packFLash.uartBps[i]    =atoi32(argv[2],10);
						uartSingConf(i,packFLash.uartBps[i]);
						rt_kprintf("%sport%d config OK\n",sign,i+1);
				}
		}
		return;
		ERR:
		rt_kprintf("%s[uart 端口(1-4) 波特率]\n",sign);
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[uart port1 9600]\n",sign);

}
MSH_CMD_EXPORT(uart,uart config);//FINSH_FUNCTION_EXPORT_CMD

////////////////////////ACUID配置///////////////////////////////////
static void acuid(int argc, char *argv[])
{
		if(argc!=2){
				goto ERR;
		}
		rt_strcpy(packFLash.acuId,argv[1]);
		rt_kprintf("%sacuid OK\n",sign);
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[acuid  100000000000001]\n",sign);

}
MSH_CMD_EXPORT(acuid,acuid config);//FINSH_FUNCTION_EXPORT_CMD



void prinfAnalogList()
{
		for(int j=0;j<ANALOG_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(sheet.analog[j].workFlag==RT_TRUE){//没打开
						rt_kprintf("%s analog ",sign);

						rt_kprintf("%s ",sheet.analog[j].name);
						rt_kprintf("%s ",sheet.analog[j].funName);
						
						rt_kprintf("%s ",sheet.analog[j].ID);
						rt_kprintf("%s ",sheet.analog[j].model);
						rt_kprintf("%s ",sheet.analog[j].port);
						rt_kprintf("采集间隔 %d\n",sheet.analog[j].colTime);

				}
		}
}
//analog  温湿度 temperature    GYNJLXSD000000499 GY280 port1  120  
//analog  温湿度 humidity       GYNJLXSD000000499 GY280 port2  120 
//analog  温湿度 humidity       GYNJLXSD000000499 GY280 port2  120 
//
//const static char  analogName[20]="温湿度";
//const static char  analogName1Val[2][20]={"temperature","humidity"};
const  static  char analogPort[ANALOG_NUM][10]={"port1","port2","port3","port4","port5","port6","port7","port8"};

int analogPortCheck(char *port)
{
	  
	  
		for(int i=0;i<ANALOG_NUM;i++){
			 // rt_kprintf("%s %s\n",analogPort[i],port);
			  if(rt_strcmp(analogPort[i],port)==0){
						return i+1;
				}
		}
	  return 0;
}
static void analog(int argc, char *argv[])
{
	  
		if(argc!=7){
				goto ERR;
		}
		if(rt_strcmp(argv[1],analogName)==0){
			 rt_kprintf("analog 1\n");
			 //rt_kprintf("%s %s\n",sign,analogName);
			 if((rt_strcmp(argv[2],analogName1Val[0])==0)||(rt_strcmp(argv[2],analogName1Val[1])==0)){
				  // rt_kprintf("%s %s %s\n",sign,analogName1Val[0],analogName1Val[1]);
	         int port = analogPortCheck(argv[5]);
				   if(0!=port){//打开
							 for(int j=0;j<ANALOG_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
									if(sheet.analog[j].workFlag!=RT_TRUE){//没打开
											sheet.analog[j].workFlag=RT_TRUE;
										  rt_strcpy(sheet.analog[j].name,   argv[1]);
										  rt_strcpy(sheet.analog[j].funName,argv[2]);
											rt_strcpy(sheet.analog[j].model,  argv[4]);
											rt_strcpy(sheet.analog[j].ID,     argv[3]);
										  sheet.analog[j].port=port;
								
										  sheet.analog[j].colTime  = atoi16(argv[6],10);
										  rt_kprintf("%s open chanl%d\n",sign,j+1);
										  return;
									}
							 }
					 }//关闭
					 else{
						 	 for(int j=0;j<ANALOG_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
									if(rt_strcmp(sheet.analog[j].ID,argv[3])==0){
											sheet.analog[j].workFlag=RT_FALSE;
										  rt_kprintf("%s close chanl%d\n",sign,j+1);
									}
									//rt_kprintf("%s %s %s\n",sign,sheet.analog[j].IDanalogName1Val[1]);
							 }
					 }
			 }
			 else{
					 rt_kprintf("%sargv[6]!=%s %s\n",sign,analogName1Val[0],analogName1Val[1]);
					 goto ERR;
			 }
		}
		else{
			  rt_kprintf("analog 2\n");
				rt_kprintf("%sargv[1]!=%s\n",sign,analogName);
				goto ERR;
		}

		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[analog  温湿度 temperature    GYNJLXSD000000499 GY280 port1  120]\n",sign);
		rt_kprintf("%s[port1-8 之外清除对应ID的所有参数]\n",sign);

}
MSH_CMD_EXPORT(analog,analog config);//FINSH_FUNCTION_EXPORT_CMD






void printfDIList()
{
		for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(packFLash.input[j].workFlag==RT_TRUE){//打开
						rt_kprintf("%s digInput ",sign);

						rt_kprintf("%s ",packFLash.input[j].name);
						rt_kprintf("%s ",packFLash.input[j].devID);
						
						rt_kprintf("%s ",packFLash.input[j].model);
						rt_kprintf("%d \n",packFLash.input[j].port);



				}
		}
}
void printfDOList()
{
		for(int j=0;j<DO_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
				if(packFLash.output[j].workFlag==RT_TRUE){//打开
						rt_kprintf("%s digOutput ",sign);
						rt_kprintf("%s ",packFLash.output[j].name);
						rt_kprintf("%s ",packFLash.output[j].devID);
						rt_kprintf("%s ",packFLash.output[j].model);
						rt_kprintf("%d \n",packFLash.output[j].port);
				}
		}
}
//digInput 水泵 GYNJLXSD000000162 GY281 1
static void digInput(int argc, char *argv[])
{
	 if(0==rt_strcmp((char *)"list", argv[1])){
				printfDIList();
				return;
		}
		if(argc!=5){
				goto ERR;
		}
	 int port = atoi16(argv[4],10);

	 if((port<8)&&(port>0)){//添加
		 	 for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFLash.input[j].devID,argv[2])==0){
							packFLash.input[j].workFlag=RT_TRUE;
							rt_strcpy(packFLash.input[j].name, argv[1]);
							rt_strcpy(packFLash.input[j].model,argv[3]);
              packFLash.input[j].port=port;
							rt_kprintf("%s readd diginput channel %d\n",sign,j+1);
						  return;
					}
			 }
			 for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(packFLash.input[j].workFlag!=RT_TRUE){//没启用
							packFLash.input[j].workFlag=RT_TRUE;
							rt_strcpy(packFLash.input[j].name, argv[1]);
						  rt_strcpy(packFLash.input[j].devID,argv[2]);
							rt_strcpy(packFLash.input[j].model,argv[3]);
				      packFLash.input[j].port=port;
							rt_kprintf("%s add diginput chanl %d\n",sign,j+1);
							return;
					}
			 }
	 }
	 else{//删除
			 for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFLash.input[j].devID,argv[2])==0){
							packFLash.input[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete diginput channel %d\n",sign,j+1);
					}
			 }
	 }
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[digInput 水泵 GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-8 之外清除对应ID的所有参数]\n",sign);

}
MSH_CMD_EXPORT(digInput,digInput config);//FINSH_FUNCTION_EXPORT_CMD




static void digOutput(int argc, char *argv[])
{
	 
	  if(0==rt_strcmp((char *)"list", argv[1])){
				printfDOList();
				return;
		}
		if(argc!=5){
				goto ERR;
		}
    int port = atoi16(argv[4],10);
	 if((port<8)&&(port>0)){//添加
		 	 for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFLash.output[j].devID,argv[2])==0){
							packFLash.output[j].workFlag=RT_TRUE;
							rt_strcpy(packFLash.output[j].name, argv[1]);
							rt_strcpy(packFLash.output[j].model,argv[3]);
              packFLash.output[j].port=port;
							rt_kprintf("%s readd digOutput channel %d\n",sign,j+1);
						  return;
					}
			 }
			 for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(packFLash.output[j].workFlag!=RT_TRUE){//没启用
							packFLash.output[j].workFlag=RT_TRUE;
							rt_strcpy(packFLash.output[j].name, argv[1]);
						  rt_strcpy(packFLash.output[j].devID,argv[2]);
							rt_strcpy(packFLash.output[j].model,argv[3]);
				      packFLash.output[j].port=port;
							rt_kprintf("%s add digOutput chanl %d\n",sign,j+1);
							return;
					}
			 }
	 }
	 else{//删除
			 for(int j=0;j<DI_NUM;j++){//查一遍 找到 GYNJLXSD000000499  如果
					if(rt_strcmp(packFLash.output[j].devID,argv[2])==0){
							packFLash.output[j].workFlag=RT_FALSE;
							rt_kprintf("%s delete digOutput channel %d\n",sign,j+1);
					}
			 }
	 }
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[digOutput 水泵 GYNJLXSD000000162 GY281 1]\n",sign);
		rt_kprintf("%s[port1-8 之外清除对应ID的所有参数]\n",sign);

}
MSH_CMD_EXPORT(digOutput,digOutput config);//FINSH_FUNCTION_EXPORT_CMD

