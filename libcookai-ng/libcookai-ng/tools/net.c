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

#include "../config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif
#include <stdlib.h>

#include <sys/types.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>

#include "net.h"
#include "tools.h"

/*
 * from getaddrinfo(3) manual page.
 *
 * ex.
 *    sock = connect_stream("www.example.com", "http");
 *    sock = connect_stream("www.example.com", "80");
 */
int connect_stream(char *remote, char *service){
    struct addrinfo hints, *res, *res0;
    int s;

    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if(getaddrinfo(remote, service, &hints, &res0)){
	return -1;
    }

    s = -1;
    for(res = res0; res; res = res->ai_next){
	s = (int)socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(s < 0){
	    continue;
	}

	if(connect(s, res->ai_addr, (int)res->ai_addrlen) < 0){
#ifdef HAVE_CLOSESOCKET
	    closesocket(s);
#else
	    close(s);
#endif
	    s = -1;
	    continue;
	}
	break;
    }
    freeaddrinfo(res0);

    return s;
}

static int bind_any(int sock, int type, char *service){
    struct addrinfo hints, *res, *res0;
    int ret;

    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = type;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if(getaddrinfo(NULL, service, &hints, &res0)){
	return -1;
    }

    ret = -1;
    for(res = res0; res; res = res->ai_next){
#ifdef _LIBCOOKAI_WINDOWS_
	ret = bind((SOCKET)sock, res->ai_addr, (int)res->ai_addrlen);
#else
	ret = bind(sock, res->ai_addr, res->ai_addrlen);
#endif
	if(ret < 0){
	    continue;
	}
	break;
    }
    freeaddrinfo(res0);

    return ret;
}

static int bind_inaddr_any(int sock, int type, int port){
    struct sockaddr_storage ss;
    struct sockaddr_in *sockadd;
    struct sockaddr_in6 *sockadd6;
    size_t sa_size;

    switch(type){
    case AF_INET:
	sockadd = (struct sockaddr_in *)&ss;
	sockadd->sin_family = type;
	sockadd->sin_port = htons(port);
	sockadd->sin_addr.s_addr = INADDR_ANY;

	sa_size = sizeof(struct sockaddr_in);
	break;
    case AF_INET6:
	sockadd6 = (struct sockaddr_in6 *)&ss;
	sockadd6->sin6_family = type;
	sockadd6->sin6_port = htons(port);
	sockadd6->sin6_addr = in6addr_any;

	sa_size = sizeof(struct sockaddr_in6);
	break;
    default:
	return -1;
	break;
    }

    return bind(sock, (struct sockaddr *)&ss, (int)sa_size);
}

/* 
http://www2s.biglobe.ne.jp/~hig/ipv6/rfc2553.html
http://www.kame.net/newsletter/20020108/v6prog.html
http://www.kt.rim.or.jp/~ksk/sock-faq/unix-socket-faq-ja.html
 * create listened socket for INET.
 * ex.
 *    sock = listen_stream("8472", AF_INET);
 *    sock = listen_stream("8472", AF_INET6);
 */
int listen_stream(char *port, int type){
    int sock;

    if((sock = (int)socket(type, SOCK_STREAM, 0)) < 0)
	return -1;
#ifdef USE_REUSEADDR
    {
	int optval;
	socklen_t optlen;

	optval = TRUE;
	optlen = sizeof(optval);
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) != 0){
	    printf("%s(%d): setsockopt SO_REUSEADDR fail.\n", __FILE__, __LINE__);
        }
    }
#endif

#if 0
    if(bind_inaddr_any(sock, type, port)){
#else
    if(bind_any(sock, SOCK_STREAM, port)){
#endif
#ifdef HAVE_CLOSESOCKET
	closesocket(sock);
#else
	close(sock);
#endif
	return -1;
    }

    if(listen(sock, 50) < 0){
#ifdef HAVE_CLOSESOCKET
	closesocket((SOCKET)sock);
#else
	close(sock);
#endif
	return -1;
    }

    return sock;
}

/*
 * create binded datagram socket for INET.
 * ex.
 *    sock = bind_dgram(8472, AF_INET);
 *    sock = bind_dgram(8472, AF_INET6);
 */
