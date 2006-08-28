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

#ifndef PASTRY_LIKE_NG_H
#define PASTRY_LIKE_NG_H

#include "../config.h"

#include <string>
#include <stdlib.h>

#include "plSendQueue.h"
#include "Key.h"
#include "RoutingTable.h"
#include "Event.h"

class PastryLike {
private:
    int accept_socket;
    RoutingTable *rt;
    plSendQueue sendQueue;
    plEventManager eventManager;

public:
    PastryLike();
    ~PastryLike();

    int join(char *remote, char *service);
    void set(plData *data);
    void set(char *key, unsigned char *data, size_t size);
    void set(plKey *key, unsigned char *data, size_t size);
    void del(plData *data);
    void del(char *key, unsigned char *data, size_t size);
    void del(plKey *key, unsigned char *data, size_t size);
    void query(plKey *key, void *userData);
    void query(char *key, void *userData);

    bool processNextEvent();

    typedef void (*QueryResponceEventFunc)(plData *data, void *userData);
    void setQueryEventFunc(PastryLike::QueryResponceEventFunc func, void *userData);
    typedef void (*PeerDeadEventFunc)(Peer *peer);
    void setPeerDeadEventFunc(PastryLike::PeerDeadEventFunc func);
    typedef void (*UserEventFunc)(Peer *peer, int type, size_t length, void *value);
    void setUserEventFunc(PastryLike::UserEventFunc func);
private:
    PastryLike::QueryResponceEventFunc queryResponceEventFunc;
    PastryLike::PeerDeadEventFunc peerDeadEventFunc;
    PastryLike::UserEventFunc userEventFunc;
};

#endif /* PASTRY_LIKE_NG_H */