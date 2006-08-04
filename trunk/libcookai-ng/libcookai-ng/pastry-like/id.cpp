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
 * $Id: id.c,v 1.3 2004/12/16 16:09:36 takuji-i Exp $
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "config.h"


#if 0
#ifdef HAVE_SHA_H
#include <sha.h>
#endif

#ifdef HAVE_OPENSSL_SHA_H
#include <openssl/sha.h>
#endif /* HAVE_OPENSSL_SHA_H */

#ifdef HAVE_SHA1_H
#include <sha1.h>
#endif /* HAVE_SHA1_H */

#ifdef HAVE_SHA1INIT
#define SHA1_Init(x) SHA1Init(x)
#define SHA1_Update(a, b, c) SHA1Update(a, b, c)
#define SHA1_Final(a, b) SHA1Final(a, b)
#endif /* HAVE_SHA1INIT */

#ifdef HAVE_SHA_CTX
#define SHA1_CTX SHA_CTX
#endif
#else
#include "openssl/sha.h"
#endif


#include "id.h"

/*
xhttp://www.geocities.co.jp/SiliconValley-Oakland/8878/lab18/lab18.html
http://www.ipa.go.jp/security/rfc/RFC2202JA.html
sha1: rfc3174.txt
http://www.ipa.go.jp/security/rfc/RFC3174JA.html
md5: rfc1321.txt
*/

unsigned char *bin2ID(char *s, size_t len, unsigned char *id){
    SHA_CTX sc;

    if(s == NULL || len <= 0 || id == NULL)
	return NULL;

    SHA1_Init(&sc);
    SHA1_Update(&sc, s, (unsigned long)len);
    SHA1_Final(id, &sc);

    return id;
}

char *id2str_r(unsigned char *id, char *buf){
    int i;

    if(id == NULL)
	return buf;

    for(i = 0; i < ID_LENGTH; i++){
		sprintf(&buf[i*2], "%02x", id[i]);
    }
    buf[ID_LENGTH * 2] = '\0';

    return buf;
}

char *id2str(unsigned char *id){
    static char buf[ID_LENGTH * 2 + 1];
    buf[0] = '\0';
    strncat(buf, "unknown", sizeof(buf));

    return id2str_r(id, buf);
}

unsigned char *str2id_r(char *id_str, unsigned char *id){
    bin2ID(id_str, strlen(id_str), id);
    return id;
}

unsigned char *str2id(char *id_str){
    static unsigned char id[ID_LENGTH];
    return str2id_r(id_str, id);
}

int id_cmp(unsigned char *id1, unsigned char *id2){
    int i;
    for(i = 0; i < ID_LENGTH; i++){
	if(id1[i] != id2[i]){
	    return id1[i] - id2[i];
	}
    }
    return 0;
}

/* "012345678..." -> 0x01, 0x23, 0x45, 0x67... */
unsigned char *strID2id(char *id_str, unsigned char *id){
    char *p;
    int n, x;

    if(id_str == NULL || id == NULL)
	return NULL;

    n = 0;
    p = id_str;
    memset(id, 0, sizeof(char) * ID_LENGTH);
    while(*p != '\0' && sscanf(p, "%x", &x) == 1){
	if(n % 2 == 0)
	    id[n/2] = x;
	else
	    id[n/2] = (id[n/2] << 4) + x;
	n++;
	p++;
    }
    return id;
}

unsigned char *plID::getID(){
	return id;
}

char *plID::getStr(){
	return idStr;
}

plID::plID(){
	memset(id, 0, sizeof(id));
	memset(idStr, '\0', sizeof(idStr));
}

plID::plID(char *key){
	bin2ID(key, strlen(key), id);
	id2str_r(id, idStr);
}

plID::plID(unsigned char *data, size_t size){
	bin2ID((char *)data, size, id);
	id2str_r(id, idStr);
}

plID::plID(string *key){
	bin2ID((char *)key->c_str(), key->length(), id);
	id2str_r(id, idStr);
}

int plID::getElem(int ElementSize, int pos){
	if(idStr[0] == '\0')
		return -1;

	if(ID_LENGTH * 8 / ElementSize <= pos || ElementSize > 31)
		return -1;

	switch(ElementSize){
		case 4:
			if(pos % 2 == 0){
				return (int)(id[pos / 2] & 0xf);
			}
			return (int)((id[pos / 2] & 0xf0) >> 4);
			break;
		case 8:
			return (int)id[pos];
			break;
		case 16:
			return (int)((id[pos*2+1] << 8) + id[pos*2]);
			break;
		default:
			break;
	}
	return -1;
}