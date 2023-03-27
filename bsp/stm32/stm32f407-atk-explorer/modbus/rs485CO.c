#include "board.h"
//#include "stmflash.h"
//一氧化碳默认地址04 波特率9600
//04 03 00 02 00 02 65 9E
//04 03 04 00 00 00 00 AF 33 
//每次利用co读取定时器来读取4种气体并实现上传 
//4种气体在一起读取 所以前三个不使用 只在CO定时器地方读取并打包发送  关闭时候只需要关闭CO就可以把所有气体全部关闭
#ifdef USE_4GAS
const static char sign[]="[一氧化碳]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //工作环流用到的最大接收buf长度


static float co[CO_485_NUM];
static uint8_t respStat[CO_485_NUM];
bool gasAlarmFlag=false;
//打包串口发送 
static void coUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.co[num].useUartNum,buf, len);
}
//返回co的通讯状态 true--通讯成功 false--通讯失败
int coState(int i)
{
		return respStat[i];
}

//co比较阈值并设置相应的flag标记
static void coCheckSetFlag(int num)
{
		gasAlarmFlag=false;
		if(sheet.modbusCo[num].coUpLimit!=0){
			if(co[num]>=sheet.modbusCo[num].coUpLimit){
					inpoutpFlag.modbusCo[num].coUpFlag=true;gasAlarmFlag=true;
			}
		}
		if(sheet.modbusCo[num].coLowLimit!=0){
			if(co[num]<=sheet.modbusCo[num].coLowLimit){
					inpoutpFlag.modbusCo[num].coLowFlag=true;gasAlarmFlag=true;
			}
		}
}
//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
//读取co值
void readCO(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.co[num].slaveAddr,0X0002,READ_03,2,buf);
		rt_mutex_take(uartDev[sheet.co[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		coUartSend(num,buf,len);
	  rt_kprintf("%sCO send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		

		while(rt_mq_recv(uartDev[sheet.co[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
				len++;
		}
		if(len!=0){
				rt_kprintf("%srec:",sign);
				for(int j=0;j<len;j++){
						rt_kprintf("%x ",buf[j]);
				}
				rt_kprintf("\n");
		}
		//提取环流值 第一步判断crc 第二部提取

		int ret2=modbusRespCheck(sheet.co[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值
        int value	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
        co[num]=(float)((float)value	/1000);
			  respStat[num]=1;
			  rt_kprintf("%s浓度值:%0.2fmol/Lread ok\n",sign,co[num]);  
			
			
				coCheckSetFlag(num);
		} 
		else{//读不到给0
			  co[num]	=0;
			  respStat[num]=0;
			  rt_kprintf("%s read fail\n",sign);
		}
//		coCheckSetFlag(num);
	  rt_mutex_release(uartDev[sheet.co[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;
}




/*gasJsonPack中整体打包
static uint16_t coJsonPack()
{
		char* out = NULL;
		//创建数组
		cJSON* Array = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return 0;
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		// 加入节点（键值对）
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_REPORTDATA");
		cJSON_AddStringToObject(root, "identifier","environment_monitor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < CO_485_NUM; i++)
		{		
			if(sheet.co[i].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.co[i].ID));
				sprintf(sprinBuf,"%d",respStat[i]);
				cJSON_AddItemToObject(nodeobj,"responseStatus",cJSON_CreateString(sprinBuf));
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				sprintf(sprinBuf,"%02f",co[i]);
				cJSON_AddItemToObject(nodeobj_p,"deepness",cJSON_CreateString(sprinBuf));
				sprintf(sprinBuf,"%llu",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		}
		}
	
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// 打印JSON数据包  
		//打包
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json长度最后再填写
		
		// 释放内存  
		out = cJSON_Print(root);
		rt_strcpy((char *)packBuf+len,out);
		len+=rt_strlen(out);
		if(out!=NULL){
				for(int i=0;i<rt_strlen(out);i++)
						rt_kprintf("%c",out[i]);
				rt_kprintf("\n");
				rt_free(out);
				out=NULL;
		}
		if(root!=NULL){
			cJSON_Delete(root);
			out=NULL;
		}

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
		mcu.repDataMessID =mcu.upMessID;
		//mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%s len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}

*/
//co工作情况下读取值
void coRead2Send()
{
	  //int workFlag=RT_FALSE;
	  for(int i=0;i<CO_485_NUM;i++){
			if(sheet.co[i].workFlag==RT_TRUE){
						readCO(i);
					//	workFlag=RT_TRUE;
				}
		}

}
//4中气体有一个在工作就认为气体传感器在工作
rt_bool_t gasWork(int num)
{
		if((sheet.o2[num].workFlag==RT_TRUE)||\
			(sheet.h2s[num].workFlag==RT_TRUE)||\
		  (sheet.ch4[num].workFlag==RT_TRUE)||\
		(sheet.co[num].workFlag==RT_TRUE)){
				return   RT_TRUE;
		}
		return RT_FALSE;
}
extern  float o2[O2_485_NUM];;
extern  float h2s[H2S_485_NUM];
extern   float ch4[CH4_485_NUM];
//4中气体打包
//输入 respFlag 为true就是回应
//              为false就是report数据
static uint16_t gasPack(bool respFlag)
{
		char* out = NULL;
		//创建数组
		cJSON* Array = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return 0;
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		// 加入节点（键值对）
		
	  if(respFlag==true){
				cJSON_AddNumberToObject(root, "mid",respMid);
				cJSON_AddStringToObject(root, "packetType","PROPERTIES_485DATA_GET_RESP");
				cJSON_AddNumberToObject(root, "code",0);
		}
		else
		{
				cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
				cJSON_AddStringToObject(root, "packetType","PROPERTIES_485DATA_REP");
		}	
		cJSON_AddStringToObject(root, "identifier","environment_mointor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < CO_485_NUM; i++)
		{		
			  if(gasWork(i)==RT_TRUE){
			//if(sheet.co[num].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.co[i].ID));
//				sprintf(sprinBuf,"%d",1);//respStat[num]);
				cJSON_AddNumberToObject(nodeobj,"responseStatus",1);
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
			 
				//if(sheet.co[i].workFlag==RT_TRUE){
					sprintf(sprinBuf,"%02f",co[i]);
					cJSON_AddItemToObject(nodeobj_p,"monoxide",cJSON_CreateString(sprinBuf));
				//}
				//if(sheet.o2[i].workFlag==RT_TRUE){
					sprintf(sprinBuf,"%02f",o2[i]);
					cJSON_AddItemToObject(nodeobj_p,"oxy",cJSON_CreateString(sprinBuf));
				//}
				//if(sheet.h2s[i].workFlag==RT_TRUE){
					sprintf(sprinBuf,"%02f",h2s[i]);
					cJSON_AddItemToObject(nodeobj_p,"hydrogenSulfide",cJSON_CreateString(sprinBuf));
				//}
				//if(sheet.ch4[i].workFlag==RT_TRUE){
					sprintf(sprinBuf,"%02f",ch4[i]);
					cJSON_AddItemToObject(nodeobj_p,"methane",cJSON_CreateString(sprinBuf));
				//}
			
				sprintf(sprinBuf,"%llu",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
			}
		//}
		}
	
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		// 打印JSON数据包  
		//打包
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json长度最后再填写
		
		// 释放内存  
		out = cJSON_Print(root);
		rt_strcpy((char *)packBuf+len,out);
		len+=rt_strlen(out);
		if(out!=NULL){
				for(int i=0;i<rt_strlen(out);i++)
						rt_kprintf("%c",out[i]);
				rt_kprintf("\n");
				rt_free(out);
				out=NULL;
		}
		if(root!=NULL){
			cJSON_Delete(root);
			out=NULL;
		}

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
		if(respFlag==false){
				mcu.repDataMessID =mcu.upMessID;
				//mcu.devRegMessID =mcu.upMessID;
				upMessIdAdd();
		}
		rt_kprintf("%s len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;
		return len;
}



//复位温湿度的warn状态值
void resetGasWarnFlag()
{
		for (int i = 0; i < CO_485_NUM; i++)
		{		
				inpoutpFlag.modbusCo[i].coLowFlag =false;
			  inpoutpFlag.modbusCo[i].coUpFlag  =false;
				inpoutpFlag.modbusO2[i].o2LowFlag =false;
			  inpoutpFlag.modbusO2[i].o2UpFlag  =false;
				inpoutpFlag.modbusH2s[i].h2sLowFlag =false;
			  inpoutpFlag.modbusH2s[i].h2sUpFlag  =false;
				inpoutpFlag.modbusCh4[i].ch4LowFlag =false;
			  inpoutpFlag.modbusCh4[i].ch4UpFlag  =false;
		}
}





//模拟温度和湿度值读取以及打包成json格式  返回true 有告警 false 无告警
bool modgasWarn2Send()
{
		if(gasAlarmFlag==false)//TEST
			return false;
		char* out = NULL;
		//创建数组
		cJSON* Array = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return false;
		// 加入节点（键值对）
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","EVENTS_485_ALARM");
		cJSON_AddStringToObject(root, "identifier","environment_mointor");
		cJSON_AddStringToObject(root, "acuId",(char *)packFlash.acuId);
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		{
				Array = cJSON_CreateArray();
				if (Array == NULL) return false;
				cJSON_AddItemToObject(root, "params", Array);
				for (int i = 0; i < CO_485_NUM; i++)
				{		
						if(gasWork(i)==RT_TRUE){
							nodeobj = cJSON_CreateObject();
							cJSON_AddItemToArray(Array, nodeobj);
							cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.co[i].ID));
							cJSON_AddNumberToObject(nodeobj,"alarmStatus",1);
							nodeobj_p= cJSON_CreateObject();
							cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
		
							cJSON_AddNumberToObject(nodeobj_p,"oxy_low_alarm",inpoutpFlag.modbusO2[i].o2LowFlag );//cJSON_CreateNumber("10"));
							cJSON_AddNumberToObject(nodeobj_p,"oxy_high_alarm",inpoutpFlag.modbusO2[i].o2UpFlag);
							cJSON_AddNumberToObject(nodeobj_p,"monoxide_low_alarm",inpoutpFlag.modbusCo[i].coLowFlag);
							cJSON_AddNumberToObject(nodeobj_p,"monoxide_high_alarm",inpoutpFlag.modbusCo[i].coUpFlag );		
							cJSON_AddNumberToObject(nodeobj_p,"hydrogenSulfide_low_alarm",inpoutpFlag.modbusH2s[i].h2sLowFlag);
							cJSON_AddNumberToObject(nodeobj_p,"hydrogenSulfide_high_alarm",inpoutpFlag.modbusH2s[i].h2sUpFlag);		
							cJSON_AddNumberToObject(nodeobj_p,"methane_low_alarm",inpoutpFlag.modbusCh4[i].ch4LowFlag );//cJSON_CreateNumber("10"));
							cJSON_AddNumberToObject(nodeobj_p,"methane_high_alar",inpoutpFlag.modbusCh4[i].ch4UpFlag);
														
							sprintf(sprinBuf,"%llu",utcTime());
							cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
						}
				}
		}
		sprintf(sprinBuf,"%llu",utcTime());
		cJSON_AddStringToObject(root,"timestamp",sprinBuf);
		//打包
		int len=0;
		packBuf[len]= (uint8_t)(HEAD>>8); len++;
		packBuf[len]= (uint8_t)(HEAD);    len++;
		len+=LENTH_LEN;//json长度最后再填写
		// 释放内存  
		out = cJSON_Print(root);
		rt_strcpy((char *)packBuf+len,out);
		len+=rt_strlen(out);
		if(out!=NULL){
				for(int i=0;i<rt_strlen(out);i++)
						rt_kprintf("%c",out[i]);
				rt_kprintf("\n");
				rt_free(out);
				out=NULL;
		}
		if(root!=NULL){
			cJSON_Delete(root);
			out=NULL;
		}
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
		mcu.repDataMessID =mcu.upMessID;
		//mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_free(sprinBuf);
		sprinBuf=RT_NULL;
		return true;
}
//4种气体json打包的二次封装
void  gasJsonPack(rt_bool_t netStat,bool respFlag)
{
		rt_kprintf("%s打包采集的gas数据\r\n",sign);

				gasPack(respFlag);
			if(netStat==RT_TRUE)
				rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				if(modgasWarn2Send()==true){
						resetGasWarnFlag();//每次判断后复位warnflag状态值
						rt_thread_mdelay(500);
						if(netStat==RT_TRUE)
								rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
				}
//			rt_thread_mdelay(2000);//延时发送

}

#endif

