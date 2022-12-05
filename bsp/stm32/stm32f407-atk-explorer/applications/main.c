/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2018-11-19     flybreak     add stm32f407-atk-explorer bsp
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>

//0V1   20220919
//初始化  没有加入版本管理 
//0V3   20220920
// 处了串口4外其他的串口收发都可以  
//0V4   20220921
// 所有io口输入输出 adc uart收发已经测试  
//0V5   20220921
//去掉menuconfig 配置IO驱动   
//1、spi时钟42MHZ 已经确认
//2、w5500内部缓存多大32K的接收缓存
//0V6  W5500参考野火网络通信成功  20220923
//0V7  w5500 中断接收数据OK  20220927
//0V9  shell命令行OK        20220927
//V0.11  增加4个主要task    20220930
//			task1  w5500网络状态的维护，以及接收数据               w5500Task
//			task2  上行数据发送的维护（包括上行心跳 注册 数据打包 ）
//       		以及重发 丢给task3                              upKeepStateTask
//			task3  net网络端的发送                                 netDataSednTask                      
//			task4  接收完数据的解析与处理                          netDataRecTask
//			task1  w5500网络状态的维护，以及接收数据               w5500Task
//			task2  上行数据发送的维护（包括上行心跳 注册 数据打包 ）
//       				以及重发 丢给task3                              upKeepStateTask
//			task3  net网络端的发送                                 netDataSednTask                      
//			task4  接收完数据的解析与处理                          netDataRecTask
//			#define RT_CONSOLEBUF_SIZE 2048  //后期需要该小 512
//V0.12  上行心跳json和回复OK    																	20221008
//V0.13   加入dataup 和devreg easy timer													 20221009
//V0.14   手动测试3条上行数据OK  																		20221010
//        串口2346 modbus 串口1 debug 串口5 串口屏


//V0.15		rs485_公众环流 modbus 初步调式读取数据成功，并能实现上传 20221011
//        rt_sprintf float类型有问题  使用sprintf代替 
//V0.16   READ WRITE modbus OK   																			20221012
//V0.17   //迅速切换其它485接口来使用 方法：只需要修改串口发送接口 和中断接收接口即可
//         rs485Circula.c-cirCurrUartSend(uint8_t *buf,int len) 
//         和drv_uart.c-USART2_IRQHandler中 -rt_mq_send(&cirCurrmque, &Res, 1);    20221012
//V0.18    modbus读不到数据填充0       2022012
//V0.19    局放读取可以  加入modbus读取保护  每次发送后再读取             
//         增加软件定时器 粗略具备初始化 开始 停止功能 需要1秒的基准 
//         增加modbus设备重启后出现的干扰过滤（非modbus读取的数据 丢弃）
//         具备定时器同时到的情况下错开发送功能                          20221013
//V0.20    加入16K代码测试  MAX_SOCK_NUM 为1时候 最大发送16k网络数据  
//         TX_RX_MAX_BUF_SIZE为实际发送缓存buf大小(与MAX_SOCK_NUM反比)                       20221017
//V0.21    加入统计代码，粗略计算掉线次数和掉线时长 命令 offline                    20221018
//V0.22    增加沉降仪代码 默认波特率9600 文档有误      20221018
//V0.23    增加三轴代码 默认波特率9600 需要修改三轴的地址为01  同沉降仪  20221019
//V0.24 	 增加支持modbus设备选择串口 UART2(1) UART3(2) UART6(3) UART1(debug) UART4(4)  未测试  20221020
//V0.25    增加4路485和网络故障log打印输出                                     20221021
//V0.26    增加ip_from==IP_FROM_DHCP 或者自定义来实现网络ip配置 （交换机自动获取ip失败） 20221025
//V0.27    1、加入utf8格式打包json格式  nodepad++设置为utf8 以及 c++中设置 --no-multibyte-chars
//            增加json格式中文字符要在dataPack.c中增加 已经把文件用nodepad++更改为utf8格式
//         2、修复接收数后死机问题 接收完json数据没有释放掉                      20221026
//         3、增加json格式打包devRegJsonPack  用json自带代替sprintf 增加数据包 465Byte增加到586Byte 谨慎使用
//V0.28    增加配置多个modbus到同一个串口上  需要同类型的放到一起  比如 局放和环流  三轴和沉降仪   20221027
//V0.29    增加配置多个文件夹到keil中      20221028
//V0.30    增加flash存储  最后4K           20221028
//V0.31    增加at指令来配置modbus  example[modbus 接地环流 uart5 12]
//				 mobus+设备名称+端口+设备地址(0-关闭设备)+采集时间(秒)           20221029
//V0.32    增加mdbus配置时候 检查不同类型设备用同一个端口
//         检查同类型设备用同一个设备地址用同一个端口
//         实现ip地址的配置和存储到flash中            20221031
//V0.33//4种气体在一起读取 所以前三个不使用 只在CO定时器地方读取并打包发送  关闭时候只需要关闭CO就可以把所有气体全部关闭
//         CO默认气体是4  为了方便打包把气体数据打包在一起    20221101
//V0.34    加入温湿度和水位读取 每个传感器只支持一种       20221102  
//V0.35    更改flash读写 调用drv_falsh_f4.c库函数          20221103
//V0.36    更改modbus读取为通用傻瓜式读取 mcu不介入具体数字含义  20221103
//V0.37    加入错误检测 devid重复 检查挂在同一个串口下边的modbus设备有同样的设备地址  
//         上电打印modbus配置过的设备      
//          开启128kflash空间来存储配置modbus设备(擦除最小单位是128K) 目前最大支持200个       20221104
//V0.38    局放GY-JF100-C01和别的设备挂载在同一个串口的情况下连续读取设备会导致局放读取失败 
//         故连续读取设备情况下需要延时2秒继续读取                   20221105
//V0.39    修改存储参数命令统一为 flash save                      
//         增加模拟配置                                            20221107
//         存在bug  uart 的MX_USART2_UART_Init(packFLash.port[i].bps); 中
//V0.40    修改傻瓜式为指定传感器       20221108
//V0.41    所用到传感器json格式已经打包完成   20221109
//V0.42    接入传感器测试 修复bug    增加模拟量配置目前支持温湿度未完成 20221110
//V0.43    加入传感器响应状态上传  1-传感器响应  0-传感器不响应  20221111
//V0.44    修复log显示问题 以及显示波特率      20221117
//V0.45    增加7寸触屏显示组件地址定义         20221201
//V0.46    增加7寸触屏部分接口函数 增加串口5调试 增加lcdtask 队列       20221205
#define APP_VER       ((0<<8)+46)//0x0105 表示1.5版本
const char date[]="20221205";

