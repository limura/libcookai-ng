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

#ifndef COOKAI_CHUNKEDCONNECTION_CONNECTIONMANAGERINTARFACE_H
#define COOKAI_CHUNKEDCONNECTION_CONNECTIONMANAGERINTARFACE_H

namespace Cookai {
namespace ChunkedConnection {
class ConnectionManager;

    typedef enum {
	CONNECTION_STATUS_NONE = 0,
	CONNECTION_STATUS_READ_OK = 1,
	CONNECTION_STATUS_WRITE_OK = 2,
    } ConnectionStatus;

    class ConnectionManagerInterface {
    public:
	//virtual ~ConnectionManagerInterface(void);

	virtual int Connect(void) = 0;
	virtual void RegisterConnectionManager(Cookai::ChunkedConnection::ConnectionManager *cm) = 0;
	virtual bool Run(Cookai::ChunkedConnection::ConnectionStatus status) = 0;
	virtual bool HasWriteQueue(void) = 0;
	virtual bool IsConnect(void) = 0;
    };
};
};

#endif /* COOKAI_CHUNKEDCONNECTION_CONNECTIONMANAGERINTARFACE_H */

