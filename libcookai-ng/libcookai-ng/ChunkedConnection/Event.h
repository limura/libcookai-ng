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

#include <sys/types.h>
#include <stdlib.h>

#include "StaticBuffer.h"

#ifndef COOKAI_CHUNKEDCONNECTION_EVENT_H
#define COOKAI_CHUNKEDCONNECTION_EVENT_H

namespace Cookai{
namespace ChunkedConnection{

    typedef enum {
	EVENT_NOTHING,
	EVENT_RECIVE_BLOCK,
	EVENT_RECIVE_STREAM,
	EVENT_ERROR_SOCKET_CLOSE,
	EVENT_ERROR_UNKNOWN,
    } EventType;
    typedef bool (*ReadHandler)(EventType type, StaticBuffer *buf, int channel);

    class Event {
    private:
	EventType type;
	StaticBuffer *buf;
	int channel;
	ReadHandler handler;

    public:
	Event(EventType type, StaticBuffer *buf, int Channel = 0, ReadHandler Handler = NULL);
	~Event(void);

	EventType GetEventType(void);
	StaticBuffer *GetBuffer(void);
	int GetChannel(void);
	bool Invoke(void);
    };
};
};

#endif /* COOKAI_CHUNKEDCONNECTION_EVENT_H */