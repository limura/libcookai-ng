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
 * $Id: $
 */

#ifndef PL_SENDQUEUE_H
#define PL_SENDQUEUE_H

#include <map>
using namespace std;

#include "Key.h"
#include "Data.h"
#include "thread.h"

namespace plSendType{
    typedef enum {
	SET,
	DEL,
	QUERY,
    } plSendType;
}

class plSendQueueData{
private:
    plSendType::plSendType type;
    plData *data;
public:
    plSendQueueData(plSendType::plSendType type, plData *data);
    plSendType::plSendType getType();
    plData *getData();
};

typedef map<int, plSendQueueData *> plSendQueueDataMapInt;

class plSendQueue{
private:
    int count;
    plSendQueueDataMapInt queueMapInt;
    thread_mutex queueMutex;
public:
    plSendQueue();
    ~plSendQueue();

    void enqueue(plSendType::plSendType type, plData *data);
    void enqueue(plSendType::plSendType type, plKey *key, unsigned char *data, size_t size);
    void process();

    void dequeue(plData *data);
};

#endif /* PL_SENDQUEUE_H */