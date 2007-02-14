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

#include "ChunkedConnection.h"
#include "Event.h"

namespace Cookai {
namespace ChunkedConnection {

    void ChunkedConnection::Initialize(char *remote, Cookai::ChunkedConnection::EventPool *Pool, size_t ChunkSize)
    {
	eventPool = Pool;
	blockReadHandler = NULL;
	streamReadHandler = NULL;
	errorHandler = NULL;
	threadEnable = false;
	connection = new Cookai::ChunkedConnection::Connection(remote);
	chunkSize = ChunkSize;
	channelWriteQueue.clear();
	connectionManager = NULL;
    }

    StaticBuffer *ChunkedConnection::CreateNewBuffer(void){
	StaticBuffer *buf = new StaticBuffer(chunkSize);
	if(buf == NULL)
	    return NULL;
	WriteHeader(1, 0, 0, buf);
	return buf;
    }
    void ChunkedConnection::WriteHeader(int nextChunkNum, int channel, uint16_t length, StaticBuffer *buf){
	if(buf == NULL)
	    return;
	size_t writePos = buf->GetDataLength();
	buf->WriteSeek(0);

	buf->WriteUint8(nextChunkNum);
	buf->WriteUint8(channel);
	buf->WriteUint16(length);

	if(writePos < COOKAI_CHUNKEDCONNECTION_HEADERLENGTH)
	    writePos = COOKAI_CHUNKEDCONNECTION_HEADERLENGTH;
	buf->WriteSeek(writePos);
    }

    Cookai::ChunkedConnection::ChunkedConnection::WriteQueue *ChunkedConnection::GetWriteQueue(int channel){
	WriteQueue *writeQueue = NULL;

	if(channelWriteQueue.empty()){
	    writeQueue = new WriteQueue();
	    if(writeQueue == NULL)
		return NULL;
	    writeQueue->clear();
	    channelWriteQueue.insert(std::pair<int, Cookai::ChunkedConnection::ChunkedConnection::WriteQueue *>(channel, writeQueue));
	}else{
	    writeQueue = channelWriteQueue[channel];
	}
	return writeQueue;
    }

    ChunkedConnection::ChunkedConnection(char *remote, Cookai::ChunkedConnection::EventPool *Pool, size_t ChunkSize){
	Initialize(remote, Pool, ChunkSize);
    }
    ChunkedConnection::ChunkedConnection(std::string remote, Cookai::ChunkedConnection::EventPool *Pool, size_t ChunkSize){
	Initialize((char *)remote.c_str(), Pool, ChunkSize);
    }
    ChunkedConnection::ChunkedConnection(int acceptedFD, char *remote, Cookai::ChunkedConnection::EventPool *Pool, size_t ChunkSize){
	Initialize(remote, Pool, ChunkSize);
	connection = new Cookai::ChunkedConnection::Connection(acceptedFD, remote, ChunkSize);
    }

    ChunkedConnection::~ChunkedConnection(void)
    {
	if(threadEnable)
	    thread_cancel(readThreadID);
	if(connection != NULL)
	    delete connection;
    }

    void ChunkedConnection::SetBlockReadHandler(ReadHandler handler){
	blockReadHandler = handler;
    }
    void ChunkedConnection::SetStreamReadHandler(ReadHandler handler){
	streamReadHandler = handler;
    }
    void ChunkedConnection::SetErrorHandler(ReadHandler handler){
	errorHandler = handler;
    }

