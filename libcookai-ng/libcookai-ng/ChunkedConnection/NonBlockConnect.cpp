/*
 * Copyright (c) 2006 IIMURA Takuji. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * $Id$
 */

#include "../tools/tools.h"

#include "NonBlockConnect.h"

#include <string.h>
#include <errno.h>
#include <sys/types.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <fcntl.h>
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif


namespace Cookai{
namespace ChunkedConnection {
    NonBlockConnect::NonBlockConnect(void){
	fd = -1;
	res = NULL;
	res0 = NULL;
	status = NOT_INITIALIZED;
	remote = NULL;
    }
 
    NonBlockConnect::~NonBlockConnect(void){
	if(res0 != NULL)
	    freeaddrinfo(res0);
	if(remote != NULL)
	    free(remote);
    }

    bool NonBlockConnect::LookupIPPort(char *remote, char *newNameBuf, char *newServiceBuf){
	if(remote == NULL || newNameBuf == NULL || newServiceBuf == NULL)
	    return false;
	*newNameBuf = *newServiceBuf = '\0';
	/* now use DNS or IP addr */
	if(strncmp(remote, "tcpip://", 8) == 0){
	    char *sp = &remote[8];
	    char *dp = newNameBuf;
	    while(*sp != '/' && *sp != '\0'){
		*dp = *sp;
		sp++; dp++;
	    }
	    *dp = '\0';
	    while(*sp == '/' && *sp != '\0')
		sp++;
	    if(*sp == '\0'){
		strcpy(newServiceBuf, "1203"); // XXXX magic word 12/03
		return true;
	    }
	    dp = newServiceBuf;
	    while(*sp != '\0'){
		*dp = *sp;
		sp++; dp++;
	    }
	    *dp = '\0';
	    return true;
	}
	return false;
    }

    bool NonBlockConnect::LookupTarget(void){
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	if(res0 != NULL)
	    freeaddrinfo(res0);
	char nameBuf[1024];
	char serviceBuf[1024];

	LookupIPPort(remote, nameBuf, serviceBuf);

	DPRINTF(10, ("resolving for connect %s\r\n", remote));

	if(getaddrinfo(nameBuf, serviceBuf, &hints, &res0)){
	    status = NOT_INITIALIZED;
	    return false;
	}
	status = INITIALIZED;
	res = NULL;
	return true;
    }

    bool NonBlockConnect::SetTarget(char *Remote){
	remote = strdup(Remote);
	if(remote == NULL)
	    return false;
	return LookupTarget();
    }

    NonBlockConnect::ConnectionStatus NonBlockConnect::Run(int *fd_return){
	if(status == NOT_INITIALIZED)
	    return NOT_INITIALIZED;
	if(status == CONNECTED){
	    *fd_return = fd;
	    return CONNECTED;
	}
	if(fd >= 0){
	    int ret;
	    errno = 0;
	    DPRINTF(10, ("NonBlockConnect::Run() try FD: %d\r\n", fd));
#ifdef HAVE_WSAGETLASTERROR
	    WSASetLastError(0);
#endif
#ifdef HAVE_WSACONNECT
	    ret = WSAConnect((SOCKET)fd, res->ai_addr, (int)res->ai_addrlen, NULL, NULL, NULL, NULL); 
#else
	    ret = connect(fd, res->ai_addr, (int)res->ai_addrlen);
#endif
	    if(ret < 0){
		if(0
#ifdef EWOULDBLOCK
		   || errno == EWOULDBLOCK
#endif
#ifdef HAVE_WSAGETLASTERROR
		   || WSAGetLastError() == WSAEWOULDBLOCK
		   || WSAGetLastError() == WSAEALREADY
#endif
#ifdef EAGAIN
		   || errno == EAGAIN
#endif
		    ){
			*fd_return = fd;
			status = TRYING;
			DPRINTF(10, ("    connect trying. fd: %d\r\n", fd));
			return TRYING;
		}
#ifdef HAVE_WSAGETLASTERROR
		else if(WSAGetLastError() == WSAEISCONN){
		    status = CONNECTED;
		    *fd_return = fd;
		    if(res0 != NULL)
			freeaddrinfo(res0);
		    res0 = res = NULL;
		    DPRINTF(10, ("connected. fd: %d\r\n", fd));
		    return CONNECTED;
		}
#endif
#ifdef HAVE_CLOSESOCKET
		closesocket(fd);
#else
		close(fd);
#endif
		status = FAILED;
#ifdef DEBUG
#ifdef HAVE_WSAGETLASTERROR
		// error list http://homepage1.nifty.com/yito/anhttpd/winsock_error.html
		int err = WSAGetLastError();
#else
		int err = errno;
#endif /* HAVE_WSAGETLASTERROR */
		DPRINTF(10, ("NonBlockConnect::Run() err: %d FD:%d\r\n", err, fd));
#endif /* DEBUG */
		fd = -1;
		return Run(fd_return);
	    }
	    *fd_return = fd;
	    status = CONNECTED;
	    if(res0 != NULL)
		freeaddrinfo(res0);
	    res0 = res = NULL;
	    DPRINTF(10, ("CONNECT FINISH fd; %d\r\n", fd));
	    return CONNECTED;
	}

	if(res0 == NULL)
	    return NOT_INITIALIZED;
	if(res == NULL){
	    res = res0;
	}else{
	    res = res->ai_next;
	    if(res == NULL){
		freeaddrinfo(res0);
		res0 = NULL;
		LookupTarget();
		return FAILED;
	    }
	}

#ifdef HAVE_WSASOCKET
	fd = (int)WSASocket(res->ai_family, res->ai_socktype, res->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED); 
#else
	fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
#endif
	DPRINTF(10, ("Creating socket for connect %d\r\n", fd));
#ifdef HAVE_FCNTL
	fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
#ifdef HAVE_IOCTLSOCKET
	{
	    unsigned long argp = 1;
	    ioctlsocket(fd, FIONBIO, &argp);
	}
#endif
	status = TRYING;
	return Run(fd_return);
    }
};
};
