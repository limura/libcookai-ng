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

#include "Event.h"
#ifdef HAVE_WINDOWS_H
#include <Windows.h>
#endif /* HAVE_WINDOWS_H */

plEventQueue::plEventQueue(void *userData){
    data = userData;
    handler = NULL;
}

plEventQueue::plEventQueue(void *userData, plEventHandler Handler){
    data = userData;
    handler = Handler;
}

void *plEventQueue::getData(){
    return data;
}

plEventHandler plEventQueue::getHandler(){
    return handler;
}

void plEventQueue::runHandler(){
    if(handler != NULL)
	handler(data);
}


plEventManager::plEventManager(){
    thread_mutex_init(&queueListMutex, NULL);
    thread_cond_init(&canReadCond);
    thread_cond_init(&canNotWriteCond);
    maxSize = 1024;
    queue = new plEventQueueList;
}

plEventManager::plEventManager(int queueSize){
    thread_mutex_init(&queueListMutex, NULL);
    thread_cond_init(&canReadCond);
    thread_cond_init(&canNotWriteCond);
    maxSize = queueSize;
    queue = new plEventQueueList;
}

plEventManager::~plEventManager(){
    plEventQueueList::iterator it;

    thread_mutex_lock(&queueListMutex);
    for(it = queue->begin(); it != queue->end(); it++){
	delete *it;
    }
    thread_mutex_unlock(&queueListMutex);
    thread_mutex_destroy(&queueListMutex);
    thread_cond_destroy(&canReadCond);
    thread_cond_destroy(&canNotWriteCond);
}

plEventQueue *plEventManager::pop(){
    plEventQueue *q = NULL;

    thread_mutex_lock(&queueListMutex);
    if(!queue->empty()){
	q = queue->front();
	queue->pop_front();
	if(queue->empty())
	    thread_cond_reset(&canReadCond);
	thread_cond_signal(&canNotWriteCond);
    }
    thread_mutex_unlock(&queueListMutex);
    return q;
}

plEventQueue *plEventManager::timedPop(int usec){
    plEventQueue *q = NULL;

    thread_mutex_lock(&queueListMutex);
    if(!queue->empty()){
	q = queue->front();
	queue->pop_front();
    }else{
	thread_cond_timedwait(&canReadCond, &queueListMutex, usec);
	if(!queue->empty()){
	    q = queue->front();
	    queue->pop_front();
	}
    }
    thread_mutex_unlock(&queueListMutex);
    return q;
}

plEventQueueList *plEventManager::popAll(){
    plEventQueueList *l = NULL;
    thread_mutex_lock(&queueListMutex);
    if(!queue->empty()){
	l = queue;
	queue = new plEventQueueList;
    }
    thread_mutex_unlock(&queueListMutex);
    return l;
}

void plEventManager::push(plEventQueue *q){
    if(q == NULL)
	return;
    thread_mutex_lock(&queueListMutex);
    queue->push_back(q);
    thread_cond_signal(&canReadCond);
    if(queue->size() > maxSize)
	thread_cond_reset(&canNotWriteCond);
    thread_mutex_unlock(&queueListMutex);
}

void plEventManager::push(void *userData){
    this->push(new plEventQueue(userData));
}

void plEventManager::push(void *userData, plEventHandler Handler){
    this->push(new plEventQueue(userData, Handler));
}

void plEventManager::next(){
    plEventQueue *q;
    q = this->pop();
    if(q != NULL)
	q->runHandler();
    delete q;
}
