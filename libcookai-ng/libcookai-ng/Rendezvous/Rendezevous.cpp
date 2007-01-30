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

#include "Rendezevous.h"
#include "../tools/net.h"

int reciverThreadFunc(void *p){
    Rendezevous *me = (Rendezevous*)p;

    if(me == NULL)
	return 0;
    while(1){
	// XXX 
	return 0;
    }
    return 0;
}

void Rendezevous::initialize(){
    thread_mutex_init(&groupNameMutex, NULL);
    thread_create(&reciverThreadID, (thread_func)reciverThreadFunc, this);
}

Rendezevous::Rendezevous(){
    initialize();
}

Rendezevous::~Rendezevous(){
}

Rendezevous* Rendezevous::getInstance(){
    static Rendezevous *Instance = NULL;
    if(Instance == NULL){
	thread_mutex_lock(&singletonMutex);
	if(Instance == NULL){
	    Instance = new Rendezevous();
	}
	thread_mutex_unlock(&singletonMutex);
    }
    return Instance;
}

int Rendezevous::search(char *group_name, char *remoteIP_return, char *port_return){
    // stub code.
    return 0;
}
