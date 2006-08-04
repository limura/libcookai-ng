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

#ifndef PASTRY_LIKE_ROUTING_TABLE_H
#define PASTRY_LIKE_ROUTING_TABLE_H

#include <list>
using namespace std;

#include "Data.h"
#include "Peer.h"

namespace RoutingTableElementType{
typedef enum {
	NEXT_HOP,
	DATA,
	UNKNOWN,
} RoutingTableElementType;
}

class RoutingTableElement{
	RoutingTableElementType::RoutingTableElementType type;
	list<Peer> peers;
	list<plData> datas;
public:
	RoutingTableElementType::RoutingTableElementType getType();
	list<Peer> getPeer();
	list<plData> getData();

	RoutingTableElement();
	~RoutingTableElement();

	void setPeer(Peer *peer);
	void setData(plData *data);
	void delData(plData *data);
};

#define RoutingTableSize (4) /* bit */

class RoutingTable{
private:
	plID *id;
	int routingTableSize;
	RoutingTableElement **elem;
	int getElementLength();
public:
	RoutingTable(int routingTableSize);
	~RoutingTable();

	void setID(plID *id);
	RoutingTableElement *getElement(plID *id);
};

#endif /* PASTRY_LIKE_ROUTING_TABLE_H */