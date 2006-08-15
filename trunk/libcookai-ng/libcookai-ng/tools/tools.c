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
 * $Id: tools.c 15 2006-08-09 04:33:49Z uirou.j $
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <string.h>
#ifdef _LIBCOOKAI_WINDOWS_
#include <dprintf>
#endif

#include "../config.h"
#include "tools.h"

#define HEXDUMP_WIDTH (16)

#ifdef DEBUG
static int tools_printlevel = 10;
#endif /* DEBUG */

void set_printlevel(int lvl){
    if(lvl < 0)
	lvl = 0;
#ifdef DEBUG
    tools_printlevel = lvl;
#endif
}

int check_printlevel(int lvl){
#ifdef DEBUG
    return lvl >= tools_printlevel;
#else
    return 0;
#endif
}

#ifdef _LIBCOOKAI_WINDOWS_
void windprintf(char *fmt, ...){
    char buf[10240];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf) -1, fmt, ap);
    OutputDebugString(buf);
    va_end(ap);
}
#endif /* _LIBCOOKAI_WINDOWS_ */

void t_warn(warn_type wt, char *fmt, ...){
    va_list ap;
    FILE *fp;
    int halt = 0;

    fp = stderr;
    switch(wt){
    case WARN_DEBUG:
	fp = stdout;
	break;
    case WARN_INFO:
	fp = stdout;
	break;
    case WARN_ERR:
	fp = stderr;
	fflush(stdout);
	halt = 1;
	break;
    }

    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);

    if(halt)
	exit(1);
}

void fatal(char *fmt, ...){
    va_list ap;

    fflush(stdout);

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}

void hexdump(unsigned char *p, size_t len){
    int i;

    while(len > 0){
	for(i = 0; i < HEXDUMP_WIDTH && len > 0; i++, len--, p++){
	    if(i == HEXDUMP_WIDTH / 2)
		DPRINTF(1000, ("|"));
	    DPRINTF(1000, ("%02x ", *p));
	}
	DPRINTF(1000, ("\n"));
    }
}

void hexdump_X(unsigned char *p, size_t len){
    int i;
    size_t tmp_len;
    unsigned char *tmp_p;

    for(i = 0; i < HEXDUMP_WIDTH; i++){
	if(i == HEXDUMP_WIDTH / 2)
	    DPRINTF(1000, ("|"));
	DPRINTF(1000, ("%2x ", i));
    }
    DPRINTF(1000, ("||"));
    for(i = 0; i < HEXDUMP_WIDTH; i++){
	if(i == HEXDUMP_WIDTH / 2)
	    DPRINTF(1000, ("|"));
	DPRINTF(1000, (" "));
    }
    DPRINTF(1000, ("|\n"));
    for(i = 0; i < HEXDUMP_WIDTH; i++){
	if(i == HEXDUMP_WIDTH / 2)
	    DPRINTF(1000, ("+"));
	DPRINTF(1000, ("---"));
    }
    DPRINTF(1000, ("++"));
    for(i = 0; i < HEXDUMP_WIDTH; i++){
	if(i == HEXDUMP_WIDTH / 2)
	    DPRINTF(1000, ("+"));
	DPRINTF(1000, ("-"));
    }
    DPRINTF(1000, ("+\n"));

    while(len > 0){
	tmp_len = len;
	tmp_p = p;
	for(i = 0; i < HEXDUMP_WIDTH && len > 0; i++, len--, p++){
	    if(i == HEXDUMP_WIDTH / 2)
		DPRINTF(1000, ("|"));
	    DPRINTF(1000, ("%02x ", *p));
	}
	for(; i < HEXDUMP_WIDTH; i++){
	    if(i == HEXDUMP_WIDTH / 2)
		DPRINTF(1000, ("|"));
	    DPRINTF(1000, ("   "));
	}
	DPRINTF(1000, ("||"));
	for(i = 0; i < HEXDUMP_WIDTH && tmp_len > 0; i++, tmp_len--, tmp_p++){
	    if(i == HEXDUMP_WIDTH / 2)
		DPRINTF(1000, ("|"));
	    if(isprint(*tmp_p)){
	       DPRINTF(1000, ("%c", *tmp_p));
	    }else{
	       DPRINTF(1000, ("."));
	    }
	}
	for(; i < HEXDUMP_WIDTH; i++){
	    if(i == HEXDUMP_WIDTH / 2)
		DPRINTF(1000, ("|"));
	    DPRINTF(1000, (" "));
	}
	DPRINTF(1000, ("|\n"));
    }
}

