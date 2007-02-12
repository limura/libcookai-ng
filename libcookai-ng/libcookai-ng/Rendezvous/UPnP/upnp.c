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

/*
 * UPnP を使って global(だと思われる)IP に TCP な port を開けるものの激しくやる気の無い実装。
 * 勝手に開けた local port を global port にくっつける。
 * http://www.aa.alpha-net.ne.jp/bosuke/trash/upnp.htm と MSN Live Messenger の挙動を参考にしている。
 */

#include "upnp.h"

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

#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif

cookai_upnp *upnp_new(void){
	cookai_upnp *upnp;
	upnp = (cookai_upnp *)malloc(sizeof(cookai_upnp));
	if(upnp == NULL)
		return NULL;
	upnp->sock = -1;
	upnp->wan_ipaddr = NULL;
	upnp->wan_port = NULL;
	upnp->local_ipaddr = NULL;
	upnp->local_port = -1;
	upnp->IGD_control_url = NULL;

	return upnp;
}

void upnp_free(cookai_upnp *upnp){
	if(upnp == NULL)
		return;
	if(upnp->wan_ipaddr != NULL)
		free(upnp->wan_ipaddr);
	if(upnp->wan_port != NULL)
		free(upnp->wan_port);
	if(upnp->local_ipaddr != NULL)
		free(upnp->local_ipaddr);
	if(upnp->IGD_control_url != NULL)
		free(upnp->IGD_control_url);
	free(upnp);
}

char *strstr_ci(char *buf, char *target){
    char *pb, *pt, *ptmp;

    if(buf == NULL || target == NULL || *target == '\0')
	return NULL;

    pb = buf;
    while(*pb != '\0'){
	if(*pb == *target){
	    ptmp = pb;
	    pt = target + 1;
	    pb++;
	    while(*pt != '\0' && *pb != '\0' && tolower(*pt) == tolower(*pb)){
		pt++;
		pb++;
	    }
	    if(*pt == '\0')
		return ptmp;
	}else{
	    pb++;
	}
    }
    return NULL;
}

/* IGD をみつける UDP bloadcast をして、その返事を malloc() した領域に入れて返す。free() はこれを呼んだ奴がやってね！ */
char *upnp_discover(char *ST){
	const char aPart[] = "M-SEARCH * HTTP/1.1\r\n"
		"MX: 3\r\n"
		"HOST: 239.255.255.250:1900\r\n"
		"MAN: \"ssdp:discover\"\r\n"
		"ST: ";
	const char bPart[] = "\r\n"
		"\r\n";
	char buf[2048];
	int sock;
	struct sockaddr_in dest;
	int i = 0;
	struct timeval t;
	fd_set rfds;
	int try = 0;

	if(ST == NULL)
		return NULL;
	if(strlen(ST) + strlen(aPart) + strlen(bPart) + 1 > sizeof(buf))
		return NULL;
	buf[0] = '\0';
	strcat(buf, aPart);
	strcat(buf, ST);
	strcat(buf, bPart);

	sock = -1;
	sock = (int)socket(PF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
		return NULL;

	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = inet_addr("239.255.255.250");
	dest.sin_port = htons(1900);
	dest.sin_family = AF_INET;

	if(sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&dest, sizeof(dest)) < 0)
		return NULL;
	{ /* なぜだかわからんが bloadcast address(またはIGDのIP addr) に投げてやらないと IGD が反応してくれないことがある。
	     IGDのIPaddrを得るのは通常無理なので、とりあえず bloadcast address で我慢。
	  */
		int yes = 1;
#ifdef _LIBCOOKAI_WINDOWS_
		{
			dest.sin_addr.s_addr = inet_addr("255.255.255.255");
#else
		struct ifreq ifr;
		if(ioctl(sock, SIOCGIFBRDADDR, &ifr) >= 0){
			dest.sin_addr.s_addr = ifr.ifr_broadaddr;
#endif
			setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&yes, sizeof(yes));
			if(sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&dest, sizeof(dest)) < 0){
				closeSocket(sock);
				return NULL;
			}
		}
	}
	buf[0] = '\0';

	i = 0;
	t.tv_sec = 0;
	t.tv_usec = 100000;
	while(i++ < 10){
		int fdSize = FD_SETSIZE;
		int recvRet;
		int selectRet;
		if(fdSize < sock)
			fdSize = sock;
		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		if((selectRet = select(fdSize, &rfds, NULL, NULL, &t)) > 0 && FD_ISSET(sock, &rfds) && 
			(recvRet = recvfrom(sock, buf, sizeof(buf) - 1, 0, NULL, NULL)) > 0){
				buf[recvRet] = '\0';
				if(strstr_ci(buf, ST) != NULL){
					char *retBuf;
					closeSocket(sock);
					retBuf = (char *)malloc(sizeof(char) * (strlen(buf) + 1));
					if(retBuf == NULL)
						return NULL;
					memcpy(retBuf, buf, strlen(buf) + 1);
					return retBuf;
				}
		}
	}
	closeSocket(sock);
	return NULL;
}

