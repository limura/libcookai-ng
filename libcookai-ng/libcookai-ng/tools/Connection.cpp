/*
 * Copyright (c) 2006-2007 IIMURA Takuji. All rights reserved.
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

#include "../config.h"
#include <string>
#include <string.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#include "Connection.h"
#include "StaticBuffer.h"

#include "net.h"

namespace Cookai {

    bool Connection::LookupIPPort(char *name, char *service, char **newName, char **newService){
	if(name == NULL || service == NULL || newName == NULL || newService == NULL)
	    return false;
	/* now use DNS or IP addr */
	remoteName = strdup(name);
	remoteService = strdup(service);
	return true;
    }

    bool Connection::Initialize(char *name, char *service, size_t newChunkSize){
	fd = -1;
	chunkSize = 500;
	if(newChunkSize > 500)
	    chunkSize = newChunkSize;
	remoteName = NULL;
	remoteService = NULL;
	readBuffer = NULL;
	writeBuffer = NULL;

	for(int i = 0; i < COOKAI_CONNECTION_MAX_CHANNEL; i++){
	    blockReadHandler[i] = NULL;
	    streamReadHandler[i] = NULL;
	}

	if(name == NULL || service == NULL)
	    return false;
	LookupIPPort(name, service, &remoteName, &remoteService);

	return true;
    }

    bool Connection::Handshake(void){
	if(fd < 0)
	    return false;
	/* Nothing to do now! */
	return true;
    }

    bool Connection::Read(void){
	if(readBuffer == NULL){
	    readBuffer = new StaticBuffer(chunkSize);
	    if(readBuffer == NULL)
		return false;
	}
	/* XXX */
	readBuffer->readFromFD(fd, 1);
    }

    Connection::Connection(char *name, char *service, size_t newChunkSize){
	Initialize(name, service, newChunkSize);
    }

    Connection::Connection(std::string name, std::string service, size_t newChunkSize){
	Initialize((char *)name.c_str(), (char *)service.c_str(), newChunkSize);
    }

    Connection::~Connection(void){
	if(remoteName != NULL)
	    free(remoteName);
	if(remoteService != NULL)
	    free(remoteService);
	if(readBuffer != NULL)
	    delete readBuffer;
	if(writeBuffer != NULL)
	    delete writeBuffer;
    }

    bool Connection::IsConnect(){
	if(fd >= 0)
	    return true;
	return false;
    }

    bool Connection::Connect(){
	if(remoteName == NULL || remoteService == NULL)
	    return false;

	fd = connect_stream(remoteName, remoteService);
#ifdef HAVE_FCNTL
	fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
#ifdef HAVE_IOCTLSOCKET
	{
	    unsigned long argp = 1;
	    ioctlsocket(fd, FIONBIO, &argp);
	}
#endif
	{
	    int optval;
	    socklen_t optlen;
	    optval = 1;
	    optlen = sizeof(optval);
	    if(setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, optlen) != 0)
		return false; // XXXXXXXX
	}
	return Handshake();
    }



};
