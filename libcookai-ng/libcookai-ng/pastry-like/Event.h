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

#include "thread.h"

typedef void (*EventHandler)(void *userData);

class EventQueue{
private:
	void *data;
	EventHandler handler;
public:
	EventQueue(void *userData);
	EventQueue(void *userData, EventHandler Handler);

	void *getData();
	EventHandler getHandler();

	void runHandler();
};

class EventManager{
private:
	thread_mutex queueListMutex;
	thread_cond canReadCond;
	thread_cond canNotWriteCond;
	list<EventQueue *> queue;
	unsigned int maxSize;
public:
	EventManager();
	EventManager(int queueSize);
	~EventManager();

	void push(EventQueue *queue);
	void push(void *userData);
	void push(void *userData, EventHandler Handler);
	EventQueue *pop();
	EventQueue *timedPop(int usec);
	void next();
};

#endif /* PASTRY_LIKE_EVENT_H */