    bool ChunkedConnection::BlockWrite(char *buf, int channel){
	if(buf == NULL)
	    return false;
	size_t length = strlen(buf);
	return BlockWrite((unsigned char *)buf, length, channel);
    }
    bool ChunkedConnection::BlockWrite(std::string buf, int channel){
	return BlockWrite((unsigned char *)buf.c_str(), buf.length(), channel);
    }
    bool ChunkedConnection::BlockWrite(unsigned char *buf, size_t length, int channel){
	WriteQueue *writeQueue = NULL;
	writeQueue = GetWriteQueue(channel);
	if(writeQueue == NULL)
	    return false;

	Cookai::ChunkedConnection::StaticBuffer *buffer = NULL;
	if(writeQueue->empty()){
	    buffer = CreateNewBuffer();
	    if(buffer == NULL)
		return false;
	    writeQueue->push_back(buffer);
	}
	buffer = writeQueue->back();
	unsigned char *p = buf;
	while(buffer->GetAvailableSize() < length){
	    size_t availableSize = buffer->GetAvailableSize();
	    buffer->Write(p, availableSize);
	    buffer = CreateNewBuffer();
	    if(buffer == NULL)
		return false;
	    writeQueue->push_back(buffer);
	    length -= availableSize;
	    p += availableSize;
	}
	if(length > 0)
	    buffer->Write(p, length);
	return true;
    }
    bool ChunkedConnection::BlockCommit(int channel){
	WriteQueue *writeQueue = GetWriteQueue(channel);

	if(writeQueue == NULL || connection == NULL)
	    return false;
	int num = (int)writeQueue->size();
	while(!writeQueue->empty()){
	    StaticBuffer *buf = writeQueue->front();
	    WriteHeader(num, channel, (uint16_t)buf->GetDataLength(), buf);
	    connection->NonBlockWrite(buf);
	    writeQueue->pop_front();
	    num--;
	}
	return true;
    }
    bool ChunkedConnection::StreamWrite(char *buf, int channel){
	if(buf == NULL)
	    return false;
	size_t length = strlen(buf);
	return StreamWrite((unsigned char *)buf, length, channel);
    }
    bool ChunkedConnection::StreamWrite(std::string buf, int channel){
	return StreamWrite((unsigned char *)buf.c_str(), buf.length(), channel);
    }
    bool ChunkedConnection::StreamWrite(unsigned char *buf, size_t length, int channel){
	if(buf == NULL || length <= 0 || connection == NULL)
	    return false;
	StaticBuffer *staticBuf = new StaticBuffer(length + COOKAI_CHUNKEDCONNECTION_HEADERLENGTH);
	if(staticBuf == NULL)
	    return false;
	WriteHeader(0, channel, (uint16_t)length, staticBuf);
	staticBuf->Write(buf, length);
	if(connection->WriteQueueEmpty())
	    connectionManager->UpdateSelectStatus(this, connection->GetFD(),
	    (Cookai::ChunkedConnection::ConnectionStatus)((int)Cookai::ChunkedConnection::CONNECTION_STATUS_READ_OK
	        | (int)Cookai::ChunkedConnection::CONNECTION_STATUS_WRITE_OK));
	connection->NonBlockWrite(staticBuf);
	return true;
    }

    void ChunkedConnection::RegisterConnectionManager(ConnectionManager *cm){
	connectionManager = cm;
    }
    bool ChunkedConnection::Run(Cookai::ChunkedConnection::ConnectionStatus status){
	if(status != Cookai::ChunkedConnection::CONNECTION_STATUS_NONE){
	    Cookai::ChunkedConnection::EventType eventType;
	    Cookai::ChunkedConnection::Event *ev;
	    while(1){
		switch(eventType = connection->Run(&ev, status)){
		    case Cookai::ChunkedConnection::EVENT_NOTHING:
			goto Run_UpdateStatus;
			break;
		    case Cookai::ChunkedConnection::EVENT_RECIVE_BLOCK:
			if(ev != NULL && blockReadHandler != NULL){
			    ev->SetEventHandler(blockReadHandler);
			    ev->SetChunkedConnection(this);
			}
			break;
		    case Cookai::ChunkedConnection::EVENT_RECIVE_STREAM:
			if(ev != NULL && streamReadHandler != NULL){
			    ev->SetEventHandler(streamReadHandler);
			    ev->SetChunkedConnection(this);
			}
			break;
		    case Cookai::ChunkedConnection::EVENT_ERROR_SOCKET_CLOSE:
		    case Cookai::ChunkedConnection::EVENT_ERROR_UNKNOWN:
			if(errorHandler != NULL)
			    ev = new Event(eventType, NULL, this, 0, errorHandler);
			break;
		    case Cookai::ChunkedConnection::EVENT_ACCEPT_NEW_SOCKET:
		    default:
			return false;
		}
		if(ev != NULL)
		    eventPool->AddEvent(ev);
		//break; // XXXXX multiple Run() call are break on Win32.
	    }
Run_UpdateStatus:
	    if(connection->WriteQueueEmpty())
		connectionManager->UpdateSelectStatus(this, connection->GetFD(),
		Cookai::ChunkedConnection::CONNECTION_STATUS_READ_OK);
	}else{
	    Connect();
	}
    	return true;
    }

    int ChunkedConnection::Connect(void){
	if(connection == NULL)
	    return -1;
	if(connection->IsConnect())
	    return connection->GetFD();

	connection->Connect();
	return connection->GetFD();
    }
};
}; /* namespace Cookai */
