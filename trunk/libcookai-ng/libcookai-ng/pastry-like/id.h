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
 * $Id$
 */

#ifndef NSNS_ID_H
#define NSNS_ID_H

#include <string>
#include <iostream>
using namespace std;

#define ID_LENGTH (20)

class plID {
private:
    unsigned char id[ID_LENGTH];
    char idStr[ID_LENGTH * 2 + 1];
public:
    plID();
    plID(char *key);
    plID(string *key);
    plID(unsigned char *data, size_t size);
    unsigned char *getID();
    char *getStr();
    int getElem(int ElementSize, int pos);

    void save(iostream& stream);
    void load(iostream& stream);

    bool operator==(plID& obj);
    bool operator!=(plID& obj);
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

unsigned char *bin2ID(char *s, size_t len, unsigned char *id);
char *id2str_r(unsigned char *id, char *buf);
char *id2str(unsigned char *id);
int id_cmp(unsigned char *id1, unsigned char *id2);
unsigned char *str2id_r(char *id_str, unsigned char *id);
unsigned char *str2id(char *id_str);
unsigned char *strID2id(char *id_str, unsigned char *id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NSNS_ID_H */
