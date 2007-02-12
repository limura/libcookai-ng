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
#ifndef PASTRY_LIKE_CONFIG_H
#define PASTRY_LIKE_CONFIG_H

#ifdef __NetBSD__
#undef HAVE_SHA_H
#undef HAVE_OPENSSL_SHA_H
#define HAVE_SHA1_H
#define HAVE_SHA1INIT
#undef HAVE_SHA_CTX
#define HAVE_STDINT_H
#define HAVE_SYS_TIME_H
#define HAVE_UNISTD_H
#define HAVE_INTTYPES_H
#define HAVE_SYS_TIME_H
#define HAVE_NETDB_H
#define HAVE_SYS_SOCKET_H
#define HAVE_NETINET_IN_H
#define HAVE_ARPA_INET_H
#define HAVE_SYS_RESOURCE_H
#define HAVE_FD_SET
#undef HAVE_WINSOCK2_H
#define HAVE_BZERO
#define HAVE_GETTIMEOFDAY
#undef HAVE_TIMEGETTIME
#define HAVE_SNPRINTF
#define HAVE_NETINET_TCP_H
#define HAVE_DEV_RANDOM
#define HAVE_RANDOM
#define HAVE_TIME
#define HAVE_RUSAGE
#undef HAVE_WS2TCPIP_H
#undef HAVE_CLOSESOCKET
#define HAVE_FCNTL
#define HAVE_BZERO
#undef HAVE_IO_H
#undef HAVE__READ
#define HAVE_POLL_H
#define HAVE_POLL
#define HAVE_SYS_SELECT_H
#define HAVE_SELECT

#define USE_THREAD
#undef HAVE_WINDOWS_H
#define HAVE_PTHREAD_H

#define DLL_EXPORT /* */
#endif

#ifdef __FreeBSD__
#define HAVE_SHA_H
#undef HAVE_OPENSSL_SHA_H
#undef HAVE_SHA1_H
#undef HAVE_SHA1INIT
#undef HAVE_SHA_CTX
#define HAVE_SYS_TIME_H
#define HAVE_UNISTD_H
#define HAVE_INTTYPES_H
#define HAVE_SYS_TIME_H
#define HAVE_NETDB_H
#define HAVE_SYS_SOCKET_H
#define HAVE_NETINET_IN_H
#define HAVE_ARPA_INET_H
#define HAVE_SYS_RESOURCE_H
#define HAVE_FD_SET
#undef HAVE_WINSOCK2_H
#define HAVE_BZERO
#define HAVE_GETTIMEOFDAY
#undef HAVE_TIMEGETTIME
#define HAVE_SNPRINTF
#define HAVE_NETINET_TCP_H
#define HAVE_DEV_RANDOM
#define HAVE_RANDOM
#define HAVE_TIME
#define HAVE_RUSAGE
#undef HAVE_WS2TCPIP_H
#undef HAVE_CLOSESOCKET
#define HAVE_FCNTL
#define HAVE_BZERO
#undef HAVE_IO_H
#undef HAVE__READ

#define HAVE_POLL_H
#define HAVE_POLL
#define HAVE_SYS_SELECT_H
#define HAVE_SELECT

#define USE_THREAD
#undef HAVE_WINDOWS_H
#define HAVE_PTHREAD_H

#define DLL_EXPORT /* */
#endif

#ifdef __Linux__
#define HAVE_OPENSSL_SHA_H
#define HAVE_SHA_CTX
#define HAVE_SYS_TIME_H
#define HAVE_UNISTD_H
#define HAVE_INTTYPES_H
#define HAVE_SYS_TIME_H
#define HAVE_NETDB_H
#define HAVE_SYS_SOCKET_H
#define HAVE_NETINET_IN_H
#define HAVE_ARPA_INET_H
#define HAVE_SYS_RESOURCE_H
#define HAVE_FD_SET
#undef HAVE_WINSOCK2_H
#define HAVE_BZERO
#define HAVE_GETTIMEOFDAY
#undef HAVE_TIMEGETTIME
#define HAVE_SNPRINTF
#define HAVE_NETINET_TCP_H
#define HAVE_DEV_RANDOM
#define HAVE_RANDOM
#define HAVE_TIME
#define HAVE_RUSAGE
#undef HAVE_WS2TCPIP_H
#undef HAVE_CLOSESOCKET
#define HAVE_FCNTL
#define HAVE_BZERO
#undef HAVE_IO_H
#undef HAVE__READ
#define HAVE_POLL_H
#define HAVE_POLL
#define HAVE_SYS_SELECT_H
#define HAVE_SELECT

