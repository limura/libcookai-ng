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

#include "Cache.h"
#include <stdio.h>
#include <fcntl.h>
#include <fstream>

namespace cookai {
    CacheData::CacheData(){
	filePath = NULL;
	data = NULL;
	dataSize = 0;
	onMemoryFlug = false;
    }
    CacheData::~CacheData(){
    }

    void CacheData::setData(string key, void *dat, size_t size, bool onMemory){
	filePath = new string(key);
	onMemoryFlug = onMemory;
	data = dat;
	dataSize = size;
	if(!onMemoryFlug){
	    std::ofstream ofile(filePath->c_str());
	    ofile.write((char *)dat, (std::streamsize)size);
	    ofile.close();
	}
    }
    void CacheData::clearData(){
	if(filePath != NULL)
	    delete filePath;

	filePath = NULL;
	data = NULL;
	dataSize = 0;
	onMemoryFlug = false;
    }
    void *CacheData::getData(size_t *size){
	if(size == NULL)
	    return NULL;
	*size = dataSize;
	return data;
    }

    Cache::Cache(){
    }
    Cache::~Cache(){
    }

    void Cache::runDataTracer(){
    }
//    typedef map<CacheData *, string> CacheDataMap;

    void *Cache::getCacheData(string key, size_t *size){
	CacheData *cd = cacheDataMap[key];
	if(cd == NULL){
	    *size = 0;
	    return NULL;
	}
	return cd->getData(size);
    }
    void Cache::setCacheData(string key, void *data, size_t siz){
	setCacheData(key, data, siz, false);
    }
    void Cache::setCacheData(string key, void *data, size_t siz, bool onMemory){
	CacheData *cd = cacheDataMap[key];
	if(data == NULL || siz <= 0){
	    if(cd != NULL)
		delete cd;
	    cacheDataMap[key] = NULL;
	    return;
	}
	if(cd == NULL){
	    cd = cacheDataMap[key] = new CacheData();
	    if(cd == NULL) // XXX fatal.
		return;
	}
	cd->setData(key, data, siz, onMemory);
    }
};
