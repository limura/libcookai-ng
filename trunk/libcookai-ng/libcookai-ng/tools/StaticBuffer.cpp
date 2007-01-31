/*
 * Copyright (c) 2006-2007 IIMURA Takuji. All rights reserved.
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

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#ifdef HAVE__H
#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "../tools/StaticBuffer.h"

namespace Cookai {

StaticBuffer::StaticBuffer(size_t bufsize)
{
    size = bufsize;
    buf = NULL;
    if(size > 0)
	buf = (unsigned char *)malloc(size);

    now = 0;
}

StaticBuffer::~StaticBuffer(void)
{
    if(buf != NULL)
	free(buf);
}

unsigned char *StaticBuffer::getBuffer(void){
    return buf;
}
size_t StaticBuffer::getDataLength(void){
    return now;
}
size_t StaticBuffer::getBufferSize(void){
    return size;
}
bool StaticBuffer::write(unsigned char *buf, size_t dataSize){
    if(dataSize > size - now)
	return false;

    memcpy(&buf[now], buf, dataSize);
    now += dataSize;
    return true;
}

bool StaticBuffer::readFromFD(int fd, size_t dataSize){
    if(fd < 0)
	return false;
    if(dataSize + now > size){
	/* skip */

	return false;
    }
    int length = read(fd, &buf[now], dataSize);
    if(length == 0){ // EOF
    }else if(length < 0){ // error
    }

    return true;
}

};
