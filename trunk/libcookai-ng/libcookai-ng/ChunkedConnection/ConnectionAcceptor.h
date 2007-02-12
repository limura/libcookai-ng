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

#include "ConnectionManager.h"
#include "ConnectionManagerInterface.h"
#include "EventPool.h"

#ifndef COOKAI_CHUNKEDCONNECTION_CONNECTIONACCEPTOR_H
#define COOKAI_CHUNKEDCONNECTION_CONNECTIONACCEPTOR_H

namespace Cookai {
namespace ChunkedConnection {

#define COOKAI_DEFAULT_ACCEPT_PORT "1203"

    class ConnectionAcceptor : public ConnectionManagerInterface {
    private:
	int acceptSocketFD;
	ConnectionManager *connectionManager;
	EventPool *eventPool;

    public:
	ConnectionAcceptor(EventPool *pool);
	~ConnectionAcceptor(void);

	int Connect(void);
	int Connect(char *ServiceName);
	void RegisterConnectionManager(Cookai::ChunkedConnection::ConnectionManager *cm);
	bool Run(Cookai::ChunkedConnection::ConnectionStatus status);
    };
};
};

#endif /* COOKAI_CHUNKEDCONNECTION_CONNECTIONACCEPTOR_H */