int HTTP_connect(char *url, char **file){
	char *host, *port, *p, *p2;
	char hostBuf[1024], portBuf[32];
	if(url == NULL)
		return -1;
	if(strncmp(url, "http://", 7) != 0)
		return -1;
	url += 7;
	host = url;
	p = strchr(url, ':');
	p2 = strchr(url, '/');
	if(p == NULL){
		port = "80";
		p = p2;
		if(p2 != NULL){
			if(p2 - host + 1 > sizeof(hostBuf))
				return -1;
			memcpy(hostBuf, host, p2 - host);
			hostBuf[p2 - host] = '\0';
			host = hostBuf;
		}
	}else{
		if(p2 != NULL && p2 < p){
			port = "80";
			p = p2;
		}else{
			memcpy(hostBuf, host, p - host);
			hostBuf[p - host] = '\0';
			host = hostBuf;

			port = p+1;
			p = strchr(port, '/');
			if(p != NULL){
				memcpy(portBuf, port, p - port);
				portBuf[p - port]  = '\0';
				port = portBuf;
			}
		}
	}
	if(file != NULL)
		*file = p;

	return connect_stream(host, port);
}

/* HTTP client のやる気の無い実装 */
char *HTTP_post(char *url, char *postData, size_t postLen, char *header){
	char buf[10240];
	char sendBuf[10240];
	char *p;
	char *file;
	int sock;
	int recvRet;
	size_t sendLen = 0;
	size_t bufLen = 0;

	sock = HTTP_connect(url, &file);
	if(sock < 0)
		return NULL;
	if(file == NULL){
		closeSocket(sock);
		return NULL;
	}
	if(*file == '/')
		file++;

	sendBuf[0] = '0';
	if(postLen > 0)
		strncpy(sendBuf, "POST /", sizeof(sendBuf));
	else
		strncpy(sendBuf, "GET /", sizeof(sendBuf));
	strncat(sendBuf, file, sizeof(sendBuf));
	strncat(sendBuf, " HTTP/1.0\r\n", sizeof(sendBuf));
	if(header != NULL)
		strncat(sendBuf, header, sizeof(sendBuf));
	if(postLen > 0){
		char numBuf[32];
		sprintf(numBuf, "%d", postLen);
		strncat(sendBuf, "Content-Length: ", sizeof(sendBuf));
		strncat(sendBuf, numBuf, sizeof(sendBuf));
		strncat(sendBuf, "\r\n", sizeof(sendBuf));
	}
	/*
	strncat(sendBuf, "Host: ", sizeof(sendBuf));
	strncat(sendBuf, host, sizeof(sendBuf));
	strncat(sendBuf, ":", sizeof(sendBuf));
	strncat(sendBuf, port, sizeof(sendBuf));
	strncat(sendBuf, "\r\n", sizeof(sendBuf));
	*/
	strncat(sendBuf, "\r\n", sizeof(sendBuf));
	sendLen = strlen(sendBuf);
	if(postLen > 0 && postData != NULL){
		if(sizeof(sendBuf) < sendLen + postLen){
			closeSocket(sock);
			return NULL;
		}
		memcpy(&sendBuf[sendLen], postData, postLen);
		sendLen += postLen;
	}

	if(send(sock, sendBuf, strlen(sendBuf), 0) < 0){
		closeSocket(sock);
		return NULL;
	}
	p = buf;
	bufLen = sizeof(buf) - 1;
	while((recvRet = recv(sock, p, bufLen, 0)) >= 0){
		bufLen -= recvRet;
		p += recvRet;
		if(bufLen <= 0 || recvRet == 0){
			char *retBuf;
			p[1] = '\0';
			closeSocket(sock);

			retBuf = (char *)malloc(sizeof(char) * (p - buf + 1));
			if(retBuf == NULL)
				return NULL;
			memcpy(retBuf, buf, p - buf + 1);
			retBuf[p-buf] = '\0';
			return retBuf;
		}
	}
	closeSocket(sock);
	return NULL;
}