#define USE_THREAD
#undef HAVE_WINDOWS_H
#define HAVE_PTHREAD_H

#define DLL_EXPORT /* */
#endif

#ifdef __APPLE__ /* for MacOS X */
#define HAVE_STDINT_H
#define HAVE_OPENSSL_SHA_H
#define HAVE_SHA_CTX
#define HAVE_SYS_TIME_H
#define HAVE_UNISTD_H
#define HAVE_INTTYPES_H
#define HAVE_SYS_TIME_H
#define HAVE_NETDB_H
#define HAVE_SYS_SOCKET_H
#define HAVE_NETINET_IN_H
#define HAVE_ARPA_INET_H
#define HAVE_SYS_RESOURCE_H
#define HAVE_FD_SET
#undef HAVE_WINSOCK2_H
#define HAVE_BZERO
#define HAVE_GETTIMEOFDAY
#undef HAVE_TIMEGETTIME
#define HAVE_SNPRINTF
#define HAVE_NETINET_TCP_H
#define HAVE_DEV_RANDOM
#define HAVE_RANDOM
#define HAVE_TIME
#define HAVE_RUSAGE
#undef HAVE_WS2TCPIP_H
#undef HAVE_CLOSESOCKET
#define HAVE_FCNTL
#define HAVE_BZERO
#undef HAVE_IO_H
#undef HAVE__READ
#define HAVE_POLL_H
#define HAVE_POLL
#define HAVE_SYS_SELECT_H
#define HAVE_SELECT

#define USE_THREAD
#undef HAVE_WINDOWS_H
#define HAVE_PTHREAD_H

#define DLL_EXPORT /* */
#endif

#if defined(WIN32) || defined(WIN64)

#define _LIBCOOKAI_WINDOWS_
#define DEBUG
#undef HAVE_SYS_TIME_H
#undef HAVE_UNISTD_H
#undef HAVE_INTTYPES_H
#undef HAVE_SYS_TIME_H
#undef HAVE_NETDB_H
#undef HAVE_SYS_SOCKET_H
#undef HAVE_NETINET_IN_H
#undef HAVE_ARPA_INET_H
#undef HAVE_SYS_RESOURCE_H
#undef HAVE_FD_SET
#define HAVE_WINSOCK2_H
#include "Win32/inttypes.h"
#undef HAVE_BZERO
#undef HAVE_GETTIMEOFDAY
#define HAVE_TIMEGETTIME
#undef HAVE_SNPRINTF
#undef HAVE_NETINET_TCP_H
#undef HAVE_DEV_RANDOM
#undef HAVE_RANDOM
#undef HAVE_TIME
#undef HAVE_RUSAGE
#define HAVE_WS2TCPIP_H
#define HAVE_CLOSESOCKET
#define HAVE_SHA_H
#undef HAVE_FCNTL
#define HAVE_IOCTLSOCKET
#define HAVE_WSAGETLASTERROR
#undef HAVE_BZERO
#define HAVE_IO_H
#define HAVE_WSASTARTUP
#define HAVE_mciSendStringW
#define HAVE_MMSYSTEM_H
#define HAVE__READ
#undef HAVE_POLL_H
#undef HAVE_POLL
#undef HAVE_SYS_SELECT_H
#define HAVE_SELECT

#define USE_THREAD
#define HAVE_WINDOWS_H
#define HAVE_WINSOCK2_H
#define HAVE_PROCESS_H
#undef HAVE_PTHREAD_H

#define NSNS_NONBLOCK
#define NSNS_NODELAY

#define DLL_EXPORT __declspec(dllexport)
#endif

#endif /* PASTRY_LIKE_CONFIG_H */
