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

#include "RoutingTable.h"
#include "id.h"

RoutingTableElement::RoutingTableElement(){
    peers = NULL;
    datas = NULL;

    thread_mutex_init(&peerMutex, NULL);
    thread_mutex_init(&dataMutex, NULL);
}

RoutingTableElement::~RoutingTableElement(){
    if(peers != NULL)
	delete peers;
    if(datas != NULL)
	delete datas;
    thread_mutex_destroy(&peerMutex);
    thread_mutex_destroy(&dataMutex);
}

void RoutingTableElement::delData(plData *data){
    plDataList::iterator it;
    if(datas == NULL)
	return;
    thread_mutex_lock(&dataMutex);
    for(it = datas->begin(); it != datas->end(); it++){
	if(**it == *data){
	    datas->erase(it);
	    if(datas->size() <= 0){
		delete datas;
		datas = NULL;
	    }
	    thread_mutex_unlock(&dataMutex);
	    return;
	}
    }
    thread_mutex_unlock(&dataMutex);
}

void RoutingTableElement::delPeer(Peer *peer){
    PeerList::iterator it;
    if(peers == NULL)
	return;

    thread_mutex_lock(&peerMutex);
    for(it = peers->begin(); it != peers->end(); it++){
	if(**it == *peer){
	    peers->erase(it);
	    if(peers->size() <= 0){
		delete peers;
		peers = NULL;
	    }
	    thread_mutex_unlock(&peerMutex);
	    return;
	}
    }
    thread_mutex_unlock(&peerMutex);
}

PeerList *RoutingTableElement::setData(plData *data){
    PeerList *peerList;

    thread_mutex_lock(&dataMutex);
    thread_mutex_lock(&peerMutex);

    peerList = peers;
    if(datas == NULL)
	datas = new plDataList();
    datas->push_back(data);
    if(peers != NULL)
	delete peers;
    peers = NULL;

    thread_mutex_unlock(&peerMutex);
    thread_mutex_unlock(&dataMutex);
    return peerList;
}

plDataList *RoutingTableElement::setPeer(Peer *peer){
    plDataList *dataList = datas;

    thread_mutex_lock(&dataMutex);
    thread_mutex_lock(&peerMutex);

    dataList = datas;
    if(peers == NULL)
	peers = new PeerList();
    peers->push_back(peer);
    if(datas != NULL)
	delete datas;
    datas = NULL;

    thread_mutex_unlock(&peerMutex);
    thread_mutex_unlock(&dataMutex);
    return dataList;
}

plDataList *RoutingTableElement::getDataList(){
    return datas;
}

PeerList *RoutingTableElement::getPeerList(){
    return peers;
}

plData *RoutingTableElement::query(plKey *key){
    plDataList::iterator it;
    if(datas == NULL)
	return NULL;
    thread_mutex_lock(&dataMutex);
    for(it = datas->begin(); it != datas->end(); it++){
	if(*((*it)->getKey()) == *key){
	    thread_mutex_unlock(&dataMutex);
	    return *it;
	}
    }
    thread_mutex_unlock(&dataMutex);
    return NULL;
}

RoutingTableElementType::RoutingTableElementType RoutingTableElement::getType(){
    if(peers != NULL)
	return RoutingTableElementType::PEER;
    if(datas != NULL)
	return RoutingTableElementType::DATA;
    return RoutingTableElementType::UNKNOWN;
}

int RoutingTable::getElementLength(){
    return (ID_LENGTH * 8 / routingTableSize) * (1 << routingTableSize);
}

RoutingTable::RoutingTable(int size){
    routingTableSize = size;
    int length = this->getElementLength();

    id = NULL;
    if(length <= 0)
	return;
    elem = (RoutingTableElement **)malloc(sizeof(RoutingTableElement *) * length);
    if(elem == NULL)
	return;

    for(int i = 0; i < length; i++){
	elem[i] = new RoutingTableElement();
    }
}

RoutingTable::~RoutingTable(){
    int length = this->getElementLength();
    if(elem == NULL)
	return;
    for(int i = 0; i < length; i++){
	delete &elem[i];
    }
}

void RoutingTable::setID(plID *newID){
    id = newID;
}

RoutingTableElement *RoutingTable::getElement(plID *targetID){
    if(id == NULL || targetID == NULL)
	return NULL;

    int maxPos = ID_LENGTH * 8 / routingTableSize;
    int windowWidth = 1 << routingTableSize;

    int pos = 0;
    while(pos < maxPos && id->getElem(routingTableSize, pos) == targetID->getElem(routingTableSize, pos))
	pos++;
    if(pos >= maxPos)
	pos = maxPos - 1;
    return elem[windowWidth * (pos) + targetID->getElem(routingTableSize, pos)];
}

RoutingTableElement *RoutingTable::getElement(plKey *targetKey){
    if(id == NULL || targetKey == NULL || targetKey->getID() == NULL)
	return NULL;
    return getElement(targetKey->getID());
}

#if 0
plDataList *RoutingTable::setPeer(Peer *peer){
    RoutingTableElement *elem;

    elem = getElement(peer->getID());
    if(elem == NULL)
	return NULL;
    return elem->setPeer(peer);
}

PeerList *RoutingTable::setData(plKey *key, plData *data){
    RoutingTableElement *elem;

    elem = getElement(key);
    if(elem == NULL)
	return NULL;
    return elem->setData(data);
}

void RoutingTable::delPeer(Peer *peer){
    RoutingTableElement *elem;
    if(peer == NULL)
	return;

    elem = getElement(peer->getID());
    if(elem == NULL)
	return;

    elem->delPeer(peer);
}

void RoutingTable::delData(plKey *key, plData *data){
    RoutingTableElement *elem;
    if(key == NULL || data == NULL)
	return;

    elem = getElement(key->getID());
    if(elem == NULL)
	return;

    elem->delData(data);
}
#endif