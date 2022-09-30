#include  "dataPack.h"


heartUpStru  heartUp;
//上行数据打包
void heartUpPack()
{
	
		heartUp.mid=0x0001;
		strcpy(heartUp.packetype,"CMD_HEARTBEAT");
		strcpy(heartUp.timestamp,"123456789099");
	  strcpy(heartUp.para.identifier,"area_control_unit");
	  strcpy(heartUp.para.id,"10000000000001");
}
//上行心跳包 注册信息建立一个task  用来维护
//1、主动发送上行数据每次发送后启动定时器 如果收不到回应就一直重发  间隔5秒
//2、发送用发送邮箱 
//3、发送分2种 主动发送的  和回复的（发一次）
//  1个task
           
//下行数据解析
void AllDownPhrase(char *data,int lenth)
{
//	1、解析头尾校验 不对丢弃
//	2、提取packettype，分别校验
}