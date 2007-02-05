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
 * $Id: Peer.h 16 2006-08-09 07:40:49Z uirou.j $
 */

#include "EventPool.h"

namespace Cookai {
namespace ChunkedConnection {
    EventPool::EventPool(void){
	thread_mutex_init(&eventListMutex, NULL);
	eventList.clear();
    }

    EventPool::~EventPool(void){
	thread_mutex_destroy(&eventListMutex);
    }

    bool EventPool::Push(Cookai::ChunkedConnection::Event *newEvent){
	thread_mutex_lock(&eventListMutex);
	eventList.push_back(newEvent);
	thread_mutex_unlock(&eventListMutex);
	return true;
    }

    Cookai::ChunkedConnection::Event *EventPool::Pop(void){
	if(eventList.empty())
	    return NULL;
	thread_mutex_lock(&eventListMutex);
	Cookai::ChunkedConnection::Event *ev = eventList.front();
	eventList.pop_front();
	thread_mutex_unlock(&eventListMutex);
	return ev;
    }

    bool EventPool::InvokeOne(void){
	Cookai::ChunkedConnection::Event *ev = Pop();
	if(ev != NULL){
	    ev->Invoke();
	    return true;
	}
	return false;
    }

    int EventPool::InvokeAll(void){
	int i = 0;

	while(InvokeOne())
	    i++;

	return i;
    }

};
};
