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

#ifndef _TOOLS_H
#define _TOOLS_H

#include "config.h"

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef DEBUG
extern int tools_printlevel;
#ifdef _LIBCOOKAI_WINDOWS_
void windprintf(char *fmt, ...);
#define DPRINTF(level, x) { if(check_printlevel(level)){ windprintf x; } }
#else
#define DPRINTF(level, x) { if(check_printlevel(level)){ printf x; fflush(stdout); } }
#endif
#define DPERROR(x) perror x;
#define DFUNC(level, x) { if(check_printlevel(level)){ x; } }
#else
#define DPRINTF(level, x) /* */
#define DPERROR(x) /* */
#define DFUNC(level, x) /* */
#endif

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif


typedef enum {
    WARN_DEBUG,
    WARN_INFO,
    WARN_ERR,
} warn_type;

void set_printlevel(int lvl);
int check_printlevel(int lvl);
void t_warn(warn_type wt, char *fmt, ...);
void fatal(char *fmt, ...);
void hexdump(unsigned char *p, size_t len);
void hexdump_X(unsigned char *p, size_t len);
char *sigid2str(int sigid);
int fd_can_read(int fd);
int fd_can_write(int fd);
char *buf2str(char *p, size_t siz, int bank);
float diff_time(struct timeval *tv1, struct timeval *tv2); /* return (float)(tv2 - tv1) */
char *realloc_push(char *buf_head, char **now, char *append, size_t size);
char *realloc_push_uint32(char *buf_head, char **now, uint32_t data);
char *pop_buf(char **src, size_t siz, char *dst, size_t *update_siz);
uint32_t pop_uint32(char **src, size_t *siz);
int bin_cmp(char *p1, char *p2, size_t siz);
void bin_inc(unsigned char *buf, int depth);
unsigned long long bin2ull(unsigned char *buf, int depth);

int rnd0(int max);
int rnd1(int max);

unsigned char *vf_create_send_buf(size_t *ret_siz, char *fmt, ...);
unsigned char *vf_get_from_send_buf(unsigned char *buf, size_t bufsiz, char *fmt, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TOOLS_H */

