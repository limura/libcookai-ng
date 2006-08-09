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

#include "PastryLike.h"
#include "RoutingTable.h"

#include "net.h"

PastryLike::PastryLike(){
    accept_socket = -1;
    rt = NULL;
    queryResponceEventFunc = NULL;
    peerDeadEventFunc = NULL;
    userEventFunc = NULL;
}

PastryLike::~PastryLike(){
    if(accept_socket >= 0)
	closeSocket(accept_socket);
}

int PastryLike::join(char *remote, char *service){
    return -1;
}

void PastryLike::set(plData *data){
    RoutingTableElement *elem;
    PeerList::iterator it = NULL;
    PeerList *peerList = NULL;

    if(data == NULL)
	return;

    elem = rt->getElement(data->getKey());
    if(elem == NULL)
	return;

    switch(elem->getType()){
	case RoutingTableElementType::UNKNOWN: // may be no data contained.
	case RoutingTableElementType::DATA:
	    elem->setData(data);
	    break;
	case RoutingTableElementType::PEER:
	    sendQueue.enqueue(plSendType::SET, data);
	    break;
	default:
	    break;
    }
}

void PastryLike::set(plKey *key, unsigned char *data, size_t size){
    if(key == NULL || data == NULL || size <= 0)
	return;
    set(new plData(key, data, size));
}

void PastryLike::set(char *key, unsigned char *data, size_t size){
    if(key == NULL || data == NULL || size <= 0)
	return;
    set(new plData(new plKey(key), data, size));
}

void PastryLike::del(plData *data){
    RoutingTableElement *elem;
    PeerList::iterator it = NULL;
    PeerList *peerList = NULL;

    if(data == NULL)
	return;

    elem = rt->getElement(data->getKey());
    if(elem == NULL)
	return;

    switch(elem->getType()){
	case RoutingTableElementType::DATA:
	    elem->delData(data);
	    break;
	case RoutingTableElementType::PEER:
	    sendQueue.enqueue(plSendType::DEL, data);
	    break;
	default:
	case RoutingTableElementType::UNKNOWN:
	    break;
    }
}

void PastryLike::query(plKey *key, void *userData){
    RoutingTableElement *elem;
    
    if(key == NULL)
	return;
    elem = rt->getElement(key->getID());
    if(elem == NULL)
	return;
    switch(elem->getType()){
	case RoutingTableElementType::DATA:
	    if(queryResponceEventFunc != NULL)
		queryResponceEventFunc(elem->query(key), userData);
	    break;
	case RoutingTableElementType::PEER:
	    break;
	default:
	case RoutingTableElementType::UNKNOWN:
	    if(queryResponceEventFunc != NULL)
		queryResponceEventFunc(NULL, userData);
	    break;
    }
}

void PastryLike::query(char *key, void *userData){
    query(new plKey(key), userData);
}

void PastryLike::setQueryEventFunc(PastryLike::QueryResponceEventFunc func, void *userData){
    queryResponceEventFunc = func;
}

void PastryLike::setPeerDeadEventFunc(PastryLike::PeerDeadEventFunc func){
    peerDeadEventFunc = func;
}

void PastryLike::setUserEventFunc(PastryLike::UserEventFunc func){
    userEventFunc = func;
}