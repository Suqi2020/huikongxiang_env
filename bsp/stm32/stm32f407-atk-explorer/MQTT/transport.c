/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/

#include <main.h>

#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1
#endif

#if defined(WIN32)
/* default on Windows is 64 - increase to make Linux and Windows the same */
#define FD_SETSIZE 1024
#include <winsock2.h>
#include <ws2tcpip.h>
#define MAXHOSTNAMELEN 256
#define EAGAIN WSAEWOULDBLOCK
#define EINTR WSAEINTR
#define EINVAL WSAEINVAL
#define EINPROGRESS WSAEINPROGRESS
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ENOTCONN WSAENOTCONN
#define ECONNRESET WSAECONNRESET
#define ioctl ioctlsocket
#define socklen_t int
#else
#define INVALID_SOCKET SOCKET_ERROR
//#include <sys/socket.h>
//#include <sys/param.h>
//#include <sys/time.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
//#include <arpa/inet.h>
//#include <netdb.h>
#include <stdio.h>
//#include <unistd.h>
//#include <errno.h>
//#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <W5500_conf.h>
#endif


/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
//static int mysock = INVALID_SOCKET;

extern rt_mutex_t   netSend_mutex;

int transport_sendPacketBuffer(unsigned char* str, int lenth)
{
	rt_mutex_take(netSend_mutex, RT_WAITING_FOREVER);
	extern uint16_t netSend(uint8_t *data,int len);
	netSend(str,lenth);
  rt_mutex_release(netSend_mutex);
	return 0;
}

extern uint32_t gu32RecDataLen;
extern uint8_t  NetRxBuffer[TX_RX_MAX_BUF_SIZE];
int transport_getdata(unsigned char* buf, int count)
{
	//int rc = wifiUartRec(buf,count);
	int len=0;
//	rt_kprintf("<%d>",count);
	while(count--){
			buf++;//指针偏移  读取实际的个数
		//(uint8_t*)NetRxBuffer++;
			len++;//指针偏移  读取实际的个数+1
	}
	//printf("received %d bytes count %d\n", rc, (int)count);
	return len; //不能返回count  实际读取的数据个数
}

//int transport_getdatanb(void *sck, unsigned char* buf, int count)
//{
//	int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
//	/* this call will return after the timeout set on initialization if no bytes;
//	   in your system you will use whatever you use to get whichever outstanding
//	   bytes your socket equivalent has ready to be extracted right now, if any,
//	   or return immediately */
//	int rc = recv(sock, buf, count, 0);	
//	if (rc == -1) {
//		/* check error conditions from your system here, and return -1 */
//		return 0;
//	}
//	return rc;
//}