/* 最初に見つかった <....>******</.....> の ***** を取り出す */
char *getTag(char *tagName, char *targetBuffer, char *returnBuffer, size_t bufferLength){
	char *p, *p2;
	char tagBuf[256];
	if(tagName == NULL || targetBuffer == NULL || returnBuffer == NULL || bufferLength <= 0
		|| strlen(tagName) + 4 > sizeof(tagBuf))
		return NULL;
	strcpy(tagBuf, "<");
	strcat(tagBuf, tagName);
	strcat(tagBuf, ">");


	p = strstr_ci(targetBuffer, tagBuf);
	if(p == NULL)
		return NULL;
	p += strlen(tagBuf);

	strcpy(tagBuf, "</");
	strcat(tagBuf, tagName);
	strcat(tagBuf, ">");

	p2 = strstr_ci(p, tagBuf);
	if(p2 == NULL)
		return NULL;
	if(p2 - p + 1 > bufferLength)
		return NULL;
	memcpy(returnBuffer, p, (p2 - p));
	returnBuffer[p2 - p] = '\0';
	return returnBuffer;
}

char *upnp_GetExternalIPAddress(char *BaseUrl, char *service, char **WAN_IP_ADDR){
	char *sendBufA = "<s:Envelope "
		"xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
		"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
		"<s:Body>"
		"<u:GetExternalIPAddress xmlns:u=\"";
	char *sendBufB = "\">"
		"</u:GetExternalIPAddress>"
		"</s:Body>"
		"</s:Envelope>";
	char *actionA = "SoapAction: \"";
	char *actionB = "#GetExternalIPAddress\"\r\n"
		"Content-Type: text/xml\r\n";
	char *ret;
	char ipaddrBuf[256];
	char sendBuf[1024];
	char actionBuf[1024];
	if(BaseUrl == NULL || service == NULL || WAN_IP_ADDR == NULL)
		return NULL;
	if(strlen(sendBufA) + strlen(service) + strlen(sendBufB) + 1 > sizeof(sendBuf))
		return NULL;
	sendBuf[0] = '\0';
	strcpy(sendBuf, sendBufA);
	strcat(sendBuf, service);
	strcat(sendBuf, sendBufB);
	if(strlen(actionA) + strlen(service) + strlen(actionB) + 1 > sizeof(actionBuf))
		return NULL;
	actionBuf[0] = '\0';
	strcpy(actionBuf, actionA);
	strcat(actionBuf, service);
	strcat(actionBuf, actionB);
	ret = HTTP_post(BaseUrl, sendBuf, strlen(sendBuf), actionBuf);
	if(ret == NULL)
		return NULL;
	if(getTag("NewExternalIPAddress", ret, ipaddrBuf, sizeof(ipaddrBuf)) == NULL)
		return NULL;
	*WAN_IP_ADDR = (char *)malloc(sizeof(char) * (strlen(ipaddrBuf) + 1));
	memcpy(*WAN_IP_ADDR, ipaddrBuf, strlen(ipaddrBuf) + 1);
	return *WAN_IP_ADDR;
}

