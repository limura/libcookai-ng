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

#include <stdlib.h>

#ifdef HAVE_POLL_H
#include <poll.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

#include "ConnectionManager.h"

namespace Cookai{
namespace ChunkedConnection {

    ConnectionManager::ConnectionManager(void){
	connectionStatusMap.clear();
#ifdef HAVE_POLL
	pollfds = NULL;
	nfds = 0;
#else /* HAVE_POLL */
#ifdef HAVE_SELECT
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	maxfd = -1;
#endif /* HAVE_SELECT */
#endif /* HAVE_POLL */
    }
    ConnectionManager::~ConnectionManager(void){
	for(ConnectionStatusMap::iterator i = connectionStatusMap.begin(); i != connectionStatusMap.end(); i++){
	    delete (i->second);
	}
	connectionStatusMap.clear();
    }

    Cookai::ChunkedConnection::ConnectionManager::ManagerInterface *ConnectionManager::GetManagerInterface(ConnectionManagerInterface *Interface){
	ManagerInterface *mi = connectionStatusMap[Interface];
	if(mi == NULL){
	    mi = new ManagerInterface();
	    if(mi == NULL)
		return NULL;
	    mi->fd = -1;
	    mi->Interface = Interface;
	    mi->status = Cookai::ChunkedConnection::CONNECTION_STATUS_NONE;
	    connectionStatusMap.insert(ConnectionStatusMap::value_type(Interface, mi));
	}
	return mi;
    }

    void ConnectionManager::AddInterface(ConnectionManagerInterface *Interface){
	if(Interface == NULL)
	    return;
	Interface->RegisterConnectionManager(this);
	int fd = Interface->Connect();
	if(fd >= 0)
	    UpdateSelectStatus(Interface, fd, Cookai::ChunkedConnection::CONNECTION_STATUS_READ_OK);
    }

    void ConnectionManager::AddFDWatcher(int fd){
#ifdef HAVE_POLL
	if(pollfds == NULL){
	    pollfds = (struct pollfd *)malloc(sizeof(struct pollfd));
	    if(pollfds == NULL)
		return;
	}else{
	    struct pollfd *tmp_pollfds = (struct pollfd *)realloc(pollfds, sizeof(struct pollfd) * (nfds + 1));
	    if(tmp_pollfds == NULL)
		return;
	    pollfds = tmp_pollfds;
	}
	pollfds[nfds].fd = fd;
	pollfds[nfds].events = 0;
	nfds++;
#else /* HAVE_POLL */
#ifdef HAVE_SELECT
	if(maxfd < fd)
	    maxfd = fd;
#endif /* HAVE_SELECT */
#endif /* HAVE_POLL */
    }
    void ConnectionManager::DeleteFDWatcher(int fd){
#ifdef HAVE_POLL
	if(nfds <= 0)
	    return;
	if(nfds == 1){
	    free(pollfds);
	    pollfds = NULL;
	    nfds--;
	    return;
	}
	for(int i = 0; i < nfds; i++){
	    if(pollfds[i].fd == fd){
		if(i != nfds - 1){
		    memcpy(&pollfds[i], &pollfds[nfds-1], sizeof(pollfds[i]));
		}
		struct pollfd *tmp_pollfds = (struct pollfd *)realloc(pollfds, sizeof(struct pollfd) * (nfds - 1));
		if(tmp_pollfds != NULL)
		    pollfds = tmp_pollfds;
		nfds--;
		return;
	    }
	}
#else /* HAVE_POLL */
#ifdef HAVE_SELECT
	if(maxfd == fd)
	    maxfd = fd-1;
#endif /* HAVE_SELECT */
#endif /* HAVE_POLL */
    }