char *sigid2str(int sigid){
    switch(sigid){
#ifdef SIGHUP
    case SIGHUP:
	return "SIGHUP";
#endif
#ifdef SIGINT
    case SIGINT:
	return "SIGINT";
#endif
#ifdef SIGQUIT
    case SIGQUIT:
	return "SIGQUIT";
#endif
#ifdef SIGILL
    case SIGILL:
	return "SIGILL";
#endif
#ifdef SIGTRAP
    case SIGTRAP:
	return "SIGTRAP";
#endif
#ifdef SIGABRT
    case SIGABRT:
	return "SIGABRT";
#endif
#ifdef SIGEMT
    case SIGEMT:
	return "SIGEMT";
#endif
#ifdef SIGFPE
    case SIGFPE:
	return "SIGFPE";
#endif
#ifdef SIGKILL
    case SIGKILL:
	return "SIGKILL";
#endif
#ifdef SIGBUS
    case SIGBUS:
	return "SIGBUS";
#endif
#ifdef SIGSEGV
    case SIGSEGV:
	return "SIGSEGV";
#endif
#ifdef SIGSYS
    case SIGSYS:
	return "SIGSYS";
#endif
#ifdef SIGPIPE
    case SIGPIPE:
	return "SIGPIPE";
#endif
#ifdef SIGALRM
    case SIGALRM:
	return "SIGALRM";
#endif
#ifdef SIGTERM
    case SIGTERM:
	return "SIGTERM";
#endif
#ifdef SIGURG
    case SIGURG:
	return "SIGURG";
#endif
#ifdef SIGSTOP
    case SIGSTOP:
	return "SIGSTOP";
#endif
#ifdef SIGTSTP
    case SIGTSTP:
	return "SIGTSTP";
#endif
#ifdef SIGCONT
    case SIGCONT:
	return "SIGCONT";
#endif
#ifdef SIGCHLD
    case SIGCHLD:
	return "SIGCHLD";
#endif
#ifdef SIGTTIN
    case SIGTTIN:
	return "SIGTTIN";
#endif
#ifdef SIGTTOU
    case SIGTTOU:
	return "SIGTTOU";
#endif
#ifdef SIGIO
    case SIGIO:
	return "SIGIO";
#endif
#ifdef SIGXCPU
    case SIGXCPU:
	return "SIGXCPU";
#endif
#ifdef SIGXFSZ
    case SIGXFSZ:
	return "SIGXFSZ";
#endif
#ifdef SIGVTALRM
    case SIGVTALRM:
	return "SIGVTALRM";
#endif
#ifdef SIGPROF
    case SIGPROF:
	return "SIGPROF";
#endif
#ifdef SIGWINCH
    case SIGWINCH:
	return "SIGWINCH";
#endif
#ifdef SIGINFO
    case SIGINFO:
	return "SIGINFO";
#endif
#ifdef SIGUSR1
    case SIGUSR1:
	return "SIGUSR1";
#endif
#ifdef SIGUSR2
    case SIGUSR2:
	return "SIGUSR2";
#endif
#ifdef SIGPWR
    case SIGPWR:
	return "SIGPWR";
#endif
#ifdef SIGSTKSZ
    case SIGSTKSZ:
	return "SIGSTKSZ";
#endif
    default:
	break;
    }
    return "UNKNOWN_SIGNAL";
}

int fd_can_read(int fd){
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 1;

    if(select(FD_SETSIZE, &rfds, NULL, NULL, &tv) > 0 && FD_ISSET(fd, &rfds))
	return TRUE;
    return FALSE;
}

