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
 * $Id: thread.c 14 2006-08-08 11:47:12Z uirou.j $
 */

#include "../config.h"

#include "thread.h"
#include "tools.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

#ifdef HAVE_PROCESS_H
#include <process.h>
#endif

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#ifdef USE_THREAD

int thread_create(threadID *id, thread_func func, void *userdata){
#ifdef HAVE_PTHREAD_H
    if(pthread_create((pthread_t *)id, NULL, (pthreadFunc)func, userdata) == 0)
	return TRUE;
#else defined(HAVE_PROCESS_H)
    HANDLE h = (HANDLE)_beginthreadex(NULL, 0, (winThreadFunc)func, userdata, 0, NULL);
    if(h != 0){
	id = h;
	return TRUE;
    }
#endif
    return FALSE;
}

int thraed_cancel(threadID id){
#ifdef HAVE_PTHREAD_H
    if(pthread_cancel((pthread_t)id) == 0)
	return TRUE;
#else defined(HAVE_WINDOWS_H)
    if(TerminateThread((HANDLE)id, 0) != 0)
	return TRUE;
#endif
    return FALSE;
}

int thread_exit(){
#ifdef HAVE_PTHREAD_H
    pthread_exit(NULL);
#else defined(HAVE_PROCESS_H)
    _endthreadex(0);
#endif
    return FALSE;
}

int thread_mutex_init(thread_mutex *mutex, char *name){
#ifdef HAVE_PTHREAD_H
    if(pthread_mutex_init((pthread_mutex_t *)mutex, NULL) == 0)
	return TRUE;
#else defined(HAVE_WINDOWS_H)
    *mutex = (thread_mutex)CreateMutex(NULL, FALSE, (LPCWSTR)name);
    if(*mutex != 0)
	return TRUE;
#endif
    return FALSE;
}

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

int thread_mutex_destroy(thread_mutex *mutex){
#ifdef HAVE_PTHREAD_H
    if(pthread_mutex_destroy((pthread_mutex_t *)mutex) == 0)
	return TRUE;
#else defined(HAVE_WINDOWS_H)
    if(CloseHandle((HANDLE)mutex) != 0)
	return TRUE;
#endif
    return FALSE;
}

int thread_cond_init(thread_cond *cond){
#ifdef HAVE_PTHREAD_H
    if(pthread_cond_init((pthread_cond_t *)cond, NULL) == 0)
	return TRUE;
#else defined(HAVE_WINDOWS_H)
    *cond = (thread_cond)CreateEvent(NULL, TRUE, TRUE, NULL);
    if(*cond != 0)
	return TRUE;
#endif
    return FALSE;
}

#ifdef HAVE_WINDOWS_H
int thread_cond_reset(thread_cond *cond){
    if(ResetEvent((HANDLE)*cond) == TRUE)
	return TRUE;
    return FALSE;
}
#endif /* HAVE_WINDOWS_H */

int thread_cond_signal(thread_cond *cond){
#ifdef HAVE_PTHREAD_H
    if(pthread_cond_signal((pthread_cond_t *)cond) == 0)
	return TRUE;
#else defined(HAVE_WINDOWS_H)
    if(SetEvent((HANDLE)*cond) == TRUE)
    	return TRUE;
#endif
    return FALSE;
}

int thread_cond_wait(thread_cond *cond, thread_mutex *mutex){
#ifdef HAVE_PTHREAD_H
    if(pthread_cond_wait((pthread_cond_t *)cond, (pthread_mutex_t *)mutex) == 0)
	return TRUE;
#else defined(HAVE_WINDOWS_H)
    thread_mutex_unlock(mutex);
    switch(WaitForSingleObject((HANDLE)*cond, INFINITE)){
	case WAIT_OBJECT_0:
	    thread_mutex_lock(mutex);
	    return TRUE;
	default:
	    thread_mutex_lock(mutex);
	    return FALSE;
    }
#endif
    return FALSE;
}

int thread_cond_timedwait(thread_cond *cond, thread_mutex *mutex, int usec){
#ifdef HAVE_PTHREAD_H
    struct timeval tv;
    struct timespec ts;
    long l;
    if(usec <= 0)
	return thread_cond_wait(cond, mutex);
    if(gettimeofday(&tv, NULL) != 0)
	return FALSE;
    ts.tv_sec = tv.tv_sec;
    l = tv.tv_usec + usec;
    while(l >= 1000000){
	ts.tv_sec++;
	l -= 1000000;
    }
    ts.tv_nsec = l * 1000;
    if(pthread_cond_timedwait((pthread_cond_t *)cond, (pthread_mutex_t *)mutex, &ts) == 0)
	return TRUE;
#else defined(HAVE_WINDOWS_H)
    if(usec <= 0)
	return thread_cond_wait(cond, mutex);
    thread_mutex_unlock(mutex);
    switch(WaitForSingleObject((HANDLE)*cond, usec / 1000)){
	case WAIT_OBJECT_0:
	    thread_mutex_lock(mutex);
	    ResetEvent((HANDLE)*cond);
	    return TRUE;
	    break;
	default:
	    thread_mutex_lock(mutex);
	    break;
    }
#endif
    return FALSE;
}

int thread_cond_destroy(thread_cond *cond){
#ifdef HAVE_PTHREAD_H
    if(pthread_cond_destroy((pthread_cond_t *)cond) == 0)
	return TRUE;
#else defined(HAVE_PTHREAD_H)
    if(CloseHandle((HANDLE)cond) != 0)
	return TRUE;
#endif
    return FALSE;
}

#endif /* USE_THREAD */
