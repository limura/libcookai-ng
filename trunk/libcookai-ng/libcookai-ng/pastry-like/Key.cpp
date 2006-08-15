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

#include "../config.h"
#include "Key.h"

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

plKey::plKey(){
    id = NULL;
    keyStr = NULL;
}

plKey::plKey(char *str){
    if(str == NULL){
	id = NULL;
	return;
    }
    id = new plID(str);
    keyStr = new string(str);
}

plKey::plKey(std::string str){
    id = new plID(&str);
    keyStr = new string(str);
}

plKey::~plKey(){
    if(id != NULL)
	delete id;
    if(keyStr != NULL)
	delete keyStr;
}

plID *plKey::getID(){
    return id;
}

bool plKey::operator==(plKey& obj){
    if(obj.getID() == id)
	return true;
    return false;
}

bool plKey::operator!=(plKey& obj){
    return !(obj == *this);
}

void plKey::save(iostream& stream){
    size_t s;
    s = htonl((long)keyStr->length());
    stream.write((char *)&s, sizeof(s));
    stream.write(keyStr->c_str(), (std::streamsize)keyStr->length());
}

void plKey::load(iostream& stream){
    size_t s;
    char *p;
    stream.read((char *)&s, sizeof(s));
    s = ntohl((long)s);
    p = (char *)malloc(sizeof(char) * (s + 1));
    if(p == NULL)
	return;
    stream.read(p, (std::streamsize)s);
    p[s] = '\0';
    keyStr = new string(p);
    id = new plID(keyStr);
}
