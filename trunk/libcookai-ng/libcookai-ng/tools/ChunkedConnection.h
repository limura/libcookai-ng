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

#ifndef COOKAI_CHUNKED_CONNECTION
#define COOKAI_CHUNKED_CONNECTION

#include <list>

#include "../config.h"
#include "thread.h"
#include "StaticBuffer.h"
#include "Connection.h"

namespace Cookai {

#define CHUNKED_CONNECTION_MAX_CHANNEL (256)

    typedef bool (*chunkReadHandler)(unsigned char *buf, size_t length, int channel);

    class ChunkedConnection{
    public:
	typedef enum {
	    THREADED,
	    NON_BLOCKING_IO,
	} AccessType;

    private:
	AccessType type;
	bool threadEnable;
	threadID readThreadID;
	chunkReadHandler blockReadHandler;
	chunkReadHandler streamReadHandler;
	std::list<Connection *> connectionList;

    public:
	ChunkedConnection(char *name, char *service, Cookai::ChunkedConnection::AccessType type);
	~ChunkedConnection(void);

	bool BlockWrite(unsigned char *buf, size_t length, int channel = 0);
	bool BlockCommit(int channel = 0);
	bool StreamWrite(unsigned char *buf, size_t length, int channel = 0);

	bool RunThread();
	unsigned char *BlockRead(size_t *length, int channel = 0);
	unsigned char *StreamRead(size_t *length, int channel = 0);

	bool RunRead();
	void SetBlockReadHandler(chunkReadHandler handler, int channel = 0);
	void SetStreamReadHandler(chunkReadHandler handler, int channel = 0);
    };

}; /* namespace Cookai */

#endif /* COOKAI_CHUNKED_CONNECTION */
