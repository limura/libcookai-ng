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

#include "Connection.h"
#include "StaticBuffer.h"
#include "Event.h"

#include "../tools/net.h"

namespace Cookai {
namespace ChunkedConnection {

    bool Connection::LookupIPPort(char *name, char *service, char **newName, char **newService){
	if(name == NULL || service == NULL || newName == NULL || newService == NULL)
	    return false;
	*newName = *newService = NULL;
	/* now use DNS or IP addr */
	*newName = strdup(name);
	if(*newName == NULL)
	    return false;
	*newService = strdup(service);
	if(*newService == NULL){
	    free(*newName);
	    return false;
	}
	return true;
    }

    bool Connection::Handshake(void){
	if(fd < 0)
	    return false;
	/* Nothing to do now! */
	return true;
    }

    bool Connection::Initialize(char *name, char *service, size_t newChunkSize){
	fd = -1;
	nbConnect = NULL;
	status = STATUS_DISOCNNECTED;
	chunkSize = 500;
	if(newChunkSize > 500)
	    chunkSize = newChunkSize;
	remoteName = NULL;
	remoteService = NULL;
	readBuffer = NULL;
	readHeaderBuffer = new StaticBuffer(COOKAI_CHUNKEDCONNECTION_HEADERLENGTH);
	writeBufferList.clear();
	thread_mutex_init(&writeBufferMutex, NULL);
	streamEvent = NULL;
	blockEvent = NULL;
	blockChunkLength = 0;
	nowChunkNum = 0;

	if(name == NULL || service == NULL)
	    return false;
	remoteName = strdup(name);
	if(remoteName == NULL)
	    return false;
	remoteService = strdup(service);
	if(remoteService == NULL){
	    free(remoteName);
	    remoteName = NULL;
	    return false;
	}

	return true;
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
	if(IsConnect()){
	    return fd;
	}
	if(Connect()){
	    return fd;
	}
	return -1;
    }

    bool Connection::Connect(){
	if(remoteName == NULL || remoteService == NULL)
	    return false;

	if(fd < 0){
	    if(nbConnect == NULL){
		nbConnect = new NonBlockConnect();
		if(nbConnect == NULL)
		    return false;
		{
		    char *name, *service;
		    if(LookupIPPort(remoteName, remoteService, &name, &service) != true)
			return false;
		    if(nbConnect->SetTarget(remoteName, remoteService) != true){
			free(name); free(service);
			return false;
		    }
		    free(name); free(service);
		}
	    }
	    switch(nbConnect->Run(&fd)){
		case NonBlockConnect::CONNECTED:
		    return true;
		    break;
		case NonBlockConnect::TRYING:
		    return true;
		    break;
		case NonBlockConnect::FAILED:
		default:
		    return false;
	    }
	    return false;
	}

	nbConnect->Run(&fd);
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

    void Connection::Disconnect(){
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

    Cookai::ChunkedConnection::EventType Connection::Run(Event **eventReturn){
	if(eventReturn != NULL)
	    *eventReturn = NULL;
	if(fd < 0)
	    Connect();
	if(fd < 0)
	    return Cookai::ChunkedConnection::EVENT_NOTHING;
Run_StreamReadPart:
	if(streamEvent != NULL){
	    int ret;
	    StaticBuffer *sb = streamEvent->GetBuffer();
	    if(sb == NULL || (ret = sb->ReadFromSocket(fd)) == 0){
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
	    goto Run_WritePart;
	}
Run_BlockReadPart:
	if(blockEvent != NULL && blockChunkLength > 0){
	    int ret;
	    StaticBuffer *sb = blockEvent->GetBuffer();
	    if(sb == NULL || (ret = sb->ReadFromSocket(fd, &blockChunkLength)) == 0){
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
	    goto Run_WritePart;
	}
Run_ChunkHeaderReadPart:
	if(readHeaderBuffer->GetAvailableSize() > 0){
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
		if(header.nextChunkNum == 0){ // Stream data.
		    StaticBuffer *tmpStreamBuffer = new StaticBuffer(header.length);
		    if(tmpStreamBuffer == NULL)
			goto Run_SocketError;
		    streamEvent = new Event(Cookai::ChunkedConnection::EVENT_RECIVE_STREAM, tmpStreamBuffer, header.channel);
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
			blockEvent = new Event(Cookai::ChunkedConnection::EVENT_RECIVE_BLOCK, tmpBlockBuffer, header.channel);
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

Run_WritePart:
	thread_mutex_lock(&writeBufferMutex);
	while(!writeBufferList.empty()){
	    StaticBuffer *sb = writeBufferList.front();
	    if(sb != NULL){
		int ret = sb->WriteToSocket(fd);
		if(ret == 0){ // this buffer send complete;
		    delete sb;
		    writeBufferList.pop_front();
		    continue;
		}else if(ret < 0){
		    if(eventReturn != NULL)
			*eventReturn = NULL;
		    thread_mutex_unlock(&writeBufferMutex);
		    goto Run_SocketError;
		}
		break;
	    }

	}
	thread_mutex_unlock(&writeBufferMutex);
	return Cookai::ChunkedConnection::EVENT_NOTHING;

Run_SocketError:
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
	writeBufferList.push_back(buf);
	thread_mutex_unlock(&writeBufferMutex);
	return true;
    }

};
};
