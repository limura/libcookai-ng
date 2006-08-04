/*
 * Copyright (c) 2005 IIMURA Takuji. All rights reserved.
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

#include "thread.h"

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#ifdef USE_THREAD
int thread_mutex_init(thread_mutex *mutex, char *name){
#ifdef HAVE_PTHREAD_H
    if(pthread_mutex_init((pthread_mutex_t *)mutex, NULL) == 0)
		return TRUE;
#else defined(HAVE_WINDOWS_H)
    *mutex = (thread_mutex)CreateMutex(NULL, FALSE, name);
    if(*mutex != 0)
		return TRUE;
#endif
    return FALSE;
}
#endif

#ifdef USE_THREAD
int thread_mutex_lock(thread_mutex *mutex){
#ifdef HAVE_PTHREAD_H
    DPRINTF(10, ("thread_mutex_lock(%p)\n", mutex));
    if(pthread_mutex_lock((pthread_mutex_t *)mutex) == 0)
		return TRUE;
#else defined(HAVE_WINDOWS_H)
    switch(WaitForSingleObject((HANDLE)*mutex, INFINITE)){
	case WAIT_OBJECT_0:
		return TRUE;
	default:
		break;
    }
#endif
    return FALSE;
}
#endif

#ifdef USE_THREAD
int thread_mutex_unlock(thread_mutex *mutex){
#ifdef HAVE_PTHREAD_H
    DPRINTF(10, ("thread_mutex_unlock(%p)\n", mutex));
    if(pthread_mutex_unlock((pthread_mutex_t *)mutex) == 0)
		return TRUE;
#else defined(HAVE_WINDOWS_H)
    if(ReleaseMutex((HANDLE)*mutex) != 0)
		return TRUE;
#endif
    return FALSE;
}
#endif
