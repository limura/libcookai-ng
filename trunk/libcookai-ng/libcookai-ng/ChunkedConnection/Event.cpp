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

#include "ChunkedConnection.h"
#include "Event.h"

namespace Cookai{
namespace ChunkedConnection {
    Event::Event(EventType Type, StaticBuffer *Buf,
	Cookai::ChunkedConnection::ChunkedConnection *Cc,
	int Channel, ReadHandler Handler){
	type = Type;
	buf = Buf;
	channel = Channel;
	handler = Handler;
	cc = Cc;
    }

    Event::~Event(void){
	if(buf != NULL)
	    delete buf;
    }

    void Event::SetEventHandler(ReadHandler Handler){
	handler = Handler;
    }

    EventType Event::GetEventType(void){
	return type;
    }
    StaticBuffer *Event::GetBuffer(void){
	return buf;
    }
    int Event::GetChannel(void){
	return channel;
    }
    Cookai::ChunkedConnection::ChunkedConnection *Event::GetChunkedConnection(void){
	return cc;
    }
    void Event::SetChunkedConnection(Cookai::ChunkedConnection::ChunkedConnection *Cc){
	cc = Cc;
    }

    bool Event::Invoke(void){
	if(handler != NULL){
	    handler(type, buf, channel, cc);
	    return true;
	}
	return false;
    }
};
};

