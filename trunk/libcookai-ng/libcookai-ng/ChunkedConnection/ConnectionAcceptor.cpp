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

#include "../config.h"
#include "../tools/net.h"
#include "../tools/tools.h"

#include <stdio.h>

#include <errno.h>
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

#include "Connector.h"
#include "ConnectionAcceptor.h"
#include "ChunkedConnection.h"

namespace Cookai {
namespace ChunkedConnection {

    ConnectionAcceptor::ConnectionAcceptor(EventPool *ep, ReadHandler AcceptEventHandler){
	acceptSocketFD = -1;
	connectionManager = NULL;
	eventPool = ep;
	acceptEventHandler = AcceptEventHandler;
    }

    ConnectionAcceptor::~ConnectionAcceptor(void){
	if(acceptSocketFD >= 0){
#ifdef _LIBCOOKAI_WINDOWS_
	shutdown(acceptSocketFD, SD_BOTH);
	closesocket(acceptSocketFD);
#else
	shutdown(acceptSocketFD, SHUT_RDWR);
	close(acceptSocketFD);
#endif
	}
	acceptSocketFD = -1;
    }

    int ConnectionAcceptor::Connect(char *serviceName){
	if(acceptSocketFD >= 0)
	    return acceptSocketFD;

	return acceptSocketFD = listen_stream(serviceName, PF_INET);
    }
    int ConnectionAcceptor::Connect(void){
	return Connect(COOKAI_DEFAULT_ACCEPT_PORT);
    }

    void ConnectionAcceptor::RegisterConnectionManager(Cookai::ChunkedConnection::ConnectionManager *cm){
	connectionManager = cm;
    }

    bool ConnectionAcceptor::Run(Cookai::ChunkedConnection::ConnectionStatus status){
	if(status & Cookai::ChunkedConnection::CONNECTION_STATUS_READ_OK){
	    sockaddr_storage addrs;
	    socklen_t addrlen = sizeof(addrs);
	    int acceptedSocket = (int)accept(acceptSocketFD, (sockaddr *)&addrs, &addrlen);
	    if(acceptedSocket >= 0){
		DPRINTF(10, ("new connection got. FD: %d (accept)\r\n", acceptedSocket));
		char nameBuf[1024], serviceBuf[64];
		nameBuf[0] = serviceBuf[0] = '\0';
		switch(addrs.ss_family){
		case AF_INET:
		    {
			struct sockaddr_in *sockadd_in = (struct sockaddr_in *)&addrs;
#ifdef HAVE__SNPRINTF
			_snprintf(nameBuf, sizeof(nameBuf), "%s", inet_ntoa(sockadd_in->sin_addr));
			_snprintf(serviceBuf, sizeof(serviceBuf), "%d", htons(sockadd_in->sin_port));
#else
			snprintf(nameBuf, sizeof(nameBuf), "%s", inet_ntoa(sockadd_in->sin_addr));
			snprintf(serviceBuf, sizeof(serviceBuf), "%d", htons(sockadd_in->sin_port));
#endif
		    }
		    break;
		default:
#ifdef _LIBCOOKAI_WINDOWS_
		    shutdown(acceptedSocket, SD_BOTH);
		    closesocket(acceptedSocket);
#else
		    shutdown(acceptedSocket, SHUT_RDWR);
		    close(acceptedSocket);
#endif
		    return false;
		    break;
		}
		ChunkedConnection *cc = new Cookai::ChunkedConnection::ChunkedConnection(acceptedSocket, nameBuf, serviceBuf, eventPool);
		if(cc == NULL)
		    return false;
		if(connectionManager != NULL){
		    connectionManager->UpdateSelectStatus(cc, acceptedSocket, Cookai::ChunkedConnection::CONNECTION_STATUS_READ_OK);
		    cc->RegisterConnectionManager(connectionManager);
		}
		Cookai::ChunkedConnection::Event *ev = new Cookai::ChunkedConnection::Event(Cookai::ChunkedConnection::EVENT_ACCEPT_NEW_SOCKET,
		    NULL, cc, 0, acceptEventHandler);
		if(ev == NULL)
		    return false;
		if(eventPool != NULL)
		    eventPool->AddEvent(ev);
	    }
	}
	return true;
    }

};
};

