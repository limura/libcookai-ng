 # Copyright (c) 2003, 2004 IIMURA Takuji. All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions
 # are met:
 # 1. Redistributions of source code must retain the above copyright
 #    notice, this list of conditions and the following disclaimer.
 # 2. Redistributions in binary form must reproduce the above copyright
 #    notice, this list of conditions and the following disclaimer in the
 #    documentation and/or other materials provided with the distribution.
 #
 # THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 # ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 # IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 # ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 # FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 # DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 # OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 # HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 # LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 # OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 # SUCH DAMAGE.

CC = gcc
CPP = g++
AR = ar
RANLIB = ranlib

SRCS = ChunkedConnection.cpp ConnectionManager.cpp EventPool.cpp
SRCS += Connection.cpp Connector.cpp NonBlockConnect.cpp
SRCS += ConnectionAcceptor.cpp Event.cpp StaticBuffer.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = libChunkedConnection.a

CFLAGS = -g -O -Wall -Wuninitialized -I.
CFLAGS += -DNSNS_NONBLOCK
CFLAGS += -DUSE_REUSEADDR
CFLAGS += -DDEBUG -pg

CFLAGS += -DBENCHMARK

LOPTIONS += -pg -g -lpthread
#LOPTIONS += -lsocket -lnsl # for Soraris
#LOPTIONS += -lmd # for FreeBSD
#LOPTIONS += -lssl -lcrypto # for openssl/sha.h systems (e.g. MacOS X)

.c.o:	
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCLUDES) $< -o $@

all:	$(TARGET)

libChunkedConnection.a:	$(OBJS)
	$(AR) rc $@ $(OBJS)
	$(RANLIB) $@

clean:	
	rm -f $(TARGET) *.core $(OBJS)

