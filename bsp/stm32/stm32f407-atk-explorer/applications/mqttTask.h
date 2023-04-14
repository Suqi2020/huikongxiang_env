#ifndef __mqttthread_h__
#define __mqttthread_h__

#include "main.h"




#define  MQTTKEEPALIVE_TIME_S      60

typedef enum{
	conMQTT_enum=0,
	subMQTT_enum,
	pubGetUtcMqtt_enum,
	pubMQTT_enum,
	pingMQTT_enum,
	dealwithMQTT_enum,
	resetMQTT_enum,
}MQTTEnum;

#define  MQTT_PACK_CNT   10
typedef struct{
	char *topic;
	char *msg;
	int msglen;
	int (*fun)(char *buf);
}MQTTDataPacktStru;
///extern MQTTDataPacktStru  MQTTDataPackt[MQTT_PACK_LEN];

extern int  mqttLoopData(void);
extern bool  mqttState(void);
extern void mqttStateSet(bool state);





#endif


