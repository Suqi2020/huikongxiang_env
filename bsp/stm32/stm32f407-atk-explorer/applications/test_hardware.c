#include "test_hardware.h"
#include <stdint.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>
#include "board.h"
#include "drv_usart.h"
#include "drv_config.h"

////////////////////////////////////
#ifdef TEST_UART


uint8_t testRecBuf[200];
int testCount=0;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
void uart2SendRecTest()
{
		static uint8_t Res;//rt_kprintf("rt_UART\n");
		if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
				HAL_UART_Receive(&huart2,&Res,1,1000); 
				testRecBuf[testCount++]=Res;
			  UART2_485_SEND;
			  HAL_UART_Transmit(&huart2,&Res,1,1000);
			  UART2_485_REC;
			  if((Res=='\n')||(testCount>=10)){
			    testRecBuf[testCount++]=0;
					UART2_485_SEND;
					HAL_UART_Transmit(&huart2,(uint8_t *)testRecBuf,(uint16_t)strlen((char *)testRecBuf),1000);
					UART2_485_REC;
					testCount = 0;
				}
		}
		HAL_UART_IRQHandler(&huart2);	
}
void uart3SendRecTest()
{
		static uint8_t Res;//rt_kprintf("rt_UART\n");
		if((__HAL_UART_GET_FLAG(&huart3,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
				HAL_UART_Receive(&huart3,&Res,1,1000); 
				testRecBuf[testCount++]=Res;
			  if(Res=='\n'){
			    testRecBuf[testCount++]=0;
					UART3_485_SEND;
					HAL_UART_Transmit(&huart3,(uint8_t *)testRecBuf,(uint16_t)strlen((char *)testRecBuf),1000);
					UART3_485_REC;
					testCount = 0;
				}
			 
		}
		HAL_UART_IRQHandler(&huart3);	
}
void uart4SendRecTest()
{
		static uint8_t Res;//rt_kprintf("rt_UART\n");
		if((__HAL_UART_GET_FLAG(&huart4,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
				HAL_UART_Receive(&huart4,&Res,1,1000); 
				testRecBuf[testCount++]=Res;
			  if(Res=='\n'){
			    testRecBuf[testCount++]=0;
					UART4_485_SEND;
					HAL_UART_Transmit(&huart4,(uint8_t *)testRecBuf,(uint16_t)strlen((char *)testRecBuf),1000);
					UART4_485_REC;
					testCount = 0;
				}
			 
		}
		HAL_UART_IRQHandler(&huart4);	
}

void uart5SendRecTest()
{
		static uint8_t Res;//rt_kprintf("rt_UART\n");
		if((__HAL_UART_GET_FLAG(&huart5,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
				HAL_UART_Receive(&huart5,&Res,1,1000); 
				testRecBuf[testCount++]=Res;
			  if(Res=='\n'){
			    testRecBuf[testCount++]=0;
		
					HAL_UART_Transmit(&huart5,(uint8_t *)testRecBuf,(uint16_t)strlen((char *)testRecBuf),1000);

					testCount = 0;
				}
			 
		}
		HAL_UART_IRQHandler(&huart5);	
}
void uart6SendRecTest()
{
	 static uint8_t Res;//rt_kprintf("rt_UART\n");
		if((__HAL_UART_GET_FLAG(&huart6,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
				HAL_UART_Receive(&huart6,&Res,1,1000); 
				testRecBuf[testCount++]=Res;
			  if(Res=='\n'){
			    testRecBuf[testCount++]=0;
					UART6_485_SEND;
					HAL_UART_Transmit(&huart6,(uint8_t *)testRecBuf,(uint16_t)strlen((char *)testRecBuf),1000);
					UART6_485_REC;
					testCount = 0;
				}
			 
		}
		HAL_UART_IRQHandler(&huart6);	
}

#endif
#ifdef  TEST_ADC
extern ADC_HandleTypeDef hadc1;


//获得ADC值
//ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
//返回值:转换结果
uint16_t Get_Adc(uint32_t ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //通道
    ADC1_ChanConf.Rank=1;                                       //第1个序列，序列1
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES;        //采样时间
    ADC1_ChanConf.Offset=0;                 
    HAL_ADC_ConfigChannel(&hadc1,&ADC1_ChanConf);        //通道配置
	
    HAL_ADC_Start(&hadc1);                               //开启ADC
	
    HAL_ADC_PollForConversion(&hadc1,10);                //轮询转换
 
	return (uint16_t)HAL_ADC_GetValue(&hadc1);	        //返回最近一次ADC1规则组的转换结果
}
//获取指定通道的转换值，取times次,然后平均 
//times:获取次数
//返回值:通道ch的times次转换结果平均值
uint16_t Get_Adc_Average(uint16_t ch,uint16_t times)
{
		uint16_t temp_val=0;
		uint16_t t;
		for(t=0;t<times;t++)
		{
				temp_val+=Get_Adc(ch);
				rt_thread_mdelay(5);
		}
		return temp_val/times;
} 
void adcGetTest()
{

				int rt=Get_Adc_Average(ADC_CHANNEL_8,10);
				rt_kprintf("ADC_CHANNEL_8 ,adc tick:%d,voltgge 扩大100倍:%dV\n",rt,rt*330/4096);
				 rt=Get_Adc_Average(ADC_CHANNEL_9,10);
				rt_kprintf("ADC_CHANNEL_9 ,adc tick:%d,voltgge 扩大100倍:%dV\n",rt,rt*330/4096);
					 rt=Get_Adc_Average(ADC_CHANNEL_10,10);
				rt_kprintf("ADC_CHANNEL_10 ,adc tick:%d,voltgge 扩大100倍:%dV\n",rt,rt*330/4096);
					 rt=Get_Adc_Average(ADC_CHANNEL_11,10);
				rt_kprintf("ADC_CHANNEL_11 ,adc tick:%d,voltgge 扩大100倍:%dV\n",rt,rt*330/4096);
					 rt=Get_Adc_Average(ADC_CHANNEL_12,10);
				rt_kprintf("ADC_CHANNEL_12 ,adc tick:%d,voltgge 扩大100倍:%dV\n",rt,rt*330/4096);
					 rt=Get_Adc_Average(ADC_CHANNEL_13,10);
				rt_kprintf("ADC_CHANNEL_13 ,adc tick:%d,voltgge 扩大100倍:%dV\n",rt,rt*330/4096);
					 rt=Get_Adc_Average(ADC_CHANNEL_14,10);
				rt_kprintf("ADC_CHANNEL_14 ,adc tick:%d,voltgge 扩大100倍:%dV\n",rt,rt*330/4096);
					 rt=Get_Adc_Average(ADC_CHANNEL_15,10);
				rt_kprintf("ADC_CHANNEL_15 ,adc tick:%d,voltgge 扩大100倍:%dV\n\r\n\r\n",rt,rt*330/4096);
}

#endif

#ifdef TEST_IOIN
static void ioInTest()
{
	   if(HAL_GPIO_ReadPin(IO_IN1_GPIO_Port, IO_IN1_Pin)==GPIO_PIN_SET){
				rt_kprintf("1路高\r\n");
		 }
		 else{
				rt_kprintf("1路低\r\n");
		 }
		 if(HAL_GPIO_ReadPin(IO_IN2_GPIO_Port, IO_IN2_Pin)==GPIO_PIN_SET){
				rt_kprintf("2路高\r\n");
		 }
		 else{
				rt_kprintf("2路低\r\n");
		 }
		 if(HAL_GPIO_ReadPin(IO_IN3_GPIO_Port, IO_IN3_Pin)==GPIO_PIN_SET){
				rt_kprintf("3路高\r\n");
		 }
		 else{
				rt_kprintf("3路低\r\n");
		 }
		 if(HAL_GPIO_ReadPin(IO_IN4_GPIO_Port, IO_IN4_Pin)==GPIO_PIN_SET){
				rt_kprintf("4路高\r\n");
		 }
		 else{
				rt_kprintf("4路低\r\n");
		 }
		 if(HAL_GPIO_ReadPin(IO_IN5_GPIO_Port, IO_IN5_Pin)==GPIO_PIN_SET){
				rt_kprintf("5路高\r\n");
		 }
		 else{
				rt_kprintf("5路低\r\n");
		 }
		 if(HAL_GPIO_ReadPin(IO_IN6_GPIO_Port, IO_IN6_Pin)==GPIO_PIN_SET){
				rt_kprintf("6路高\r\n");
		 }
		 else{
				rt_kprintf("6路低\r\n");
		 }
		 if(HAL_GPIO_ReadPin(IO_IN7_GPIO_Port, IO_IN7_Pin)==GPIO_PIN_SET){
				rt_kprintf("7路高\r\n");
		 }
		 else{
				rt_kprintf("7路低\r\n");
		 }
		 if(HAL_GPIO_ReadPin(IO_IN8_GPIO_Port, IO_IN8_Pin)==GPIO_PIN_SET){
				rt_kprintf("8路高\r\n");
		 }
		 else{
				rt_kprintf("8路低\r\n");
		 }
		 
			 
}
#endif

#ifdef TEST_IOOUT
static void ioOutTest()//注意调用函数中的延时加上此处的延时
{
			IOOUT1_ON;
	    IOOUT2_ON;
			IOOUT3_ON;
			IOOUT4_ON;
			IOOUT5_ON;
			IOOUT6_ON;
			IOOUT7_ON;
			IOOUT8_ON;
			rt_thread_mdelay(1000);
			IOOUT1_OFF;
	    IOOUT2_OFF;
			IOOUT3_OFF;
			IOOUT4_OFF;
			IOOUT5_OFF;
			IOOUT6_OFF;
			IOOUT7_OFF;
			IOOUT8_OFF;
			rt_thread_mdelay(1000);

}
#endif






#ifdef  TEST_SPAKER
static void  spakerTest()
{
			SPAKER1_ON;
	    SPAKER2_ON;
	    SPAKER3_ON;
	    SPAKER4_ON;

			rt_thread_mdelay(5000);
			SPAKER1_OFF;
	    SPAKER2_OFF;
	    SPAKER3_OFF;
	    SPAKER4_OFF;

			rt_thread_mdelay(5000);
}

#endif
#ifdef  TEST_RELAY
static void  relayTest()
{
			RELAY1_ON;
	    RELAY2_ON;
	    RELAY3_ON;
	    RELAY4_ON;

			rt_thread_mdelay(5000);
			RELAY1_OFF;
	    RELAY2_OFF;
	    RELAY3_OFF;
	    RELAY4_OFF;

			rt_thread_mdelay(5000);
}

#endif

#ifdef   TEST_W5500


#endif
//放在main函数中 while中掉用测试 注意while中的延时会跟本函数叠加
//IOoutput  
//adc
//ioinput
//uart在drv_gpio中测试
void  hardWareDriverTest(void)
{
		
#ifdef TEST_IOOUT
		ioOutTest();//注意调用函数中的延时加上此处的延时
#endif
	
#ifdef TEST_ADC
		adcGetTest();
#endif
	
#ifdef TEST_IOIN
    ioInTest();
#endif
	
#ifdef  TEST_RELAY
		relayTest();
#endif
	
#ifdef  TEST_SPAKER
   spakerTest();
#endif
#ifdef  TEST_W5500
		w5500Test();
#endif
}
//TX_RX_MAX_BUF_SIZE=16*1024  以及MAX_SOCK_NUM=1

//获取tick值
//不能打印 uint64_t 数据  拆分成32位打印
int tick()
{

	  uint64_t time =utcTime();

	  uint32_t rtc_s=time/1000;
	  uint32_t rtc_ms=time%1000;
		rt_kprintf("[tick][%lu]-s\r\n",rtc_s);//不能同时打印需要分开打印才正确
		rt_kprintf("[tick][%lu]-ms\r\n",rtc_ms);
	  return 0;
}
//FINSH_FUNCTION_EXPORT(tick, tick finsh);//FINSH_FUNCTION_EXPORT_CMD
MSH_CMD_EXPORT(tick,tick stamp);//FINSH_FUNCTION_EXPORT_CMD

//uint32_t 	offLTimes;
//uint32_t 	onLTimes;
offLineStru offLine={0};
//实现网络掉线的监测  掉线次数 每次掉线时长，
//如果拔掉网线情况下时长不准确 需要加上心跳周期 或者发送数据的间隔时长 
//拔掉网线时候需要发送数据才能验证网络断开
int offline()
{
	extern rt_bool_t gbNetState;
if(gbNetState ==RT_FALSE)
		rt_kprintf("[offLine]offtotal net[%d] mqtt[%d]Times\r\n",offLine.netTimes,offLine.mqttTimes);
else
		rt_kprintf("[onLine]offtotal net[%d] mqtt[%d]Times\r\n",offLine.netTimes,offLine.mqttTimes);

		return 0;
}

MSH_CMD_EXPORT(offline,offline stamp);//FINSH_FUNCTION_EXPORT_CMD




static char name[8000]="{TheChinese economy started off the year with better-than-expected 4.5 percent year-on-year growth in the first quarter, laying a solid foundation for the full-year economic rebound in 2023 while providing stronger impetus for global economic recovery amid a volatile international environment.\
China's GDP 4.5 percent accordingqertyuiopasdfghjkl;zxcvbnm1to don Tuesday.\
Overall, the economy showed month-on-month recovery trend in the first quarter, with total retail sales far beating market expectations in March, reflecting that consumer confidence has gradually been restored, Chen Fengying, an economist and former director of the Institute of World Economic Studies at the China Institutes of Contemporary International Relations, told the Global Times on Tuesday.\
The country's total retail sales grew by 5.8 percent year-on-year in the first quarter, reversing a downward trend of 2.7 percent year-on-year seen in the fourth quarter of 2022. Specifically, total retail sales increased by 10.6 percent year-on-year in March, according to NBS data.\
\"The first-quarter macroeconomic figures indicate notable optimization of China's economic structure, with continuous industrial transformation and upgrading,\" Wan Zhe, an economist and professor at the Belt and Road School of Beijing Normal University, told the Global Times on Tuesday.\
China's push for innovation in bottle-neck technologies promotes basic and applied research and accelerates digital transformation across various industries, consolidating the growth momentum of the Chinese economy, she said.\
While the fixed-asset investment grew 5.1 percent year-on-year in the first quarter, the investment in high-tech industries increased by 16 percent and the investment in e-commerce services surged by 51.5 percent, official data showed. \"This indicates China's economic transformation is accelerating, and we're ushering in a digital economy and high-tech transformation,\" Chen said.\
The Chinese economy started off the year with better-than-expected 4.5 percent year-on-year growth in the first quarter, laying a solid foundation for the full-year economic rebound in 2023 while providing stronger impetus for global economic recovery amid a volatile international environment.\
China's GDP in the first three months reached 28.5 trillion yuan ($4.15 trillion), up 4.5 percent year-on-year, or an increase of 2.2 percent compared with the fourth quarter of 2022, according to data released by the National Bureau of Statistics (NBS) on Tuesday.\
The growth is hard-won against grave and complex international environment, NBS Spokesperson Fu Linghui said, noting that positive factors kept accumulating in the first quarter, as multiple supportive policies and measures took effect early on.\
Overall, the economy showed month-on-month recovery trend in the first quarter, with total retail sales far beating market expectations in March, reflecting that consumer confidence has gradually been restored, Chen Fengying, an economist and former director of the Institute of World Economic Studies at the China Institutes of Contemporary International Relations, told the Global Times on Tuesday.\
The country's total retail sales grew by 5.8 percent year-on-year in the first quarter, reversing a downward trend of 2.7 percent year-on-year seen in the fourth quarter of 2022. Specifically, total retail sales increased by 10.6 percent year-on-year in March, according to NBS data.\
\"The first-quarter macroeconomic figures indicate notable optimization of China's economic structure, with continuous industrial transformation and upgrading,\" Wan Zhe, an economist and professor at the Belt and Road School of Beijing Normal University, told the Global Times on Tuesday.\
China's push for innovation in bottle-neck technologies promotes basic and applied research and accelerates digital transformation across various industries, consolidating the growth momentum of the Chinese economy, she said.\
While the fixed-asset investment grew 5.1 percent year-on-year in the first quarter, the investment in high-tech industries increased by 16 percent and the investment in e-commerce services surged by 51.5 percent, official data showed. \"This indicates China's economic transformation is accelerating, and we're ushering in a digital economy and high-tech transformation,\" Chen said.\
Despite external uncertainties and challenges, China's exports rose by 23.4 percent year-on-year in yuan-denominated terms in March, exceeding market expectations significantly.\
China's investment in green and low carbon development in advance under the high-quality development, high-standard opening-up and the Belt and Road Initiative ensures resilience in exports despite growing protectionism, potential economic recession in the US and the EU and other risks, Wan said.\
The better-than-expected macroeconomic figures have boosted international financial institutions' upbeat sentiment on China's economic growth this year.\
\"The macroeconomic data for the first quarter of 2023 implies the positive feedback from the Chinese economy on the optimization of COVID-19 prevention measures, and we maintain our optimistic attitude towards China's economic growth in 2023,\" Darius Tang, Associate Director of Corporates of Fitch Bohua, a subsidiary of Fitch Ratings, said in a note sent to the Global Times.\
In addition, JP Morgan upgraded their forecast for China to 6.4 percent year-on-year from a previous estimate of 6.0 percent year-on-year, according to media reports\
Looking ahead, China's economic performance will continue to see overall improvement on the back of strengthening internal momentum and stepped-up policies, Fu said, noting that China's economic growth will notably speed up in the second quarter.\
\"The economy will likely further rebound to as high as 8 percent between April and June given a lower base the same quarter last year,\" Chen said, noting that the full-year GDP growth rate will probably exceed the around 5 percent target set during this year's two sessions.\
However, we must be aware that the situation abroad is still complex and volatile, inadequate domestic demand remains prominent and the foundation for economic recovery is not solid yet, Wu Chaoming, deputy head of the Chasing Research Institute, told the Global Times on Tuesday.\
He said efforts should be made to further stimulate private investment and real estate while increasing residents' actual income for tapping the potential of the huge domestic market and bolstering full-fledged economic recovery this year.\
China's remarkable economic performance serves as \"a slap in the face\" of Western media that badmouth China's recovery in the post-COVID-19 era. For instance, some media have hyped about deflation worries while others are undermining China's export and consumption prospects regardless of facts.\
\"The current Chinese economy does not appear deflated and in the near future deflation is not likely,\" Fu said. Seasonal factors, price drop in international bulk commodities and geopolitics jointly contributed to domestic price drop but the price will recover to a reasonable level along with the disappearance of these factors in the second half of the year, he said.\
With promising prospect for full-year economic recovery, China will provide stronger impetus to the world economy and be a stabilizer in volatile international environment, analysts said.\
\"The robust rebound means China is set to account for around one third of global growth in 2023 - giving a welcome lift to the world economy,\" Kristalina Georgieva, Managing Director of the IMF, said at the China Development Forum in March.\
The robust rebound means China is set to account for around one third of global growth in 2023—giving a welcome lift to the world economy. And beyond the direct contribution to global growth, our analysis shows that a 1 percentage point increase in GDP growth in China leads to 0.3 percentage point increase in growth in other Asian economies, on average—a welcome boost.}";


static char name2[1000]="{The Chinese economy started off the year with better-than-expected 4.5 percent year-on-year growth in the first quarter, laying a solid foundation for the full-year economic rebound in 2023 while providing stronger impetus for global economic recovery amid a volatile international environment.\
China's GDP in the first three months reached 28.5 trillion yuan ($4.15 trillion), up 4.5 percent year-on-year, or an increase of 2.2 percent compared with the fourth quarter of 2022, according to data released by the National Bureau of Statistics (NBS) on Tuesday.\
The robust rebound means China is set to account for around one third of global growth in 2023—giving a welcome lift to the world economy. And beyond the direct contribution to global growth, our analysis shows that a 1 percentage point increase in GDP growth in China leads to 0.3 percentage point increase in growth in other Asian economies, on average—a welcome boost.";


int jsonBufPackTest()
{
	 
	NetTxBuffer[0]=0xff;
	NetTxBuffer[1]=0xff;
	NetTxBuffer[2]=0xff;
	NetTxBuffer[3]=0xff;
			sprintf((char *)NetTxBuffer+PACK_HEAD_LEN,"%s",name);
	
	NetTxBuffer[strlen(name)]=0;
	printf("strlen(name)=%d  %d\n",strlen(name),strlen((char*)NetTxBuffer));
	return strlen(name);
	//sprintf((char *)NetTxBuffer,"%s","\"}");
	
}
