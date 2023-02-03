#include <board.h>




const static char sign[]="[threshold]";



//������������²��ܵõ���ֵ
void printfThresholdList()
{
	  int i;
		for(int j=0;j<ANALOG_NUM;j++){//��һ�� �ҵ� ��ʪ�ȵ�ģ����ֵ
			  if(sheet.analog[j].workFlag==RT_TRUE){
						if(rt_strcmp(sheet.analog[j].name,analogName[0])==0){
								rt_kprintf("%s threshold ",sign);
								rt_kprintf("%s ",analogName[0]);
								rt_kprintf("%s ",sheet.analog[j].ID);
								rt_kprintf("1 ");
								rt_kprintf("%d ",sheet.analog[j].subName);
							  if(sheet.analog[j].subName==1){//�¶�
										rt_kprintf("%f ",sheet.analogTempHum.tempUpLimit);
										rt_kprintf("%f /n",sheet.analogTempHum.tempLowLimit);
								}
								else if(sheet.analog[j].subName==2){//ʪ��
										rt_kprintf("%f ",sheet.analogTempHum.humUpLimit);
										rt_kprintf("%f /n",sheet.analogTempHum.humLowLimit);

								}
						}
				}
		}
		for(i=0;i<CIRCULA_485_NUM;i++){
						if(sheet.cirCula[i].workFlag==RT_TRUE){//�ҵ�������  ������Ҫ������ӡ����
								rt_kprintf("%s threshold ",sign);
								rt_kprintf("%s ",modbusName[i]);
								rt_kprintf("%s ",sheet.cirCula[i].ID);
								rt_kprintf("2 ");
								rt_kprintf("1 ");
								rt_kprintf("%f ",sheet.modbusCircul[i].cirCurAUpLimit);
								rt_kprintf("%f \n",sheet.modbusCircul[i].cirCurALowLimit);
			
								rt_kprintf("%s threshold ",sign);
								rt_kprintf("%s ",modbusName[i]);
								rt_kprintf("%s ",sheet.cirCula[i].ID);
								rt_kprintf("2 ");
								rt_kprintf("2 ");
								rt_kprintf("%f ",sheet.modbusCircul[i].cirCurBUpLimit);
								rt_kprintf("%f \n",sheet.modbusCircul[i].cirCurBLowLimit);
							
								rt_kprintf("%s threshold ",sign);
								rt_kprintf("%s ",modbusName[i]);
								rt_kprintf("%s ",sheet.cirCula[i].ID);
								rt_kprintf("2 ");
								rt_kprintf("3 ");
								rt_kprintf("%f ",sheet.modbusCircul[i].cirCurCUpLimit);
								rt_kprintf("%f \n",sheet.modbusCircul[i].cirCurCLowLimit);
						}
				}
		//��ӡmodbus���õ���ֵ
}
//��modbus��������֮ͬ������ �����ж�ID����Ҫ�ж���ѡ��
bool   analogThresholdConfig(int num,char *ID,int sensorType,float upLimit,float lowLimit)
{ 
		switch(num)
		{
			case  0://modbus ��ʪ��
				for(int i=0;i<ANALOG_NUM;i++){//
						if(rt_strcmp(sheet.analog[i].ID,ID)==0){//�Ⱥ˶�ID
								if(sheet.analog[i].subName==sensorType){
									 if(sensorType==1){//�¶�
											sheet.analogTempHum.humLowLimit=lowLimit;
											sheet.analogTempHum.humUpLimit=upLimit;
											return true;
									 }
									 else if(sensorType==2){//ʪ��
											sheet.analogTempHum.tempLowLimit=lowLimit;
											sheet.analogTempHum.tempUpLimit =upLimit;	
											return true;
									 }
								}
						}
				}
			break;
		}
		return false;
}
//modbus�豸  ��������Ϊ ���������� ID�� ����������  ������  ������
bool   modbusThresholdConfig(int num,char *ID,int sensorType,float upLimit,float lowLimit)
{
	  int i=0;
		switch(num)
		{
			case CIRCULA:
				for(i=0;i<CIRCULA_485_NUM;i++){
						if(rt_strcmp(sheet.cirCula[i].ID,ID)==0){//�ҵ���ͬID��
								if(sensorType==1){
										sheet.modbusCircul[i].cirCurAUpLimit  = upLimit;
									  sheet.modbusCircul[i].cirCurALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==2){
										sheet.modbusCircul[i].cirCurBUpLimit  = upLimit;
										sheet.modbusCircul[i].cirCurBLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==3){
										sheet.modbusCircul[i].cirCurCUpLimit  = upLimit;
										sheet.modbusCircul[i].cirCurCLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
				break;
			case PARTDISCHAG:
				for(i=0;i<PARTDISCHAG_485_NUM;i++){
						if(rt_strcmp(sheet.partDischag[i].ID,ID)==0){//�ҵ���ͬID��
								if(sensorType==1){
										sheet.modbusPartDisChg[i].amplitudeAUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].amplitudeALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==2){
										sheet.modbusPartDisChg[i].freqAUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].freqALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==3){
										sheet.modbusPartDisChg[i].dischargeAUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].dischargeALowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==4){
										sheet.modbusPartDisChg[i].amplitudeBUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].amplitudeBLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==5){
										sheet.modbusPartDisChg[i].freqBUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].freqBLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==6){
										sheet.modbusPartDisChg[i].dischargeBUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].dischargeBLowLimit = lowLimit;
									  return  true;
								}
								if(sensorType==7){
										sheet.modbusPartDisChg[i].amplitudeCUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].amplitudeCLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==8){
										sheet.modbusPartDisChg[i].freqCUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].freqCLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==9){
										sheet.modbusPartDisChg[i].dischargeCUpLimit  = upLimit;
									  sheet.modbusPartDisChg[i].dischargeCLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
				break;
			case PRESSSETTL:
				for(i=0;i<PRESSSETTL_485_NUM;i++){
						if(rt_strcmp(sheet.pressSetl[i].ID,ID)==0){//�ҵ���ͬID��
								if(sensorType==1){
										sheet.modbusPreSettl[i].tempUpLimit  = upLimit;
									  sheet.modbusPreSettl[i].tempLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==2){
										sheet.modbusPreSettl[i].heightUpLimit = upLimit;
										sheet.modbusPreSettl[i].heightLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
				break;
			case THREEAXIS:
				for(i=0;i<THREEAXIS_485_NUM;i++){
						if(rt_strcmp(sheet.threeAxiss[i].ID,ID)==0){//�ҵ���ͬID��
								if(sensorType==1){
										sheet.modbusThreAxis[i].tempUpLimit  = upLimit;
									  sheet.modbusThreAxis[i].tempLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==2){
										sheet.modbusThreAxis[i].accXUpLimit  = upLimit;
										sheet.modbusThreAxis[i].accXLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==3){
										sheet.modbusThreAxis[i].accYUpLimit  = upLimit;
										sheet.modbusThreAxis[i].accYLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==4){
										sheet.modbusThreAxis[i].accZUpLimit  = upLimit;
										sheet.modbusThreAxis[i].accZLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
				break;
#ifdef  USE_4GAS
			case CH4:
				for(i=0;i<CH4_485_NUM;i++){
						if(rt_strcmp(sheet.ch4[i].ID,ID)==0){//�ҵ���ͬID��
								if(sensorType==1){
										sheet.modbusCh4[i].ch4UpLimit  = upLimit;
									  sheet.modbusCh4[i].ch4LowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
			case O2:
				for(i=0;i<O2_485_NUM;i++){
						if(rt_strcmp(sheet.o2[i].ID,ID)==0){//�ҵ���ͬID��
								if(sensorType==1){
										sheet.modbusO2[i].o2UpLimit  = upLimit;
									  sheet.modbusO2[i].o2LowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
			case H2S:
				for(i=0;i<H2S_485_NUM;i++){
						if(rt_strcmp(sheet.h2s[i].ID,ID)==0){//�ҵ���ͬID��
								if(sensorType==1){
										sheet.modbusH2s[i].h2sUpLimit  = upLimit;
									  sheet.modbusH2s[i].h2sLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
			case CO:
				for(i=0;i<CO_485_NUM;i++){
						if(rt_strcmp(sheet.co[i].ID,ID)==0){//�ҵ���ͬID��
								if(sensorType==1){
										sheet.modbusCo[i].coUpLimit  = upLimit;
									  sheet.modbusCo[i].coLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
#endif
			case TEMPHUM:
				for(i=0;i<TEMPHUM_485_NUM;i++){
						if(rt_strcmp(sheet.tempHum[i].ID,ID)==0){//�ҵ���ͬID��
								if(sensorType==1){
										sheet.modbusTempHum[i].tempUpLimit  = upLimit;
									  sheet.modbusTempHum[i].tempLowLimit = lowLimit;
									  return  true;
								}
								else if(sensorType==2){
										sheet.modbusTempHum[i].humUpLimit  = upLimit;
									  sheet.modbusTempHum[i].humLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
			case WATERDEPTH:
				for(i=0;i<WATERDEPTH_485_NUM;i++){
						if(rt_strcmp(sheet.waterDepth[i].ID,ID)==0){//�ҵ���ͬID��
								if(sensorType==1){
										sheet.modbusWaterDepth[i].depthUpLimit  = upLimit;
									  sheet.modbusWaterDepth[i].depthLowLimit = lowLimit;
									  return  true;
								}
								else
										return  false;
						}
				}
			  break;
			default:
				rt_kprintf("%s ERR:modbusThresholdConfig %d\n",sign,num);
				break;
		}
		return false;
}
/*
����(threshold+����+ID+����(1-modbus 2-analog)+ѡ��(1.2.3 ֻ��һ��ʱ������Ϊ1)+��ֵ����+��ֵ����)(��ֵΪ0����0xffffffffʱ����ֵ������)
threshold��ʪ�� GYNJLXSD000000164 1 1 10 80
��ֵ�����������
threshold���̶�ͷ��
���ƣ�modbus��ģ�⴫��������
���ͣ�(1-modbus 2-analog)  Ϊ0��255ʱ��ɾ����Ӧ����ֵ����
ѡ�����������ֻ��һ�������ֵΪ1���ж�����������ε���(1.2.3)����
     ģ����ʪ�ȣ�1-�¶�  2-ʪ��
	 modbus��ʪ�ȣ�1-�¶� 2-ʪ��
     modbus�����ƣ�1-�¶� 2-x������ٶ� 3-y������ٶ� 4-z������ٶ�
     modbus�����ǣ�1-�¶� 2-�߶�
     modbus������ 1-cirCurA 2-cirCurB 3-cirCurC
     modbus�ַţ�1-pdA 2-freqA 3-dischargeDataA            
				   4-pdB 5-freqB 6-dischargeDataB				
7-pdC 8-freqC 9-dischargeDataC 
��ֵ���ޣ�0-99999999(Ϊ0��ֵ������)
��ֵ���ޣ�0-99999999(Ϊ0��ֵ������)

*/

static void threshold(int argc,char *argv[])
{
	  if(0==rt_strcmp((char *)"list", argv[1])){
				printfThresholdList();
				return;
		}
		if(argc!=7){
				goto ERR;
		}
		int sensorType= atoi32(argv[3],10);
		int sensorSubName=atoi32(argv[4],10);
		if((sensorType!=1)||(sensorType!=2)){
				rt_kprintf("%s sensorType argv[3] should be 1 or 2\n",sign);
			  goto ERR;
		}
		if(sensorSubName>100){
				rt_kprintf("%s sensorSubName argv[4] should be <100\n",sign);
			  goto ERR;
		}
		float uplimit  = atof(argv[5]);
		float lowlimit = atof(argv[6]);
		if(sensorType==1){//modbus������
			  int i=0;
			  for(i=0;i<MODBUS_NUM;i++){
						if(rt_strcmp(argv[1],modbusName[i])==0){
								if(true==modbusThresholdConfig(i,argv[2],sensorType,uplimit,lowlimit)){
									  rt_kprintf("%s threshold mb OK\n",sign);
										return;
								}
								else{
										goto ERR;
								}
						}
				}
		}
		if(sensorType==2){//analog������
			  int i=0;
			  for(i=0;i<ANALOGNAME_NUM;i++){
						if(rt_strcmp(argv[1],modbusName[i])==0){
								if(true==analogThresholdConfig(i,argv[2],sensorType,uplimit,lowlimit)){
										rt_kprintf("%s threshold ana OK\n",sign);
										return;
								}
								else{
										goto ERR;
								}
						}
				}	
		}
		return;
		ERR:
		rt_kprintf("%sfor example\n",sign);
		rt_kprintf("%s[threshold ��ʪ�� GYNJLXSD000000164 1 1 10 80]\n",sign);
		rt_kprintf("%s[��ֵ������Ϊ0ʱ�� ��������ֵ]\n",sign);

}
MSH_CMD_EXPORT(threshold,threshold config);