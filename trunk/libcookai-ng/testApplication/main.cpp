/*
 * Copyright (c) 2006 IIMURA Takuji. All rights reserved.
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

#include <ChunkedConnection/Connector.h>
#include <ChunkedConnection/ChunkedConnection.h>
#include <ChunkedConnection/Event.h>

#include <tools/tools.h>

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

using namespace Cookai::ChunkedConnection;

#ifdef _LIBCOOKAI_WINDOWS_
int winsock_init(void){
    WSADATA wsad;
    short int wVersion = MAKEWORD(2,2);
    int err;
    err = WSAStartup(wVersion, &wsad);
    if(err != 0)
	return 1;

    if (HIWORD(wVersion) != HIWORD(wsad.wVersion) || LOWORD(wVersion) != LOWORD(wsad.wVersion)) {
	WSACleanup();
	return 1;
    }
    return 0;
}
#endif

char *getEventName(EventType type){
    switch(type){
    case EVENT_NOTHING:
	return "EVENT_NOTHING";
	break;
    case EVENT_RECIVE_BLOCK:
	return "EVENT_RECIVE_BLOCK";
	break;
    case EVENT_RECIVE_STREAM:
	return "EVENT_RECIVE_STREAM";
	break;
    case EVENT_ERROR_SOCKET_CLOSE:
	return "EVENT_ERROR_SOCKET_CLOSE";
	break;
    case EVENT_ACCEPT_NEW_SOCKET:
	return "EVENT_ACCEPT_NEW_SOCKET";
	break;
    case EVENT_ERROR_UNKNOWN:
	return "EVENT_ERROR_UNKNOWN";
	break;
    default:
	break;
    }
    return "UNKNOWN_EVENT";
}

bool readHandler(EventType type, StaticBuffer *buf, int channel, ChunkedConnection *cc){
    DPRINTF(10, ("got event: %s\r\n", getEventName(type)));
    switch(type){
    case EVENT_RECIVE_BLOCK:
	DPRINTF(10, ("got message BLOCK: '%s'\r\n", buf->GetBuffer()));
	break;
    case EVENT_RECIVE_STREAM:
	DPRINTF(10, ("got message STREAM: '%s'\r\n", buf->GetBuffer()));
	break;
    case EVENT_ERROR_SOCKET_CLOSE:
	DPRINTF(10, ("socket close. try reconnect.\r\n"));
	cc->Connect();
	break;
    case EVENT_ACCEPT_NEW_SOCKET:
	DPRINTF(10, ("new socket come. set event handler.\r\n"));
	if(cc != NULL){
	    cc->SetBlockReadHandler(readHandler);
	    cc->SetStreamReadHandler(readHandler);
	    cc->SetErrorHandler(readHandler);

#if 0
	    cc->StreamWrite((unsigned char *)"stream send No 1", 17);
	    cc->StreamWrite((unsigned char *)"stream No 2", 12);
	    cc->StreamWrite((unsigned char *)"STREAM No 3 hoge hoge", 22);
	    cc->BlockWrite("block writing.");
	    cc->BlockWrite(" add block.");
	    cc->BlockWrite(" add block No 2.");
	    cc->BlockWrite((unsigned char *)"", 1);
	    cc->BlockCommit();
#endif
	}
	break;
    case EVENT_ERROR_UNKNOWN:
	DPRINTF(10, ("unknown error. no handle.\r\n"));
	break;
    case EVENT_NOTHING:
    default:
	DPRINTF(10, ("unknown event. no handle.\r\n"));
	break;
    }

    return true;
}

int main(int argc, char *argv[]){
    Connector *c1, *c2;

#ifdef _LIBCOOKAI_WINDOWS_
    if(winsock_init() != 0){
	DPRINTF(10, ("winsock init fail\r\n"));
	return 1;
    }
#endif

    c1 = new Connector("tcpport://8472", readHandler);
    if(c1 == NULL)
	return 1;
    c2 = new Connector("tcpport://8473", readHandler);
    if(c2 == NULL)
	return 1;

    ChunkedConnection *cc = c1->Connect("tcpip://localhost/8473", readHandler, readHandler, readHandler);
    
    if(cc != NULL){
	cc->StreamWrite((unsigned char *)"HogeHoge", 9);
	cc->StreamWrite((unsigned char *)"HogEHogEH", 10);
	cc->StreamWrite((unsigned char *)"HoGEHoGE", 9);
	cc->BlockWrite((unsigned char *)"HoGEHOG", 7);
	cc->BlockWrite((unsigned char *)"HOgEHO", 6);
	cc->BlockWrite((unsigned char *)"HOGeHOGE", 9);
	cc->BlockCommit();
    }

    for(int i = 0; i < 10; i++){
	char buf[1024];
#ifdef HAVE__SNPRINTF
	_snprintf_s(buf, sizeof(buf), sizeof(buf), "Message No: %d", i);
#else
	snprintf(buf, sizeof(buf), "Message No: %d", n);
#endif
	DPRINTF(10, ("write message '%s'\r\n", buf));
	cc->BlockWrite((unsigned char *)buf, strlen(buf) + 1);
	cc->BlockCommit();
	c1->InvokeAllEvent();
	c2->InvokeAllEvent();
#ifdef HAVE_SLEEPEX
	SleepEx(1000, TRUE);
#else
	sleep(1);
#endif
    }
    return 0;
}
