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

#ifndef PASTRY_LIKE_EVENT_H
#define PASTRY_LIKE_EVENT_H

#include <list>
using namespace std;

#include "../tools/thread.h"

typedef void (*plEventHandler)(void *userData);

class plEventQueue{
private:
    void *data;
    plEventHandler handler;
public:
    plEventQueue(void *userData);
    plEventQueue(void *userData, plEventHandler Handler);

    void *getData();
    plEventHandler getHandler();

    void runHandler();
};

typedef list<plEventQueue *> plEventQueueList;

class plEventManager{
private:
    thread_mutex queueListMutex;
    thread_cond canReadCond;
    thread_cond canNotWriteCond;
    unsigned int maxSize;
protected:
    plEventQueueList *queue;
public:
    plEventManager();
    plEventManager(int queueSize);
    ~plEventManager();

    void push(plEventQueue *queue);
    void push(void *userData);
    void push(void *userData, plEventHandler Handler);
    plEventQueue *pop();
    plEventQueue *timedPop(int usec);
    plEventQueueList *popAll();
    void next();
};

#endif /* PASTRY_LIKE_EVENT_H */