//static    rt_thread_t tid 	= RT_NULL;
static    rt_thread_t tidW5500 	  = RT_NULL;
static    rt_thread_t tidNetRec 	= RT_NULL;
static    rt_thread_t tidNetSend 	= RT_NULL;
static    rt_thread_t tidUpkeep 	= RT_NULL;
static    rt_thread_t tidLCD      = RT_NULL;
//信号量的定义
extern  rt_sem_t  w5500Iqr_semp ;//w5500有数据时候中断来临


//邮箱的定义
extern struct  rt_mailbox mbNetRecData;
extern struct  rt_mailbox mbNetSendData;
static char	 	 mbRecPool[20];//接收缓存20条
static char 	 mbSendPool[20];//发送缓存20条


//队列的定义
struct  rt_messagequeue LCDmque;//= {RT_NULL} ;//创建LCD队列
uint8_t LCDQuePool[LCD_BUF_LEN];  //创建lcd队列池
//任务的定义
extern  void   netDataRecTask(void *para);//网络数据接收
extern  void   netDataSendTask(void *para);//网络数据发送
extern  void   upKeepStateTask(void *para);//定时打包数据 后期可能加入定时读取modbus
extern  void   w5500Task(void *parameter);//w5500网络状态的维护
extern  void   hardWareDriverTest(void);
extern  void   LCDTask(void *parameter);

const static char sign[]="[main]";
//const char errStr[]="[ERR]";


/* 定时器的控制块 */
static rt_timer_t timer1;

//static int cnt = 0;
/* 定时器1超时函数 */
//10秒提醒一次 uart offline状态
static void timeout1(void *parameter)
{
		static int count=0;
	  static int alarmTick=10;
		extern rt_bool_t gbNetState;
	  extern void modbusWorkErrCheck(void);
	  count++;
	  
		if(gbNetState==RT_TRUE){
				if(count%10==0){
						HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
				}
		}
		else
				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		if(count%alarmTick==0){
			  alarmTick+=20;
			  if(alarmTick>=100){
						alarmTick=100;// 1 2 3 最终10秒提醒一次
				}
//				modbusWorkErrCheck();//modbus 错误工作状态打印
//				errConfigCheck();//	modbusWorkErrCheck();//errConfigCheck();
				//modbusPrintRead();
//				if(gbNetState ==RT_FALSE){
//						rt_kprintf("%sERR:网络故障\n",sign);
//				}
		}
}


