#ifndef   __DATAPHRASE_H__
#define   __DATAPHRASE_H__

#include   "board.h"
#include   "rtdef.h"

//û��ö����
typedef enum
{
		heartResp=0,
	  devRegResp,
	  repDataResp,
	  CMDRepData,
	  CMDRepEvt,
	  CMDReqData,
	  CMDReqHis,
	  errResp=0xff
}packTypeEnum;












#endif



