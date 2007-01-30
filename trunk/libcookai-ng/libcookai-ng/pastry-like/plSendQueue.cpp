/*
 * Copyright (c) 2003, 2004 IIMURA Takuji. All rights reserved.
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

#include "plSendQueue.h"

plSendQueueData::plSendQueueData(plSendType::plSendType Type, plData *Data){
    type = Type;
    data = Data;
}

plSendQueue::plSendQueue(){
    count = 0;
    thread_mutex_init(&queueMutex, NULL);
}

plSendQueue::~plSendQueue(){
    thread_mutex_destroy(&queueMutex);
}

void plSendQueue::enqueue(plSendType::plSendType type, plData *data){
    if(data == NULL)
	return;
    thread_mutex_lock(&queueMutex);
    count++;
    
    queueMapInt[count] = new plSendQueueData(type, data);
    thread_mutex_unlock(&queueMutex);
}

void plSendQueue::enqueue(plSendType::plSendType type, plKey *key, unsigned char *data, size_t size){
    if(key == NULL || data == NULL || size <= 0)
	return;
    enqueue(type, new plData(key, data, size));
}

void plSendQueue::process(){

}