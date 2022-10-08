#ifndef _LOOPBACK_H
#define _LOOPBACK_H

#include "board.h"
#include "socket.h"
#include "w5500.h"
#include "types.h"


//#define   NET_LEN  2048

#define IINCHIP_ISR_DISABLE()
#define IINCHIP_ISR_ENABLE()	

void loopback_tcps(SOCKET s, uint16 port);
void loopback_tcpc(SOCKET s, uint16 port);
void loopback_udp(SOCKET s, uint16 port);
void W5500ISR(void);
void w5500Init(void);
#endif
