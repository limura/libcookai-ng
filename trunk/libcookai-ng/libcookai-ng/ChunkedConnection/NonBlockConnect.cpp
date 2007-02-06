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

#include "NonBlockConnect.h"

namespace Cookai{
namespace ChunkedConnection {
    NonBlockConnect::NonBlockConnect(void){
	fd = -1;
	res = NULL;
	res0 = NULL;
	status = NOT_INITIALIZED;
    }
 
    NonBlockConnect::~NonBlockConnect(void){
	if(res0 != NULL)
	    freeaddrinfo(res0);
    }

    bool NonBlockConnect::SetTarget(char *name, char *service){
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	if(getaddrinfo(name, service, &hints, &res0)){
	    status = NOT_INITIALIZED;
	    return false;
	}
	status = INITIALIZED;
	res = NULL;
	return true;
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
	    ret = connect(fd, res->ai_addr, (int)res->ai_addrlen);
	    if(ret < 0){
		if(
#ifdef EWOULDBLOCK
		    errno == EWOULDBLOCK
#endif
#ifdef HAVE_WSAGETLASTERROR
		    WSAGetLastError() == WSAEWOULDBLOCK
#endif
#ifdef EAGAIN
		    errno == EAGAIN
#endif
		    ){
			*fd_return = fd;
			status = TRYING;
			return TRYING;
		}
#ifdef HAVE_CLOSESOCKET
		closesocket(fd);
#else
		close(fd);
#endif
		fd = -1;
		return Run(fd_return);
	    }
	    *fd_return = fd;
	    status = CONNECTED;
	    return CONNECTED;
	}

	if(res0 == NULL)
	    return NOT_INITIALIZED;
	if(res == NULL){
	    res = res0;
	}else{
	    res = res->ai_next;
	    if(res == NULL)
		return FAILED;
	}

	fd = (int)socket(res->ai_family, res->ai_socktype, res->ai_protocol);
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