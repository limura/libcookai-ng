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
 * $Id: node.c,v 1.3 2004/12/16 16:09:36 takuji-i Exp $
 */

#ifndef PASTRY_LIKE_THREAD_H
#define PASTRY_LOKE_THREAD_H

#include "config.h"

#ifdef USE_THREAD
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
typedef pthread_mutex_t thread_mutex;
#elif defined(HAVE_WINDOWS_H)
#include <windows.h>
typedef HANDLE thread_mutex;
#else
#error "can not use thread. need pthread.h or windows.h"
#endif
#else
typedef int thread_mutex;
#endif

#ifdef __cplusplus
extern "C" {
#endif
int thread_mutex_init(thread_mutex *mutex, char *name);
int thread_mutex_lock(thread_mutex *mutex);
int thread_mutex_unlock(thread_mutex *mutex);
#ifndef USE_THREAD
#define thread_mutex_init(a, b) /* */
#define thread_mutex_lock(a) /* */
#define thread_mutex_unlock(a) /* */
#endif
#ifdef __cplusplus
}
#endif
#endif /* PASTRY_LIKE_THREAD_H */