int main(void)
{

		RELAY1_ON;
		RELAY2_ON;
		RELAY3_ON;
		RELAY4_ON;//上电后外部485全部供

    rt_kprintf("\n%s%s  ver=%02d.%02d\n",sign,date,(uint8_t)(APP_VER>>8),(uint8_t)APP_VER);
	  rt_err_t result;
		stm32_flash_read(FLASH_IP_SAVE_ADDR,    (uint8_t*)&packFLash,sizeof(packFLash));
		stm32_flash_read(FLASH_MODBUS_SAVE_ADDR,(uint8_t*)&sheet,    sizeof(sheet));
//	  extern void  flashTest();
//	  flashTest();
//////////////////////////////////////信号量//////////////////////////////
	  w5500Iqr_semp = rt_sem_create("w5500Iqr_semp",0, RT_IPC_FLAG_FIFO);
		if (w5500Iqr_semp == RT_NULL)
    {
        rt_kprintf("%screate w5500Iqr_semp failed\n",sign);
    }
		
		
		  /* 创建定时器 周期定时器 */
    timer1 = rt_timer_create("timer1", timeout1,
                             RT_NULL, 100,
                             RT_TIMER_FLAG_PERIODIC);
		if (timer1 != RT_NULL)
        rt_timer_start(timer1);
		//创建285设备用到的互斥 队列
		
		int ret = rt_mq_init(&LCDmque,"LCDrecBuf",&LCDQuePool[0],1,LCD_BUF_LEN,RT_IPC_FLAG_FIFO);       
		if (ret != RT_EOK)
		{
				rt_kprintf("%sinit LCD msgque failed.\n",sign);
				return -1;
		}
		extern void 	uartMutexQueueCreate();
		uartMutexQueueCreate();
////////////////////////////////////邮箱//////////////////////////////////
		

    result = rt_mb_init(&mbNetRecData,"mbRec",&mbRecPool[0],sizeof(mbRecPool)/4,RT_IPC_FLAG_FIFO);         
    if (result != RT_EOK)
    {
        rt_kprintf("%sinit mailbox NetRecData failed.\n",sign);
        return -1;
    }
    result = rt_mb_init(&mbNetSendData,"mbSend",&mbSendPool[0],sizeof(mbSendPool)/4,RT_IPC_FLAG_FIFO);         
    if (result != RT_EOK)
    {
        rt_kprintf("%sinit mailbox NetSend failed.\n",sign);
        return -1;
    }
		

////////////////////////////////任务////////////////////////////////////
    tidW5500 =  rt_thread_create("w5500",w5500Task,RT_NULL,1024,2, 10 );
		if(tidW5500!=NULL){
				rt_thread_startup(tidW5500);													 
				rt_kprintf("%sRTcreat w5500Task task\r\n",sign);
		}
		tidNetRec =  rt_thread_create("netRec",netDataRecTask,RT_NULL,1024,2, 10 );
		if(tidNetRec!=NULL){
				rt_thread_startup(tidNetRec);													 
				rt_kprintf("%sRTcreat netDataRecTask \r\n",sign);
		}
		tidNetSend =  rt_thread_create("netSend",netDataSendTask,RT_NULL,1024,2, 10 );
		if(tidNetSend!=NULL){
				rt_thread_startup(tidNetSend);													 
				rt_kprintf("%sRTcreat netDataSendTask \r\n",sign);
		}

		
		tidUpkeep =  rt_thread_create("upKeep",upKeepStateTask,RT_NULL,512*3,2, 10 );
		if(tidUpkeep!=NULL){
				rt_thread_startup(tidUpkeep);													 
				rt_kprintf("%sRTcreat upKeepStateTask \r\n",sign);
		}
		tidLCD    =  rt_thread_create("LCD",LCDTask,RT_NULL,512*3,2, 10 );
		if(tidLCD!=NULL){
				rt_thread_startup(tidLCD);													 
				rt_kprintf("%sRTcreat LCDStateTask \r\n",sign);
		}
		//队列初始化之后再开启串口中断接收
//////////////////////////////结束//////////////////////////////////////
    while (0)//task用于测试 以及闪灯操作
    {
				hardWareDriverTest();
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
				rt_thread_mdelay(250);
			  extern rt_bool_t gbNetState;
			  if(gbNetState==RT_TRUE){
					  rt_thread_mdelay(250);
						
				}
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
				rt_thread_mdelay(250);
				if(gbNetState==RT_TRUE){
					  rt_thread_mdelay(250);	
				}
    }
}
//tasklog delete
void  tasklog(int argc, char *argv[])
{
		if(argc!=2){
				goto ERR;
		}
		if(rt_strcmp("delete",argv[1])==0){
				rt_thread_delete(tidW5500);
			  rt_thread_delete(tidNetRec);
			  rt_thread_delete(tidNetSend);
			  rt_thread_delete(tidUpkeep);
			  rt_kprintf("%s[tasklog delete OK]",sign);
		}
		ERR:
		rt_kprintf("%s[tasklog delete]",sign);
}
MSH_CMD_EXPORT(tasklog,tasklog del);//FINSH_FUNCTION_EXPORT_CMD