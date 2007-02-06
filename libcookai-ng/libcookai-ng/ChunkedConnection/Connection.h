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
 * $Id: Connection.h 29 2007-02-05 00:51:58Z uirou.j $
 */

#ifndef COOKAI_CONNECTION
#define COOKAI_CONNECTION

#include <string>
#include <list>

#include "../config.h"
#include "StaticBuffer.h"
#include "NonBlockConnect.h"
#include "../tools/thread.h"

namespace Cookai {
namespace ChunkedConnection {

    typedef bool (*chunkReadHandler)(unsigned char *buf, size_t length, int channel);
    typedef struct _ChunkData{
	uint16_t length;
	unsigned char nextChunkNum;
	unsigned char channel;
	unsigned char dataStartPoint;
    } ChunkData;

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
	typedef std::list<StaticBuffer *> WriteBufferList;
	WriteBufferList writeBufferList;
	thread_mutex writeBufferMutex;

	bool LookupIPPort(char *name, char *service, char **newName, char **newService);
	bool Initialize(char *name, char *service, size_t newChunkSize);
	bool Handshake(void);
	bool Read();

    public:
	Connection(char *name, char *service, size_t chunkSize = 1414);
	Connection(std::string name, std::string service, size_t chunkSize = 1414);
	~Connection(void);

	bool Connect(void);
	bool IsConnect(void);
	void Disconnect(void);

	ConnectionStatus Run(void);
	bool NonBlockWrite(unsigned char *buf, size_t length);
	bool NonBlockWrite(StaticBuffer *buf);
	StaticBuffer *NonBlockRead(int *channel, bool *isStream);
    };
};
};

#endif /* COOKAI_STATIC_BUFFER */
