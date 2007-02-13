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
#include "../tools/tools.h"

#include <stdio.h>
#include <errno.h>
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
#ifdef HAVE_IO_H
#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "StaticBuffer.h"

namespace Cookai {
namespace ChunkedConnection {

StaticBuffer::StaticBuffer(size_t bufsize)
{
    size = bufsize;
    buf = NULL;
    if(size > 0)
	buf = (unsigned char *)malloc(size);
    if(buf != NULL)
	memset(buf, 0, size);

    writePos = 0;
    readPos = 0;
}
StaticBuffer::StaticBuffer(unsigned char *originalBuf, size_t size){
    if(originalBuf == NULL && size > 0){
	buf = (unsigned char *)malloc(size);
	if(buf != NULL)
	    memset(buf, 0, size);
    }else{
	buf = originalBuf;
    }
    size = size;
    writePos = 0;
    readPos = 0;
}

StaticBuffer::~StaticBuffer(void)
{
    if(buf != NULL)
	free(buf);
}

unsigned char *StaticBuffer::GetBuffer(void){
    if(readPos >= size)
	return NULL;
    return &buf[readPos];
}
size_t StaticBuffer::GetDataLength(void){
    return writePos;
}
size_t StaticBuffer::GetBufferSize(void){
    return size;
}
size_t StaticBuffer::GetAvailableSize(void){
    return size - writePos;
}

// return value list
// value == 0: read data complete.
// value < 0: error
// value > 0: not complete.
int StaticBuffer::ReadFromSocket(int fd, size_t *readSize){
    if(fd < 0)
	return -1;
    if(writePos >= size){
	return 0;
    }
    if(*readSize > size - writePos)
	*readSize = size - writePos;

    errno = 0;
#ifdef HAVE_WSAGETLASTERROR
    WSASetLastError(0);
#endif
    int length = recv(fd, (char *)&buf[writePos], (int)*readSize, 0);
    if(length < 0){ // error
	if(0
#ifdef EWOULDBLOCK
	   || errno == EWOULDBLOCK
#endif
#ifdef HAVE_WSAGETLASTERROR
	   || WSAGetLastError() == WSAEWOULDBLOCK
#endif
#ifdef EAGAIN
	   || errno == EAGAIN
#endif
	    ){
		DPRINTF(10, ("  reciv FD:%d WOULDBLOCK\r\n", fd));
		return 1;
	}
#ifdef HAVE_WSAGETLASTERROR
	int err = WSAGetLastError();
#else
	int err = errno;
#endif
	DPRINTF(10, ("err: %d\r\n", err));
	return -1;
    }else{
	writePos += length;
	*readSize -= length;
	if(*readSize == 0){
	    return 0;
	}
    }

    return 1;
}

int StaticBuffer::ReadFromSocket(int fd){
    size_t tmpSize = size - writePos;
    return ReadFromSocket(fd, &tmpSize);
}

// return value list
// value == 0: send data complete.
// value < 0: error
// value > 0: not complete.
int StaticBuffer::WriteToSocket(int fd){
    if(fd < 0)
	return -1;
    if(readPos >= size)
	return 0;
    errno = 0;
#ifdef HAVE_WSAGETLASTERROR
    WSASetLastError(0);
#endif
    int length = send(fd, (char *)&buf[readPos], (int)(size - readPos), 0);
    if(length < 0){
	if(0
#ifdef EWOULDBLOCK
	   || errno == EWOULDBLOCK
#endif
#ifdef HAVE_WSAGETLASTERROR
	   || WSAGetLastError() == WSAEWOULDBLOCK
#endif
#ifdef EAGAIN
	   || errno == EAGAIN
#endif
	    ){
	    return 1;
	}
	return -1;
    }
    readPos += length;
    if(readPos == size)
	return 0;
    return length;
}

bool StaticBuffer::WriteSeek(size_t newPosition){
    if(newPosition > size)
	return false;
    writePos = newPosition;
    return true;
}

bool StaticBuffer::ReadSeek(size_t newPosition){
    if(newPosition > size)
	return false;
    readPos = newPosition;
    return true;
}

bool StaticBuffer::Write(unsigned char *WriteBuf, size_t dataSize){
    if(dataSize > size - writePos)
	return false;

    memcpy(&buf[writePos], WriteBuf, dataSize);
    writePos += dataSize;
    return true;
}

bool StaticBuffer::WriteUint8(uint8_t n){
    if(size - writePos < sizeof(uint8_t))
	return false;
    buf[writePos] = (unsigned char)n;
    writePos++;
    return true;
}
bool StaticBuffer::WriteUint16(uint16_t n){
    if(size - writePos < sizeof(uint16_t))
	return false;
    n = htons(n);
    memcpy(&buf[writePos], &n, sizeof(uint16_t));
    writePos += sizeof(uint16_t);
    return true;
}
bool StaticBuffer::WriteUint32(uint32_t n){
    if(size - writePos < sizeof(uint32_t))
	return false;
    n = htonl(n);
    memcpy(&buf[writePos], &n, sizeof(uint32_t));
    writePos += sizeof(uint32_t);
    return true;
}
bool StaticBuffer::WriteInt8(int8_t n){
    if(size - writePos < sizeof(int8_t))
	return false;
    buf[writePos] = (unsigned char)n;
    writePos++;
    return true;
}
bool StaticBuffer::WriteInt16(int16_t n){
    if(size - writePos < sizeof(int16_t))
	return false;
    n = htons(n);
    memcpy(&buf[writePos], &n, sizeof(int16_t));
    writePos += sizeof(int16_t);
    return true;
}
bool StaticBuffer::WriteInt32(int32_t n){
    if(size - writePos < sizeof(int32_t))
	return false;
    n = htonl(n);
    memcpy(&buf[writePos], &n, sizeof(int32_t));
    writePos += sizeof(int32_t);
    return true;
}

size_t StaticBuffer::Read(unsigned char *dst, size_t size){
    if(writePos - readPos < size)
	return 0;
    memcpy(dst, &buf[readPos], size);
    readPos += size;
    return size;
}

uint8_t StaticBuffer::ReadUint8(void){
    if(writePos - readPos < sizeof(uint8_t))
	return 0;
    return (uint8_t)buf[readPos++];
}
uint16_t StaticBuffer::ReadUint16(void){
    if(writePos - readPos < sizeof(uint16_t))
	return 0;
    uint16_t n;
    memcpy(&n, &buf[readPos], sizeof(uint16_t));
    n = ntohs(n);
    readPos += sizeof(uint16_t);
    return n;
}
uint32_t StaticBuffer::ReadUint32(void){
    if(writePos - readPos < sizeof(uint32_t))
	return 0;
    uint32_t n;
    memcpy(&n, &buf[readPos], sizeof(uint32_t));
    n = ntohl(n);
    readPos += sizeof(uint32_t);
    return n;
}
int8_t StaticBuffer::ReadInt8(void){
    if(writePos - readPos < sizeof(int8_t))
	return 0;
    return (int8_t)buf[readPos++];
}
int16_t StaticBuffer::ReadInt16(void){
    if(writePos - readPos < sizeof(int16_t))
	return 0;
    int16_t n;
    memcpy(&n, &buf[readPos], sizeof(int16_t));
    n = ntohs(n);
    readPos += sizeof(int16_t);
    return n;
}
int32_t StaticBuffer::ReadInt32(void){
    if(writePos - readPos < sizeof(int32_t))
	return 0;
    int32_t n;
    memcpy(&n, &buf[readPos], sizeof(int32_t));
    n = ntohl(n);
    readPos += sizeof(int32_t);
    return n;
}

};
};
