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

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifndef COOKAI_STATIC_BUFFER
#define COOKAI_STATIC_BUFFER

namespace Cookai {
namespace ChunkedConnection {

    class StaticBuffer
    {
    private:
	size_t size;
	size_t writePos, readPos;
	unsigned char *buf;
    public:
	StaticBuffer(size_t size);
	StaticBuffer(unsigned char *buf, size_t size); // Ç±Ç±Ç≈éwíËÇ≥ÇÍÇΩ buf ÇÕ StaticBuffer ä«óùÇ≈ free() Ç≥ÇÍÇÈ
	~StaticBuffer(void);

	unsigned char *GetBuffer(void);
	size_t GetDataLength(void);
	size_t GetBufferSize(void);
	size_t GetAvailableSize(void);
	int ReadFromSocket(int fd);
	int ReadFromSocket(int fd, size_t *size);
	int WriteToSocket(int fd);

	bool Write(unsigned char *buf, size_t size);
	bool WriteSeek(size_t newPosition);
	bool WriteUint8(uint8_t n);
	bool WriteUint16(uint16_t n);
	bool WriteUint32(uint32_t n);
	bool WriteInt8(int8_t n);
	bool WriteInt16(int16_t n);
	bool WriteInt32(int32_t n);

	size_t Read(unsigned char *dst, size_t size);
	bool ReadSeek(size_t newPosition);
	uint8_t ReadUint8(void);
	uint16_t ReadUint16(void);
	uint32_t ReadUint32(void);
	int8_t ReadInt8(void);
	int16_t ReadInt16(void);
	int32_t ReadInt32(void);
    };
};
};

#endif /* COOKAI_STATIC_BUFFER */