cookai_upnp *upnp_AddPortMapping(int targetPort, cookai_upnp *upnp){
	char *sendBufA = "<s:Envelope"
		" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
		" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
		"<s:Body>"
		"<u:AddPortMapping xmlns:u=\"";
	char *sendBufB = "\">"
		"<NewRemoteHost></NewRemoteHost>";
	/*
      <NewExternalPort>37849</NewExternalPort>
	  <NewProtocol>TCP</NewProtocol>
      <NewInternalPort>4670</NewInternalPort>
      <NewInternalClient>192.168.11.5</NewInternalClient>
	  */
	char *sendBufC = "<NewEnabled>1</NewEnabled>"
	    "<NewPortMappingDescription>libcookai UPnP port (TCP)</NewPortMappingDescription>"
		"<NewLeaseDuration>0</NewLeaseDuration>"
		"</u:AddPortMapping>"
		"</s:Body>"
		"</s:Envelope>";
	int sock, tmpSock;
	struct sockaddr_in sin;
	int i, port;
	char *myAddr;
	char sendBuf[20480];
	char headerBuf[1024];
	char *headerA = "SoapAction: \"";
	char *headerB = "#AddPortMapping\"\r\n"
		"Content-Type: text/xml\r\n";
	char *ret;
	int wanPort;

	if(upnp == NULL || upnp->IGD_control_url == NULL || upnp->IGD_service_type == NULL)
		return NULL;

	sock = -1;
	port = -1;
	for(i = 2048; i < 2048 + 100; i++){
		char portStr[8];
		sprintf(portStr, "%d", i);
		sock = listen_stream(portStr, PF_INET);
		if(sock > 0){
			port = i;
			break;
		}
	}
	if(sock < 0 || port < 0)
		return NULL;
	upnp->sock = sock;
	upnp->local_port = port;

	tmpSock = HTTP_connect(upnp->IGD_control_url, NULL);
	if(tmpSock < 0)
		return NULL;
	i = sizeof(sin);
	if(getsockname(tmpSock, (struct sockaddr *)&sin, &i) != 0){
#if 0
		char *e = NULL;
		closeSocket(sock);
		closeSocket(tmpSock);
		switch(WSAGetLastError()){
			case WSANOTINITIALISED:
				e = "WSANOTINITIALISED";
				break;
			case WSAENETDOWN:
				e = "WSAENETDOWN";
				break;
			case WSAEFAULT:
				e = "WSAEFAULT";
				break;
			case WSAEINPROGRESS:
				e = "WSAEINPROGRESS";
				break;
			case WSAENOTSOCK:
				e = "WSAENOTSOCK";
				break;
			case WSAEINVAL:
				e = "WSAEINVAL";
				break;
			default:
				e = "UNKNOWN";
				break;
		}
		printf("%s", e);
#endif
		return NULL;
	}
	closeSocket(tmpSock);

	myAddr = inet_ntoa(sin.sin_addr);
	if(myAddr == NULL){
		closeSocket(sock);
		return NULL;
	}
	upnp->local_ipaddr = (char *)malloc(sizeof(char) * (strlen(myAddr) + 1));
	if(upnp->local_ipaddr == NULL){
		closeSocket(sock);
		return NULL;
	}
	strcpy(upnp->local_ipaddr, myAddr);

	i = -1;
	while(++i < 100){
		if(targetPort > 0){
			wanPort = targetPort;
			targetPort = -1;
		}else{
			wanPort = port + 32768 + 200 + i * 20;
		}
		strcpy(sendBuf, sendBufA);
		strncat(sendBuf, upnp->IGD_service_type, sizeof(sendBuf));
		strncat(sendBuf, sendBufB, sizeof(sendBuf));
		{
			char portNumStr[10];
			sprintf(portNumStr, "%d", wanPort);
			strncat(sendBuf, "<NewExternalPort>", sizeof(sendBuf));
			strncat(sendBuf, portNumStr, sizeof(sendBuf));
			strncat(sendBuf, "</NewExternalPort>", sizeof(sendBuf));

			strncat(sendBuf, "<NewProtocol>TCP</NewProtocol>", sizeof(sendBuf));

			sprintf(portNumStr, "%d", port);
			strncat(sendBuf, "<NewInternalPort>", sizeof(sendBuf));
			strncat(sendBuf, portNumStr, sizeof(sendBuf));
			strncat(sendBuf, "</NewInternalPort>", sizeof(sendBuf));

			strncat(sendBuf, "<NewInternalClient>", sizeof(sendBuf));
			strncat(sendBuf, myAddr, sizeof(sendBuf));
			strncat(sendBuf, "</NewInternalClient>", sizeof(sendBuf));
		}
		strncat(sendBuf, sendBufC, sizeof(sendBuf));

		strcpy(headerBuf, headerA);
		strncat(headerBuf, upnp->IGD_service_type, sizeof(headerBuf));
		strncat(headerBuf, headerB, sizeof(headerBuf));

		ret = HTTP_post(upnp->IGD_control_url, sendBuf, strlen(sendBuf), headerBuf);
		if(ret == NULL){
			closeSocket(sock);
			return NULL;
		}
		if(strstr_ci(ret, upnp->IGD_service_type) != NULL && strstr_ci(ret, "200 OK\r\n") != NULL){
			upnp->wan_port = (char *)malloc(sizeof(char) * 10);
			if(upnp->wan_port == NULL){
				closeSocket(sock);
				return NULL;
			}
			sprintf(upnp->wan_port, "%d", wanPort);
			return upnp;
		}
	}
	closeSocket(sock);
	return NULL;
}

