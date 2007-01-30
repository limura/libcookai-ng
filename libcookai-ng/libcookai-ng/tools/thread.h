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

#ifndef PASTRY_LIKE_THREAD_H
#define PASTRY_LOKE_THREAD_H

#include "../config.h"
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef USE_THREAD
#ifdef HAVE_PTHREAD_H
typedef pthread_t threadID;
typedef pthread_mutex_t thread_mutex;
typedef pthread_cond_t thread_cond;
typedef void *(*pthreadFunc)(void *data);
typedef pthreadFunc thread_func;
#elif defined(HAVE_WINDOWS_H)
typedef HANDLE threadID;
typedef HANDLE thread_mutex;
typedef HANDLE thread_cond;
typedef unsigned int (__stdcall *winThreadFunc)(void *option);
typedef winThreadFunc thread_func;
#else
#error "can not use thread. need pthread.h or windows.h"
#endif
#else
typedef int threadID;
typedef int thread_mutex;
typedef int thread_cond;
typedef void *thread_func;
#endif

int thread_create(threadID *id, thread_func func, void *userdata);
int thread_cancel(threadID id);
int thread_exit();
int thread_mutex_init(thread_mutex *mutex, char *name);
int thread_mutex_lock(thread_mutex *mutex);
int thread_mutex_unlock(thread_mutex *mutex);
int thread_mutex_destroy(thread_mutex *mutex);
int thread_cond_init(thread_cond *cond);
#ifdef HAVE_PTHRED_H
#define thread_cond_reset(a) /* */
#else
int thread_cond_reset(thread_cond *cond);
#endif
int thread_cond_signal(thread_cond *cond);
int thread_cond_wait(thread_cond *cond, thread_mutex *mutex);
int thread_cond_timedwait(thread_cond *cond, thread_mutex *mutex, int usec);
int thread_cond_destroy(thread_cond *cond);
#ifndef USE_THREAD
#define thread_create(a, b, c) /* */
#define thread_cancel(a) /* */
#define thread_exit() /* */
#define thread_mutex_init(a, b) /* */
#define thread_mutex_lock(a) /* */
#define thread_mutex_unlock(a) /* */
#define thread_mutex_destroy(a) /* */
#define thread_cond_init(a) /* */
#define thread_cond_reset(a) /* */
#define thread_cond_signal(a) /* */
#define thread_cond_wait(a, b) /* */
#define thread_cond_timedwait(a, b, c) /* */
#define thread_cond_destroy(a) /* */
#endif
#ifdef __cplusplus
}
#endif
#endif /* PASTRY_LIKE_THREAD_H */
