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

#ifndef COOKAI_CONNECTION
#define COOKAI_CONNECTION

#include <string>
#include <list>

#include "../config.h"
#include "../tools/thread.h"
#include "StaticBuffer.h"
#include "NonBlockConnect.h"
#include "Event.h"

namespace Cookai {
namespace ChunkedConnection {

#define COOKAI_CHUNKEDCONNECTION_HEADERLENGTH (4)
#define COOKAI_CHUNKEDCONNECTION_MAX_DATALENGTH (1414*256)
    typedef struct {
	uint8_t nextChunkNum;
	uint8_t channel;
	uint16_t length;
    } ChunkHeader;

    class Connection
    {
    private:
	typedef enum {
	    STATUS_CONNECTING,
	    STATUS_CONNECTED,
	    STATUS_DISOCNNECTED,
	} ConnectionStatus;
	ConnectionStatus status;
	int fd;
	NonBlockConnect *nbConnect;
	size_t chunkSize;
	char *remoteName, *remoteService;
	StaticBuffer *readBuffer;
	StaticBuffer *readHeaderBuffer;
	typedef std::list<StaticBuffer *> WriteBufferList;
	WriteBufferList writeBufferList;
	thread_mutex writeBufferMutex;
	Cookai::ChunkedConnection::Event *streamEvent;
	Cookai::ChunkedConnection::Event *blockEvent;
	size_t blockChunkLength;
	int nowChunkNum;

	bool LookupIPPort(char *name, char *service, char **newName, char **newService);
	bool Initialize(char *name, char *service, size_t newChunkSize);
	bool Handshake(void);
	bool Read(void);

    public:
	Connection(char *name, char *service, size_t chunkSize = 1414);
	Connection(std::string name, std::string service, size_t chunkSize = 1414);
	~Connection(void);

	bool Connect(void);
	bool IsConnect(void);
	void Disconnect(void);

	Cookai::ChunkedConnection::EventType Run(Event **eventReturn);
	bool NonBlockWrite(unsigned char *buf, size_t length);
	bool NonBlockWrite(StaticBuffer *buf);
	StaticBuffer *NonBlockRead(int *channel, bool *isStream);
    };
};
};

#endif /* COOKAI_STATIC_BUFFER */