int bind_dgram(char *port, int type){
    int sock;

    if((sock = (int)socket(type, SOCK_DGRAM, 0)) < 0)
	return -1;

#if 0
    if(bind_inaddr_any(sock, type, port)){
#else
    if(bind_any(sock, SOCK_DGRAM, port)){
#endif
#ifdef HAVE_CLOSESOCKET
	closesocket((SOCKET)sock);
#else
	close(sock);
#endif
	return -1;
    }

    return sock;
}

/*
 * ex.
 *    sock = connect_datagram("www.example.com", "8472");
 *
 * this function need accept_dgram().
 */
int connect_dgram(char *remote, char *service){
    struct addrinfo hints, *res, *res0;
    int s;
    char t;
    struct sockaddr_storage ss;
    socklen_t len;

    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if(getaddrinfo(remote, service, &hints, &res0)){
	return -1;
    }

    s = -1;
    for(res = res0; res; res = res->ai_next){
	s = (int)socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(s < 0){
	    continue;
	}

/*	if(bind_inaddr_any(s, res->ai_family, 0)){
	    DPRINTF(0, ("bind failed.\n"));
	    close(s);
	    s = -1;
	    continue;
	}*/

#ifdef DEBUG
	switch(res->ai_family){
	case PF_INET:
	    t = '4';
	    break;
	case PF_INET6:
	    t = '6';
	    break;
	default:
	    t = '?';
	    break;
	}
	DPRINTF(0, ("type: %c\n", t));
#endif /* DEBUG */

	sendto(s, &t, 1, 0, res->ai_addr, (int)res->ai_addrlen);
	DPRINTF(0, ("send SYN\n"));

	{
	    fd_set r_fds;
	    struct timeval tv;
	    int ret;

	    tv.tv_sec = 1;
	    tv.tv_usec = 0;

	    FD_ZERO(&r_fds);
	    FD_SET(s, &r_fds);

	    ret = select(FD_SETSIZE, &r_fds, NULL, NULL, &tv);

	    if(ret <= 0 || FD_ISSET(s, &r_fds) == 0){
#ifdef HAVE_CLOSESOCKET
		closesocket((SOCKET)s);
#else
		close(s);
#endif
		s = -1;
		continue;
	    }
	}

	len = sizeof(ss);
	recvfrom(s, &t, sizeof(t), 0, (struct sockaddr *)&ss, &len);
	DPRINTF(0, ("recv OK.\n"));

	if(connect(s, (struct sockaddr *)&ss, len) < 0){
	    DPERROR(("connect failed"));
#ifdef HAVE_CLOSESOCKET
	    closesocket((SOCKET)s);
#else
	    close(s);
#endif
	    s = -1;
	    continue;
	}
	break;
    }
    freeaddrinfo(res0);

    return s;
}

/*
 * ex.
 *
 * int sock = bind_dgram(8472, AF_INET6);
 * int new_sock = accept_dgram(sock, NULL, 0);
 * 
 * this function need connect_dgram().
 */
int accept_dgram(int sock, struct sockaddr *r_sa, socklen_t *r_len){
    char t;
    struct sockaddr_storage ss;
    struct sockaddr *sa;
    socklen_t len;
    int new_sock;

    if(sock < 0)
	return -1;

    len = sizeof(ss);
    if(recvfrom(sock, &t, sizeof(t), 0, (struct sockaddr *)&ss, &len) != 1)
	return -1;
    DPRINTF(0, ("recv SYN\n"));
    if(r_sa != NULL && *r_len >= len){
	memcpy(r_sa, &ss, len);
	*r_len = len;
    }

    sa = (struct sockaddr *)&ss;

    DPRINTF(0, ("create new socket.\n"));
    new_sock = (int)socket(sa->sa_family, SOCK_DGRAM, 0);
    if(new_sock < 0){
	return -1;
    }
    DPRINTF(0, ("\tbinding.\n"));
    if(bind_inaddr_any(new_sock, sa->sa_family, 0)){
	DPRINTF(0, ("bind failed.\n"));
#ifdef HAVE_CLOSESOCKET
	closesocket((SOCKET)new_sock);
#else
	close(new_sock);
#endif
	return -1;
    }
    DPRINTF(0, ("\tconnecting.\n"));
    if(connect(new_sock, (struct sockaddr *)&ss, len) < 0){
#ifdef HAVE_CLOSESOCKET
	closesocket((SOCKET)new_sock);
#else
	close(new_sock);
#endif
	DPRINTF(0, ("connect failed"));
	return -1;
    }
    DPRINTF(0, ("\tconnect OK. ACK send.\n"));
    send(new_sock, &t, 1, 0);

    return new_sock;
}

void closeSocket(int sock){
#ifdef _LIBCOOKAI_WINDOWS_
	shutdown(sock, SD_BOTH);
	closesocket(sock);
#else
	shutdown(sock, SHUT_RDWR);
	close(sock);
#endif
}
