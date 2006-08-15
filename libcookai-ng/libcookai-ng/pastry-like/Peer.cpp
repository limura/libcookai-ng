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

#include "Peer.h"
#include "../tools/net.h"

Peer::Peer(){
    sock = -1;
    id = NULL;
    remote = service = NULL;
    Enabled = false;
    distance = NULL;
}

Peer::~Peer(){
    if(remote != NULL)
	delete remote;
    if(service != NULL)
	delete service;
    Enabled = false;
}

bool Peer::enabled(){
    return Enabled;
}

plID* Peer::getID(){
    return id;
}

char *Peer::getIDStr(){
    return id->getStr();
}

bool Peer::operator==(Peer& peer){
    return *id == *peer.getID();
}

bool Peer::operator!=(Peer& peer){
    return *id != *peer.getID();
}

void Peer::connect(char *Remote, char *Service){
    remote = new string(Remote);
    service = new string(Service);

    sock = connect_stream(Remote, Service);
    Enabled = true;
}

Distance *Peer::getDistance(){
    return distance;
}