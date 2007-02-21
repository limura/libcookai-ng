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
#include "../tools/tools.h"
#include <string>
#include <string.h>

#include <sys/types.h>
#include <errno.h>
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

#include "Connection.h"
#include "StaticBuffer.h"
#include "Event.h"

#include "../tools/net.h"

namespace Cookai {
namespace ChunkedConnection {

    bool Connection::Handshake(void){
	if(fd < 0)
	    return false;
	/* Nothing to do now! */
	return true;
    }

    bool Connection::Initialize(char *Remote, size_t newChunkSize){
	fd = -1;
	nbConnect = NULL;
	status = STATUS_DISCONNECTED;
	chunkSize = 500;
	if(newChunkSize > 500)
	    chunkSize = newChunkSize;
	remote = NULL;
	readBuffer = NULL;
	readHeaderBuffer = new StaticBuffer(COOKAI_CHUNKEDCONNECTION_HEADERLENGTH);
	writeBufferList.clear();
	thread_mutex_init(&writeBufferMutex, NULL);
	streamEvent = NULL;
	blockEvent = NULL;
	blockChunkLength = 0;
	nowChunkNum = 0;

	if(Remote == NULL)
	    return false;
	remote = strdup(Remote);
	if(remote == NULL)
	    return false;

	return true;
    }

    Connection::Connection(char *remote, size_t newChunkSize){
	Initialize(remote, newChunkSize);
    }

    Connection::Connection(std::string remote, size_t newChunkSize){
	Initialize((char *)remote.c_str(), newChunkSize);
    }
    Connection::Connection(int acceptedFD, char *remote, size_t newChunkSize){
	Initialize(remote, newChunkSize);
	status = STATUS_CONNECTED;
	fd = acceptedFD;
    }

    Connection::~Connection(void){
	if(remote != NULL)
	    free(remote);
	if(readBuffer != NULL)
	    delete readBuffer;
	while(!writeBufferList.empty()){
	    StaticBuffer *sb = writeBufferList.front();
	    delete sb;
	    writeBufferList.pop_front();
	}
	if(nbConnect != NULL)
	    delete nbConnect;
	thread_mutex_destroy(&writeBufferMutex);
    }

    bool Connection::IsConnect(){
	if(fd >= 0 && status == STATUS_CONNECTED)
	    return true;
	return false;
    }

    int Connection::GetFD(void){
	return fd;
	if(IsConnect()){
	    return fd;
	}
	if(Connect()){
	    return fd;
	}
	return -1;
    }

    bool Connection::Connect(){
	if(remote == NULL)
	    return false;

	if(fd >= 0 && status == STATUS_CONNECTED)
	    return true;

	if(nbConnect == NULL){
	    nbConnect = new NonBlockConnect();
	    if(nbConnect == NULL)
		return false;
	    if(nbConnect->SetTarget(remote) == false){
		delete nbConnect;
		nbConnect = NULL;
		return false;
	    }
	}
	DPRINTF(10, ("connecting to %s (%d)\r\n", remote, fd));
	switch(nbConnect->Run(&fd)){
	case NonBlockConnect::CONNECTED:
	    status = STATUS_CONNECTED;
	    DPRINTF(10, ("connect success %d\r\n", fd));
	    {
		int optval;
		socklen_t optlen;
		optval = 1;
		optlen = sizeof(optval);
		if(setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, optlen) != 0){
		    // nothing to do! we must not need TCP_NODELAY.
		}
	    }
	    return Handshake();
	case NonBlockConnect::TRYING:
	    status = STATUS_CONNECTING;
	    return true;
	case NonBlockConnect::FAILED:
	default:
	    status = STATUS_DISCONNECTED;
	    DPRINTF(10, ("connect FAILED %d\r\n", fd));
	    return false;
	}
	return false;
    }

    void Connection::Disconnect(){
	status = STATUS_DISCONNECTED;
	if(fd >= 0){
	    shutdown(fd, 2);
#ifdef HAVE_CLOSESOCKET
	    closesocket(fd);
#else
	    close(fd);
#endif
	    fd = -1;
	}
    }