int fd_can_write(int fd){
    fd_set wfds;
    struct timeval tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
    tv.tv_sec = 0;
    tv.tv_usec = 1;

    if(select(FD_SETSIZE, NULL, &wfds, NULL, &tv) > 0 && FD_ISSET(fd, &wfds))
	return TRUE;
    return FALSE;
}

char *buf2str(char *p, size_t siz, int bank){
    static char buf[10][8192];
    if(bank >= 10 || bank < 0)
	bank = 9;
    if(siz > sizeof(buf) - 1)
	siz = sizeof(buf) - 1;
#if 0
    memcpy(buf[bank], p, siz);
#else
    {
	char *pp;
	int i;
	pp = buf[bank];
	for(i = 0; i < (int)siz; i++){
	    if(isprint(*p)){
		*pp = *p;
	    }else{
		*pp = '.';
	    }
	    pp++, p++;
	}
    }
#endif
    buf[bank][siz] = '\0';

    return buf[bank];
}

/* return (float)(tv2 - tv1) */
float diff_time(struct timeval *tv1, struct timeval *tv2){
    long diff_sec;
    long diff_usec;

    if(tv1 == NULL || tv2 == NULL)
	return -0.0;

    diff_sec = tv2->tv_sec - tv1->tv_sec;
    diff_usec = tv2->tv_usec - tv1->tv_usec;
    while(diff_usec < 0){
	diff_sec--;
	diff_usec += 1000000;
    }

    return (float)diff_sec + (float)diff_usec / 1000000;
}

char *realloc_push(char *buf_head, char **now, char *append, size_t size){
    char *p;
    size_t len;

    if(buf_head == NULL){
	buf_head = (char *)malloc(size);
	if(buf_head == NULL)
	    fatal("%s(%d): no more memory! (size: %d)\n", __FILE__, __LINE__, size);
	*now = buf_head;

	memcpy(*now, append, size);
	*now += size;
	return buf_head;
    }
    len = *now - buf_head;
    p = (char *)realloc(buf_head, *now - buf_head + size);
    if(p == NULL)
	fatal("%s(%d): no more memory! (size: %d)\n", __FILE__, __LINE__, *now - buf_head + size);
    *now = p + len;
    buf_head = p;

    memcpy(*now, append, size);
    *now += size;
    return buf_head;
}

char *realloc_push_uint32(char *buf_head, char **now, uint32_t data){
    uint32_t l = htonl(data);
    return realloc_push(buf_head, now, (char *)&l, sizeof(l));
}

char *pop_buf(char **src, size_t siz, char *dst, size_t *update_siz){
    if(update_siz != NULL && *update_siz < siz)
	return NULL;

    memcpy(dst, *src, siz);
    *src += siz;

    if(update_siz != NULL)
	*update_siz -= siz;

    return dst;
}

uint32_t pop_uint32(char **src, size_t *siz){
    uint32_t l;

    if(siz != NULL && *siz < sizeof(uint32_t))
	return 0;

    memcpy(&l, *src, sizeof(l));
    l = ntohl(l);
    *src += sizeof(uint32_t);
    if(siz != NULL)
	*siz -= sizeof(uint32_t);
    return l;
}

int bin_cmp(char *p1, char *p2, size_t siz){
    while(siz-- > 0){
	if(*p1 != *p2){
	    return *p1 - *p2;
	}
	p1++, p2++;
    }
    return 0;
}

static unsigned long seed = 0;

/* return  0 to max - 1  */
int rnd0(int max){
    if(max <= 0)
	return 0;
    if(seed == 0){
	int pid = 0;
	int now_time = 0;
#ifdef HAVE_GETPID
	pid = getpid();
#endif
#ifdef HAVE_TIME
	now_time = time(NULL);
#endif
#ifdef HAVE_TIMEGETTIME
	now_time = timeGetTime();
#endif
	seed = pid + now_time;
#ifdef HAVE_RANDOM
	srandom(seed);
#else
	srand(seed);
#endif
    }
#ifdef HAVE_RANDOM
    return (int)(random() * (double)max / 0x7fffffff);
#else
    return (int)(rand() * (double)max / RAND_MAX);
#endif
}
int rnd1(int max){
    return rnd0(max) + 1;
}

