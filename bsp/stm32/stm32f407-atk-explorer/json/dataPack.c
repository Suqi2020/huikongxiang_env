#include  "board.h"
#include  "cJSON.h"
const static char sign[]="[dataPack]";






extern void pressSettRead2Send(bool respFlag);
extern void threeAxisRead2Send(bool respFlag);
extern void partDischagRead2Send(bool respFlag);
extern void circulaRead2Send(bool respFlag);
extern void waterDepthRead2Send(bool respFlag);
extern void tempHumRead2Send(bool respFlag);
extern void o2Read2Send(void);
extern void h2sRead2Send(void);	
extern void ch4Read2Send(void);	
extern void coRead2Send(void);	
extern void crackMeterRead2Send(bool respFlag);

#ifndef     ANA_MASK
extern void anaTempHumReadPack2Send(bool gbNetState,bool respFlag);
#endif

extern void gasJsonPack(bool respFlag);

//485数据读取进行打包
void  readModbusDataResp(char *monitor)
{

		if(0==rt_strcmp(monitor,"partial_discharge_monitor")){
				partDischagRead2Send(true);
		}
		else if(0==rt_strcmp(monitor,"grounding_current_monitor")){
				circulaRead2Send(true);
		}
		else if(0==rt_strcmp(monitor,"water_level_monitor")){
				waterDepthRead2Send(true);
		}
		else if(0==rt_strcmp(monitor,"temperature_and_humidity_monitor")){
				tempHumRead2Send(true);
		}
		else if(0==rt_strcmp(monitor,"crackmeter_monitor")){
			crackMeterRead2Send(true);
		}
		else if(0==rt_strcmp(monitor,"environment_mointor")){
		#ifdef USE_4GAS 	
   			ch4Read2Send();
				o2Read2Send();
				h2sRead2Send();
			  coRead2Send();
			  gasJsonPack(false);
		#endif
		}
		else if(0==rt_strcmp(monitor,"vibration_monitor")){
				threeAxisRead2Send(true);
		}
		else if(0==rt_strcmp(monitor,"settlement_monitor")){
				pressSettRead2Send(true);
		}
		else{
				rt_kprintf("%sget 485 type err[%s]\r\n",sign,monitor);
		}
//		rt_mutex_release(read485_mutex);
}
#ifndef     ANA_MASK

void  readAnaDataResp(char *monitor)
{

		if(0==rt_strcmp(monitor,"temperature_and_humidity_monitor")){
				 anaTempHumReadPack2Send(true,true);
		}
		else{
				rt_kprintf("%sget ana type err[%s]\r\n",sign,monitor);
		}
}
#endif

//函数作用 在原有的json函数基础上NetTxBuffer 加入mqtt的头部 topic头部
//extern rt_mutex_t txBuf_mutex;
void   packMqttSend()
{
//		rt_mutex_take(txBuf_mutex,RT_WAITING_FOREVER);
	 
		extern bool  mqttState(void);
	  extern rt_bool_t  gbNetState;
	  rt_kprintf("%smqttstat[%d],netsate[%d]\n",sign,mqttState(),gbNetState);
	  if( mqttState()!=RT_TRUE)
			return;
		if(gbNetState!=RT_TRUE)
			return;

	  static MQTTString topic= MQTTString_initializer;
	  int sendBufLen=0;
		topic.cstring = rt_malloc(100);
		rt_sprintf(topic.cstring,"/acu/%s/up",packFlash.acuId);
	  extern int MQTTSerialize_publish_suqi(int buflen,unsigned char dup, int qos, unsigned char retained, unsigned short packetid,
		MQTTString topicName, unsigned char* payload, int payloadlen);
		if((sendBufLen=MQTTSerialize_publish_suqi(sizeof(NetTxBuffer),0, 0, 0,0,topic, (unsigned char *)NetTxBuffer+PACK_HEAD_LEN,strlen((char*)NetTxBuffer)-PACK_HEAD_LEN))>0){ //qos=1????packetid
				rt_kprintf("%sok publish pack\n",sign);
			}
		else
			rt_kprintf("%serr publish pack\n",sign);
		NetTxBuffer[sendBufLen]=0;
		rt_free(topic.cstring);
		topic.cstring=RT_NULL;
		NetTxBuffer[0]=(uint8_t)(sendBufLen>>24);
		NetTxBuffer[1]=(uint8_t)(sendBufLen>>16);
		NetTxBuffer[2]=(uint8_t)(sendBufLen>>8);
		NetTxBuffer[3]=(uint8_t)sendBufLen;
		rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&NetTxBuffer,RT_WAITING_FOREVER); 
//	  rt_mutex_release(txBuf_mutex);
}