    Cookai::ChunkedConnection::EventType Connection::Run(Event **eventReturn, Cookai::ChunkedConnection::ConnectionStatus connectionStatus){
	EventType eventType;
	DPRINTF(10, ("Connection Run(%d) FD:%d\r\n", connectionStatus, fd));
	if(connectionStatus & Cookai::ChunkedConnection::CONNECTION_STATUS_WRITE_OK
	    && status == STATUS_CONNECTED){
	    eventType = RunWrite(eventReturn);
	    if(eventType != Cookai::ChunkedConnection::EVENT_NOTHING)
		return eventType;
	}
	if(connectionStatus & Cookai::ChunkedConnection::CONNECTION_STATUS_READ_OK)
	    return RunRead(eventReturn);
	return Cookai::ChunkedConnection::EVENT_NOTHING;
    }

    Cookai::ChunkedConnection::EventType Connection::RunWrite(Event **eventReturn){
	if(eventReturn != NULL)
	    *eventReturn = NULL;
	if(fd < 0){
	    Connect();
	}else if(status != STATUS_CONNECTED){
	    Connect();
	    if(status == STATUS_DISCONNECTED){
		Disconnect();
		return Cookai::ChunkedConnection::EVENT_ERROR_SOCKET_CLOSE;
	    }
	}

	if(fd < 0 || status != STATUS_CONNECTED)
	    return Cookai::ChunkedConnection::EVENT_NOTHING;

	thread_mutex_lock(&writeBufferMutex);
	while(!writeBufferList.empty()){
	    StaticBuffer *sb = writeBufferList.front();
	    if(sb != NULL){
DPRINTF(10, (" WriteToSocket(%d) %d bytes\r\n", fd, sb->GetDataLength()));
#ifdef DEBUG
{
unsigned char *p = sb->GetBuffer();
DPRINTF(10, ("  %02x%02x%02x%02x %c%c%c%c\r\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]));
}
#endif /* DEBUG */
		int ret = sb->WriteToSocket(fd);
		if(ret == 0){ // this buffer send complete;
		    delete sb;
		    writeBufferList.pop_front();
		    continue;
		}else if(ret < 0){
		    if(eventReturn != NULL)
			*eventReturn = NULL;
		    thread_mutex_unlock(&writeBufferMutex);
		    Disconnect();
		    return Cookai::ChunkedConnection::EVENT_ERROR_SOCKET_CLOSE;
		}
		break;
	    }else{
		break;
	    }
	}
	thread_mutex_unlock(&writeBufferMutex);
	return Cookai::ChunkedConnection::EVENT_NOTHING;
    }

    Cookai::ChunkedConnection::EventType Connection::RunRead(Event **eventReturn){
	//DPRINTF(10, ("reciving from %d.\r\n", fd));
	if(eventReturn != NULL)
	    *eventReturn = NULL;
	if(fd < 0){
	    Connect();
	}else if(status != STATUS_CONNECTED){
	    Connect();
	    if(status == STATUS_DISCONNECTED)
		goto Run_SocketError;
	}
	if(fd < 0 || status != STATUS_CONNECTED)
	    return Cookai::ChunkedConnection::EVENT_NOTHING;
	DPRINTF(10, ("reciving from %d.\r\n", fd));
Run_StreamReadPart:
	if(streamEvent != NULL){
	DPRINTF(10, ("reading stream event from %d.\r\n", fd));
	    int ret;
	    StaticBuffer *sb = streamEvent->GetBuffer();
	    if(sb == NULL)
		return Cookai::ChunkedConnection::EVENT_NOTHING;
	    if((ret = sb->ReadFromSocket(fd)) == 0){
		sb->ReadSeek(0);
		if(eventReturn != NULL)
		    *eventReturn = streamEvent;
		else
		    delete streamEvent;
		streamEvent = NULL;
		return Cookai::ChunkedConnection::EVENT_RECIVE_STREAM;
	    }else if(ret < 0){
		delete streamEvent;
		streamEvent = NULL;
		goto Run_SocketError;
	    }
	    return Cookai::ChunkedConnection::EVENT_NOTHING;
	}
Run_BlockReadPart:
	if(blockEvent != NULL && blockChunkLength > 0){
	DPRINTF(10, ("reading block event from %d.\r\n", fd));
	    int ret;
	    StaticBuffer *sb = blockEvent->GetBuffer();
	    if(sb == NULL)
		return Cookai::ChunkedConnection::EVENT_NOTHING;
	    if((ret = sb->ReadFromSocket(fd, &blockChunkLength)) == 0){
		blockChunkLength = 0;
		if(nowChunkNum > 1)
		    goto Run_ChunkHeaderReadPart;
		sb->ReadSeek(0);
		if(eventReturn != NULL)
		    *eventReturn = blockEvent;
		else
		    delete blockEvent;
		blockEvent = NULL;
		return Cookai::ChunkedConnection::EVENT_RECIVE_BLOCK;
	    }else if(ret < 0){
		delete blockEvent;
		blockEvent = NULL;
		goto Run_SocketError;
	    }
	    return Cookai::ChunkedConnection::EVENT_NOTHING;
	}
Run_ChunkHeaderReadPart:
	if(readHeaderBuffer->GetAvailableSize() >= 0){
	DPRINTF(10, ("reading chunk header from %d.\r\n", fd));
	    int ret = readHeaderBuffer->ReadFromSocket(fd);
	    if(ret < 0){
		if(eventReturn != NULL)
		    *eventReturn = NULL;
		readHeaderBuffer->ReadSeek(0);
		goto Run_SocketError;
	    }else if(ret == 0){
		ChunkHeader header;
		readHeaderBuffer->ReadSeek(0);
		header.nextChunkNum = readHeaderBuffer->ReadUint8();
		header.channel = readHeaderBuffer->ReadUint8();
		header.length = readHeaderBuffer->ReadUint16();
	DPRINTF(10, ("readed chunk header: %d, %d, %d %d.\r\n", header.nextChunkNum, header.channel, header.length, fd));
		readHeaderBuffer->Clear();
		if(header.nextChunkNum == 0){ // Stream data.
		    StaticBuffer *tmpStreamBuffer = new StaticBuffer(header.length);
		    if(tmpStreamBuffer == NULL)
			goto Run_SocketError;
		    streamEvent = new Event(Cookai::ChunkedConnection::EVENT_RECIVE_STREAM, tmpStreamBuffer, NULL, header.channel);
		    if(streamEvent == NULL){
			delete tmpStreamBuffer;
			goto Run_SocketError;
		    }
		    goto Run_StreamReadPart;
		}else{
		    if(blockEvent == NULL){
			size_t bufferLength = header.length * header.nextChunkNum;
			if(bufferLength > COOKAI_CHUNKEDCONNECTION_MAX_DATALENGTH)
			    goto Run_SocketError;
			StaticBuffer *tmpBlockBuffer = new StaticBuffer(bufferLength);
			if(tmpBlockBuffer == NULL)
			    goto Run_SocketError;
			blockEvent = new Event(Cookai::ChunkedConnection::EVENT_RECIVE_BLOCK, tmpBlockBuffer, NULL, header.channel);
			if(blockEvent == NULL){
			    delete tmpBlockBuffer;
			    goto Run_SocketError;
			}
		    }
		    blockChunkLength = header.length;
		    nowChunkNum = header.nextChunkNum;
		    goto Run_BlockReadPart;
		}
	    }
	}
	return Cookai::ChunkedConnection::EVENT_NOTHING;

Run_SocketError:
	DPRINTF(10, ("fd: %d error. disconnect.\r\n", fd));
	Disconnect();
	return Cookai::ChunkedConnection::EVENT_ERROR_SOCKET_CLOSE;
    }

    bool Connection::NonBlockWrite(unsigned char *buf, size_t length){
	if(length <= 0 || buf == NULL)
	    return false;
	StaticBuffer *staticBuf = new StaticBuffer(length);
	if(staticBuf == NULL)
	    return false;
	staticBuf->Write(buf, length);
	return NonBlockWrite(staticBuf);
    }
    bool Connection::NonBlockWrite(StaticBuffer *buf){
	thread_mutex_lock(&writeBufferMutex);
DPRINTF(10, (" Write %d bytes append.\r\n", buf->GetDataLength()));
	writeBufferList.push_back(buf);
	thread_mutex_unlock(&writeBufferMutex);
	return true;
    }

    bool Connection::WriteQueueEmpty(void){
	return writeBufferList.empty();
    }

};
};
