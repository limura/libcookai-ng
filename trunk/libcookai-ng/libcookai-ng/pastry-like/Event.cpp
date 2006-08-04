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
 * $Id: $
 */

#include "Event.h"
#include <Windows.h>

EventQueue::EventQueue(void *userData){
	data = userData;
	handler = NULL;
}

EventQueue::EventQueue(void *userData, EventHandler Handler){
	data = userData;
	handler = Handler;
}

void *EventQueue::getData(){
	return data;
}

EventHandler EventQueue::getHandler(){
	return handler;
}

void EventQueue::runHandler(){
	if(handler != NULL)
		handler(data);
}


EventManager::EventManager(){
	thread_mutex_init(&queueListMutex, NULL);
	//thread_mutex_init(&nextMutex, NULL);
}

EventManager::~EventManager(){
	list<EventQueue *>::iterator it;

	for(it = queue.begin(); it != queue.end(); it++){
		delete *it;
	}
}

EventQueue *EventManager::pop(){
	EventQueue *q;

	if(queue.empty())
		return NULL;

	thread_mutex_lock(queueListMutex);
	q = queue.front();
	queue.pop_front();
	thread_mutex_unlock(queueListMutex);
	//thread_mutex_unlock(nextMutex);

	return q;
}

void EventManager::push(EventQueue *q){
	if(q == NULL)
		return;
	thread_mutex_lock(queueListMutex);
	queue.push_back(q);
	thread_mutex_unlock(queueListMutex);
}

void EventManager::push(void *userData){
	this->push(new EventQueue(userData));
}

void EventManager::push(void *userData, EventHandler Handler){
	this->push(new EventQueue(userData, Handler));
}

void EventManager::next(){
	EventQueue *q;
	q = this->pop();
	if(q != NULL)
		q->runHandler();
	delete q;
}