/*
 * "%i"  int32_t (int32_t)
 * "%ui" uint32_t (uint32_t)
 * "%s"  '\0' terminated string (char *)
 * "%d"  data. with data length (size_t size, unsigned char *dataP)
 * "%f"  fixed length value. buf is not include data length. (size_t size, unsigned char *dataP)
 */
enum {
    FLAGS_UNSIGNED = 1 << 0,
};
unsigned char *vf_create_send_buf(size_t *ret_siz, char *fmt, ...){
    va_list ap;
    size_t siz = 0, tmp_siz;
    unsigned char *prev_buf, *p, *pp, *str;
    int32_t i32;
    uint32_t ui32;
    int flags;
    static unsigned char *buf = NULL;
    static size_t buf_siz = 0x1000;

    if(fmt == NULL || ret_siz == NULL)
	return NULL;
    *ret_siz = 0;
    if(buf == NULL){
	buf_siz = 0x1000;
	buf = (unsigned char *)malloc(sizeof(char)*buf_siz);
	if(buf == NULL)
	    return NULL;
    }
    prev_buf = NULL;

#define CHECK_SIZE() while(siz > buf_siz) \
    { \
	prev_buf = buf; \
	if(buf_siz > buf_siz + buf_siz) \
	    return NULL; \
	buf_siz += buf_siz; \
	buf = (unsigned char *)realloc(buf, sizeof(char) * buf_siz); \
	if(buf == NULL) \
	    return NULL; \
	pp = buf + (pp - prev_buf); \
    }

    flags = 0;
    va_start(ap, fmt);
    p = (unsigned char *)fmt;
    pp = buf;
    while(*p != '\0'){
	switch(*p){
	case '%': /* flash */
	    flags = 0;
	    break;
	case 'u': /* unsigned */
	    flags |= FLAGS_UNSIGNED;
	    break;
	case 'i': /* int32 */
	    if(flags & FLAGS_UNSIGNED){
		siz += sizeof(uint32_t);
		CHECK_SIZE();
		ui32 = va_arg(ap, uint32_t);
		ui32 = htonl(ui32);
		memcpy(pp, &ui32, sizeof(uint32_t));
		pp += sizeof(uint32_t);
	    }else{
		siz += sizeof(int32_t);
		CHECK_SIZE();
		i32 = va_arg(ap, int32_t);
		i32 = htonl(i32);
		memcpy(pp, &i32, sizeof(int32_t));
		pp += sizeof(int32_t);
	    }
	    break;
	case 'f': /* fixed size value */
	    tmp_siz = va_arg(ap, size_t);
	    siz += tmp_siz;
	    CHECK_SIZE();
	    str = va_arg(ap, unsigned char *);
	    memcpy(pp, str, tmp_siz);
	    pp += tmp_siz;
	    break;
	case 's': /* string */
	    str = (unsigned char *)va_arg(ap, char *);
	    tmp_siz = strlen((char *)str);
	    siz += sizeof(uint32_t);
	    CHECK_SIZE();
	    ui32 = (uint32_t)htonl((u_long)tmp_siz);
	    memcpy(pp, &ui32, sizeof(ui32));
	    pp += sizeof(ui32);
	    siz += tmp_siz;
	    CHECK_SIZE();
	    memcpy(pp, str, tmp_siz);
	    pp += tmp_siz;
	    break;
	case 'd': /* data */
	    tmp_siz = va_arg(ap, size_t);
	    siz += sizeof(uint32_t);
	    CHECK_SIZE();
	    ui32 = (uint32_t)htonl((u_long)tmp_siz);
	    memcpy(pp, &ui32, sizeof(ui32));
	    pp += sizeof(ui32);
	    str = va_arg(ap, unsigned char *);
	    siz += tmp_siz;
	    CHECK_SIZE();
	    memcpy(pp, str, tmp_siz);
	    pp += tmp_siz;
	    break;
	default:
	    break;
	}
	p++;
    }
    va_end(ap);
#undef CHECK_SIZE

    p = (unsigned char *)malloc(siz + sizeof(uint32_t));
    if(p == NULL)
	return NULL;
    ui32 = (uint32_t)htonl((u_long)siz);
    memcpy(p, &ui32, sizeof(ui32));
    memcpy(p + sizeof(uint32_t), buf, siz);
    *ret_siz = siz + sizeof(uint32_t);

    return p;
}

