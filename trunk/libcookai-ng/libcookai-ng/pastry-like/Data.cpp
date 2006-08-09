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

#include "config.h"

#include "Data.h"
#include "Key.h"

#include <iostream>

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

plData::plData(plKey *Key, unsigned char *Data, size_t Size){
    key = Key;
    data = Data;
    size = Size;
}

plKey *plData::getKey(){
    return key;
}

unsigned char *plData::getDataBuf(){
    return data;
}

unsigned char *plData::getDataBuf(size_t *size_return){
    if(size_return == NULL)
	return NULL;
    *size_return = size;
    return data;
}

size_t plData::getDataSize(){
    return size;
}

bool plData::operator==(plData& obj){
    plKey *k = obj.getKey();
    if(k == NULL)
	return false;
    if(*k == *key && obj.getDataSize() == size){
	unsigned char *p1 = data;
	unsigned char *p2 = obj.getDataBuf();

	for(unsigned int i = 0; i < size; i++){
	    if(p1[i] != p2[i])
		return false;
	}
	return true;
    }
    return false;
}

bool plData::operator!=(plData& obj){
    return !(obj == *this);
}

void plData::save(iostream& stream){
    size_t s = htonl((long)size);
    key->save(stream);
    stream.write((char *)&s, sizeof(s));
    stream.write((char *)data, (std::streamsize)size);
}

void plData::load(iostream& stream){
    size_t s;
    key = new plKey();
    key->load(stream);
    stream.read((char *)&s, sizeof(s));
    size = ntohl((long)s);
    data = (unsigned char *)malloc(sizeof(char) * size);
    stream.read((char *)data, (std::streamsize)size);
}

