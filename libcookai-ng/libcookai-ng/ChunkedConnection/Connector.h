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

#include "../tools/thread.h"
#include "ChunkedConnection.h"
#include "ConnectionManager.h"
#include "Event.h"
#include "EventPool.h"

#ifndef COOKAI_CHUNKEDCONNECTION_CONNECTOR_H
#define COOKAI_CHUNKEDCONNECTION_CONNECTOR_H

namespace Cookai {
namespace ChunkedConnection {

#define COOKAI_DEFAULT_ACCEPT_PORT "1203"

    class Connector {
    private:
	ConnectionManager *manager;
	EventPool *eventPool;
	threadID tid;
	static void LoopFunc(void *userdata);

    protected:
	void RunTick(int usec);

    public:
	Connector(ReadHandler AcceptEventHandler, char *serviceName = COOKAI_DEFAULT_ACCEPT_PORT);
	~Connector(void);

	ChunkedConnection *Connect(char *name, char *service,
	    ReadHandler streamHander = NULL, ReadHandler blockHandler = NULL,
	    ReadHandler errorHandler = NULL, size_t chunkSize = 1414);
	Cookai::ChunkedConnection::Event *NextEvent(void);
	void InvokeAllEvent(void);
    };
}
};

#endif /* COOKAI_CHUNKEDCONNECTION_CONNECTOR_H */