unsigned char *vf_get_from_send_buf(unsigned char *buf, size_t bufsiz, char *fmt, ...){
    va_list ap;
    unsigned char *p, *pp;
    uint32_t ui32, *ui32p;
    int32_t i32, *i32p;
    size_t *sizep = NULL;
    unsigned char **strp, *str;
    int flags;
    size_t tmp_siz;

#define CHECK_SIZE(siz) if(bufsiz < siz) return NULL; bufsiz -= siz;

    flags = 0;
    va_start(ap, fmt);
    p = (unsigned char *)fmt;
    pp = buf;
    while(*p != '\0'){
	switch(*p){
	case '%': /* flash */
	    flags = 0;
	    sizep = NULL;
	    break;
	case 'u': /* unsigned */
	    flags |= FLAGS_UNSIGNED;
	    break;
	case 'i': /* int32 */
	    CHECK_SIZE(sizeof(uint32_t));
	    if(flags & FLAGS_UNSIGNED){
		ui32p = va_arg(ap, uint32_t *);
		memcpy(&ui32, pp, sizeof(uint32_t));
		pp += sizeof(uint32_t);
		*ui32p = ntohl(ui32);
	    }else{
		i32p = va_arg(ap, int32_t *);
		memcpy(&i32, pp, sizeof(int32_t));
		pp += sizeof(int32_t);
		*i32p = ntohl(i32);
	    }
	    break;
	case 'f': /* fixed size value */
	    tmp_siz = va_arg(ap, size_t);
	    str = va_arg(ap, unsigned char *);
	    memcpy(str, pp, tmp_siz);
	    pp += tmp_siz;
	    break;
	case 'd': /* data */
	    sizep = va_arg(ap, size_t *);
	case 's': /* string */
	    strp = va_arg(ap, unsigned char **);
	    CHECK_SIZE(sizeof(uint32_t));
	    memcpy(&ui32, pp, sizeof(uint32_t));
	    pp += sizeof(uint32_t);
	    ui32 = ntohl(ui32);
	    if(*p == 'd' && sizep != NULL)
		*sizep = ui32;
	    *strp = (unsigned char *)malloc(sizeof(char) *
					    (ui32 + (*p == 's' ? 1 : 0)));
	    if(*strp == NULL)
		return NULL;
	    CHECK_SIZE(ui32);
	    memcpy(*strp, pp, ui32);
	    if(*p == 's')
		(*strp)[ui32] = '\0';
	    pp += ui32;
	    break;
	default:
	    break;
	}
	p++;
    }
#undef CHECK_SIZE
    
    va_end(ap);

    return pp;
}

/* binary incriment. required buf have depth bits. */
void bin_inc(unsigned char *buf, int depth){
    int i;
    int n;
    for(i = depth; i >= 0; i-= (i % 8 + 1)){
	n = buf[i/8];
	n += 1 << (7 - (i % 8));
	buf[i/8] = 0xff & n;
	if(!(n & 0x100))
	    break;
    }
}

unsigned long long bin2ull(unsigned char *buf, int depth){
    int i, n, gap;
    unsigned long long ull = 0;
    for(i = 0; i < depth; i += 8){
	n = buf[i/8];
	gap = 8;
	if((depth - i) < 8){
	    gap = (depth - i + 1);
	    n = n >> (8 - gap);
	}
	ull = ull << gap;
	ull += n;
    }
    return ull;
}

