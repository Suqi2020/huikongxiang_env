#include  "board.h"
#include  "cJSON.h"
const static char sign[]="[dataPack]";




//char  string[]="{\
//\"mid\":1234,\
//\"packetType\":\"PROPERTIES_485DATA_RESP\",\
//\"identifier\":\"partial_discharge_monitor\",\
//\"acuId\":\"1000000000001\",\
//\"timestamp\":\"1655172531937\"\
//}";



extern void pressSettRead2Send(rt_bool_t netStat,bool respFlag);
extern void threeAxisRead2Send(rt_bool_t netStat,bool respFlag);
extern void partDischagRead2Send(rt_bool_t netStat,bool respFlag);
extern void circulaRead2Send(rt_bool_t netStat,bool respFlag);
extern void waterDepthRead2Send(rt_bool_t netStat,bool respFlag);
extern void tempHumRead2Send(rt_bool_t netStat,bool respFlag);
extern void o2Read2Send(rt_bool_t netStat);
extern void h2sRead2Send(rt_bool_t netStat);	
extern void ch4Read2Send(rt_bool_t netStat);	
extern void coRead2Send(rt_bool_t netStat);	
extern void crackMeterRead2Send(rt_bool_t netStat,bool respFlag);
//void analogTempHumJsonPack(uint8_t chanl);
#ifndef     ANA_MASK
extern void anaTempHumReadPack2Send(bool gbNetState,bool respFlag);
#endif
//extern uint8_t analogTemChanl;
extern void gasJsonPack(rt_bool_t netStat,bool respFlag);

//485数据读取进行打包
void  readModbusDataResp(char *monitor)
{

		if(0==rt_strcmp(monitor,"partial_discharge_monitor")){
				partDischagRead2Send(true,true);
		}
		else if(0==rt_strcmp(monitor,"grounding_current_monitor")){
				circulaRead2Send(true,true);
		}
		else if(0==rt_strcmp(monitor,"water_level_monitor")){
				waterDepthRead2Send(true,true);
		}
		else if(0==rt_strcmp(monitor,"temperature_and_humidity_monitor")){
				tempHumRead2Send(true,true);
		}
		else if(0==rt_strcmp(monitor,"crackmeter_monitor")){
			crackMeterRead2Send(true,true);
		}
		else if(0==rt_strcmp(monitor,"environment_mointor")){
		#ifdef USE_4GAS 	
   			ch4Read2Send(true);
				o2Read2Send(true);
				h2sRead2Send(true);
			  coRead2Send(true);
			  gasJsonPack(true,false);
		#endif
		}
		else if(0==rt_strcmp(monitor,"vibration_monitor")){
				threeAxisRead2Send(true,true);
		}
		else if(0==rt_strcmp(monitor,"settlement_monitor")){
				pressSettRead2Send(true,true);
		}
		else{
				rt_kprintf("%sget 485 type err[%s]\r\n",sign,monitor);
		}
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