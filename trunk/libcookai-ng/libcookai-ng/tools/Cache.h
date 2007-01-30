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

#ifndef COOKAI_CACHE_H
#define COOKAI_CACHE_H

#include <string>
#include <map>
using namespace std;

#include "thread.h"

namespace cookai {
    class CacheData {
    private:
	bool onMemoryFlug;
	void *data;
	size_t dataSize;
	string *filePath;
    public:
	CacheData();
	~CacheData();
	void setData(string key, void *data, size_t size, bool onMemory = true);
	void clearData();
	void *getData(size_t *size);
    };

    typedef map<string, CacheData *> CacheDataMap;
    class Cache {
    private:
	threadID dataTracerThreadID;
	CacheDataMap cacheDataMap;
    public:
	Cache();
	~Cache();
	void runDataTracer();
	void *getCacheData(string key, size_t *size);
	void setCacheData(string key, void *data, size_t siz);
	void setCacheData(string key, void *data, size_t siz, bool onMemory);
    };
};

#endif /* COOKAI_CACHE_H */