    void ConnectionManager::UpdateSelectStatus(Cookai::ChunkedConnection::ConnectionManagerInterface *Interface,
	int fd, Cookai::ChunkedConnection::ConnectionStatus status){
	ManagerInterface *mi = connectionStatusMap[Interface];
	if(mi != NULL){
	    if(mi->fd != fd && mi->fd >= 0)
		DeleteFDWatcher(fd);
	    mi->status = status;
	    mi->fd = fd;
	    mi->Interface = Interface;
	}else{
	    mi = GetManagerInterface(Interface); // alloc
	    if(mi == NULL)
		return;
	    mi->status = status;
	    mi->fd = fd;
	    mi->Interface = Interface;
	    AddFDWatcher(fd);
	}
#ifdef HAVE_POLL
	if(pollfds != NULL){
	    for(int i = 0; i < nfds; i++){
		if(pollfds[i].fd == fd){
		    pollfds[i].events = 0;
		    if(status & Cookai::ChunkedConnection::CONNECTION_STATUS_READ_OK)
			pollfds[i].events |= POLLRDNORM;
		    if(status & Cookai::ChunkedConnection::CONNECTION_STATUS_WRITE_OK)
			pollfds[i].events |= POLLWRNORM;
		    break;
		}
	    }
	}
#else /* HAVE_POLL */
#ifdef HAVE_SELECT
	if(status & Cookai::ChunkedConnection::CONNECTION_STATUS_READ_OK)
	    FD_SET(fd, &rfds);
	else
	    FD_CLR(fd, &rfds);
	if(status & Cookai::ChunkedConnection::CONNECTION_STATUS_WRITE_OK)
	    FD_SET(fd, &wfds);
	else
	    FD_CLR(fd, &wfds);
#endif /* HAVE_SELECT */
#endif /* HAVE_POLL */
    }

    void ConnectionManager::Invoke(int fd, Cookai::ChunkedConnection::ConnectionStatus status){
	if(fd < 0 || status == 0)
	    return;
	ManagerInterface *mi = NULL;
	for(ConnectionStatusMap::iterator i = connectionStatusMap.begin(); i != connectionStatusMap.end(); i++){
	    if(i->second->fd == fd){
		mi = i->second;
		break;
	    }
	}
	if(mi != NULL && mi->Interface != NULL)
	    mi->Interface->Run(status);
    }

    bool ConnectionManager::Run(int usec){
#ifdef HAVE_POLL
	if(pollfds == NULL)
	    return true;
	if(usec > 0)
	int pollRet = poll(pollfds, nfds, usec * 1000);
	if(pollRet < 0)
	    return false;
	if(pollRet == 0)
	    return true;
	for(int i; pollRet > 0 i < nfds; i++){
	    int status = 0;
	    if(pollfds[i].revents & POLLRDNORM)
		status = status | (int)Cookai::ChunkedConnection::CONNECTION_STATUS_READ_OK;
	    if(pollfds[i].revents & POLLWRNORM)
		status = status | (int)Cookai::ChunkedConnection::CONNECTION_STATUS_WRITE_OK;
	    if(status != 0){
		Invoke(pollfds[i].fd, (Cookai::ChunkedConnection::ConnectionStatus)status);
		pollRet--;
	    }
	}
	return true;
#else /* HAVE_POLL */
#ifdef HAVE_SELECT
	if(maxfd < 0)
	    return true;
	struct timeval tv;
	tv.tv_sec = usec / 1000000;
	tv.tv_usec = usec % 1000000;
	fd_set Rfds, Wfds;
	memcpy(&Rfds, &rfds, sizeof(rfds));
	memcpy(&Wfds, &wfds, sizeof(wfds));
	int selectRet = select(maxfd, &Rfds, &Wfds, NULL, &tv);
	if(selectRet < 0)
	    return false;
	if(selectRet == 0)
	    return true;
	for(int i = 0; selectRet > 0 && i < maxfd; i++){
	    int status = 0;
	    if(FD_ISSET(i, &rfds))
		status = status | (int)Cookai::ChunkedConnection::CONNECTION_STATUS_READ_OK;
	    if(FD_ISSET(i, &wfds))
		status = status | (int)Cookai::ChunkedConnection::CONNECTION_STATUS_WRITE_OK;
	    if(status != 0){
		Invoke(i, (Cookai::ChunkedConnection::ConnectionStatus)status);
		selectRet--;
	    }
	}
	return true;
#endif /* HAVE_SELECT */
#endif /* HAVE_POLL */
	return false;
    }

};
};