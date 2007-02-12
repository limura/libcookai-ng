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

#include "Connector.h"
#include "ConnectionManager.h"
#include "EventPool.h"

namespace Cookai {
namespace ChunkedConnection {
    Connector::Connector(void){
	manager = new ConnectionManager();
	eventPool = new EventPool();
	thread_create(&tid, (thread_func)LoopFunc, this);
    }

    Connector::~Connector(void){
	thread_cancel(&tid);
	if(manager != NULL)
	    delete manager;
	if(eventPool != NULL)
	    delete eventPool;
    }

    void Connector::LoopFunc(void *userData){
	Connector *self = (Connector *)userData;
	if(self == NULL)
	    return;

	while(1){
	    self->RunTick(1000000);
	}
    }

    void Connector::RunTick(int usec){
	if(manager != NULL)
	    manager->Run(usec);
    }

    Cookai::ChunkedConnection::Event *Connector::NextEvent(void){
	if(eventPool != NULL)
	    return eventPool->GetEvent();
	return NULL;
    }

    void Connector::InvokeAllEvent(void){
	if(eventPool != NULL)
	    eventPool->InvokeAll();
    }

    Cookai::ChunkedConnection::ChunkedConnection *Connector::Connect(char *name, char *service,
	ReadHandler streamHandler, ReadHandler blockHandler,
	ReadHandler errorHandler, size_t chunkSize){
	    Cookai::ChunkedConnection::ChunkedConnection *cc = new Cookai::ChunkedConnection::ChunkedConnection(name, service, eventPool, chunkSize);
	    if(cc == NULL)
		return NULL;

	    cc->SetStreamReadHandler(streamHandler);
	    cc->SetBlockReadHandler(blockHandler);
	    cc->SetErrorHandler(errorHandler);
	    manager->AddInterface(cc);

	    cc->Connect();

	    return cc;
    }
};
};