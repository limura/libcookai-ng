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

#ifndef COOKAI_CHUNKED_CONNECTION_H
#define COOKAI_CHUNKED_CONNECTION_H

#include <string>
#include <list>
#include <map>

#include "../config.h"
#include "../tools/thread.h"
#include "StaticBuffer.h"
#include "Connection.h"
#include "Event.h"
#include "EventPool.h"
#include "ConnectionManager.h"
#include "ConnectionManagerInterface.h"

namespace Cookai {
namespace ChunkedConnection {

#define CHUNKED_CONNECTION_MAX_CHANNEL (256) /* unsigned char */

    class ChunkedConnection : public ConnectionManagerInterface {
    private:
	bool threadEnable;
	threadID readThreadID;
	ReadHandler blockReadHandler;
	ReadHandler streamReadHandler;
	ReadHandler errorHandler;
	size_t chunkSize;
	Connection *connection;
	ConnectionManager *connectionManager;

	typedef std::list<StaticBuffer *> WriteQueue;
	typedef std::map<int, WriteQueue *> ChannelWriteQueue;
	ChannelWriteQueue channelWriteQueue;
	Cookai::ChunkedConnection::EventPool *eventPool;

	void Initialize(char *remote, Cookai::ChunkedConnection::EventPool *eventPool, size_t ChunkSize = 1414);
	StaticBuffer *CreateNewBuffer(void);
	WriteQueue *GetWriteQueue(int channel);
	void WriteHeader(int nextChunkNum, int channel, uint16_t length, StaticBuffer *buf);
	void Run(Connection *targetConnection);

    public:
	ChunkedConnection(char *remote, Cookai::ChunkedConnection::EventPool *eventPool, size_t chunkSize = 1414);
	ChunkedConnection(std::string remote, Cookai::ChunkedConnection::EventPool *eventPool, size_t chunkSize = 1414);
	ChunkedConnection(int acceptedFD, char *remote, Cookai::ChunkedConnection::EventPool *eventPool, size_t chunkSize = 1414);
	~ChunkedConnection(void);

	void SetBlockReadHandler(ReadHandler handler);
	void SetStreamReadHandler(ReadHandler handler);
	void SetErrorHandler(ReadHandler handler);

	bool BlockWrite(unsigned char *buf, size_t length, int channel = 0);
	bool BlockWrite(char *buf, int channel = 0);
	bool BlockWrite(std::string buf, int channel = 0);
	bool BlockCommit(int channel = 0);
	bool StreamWrite(unsigned char *buf, size_t length, int channel = 0);
	bool StreamWrite(char *buf, int channel = 0);
	bool StreamWrite(std::string buf, int channel = 0);

	int Connect(void);
	void RegisterConnectionManager(ConnectionManager *cm);
	bool Run(Cookai::ChunkedConnection::ConnectionStatus status);
    };

}; /* namespace ChunkedConnection */
}; /* namespace Cookai */

#endif /* COOKAI_CHUNKED_CONNECTION */
