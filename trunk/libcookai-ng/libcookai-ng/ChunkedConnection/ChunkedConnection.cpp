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
 * $Id: ChunkedConnection.cpp 25 2007-01-31 14:36:32Z uirou.j $
 */

#include "ChunkedConnection.h"

namespace Cookai {
namespace ChunkedConnection {

	void ChunkedConnection::Initialize(char *Name, char *Service, Cookai::ChunkedConnection::EventPool *Pool, size_t ChunkSize)
	{
	    eventPool = Pool;
	    blockReadHandler = NULL;
	    streamReadHandler = NULL;
	    errorHandler = NULL;
	    threadEnable = false;
	    connection = new Cookai::ChunkedConnection::Connection(Name, Service);
	    chunkSize = ChunkSize;
	    channelWriteQueue.clear();
	}

	StaticBuffer *ChunkedConnection::CreateNewBuffer(void){
	    uint16_t dummy = 0;
	    StaticBuffer *buf = new StaticBuffer(chunkSize);
	    if(buf == NULL)
		return NULL;
	    WriteHeader(1, 0, 0, buf->GetBuffer());
	    return buf;
	}
	void ChunkedConnection::WriteHeader(int nextChunkNum, int channel, uint16_t length, unsigned char *buf){
	    if(buf = NULL)
		return;
	    unsigned char uc;
	    uc = (unsigned char)nextChunkNum;
	    memcpy(buf, &uc, sizeof(uc));
	    buf += sizeof(uc);
	    uc = (unsigned char)channel;
	    memcpy(buf, &uc, sizeof(uc));
	    buf += sizeof(uc);
	    memcpy(buf, &length, sizeof(length));
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

	ChunkedConnection::ChunkedConnection(char *Name, char *Service, Cookai::ChunkedConnection::EventPool *Pool, size_t ChunkSize){
	    Initialize(Name, Service, Pool, ChunkSize);
	}
	ChunkedConnection::ChunkedConnection(std::string Name, std::string Service, Cookai::ChunkedConnection::EventPool *Pool, size_t ChunkSize){
	    Initialize((char *)Name.c_str(), (char *)Service.c_str(), Pool, ChunkSize);
	}

	ChunkedConnection::~ChunkedConnection(void)
	{
	    if(threadEnable)
		thread_cancel(readThreadID);
	    if(connection != NULL)
		delete connection;
	}


	void ChunkedConnection::SetBlockReadHandler(chunkReadHandler handler){
	    blockReadHandler = handler;
	}
	void ChunkedConnection::SetStreamReadHandler(chunkReadHandler handler){
	    streamReadHandler = handler;
	}
	void ChunkedConnection::SetErrorHandler(chunkErrorHandler handler){
	    errorHandler = handler;
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
		WriteHeader(num, channel, (uint16_t)buf->GetDataLength(), buf->GetBuffer());
		connection->NonBlockWrite(buf); // connection‚ª StaticBuffer ‚ğ delete ‚·‚é
		writeQueue->pop_front();
		num--;
	    }
	    return true;
	}
	bool ChunkedConnection::StreamWrite(unsigned char *buf, size_t length, int channel){
	    if(buf == NULL || length <= 0 || connection == NULL)
		return false;
#define CHUNKED_CONNECTION_HEADERLENGTH (4)
	    StaticBuffer *staticBuf = new StaticBuffer(length + CHUNKED_CONNECTION_HEADERLENGTH);
	    if(staticBuf == NULL)
		return false;
	    WriteHeader(0, channel, (uint16_t)length, staticBuf->GetBuffer());
	    staticBuf->Write(buf, length);
	    connection->NonBlockWrite(staticBuf);
	    return true;
	}

	bool ChunkedConnection::RunThread(void){
	}
	int ChunkedConnection::ProcessBuffer(void){ // thread ‚ª“®‚¢‚Ä‚½‚ç ProcessBuffer() ‚Í EventPool ‚Ìˆ—‚ğ‚·‚é‚¾‚¯Bthread ‚ª“®‚¢‚Ä‚¢‚È‚¯‚ê‚ÎAsend(2) ‚à‚·‚éB
	}


};
}; /* namespace Cookai */