/* return value -> success: 0, fail: -1 */
int upnp_DeletePortMapping(cookai_upnp *upnp){
	char *headerA = "SoapAction: \"";
	char *headerB = "#DeletePortMapping\"\r\n"
		"Content-Type: text/xml\r\n";
	char *sendBufA = "<s:Envelope"
		" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
		" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
		"<s:Body>"
		"<u:DeletePortMapping xmlns:u=\"";
	char *sendBufB = "\">"
		"<NewRemoteHost></NewRemoteHost>"
		"<NewExternalPort>";
	char *sendBufC = "</NewExternalPort>"
		"<NewProtocol>TCP</NewProtocol>"
		"</u:DeletePortMapping>"
		"</s:Body>"
		"</s:Envelope>";
	char sendBuf[20480];
	char headerBuf[2048];
	char *ret;
	if(upnp == NULL || upnp->wan_port == NULL || upnp->IGD_service_type == NULL
		|| upnp->IGD_control_url == NULL)
		return -1;

	strcpy(sendBuf, sendBufA);
	strncat(sendBuf, upnp->IGD_service_type, sizeof(sendBuf));
	strncat(sendBuf, sendBufB, sizeof(sendBuf));
	strncat(sendBuf, upnp->wan_port, sizeof(sendBuf));
	strncat(sendBuf, sendBufC, sizeof(sendBuf));

	strcpy(headerBuf, headerA);
	strncat(headerBuf, upnp->IGD_service_type, sizeof(headerBuf));
	strncat(headerBuf, headerB, sizeof(headerBuf));

	ret = HTTP_post(upnp->IGD_control_url, sendBuf, strlen(sendBuf), headerBuf);
	if(ret == NULL)
		return -1;
	if(strstr_ci(ret, "DeletePortMappingResponse") != NULL
		&& strstr_ci(ret, upnp->IGD_service_type) != NULL
		&& strstr_ci(ret, "200 OK\r\n") != NULL)
		return 0;
	return -1;
}

