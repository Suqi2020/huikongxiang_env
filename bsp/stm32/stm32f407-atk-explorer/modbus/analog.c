#include "board.h"
#include  "cJSON.h"
char *sign="[analog]";
//anolog 数据量比较小 每次统一上传8个通道  按照第一路设置的时间上传

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
static uint16_t ch[8]={ADC_CHANNEL_8,ADC_CHANNEL_9,ADC_CHANNEL_10,ADC_CHANNEL_11\
											,ADC_CHANNEL_12,ADC_CHANNEL_13,ADC_CHANNEL_14,ADC_CHANNEL_15};
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
extern rt_bool_t  analogCheckWork(uint8_t port);
uint16_t analogJsonPack(int count)
{
	  if(count%packFLash.analog[0].calTime!=0){
			return 0;//时间没到直接跳出
		}
		rt_kprintf("%s get adc timeout\n", sign);
		char* out = NULL;
		//创建数组
		cJSON* Array = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return 0;
		// 加入节点（键值对）
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_REPORTDATA");
		cJSON_AddStringToObject(root, "acuID","100000000000001");
	  cJSON_AddStringToObject(root, "modbusType","analog");
		{
			Array = cJSON_CreateArray();
			if (Array == NULL) return 0;
			cJSON_AddItemToObject(root, "params", Array);
			for (int i = 0; i < ANALOG_NUM; i++)
			{				 				
				 if(analogCheckWork(packFLash.analog[i].port)==RT_TRUE){
						nodeobj = cJSON_CreateObject();
						cJSON_AddItemToArray(Array, nodeobj);
						cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(packFLash.analog[i].devID));
						cJSON_AddItemToObject(nodeobj,"port",cJSON_CreateNumber(packFLash.analog[i].port));
						int rt=Get_Adc_Average(ch[i],10);
						char str[11];
				    sprintf(str,"%0.2f",(float)rt*3.3/4096);

						cJSON_AddItemToObject(nodeobj,"voltage",cJSON_CreateString(str));
				 }
			}
		}
		char time[20];
		sprintf(time,"%d",utcTime());
		cJSON_AddStringToObject(root, "timestamp",time);
		// 打印JSON数据包  
		out = cJSON_Print(root);
		cJSON_Delete(root);
		rt_kprintf("\n %s \n", out);

	//打包
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json长度最后再填写
		
		// 释放内存  
		rt_strcpy((char *)packBuf+len,out);
    len+=rt_strlen(out);
	

		//lenth
	  packBuf[2]=(uint8_t)((len-LENTH_LEN-HEAD_LEN)>>8);//更新json长度
	  packBuf[3]=(uint8_t)(len-LENTH_LEN-HEAD_LEN);
	  uint16_t jsonBodyCrc=RTU_CRC(packBuf+HEAD_LEN+LENTH_LEN,len-HEAD_LEN-LENTH_LEN);
	  //crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc>>8); len++;//更新crc
	  packBuf[len]=(uint8_t)(jsonBodyCrc);    len++;

		//tail
		packBuf[len]=(uint8_t)(TAIL>>8); len++;
		packBuf[len]=(uint8_t)(TAIL);    len++;
		packBuf[len]=0;//len++;//结尾 补0
		
		mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%s len:%d\r\n",sign,len);
		
		for(int i=0;i<len;i++)
				rt_kprintf("%02x",packBuf[i]);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);
		//rt_kprintf("heart:%s \n",packBuf);
		return len;

}