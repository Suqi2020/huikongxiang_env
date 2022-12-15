#include "board.h"
//#include "stmflash.h"
//水位默认地址02  波特率9600  量程0~49米
//读取水位
const static char sign[]="[水位]";

//#define   SLAVE_ADDR     0X01 
#define   LENTH          50  //工作环流用到的最大接收buf长度

static float waterDepth[WATERDEPTH_485_NUM];
static uint8_t respStat[WATERDEPTH_485_NUM];
//打包串口发送 
static void waterDepthUartSend(int num,uint8_t *buf,int len)
{
		rs485UartSend(sheet.waterDepth[num].useUartNum,buf, len);
}

int waterDepthState(int i)
{
		return respStat[i];
}

//发 1A 04 00 01 00 02 23 E0
//收 1A 04 04 0B 1B 00 1C 23 6F
void readWaterDepth(int num)
{
	  uint8_t offset=3;//add+regadd+len
	  uint8_t  *buf = RT_NULL;
		buf = rt_malloc(LENTH);
	  uint16_t len = modbusReadReg(sheet.waterDepth[num].slaveAddr,0X0002,READ_04,2,buf);
		rt_mutex_take(uartDev[sheet.waterDepth[num].useUartNum].uartMutex,RT_WAITING_FOREVER);
	  //485发送buf  len  等待modbus回应
		waterDepthUartSend(num,buf,len);
	  rt_kprintf("%swater send:",sign);
		for(int j=0;j<len;j++){
				rt_kprintf("%x ",buf[j]);
		}
		rt_kprintf("\n");
    len=0;
		memset(buf,0,LENTH);
		

		while(rt_mq_recv(uartDev[sheet.waterDepth[num].useUartNum].uartMessque, buf+len, 1, 500) == RT_EOK){//115200 波特率1ms 10个数据
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
		int ret2=modbusRespCheck(sheet.waterDepth[num].slaveAddr,buf,len,RT_TRUE);
		if(0 == ret2){//刷新读取到的值
        uint32_t read	=(buf[offset]<<24)+(buf[offset+1]<<16)+(buf[offset+2]<<8)+buf[offset+3];offset+=4;
        extern float write_hex_to_float(uint32_t number);
			  float waterDepth_p= write_hex_to_float(read);
        waterDepth[num]=(float)(waterDepth_p	/9.8);
			  rt_kprintf("%s水深:%0.4f米\n",sign,waterDepth[num]);  
			  respStat[num]=1;
		} 
		else{//读不到给0
				if(ret2==2){
						//rt_kprintf("%sERR:请检查485接线或者供电\r\n",sign);
				}
				respStat[num]=0;
			  waterDepth[num]	=0;
			  rt_kprintf("%s read fail\n",sign);
		}
	  rt_mutex_release(uartDev[sheet.waterDepth[num].useUartNum].uartMutex);
		rt_free(buf);
	  buf=RT_NULL;

}




static uint16_t waterDepthJsonPack()
{
//		char *sprinBuf=RT_NULL;
//		sprinBuf=rt_malloc(20);//20个字符串长度 够用了
		char* out = NULL;
		//创建数组
		cJSON* Array = NULL;
		// 创建JSON Object  
		cJSON* root = NULL;
		cJSON* nodeobj = NULL;
		cJSON* nodeobj_p = NULL;
		root = cJSON_CreateObject();
		if (root == NULL) return 0;
		// 加入节点（键值对）
		cJSON_AddNumberToObject(root, "mid",mcu.upMessID);
		cJSON_AddStringToObject(root, "packetType","CMD_REPORTDATA");
		cJSON_AddStringToObject(root, "identifier","water_depth");
		cJSON_AddStringToObject(root, "acuId",(char *)packFLash.acuId);
//	  cJSON_AddItemToObject(root,"acuId",cJSON_CreateString(packFLash.acuId));
		char *sprinBuf=RT_NULL;
		sprinBuf=rt_malloc(20);//20个字符串长度 够用了	
		
		{
		Array = cJSON_CreateArray();
		if (Array == NULL) return 0;
		cJSON_AddItemToObject(root, "params", Array);
		for (int i = 0; i < WATERDEPTH_485_NUM; i++)
		{		
			if(sheet.waterDepth[i].workFlag==RT_TRUE){
				nodeobj = cJSON_CreateObject();
				cJSON_AddItemToArray(Array, nodeobj);
			  cJSON_AddItemToObject(nodeobj,"deviceId",cJSON_CreateString(sheet.waterDepth[i].ID));
				sprintf(sprinBuf,"%d",respStat[i]);
				cJSON_AddItemToObject(nodeobj,"responseStatus",cJSON_CreateString(sprinBuf));
				
				nodeobj_p= cJSON_CreateObject();
				cJSON_AddItemToObject(nodeobj, "data", nodeobj_p);
				sprintf(sprinBuf,"%02f",waterDepth[i]);
				cJSON_AddItemToObject(nodeobj_p,"depth",cJSON_CreateString(sprinBuf));

				sprintf(sprinBuf,"%d",utcTime());
				cJSON_AddItemToObject(nodeobj_p,"monitoringTime",cJSON_CreateString(sprinBuf));
			}
		}
		}
	
		sprintf(sprinBuf,"%d",utcTime());
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
		
		mcu.devRegMessID =mcu.upMessID;
		upMessIdAdd();
		rt_kprintf("%s len:%d\r\n",sign,len);
		rt_kprintf("\r\n%slen：%d str0:%x str1:%x str[2]:%d  str[3]:%d\r\n",sign,len,packBuf[0],packBuf[1],packBuf[2],packBuf[3]);

		rt_free(sprinBuf);
		sprinBuf=RT_NULL;

		return len;
}

void waterDepthRead2Send(rt_bool_t netStat)
{
	 int workFlag=RT_FALSE;
			for(int i=0;i<WATERDEPTH_485_NUM;i++){
					if(sheet.waterDepth[i].workFlag==RT_TRUE){
							readWaterDepth(i);
							workFlag=RT_TRUE;
					}
			}
			if(workFlag==RT_TRUE){
					rt_kprintf("%s打包采集的waterdepth数据\r\n",sign);
					waterDepthJsonPack();
					if(netStat==RT_TRUE)
							rt_mb_send_wait(&mbNetSendData, (rt_ubase_t)&packBuf,RT_WAITING_FOREVER);
			}
}