cookai_upnp *upnp_listen_stream(int targetPort){
	int dgramPort = 1900;
	char *ret = NULL;
	char *p, *p2;
	char BaseUrl[256], BaseUrlTmp[256], ControlUrl[256];
	static char *services[] = { /* これらの値を後で使うので static にしている */
		"urn:schemas-upnp-org:service:WANPPPConnection:1",
		"urn:schemas-upnp-org:service:WANIPConnection:1",
		NULL
	};
	int i;
	char *nowServiceType = NULL;
	cookai_upnp *upnp;

	upnp = upnp_new();
	if(upnp == NULL)
		return NULL;

	/* phase1: discover UPNP Internet Gateway Device (IGD) */
	ret = NULL;
	for(i = 0; services[i] != NULL; i++)
		if((ret = upnp_discover(services[i])) != NULL)
			break;
	if(ret == NULL)
		return NULL;
	if((p = strstr_ci(ret, "\r\nLocation:")) == NULL){
	    upnp_free(upnp);
	    free(ret);
	    return NULL;
	}
	if(p != NULL && strlen(p) < 13){
		upnp_free(upnp);
		free(ret);
		return NULL;
	}
	p+=11;
	while(*p == ' ')
		p++;
	p2 = strstr(p, "\r\n");
	if(p2 == NULL){
		upnp_free(upnp);
		free(ret);
		return NULL;
	}
	*p2 = '\0';
	BaseUrl[0] = '\0';
	if(strncmp(p, "http://", 7) == 0){
		p2 = strchr(p + 7, '/');
		if(p2 != NULL && sizeof(BaseUrl) > (p2 - p + 1)){
			memcpy(BaseUrl, p, p2 - p);
			BaseUrl[p2 - p] = '\0';
		}
	}

	/* phase2: get UPNP service list */
	p2 = HTTP_post(p, NULL, 0, NULL);
	free(ret);
	ret = p2;
	p = NULL;
	for(i = 0; services[i] != NULL; i++){
		char serviceTypeBuf[1024];
		strcpy(serviceTypeBuf, "<serviceType>");
		strcat(serviceTypeBuf, services[i]);
		strcat(serviceTypeBuf, "</serviceType>");
		p = strstr_ci(ret, serviceTypeBuf);
		if(p != NULL)
			break;
	}
	if(p == NULL){
		upnp_free(upnp);
		free(ret);
		return NULL;
	}
	upnp->IGD_service_type = services[i];
	p2 = strstr_ci(p, "</service>"); /* 拾い出したところから </service> までを読む */
	if(p2 == NULL){
		upnp_free(upnp);
		free(ret);
		return NULL;
	}
	if(getTag("URLBase", ret, BaseUrlTmp, sizeof(BaseUrlTmp)) == NULL){
		if(getTag("modelURL", ret, BaseUrlTmp, sizeof(BaseUrlTmp)) == NULL){
			upnp_free(upnp);
			free(ret);
			return NULL;
		}
	}
	if(strncmp(BaseUrlTmp, "http://", 7) == 0 && strchr(BaseUrlTmp + 7, ';') != NULL)
		strcpy(BaseUrl, BaseUrlTmp);
	*p2 = '\0';
	if(getTag("controlURL", p, ControlUrl, sizeof(ControlUrl)) == NULL){
		upnp_free(upnp);
		free(ret);
		return NULL;
	}
	free(ret);
	i = (int)strlen(BaseUrl);
	if(ControlUrl[0] == '/' && BaseUrl[i - 1] == '/')
		BaseUrl[--i] = '\0';
	if(i + strlen(ControlUrl) + 1 > sizeof(BaseUrl)){
		upnp_free(upnp);
		return NULL;
	}
	strcat(BaseUrl, ControlUrl);
	upnp->IGD_control_url = (char *)malloc(sizeof(char) * (strlen(BaseUrl) + 1));
	strcpy(upnp->IGD_control_url, BaseUrl);

	/* phase3: get External IP address */
	if(upnp_GetExternalIPAddress(upnp->IGD_control_url, upnp->IGD_service_type, &(upnp->wan_ipaddr)) == NULL){
		upnp_free(upnp);
		return NULL;
	}
	/* phase4: request Port Mapping */
	if(upnp_AddPortMapping(targetPort, upnp) == NULL){
		upnp_free(upnp);
		return NULL;
	}
	return upnp;
}

void upnp_close(cookai_upnp *upnp){
	if(upnp == NULL)
		return;
	if(upnp->sock >= 0)
		closeSocket(upnp->sock);
	if(upnp->wan_port != NULL)
		upnp_DeletePortMapping(upnp);

	upnp_free(upnp);
}
