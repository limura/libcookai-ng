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

#include "RoutingTable.h"
#include "id.h"

RoutingTableElement::RoutingTableElement(){
	type = RoutingTableElementType::UNKNOWN;
}

RoutingTableElement::~RoutingTableElement(){
}

void RoutingTableElement::setData(plData *data){
	datas.push_back(*data);
}

void RoutingTableElement::delData(plData *data){
	list<plData>::iterator it;
	for(it = datas.begin(); it != datas.end(); it++){
		if(&(*it) == data){
			datas.erase(it);
			return;
		}
	}
}

void RoutingTableElement::setPeer(Peer *peer){
	peers.push_back(*peer);
}

list<plData> RoutingTableElement::getData(){
	return datas;
}

list<Peer> RoutingTableElement::getPeer(){
	return peers;
}

RoutingTableElementType::RoutingTableElementType RoutingTableElement::getType(){
	return type;
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

void RoutingTable::setID(plID *id){
	id = id;
}

RoutingTableElement *RoutingTable::getElement(plID *targetID){
	if(id == NULL)
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