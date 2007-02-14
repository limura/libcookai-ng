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
#include "../config.h"

#include <map>
#include <list>

#ifdef HAVE_POLL_H
#include <poll.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

#include "../tools/thread.h"

#include "ConnectionManagerInterface.h"

#ifndef COOKAI_CHUNKEDCONNECTION_CONNECTIONMANAGER_H
#define COOKAI_CHUNKEDCONNECTION_CONNECTIONMANAGER_H

namespace Cookai{
namespace ChunkedConnection {

    class ConnectionManager {
    private:
	typedef struct {
	    int status;
	    int fd;
	    ConnectionManagerInterface *Interface;
	} ManagerInterface;
	typedef std::map<ConnectionManagerInterface *, ManagerInterface *> ConnectionStatusMap;
	ConnectionStatusMap connectionStatusMap;
	thread_mutex initialConnectionListMutex;
	typedef std::list<ConnectionManagerInterface *> InitialConnectionList;
	InitialConnectionList initialConnectionList;

	void Invoke(int fd, Cookai::ChunkedConnection::ConnectionStatus status);
	void AddFDWatcher(int fd);
	void DeleteFDWatcher(int fd);
	ManagerInterface *GetManagerInterface(ConnectionManagerInterface *Interface);
#ifdef HAVE_POLL
	struct pollfd *pollfds;
	int nfds;
#else /* HAVE_POLL */
#ifdef HAVE_SELECT
	fd_set rfds, wfds;
	int maxfd;
#endif /* HAVE_SELECT */
#endif /* HAVE_POLL */
    public:
	ConnectionManager(void);
	~ConnectionManager(void);

	void AddInterface(ConnectionManagerInterface *Interface);
	void UpdateSelectStatus(ConnectionManagerInterface *Interface, int fd, Cookai::ChunkedConnection::ConnectionStatus status);
	bool Run(int usec);
    };
};
};

#endif /* COOKAI_CHUNKEDCONNECTION_CONNECTIONMANAGER_H */
