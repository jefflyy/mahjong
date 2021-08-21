#ifndef __COMMON_H__
#define __COMMON_H__

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>

#if defined(__linux) || defined(__linux__)

#define OS_LINUX

#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<netdb.h>

#define SOCKET int
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

#else

#include<windows.h>

typedef int(*fun1)(WORD,LPWSADATA);
fun1 _WSAStartup;

typedef SOCKET(*fun2)(int,int,int);
fun2 _socket;

typedef int(*fun3)(SOCKET,const struct sockaddr*,int);
fun3 _bind;

typedef u_short(*fun4)(u_short);
fun4 _htons;

typedef int(*fun5)(SOCKET,int);
fun5 _listen;

typedef SOCKET(*fun6)(SOCKET,struct sockaddr*,int*);
fun6 _accept;

typedef char*(*fun7)(struct in_addr);
fun7 _inet_ntoa;

typedef int(*fun8)(SOCKET,char*,int,int);
fun8 _recv;

typedef int(*fun9)(SOCKET);
fun9 _close;

typedef int(*fun10)();
fun10 _WSACleanup;

typedef int(*fun11)(SOCKET,const char*,int,int);
fun11 _send;

typedef struct hostent*(*fun12)(const char*);
fun12 _gethostbyname;

typedef int(*fun13)(SOCKET,const struct sockaddr*,int);
fun13 _connect;

struct winsockactor{
	winsockactor(){
		HINSTANCE hdll=LoadLibrary("ws2_32.dll");
		assert(hdll!=0);

		_WSAStartup=(fun1)GetProcAddress(hdll,"WSAStartup");
		assert(_WSAStartup!=0);

		_socket=(fun2)GetProcAddress(hdll,"socket");
		assert(_socket!=0);

		_bind=(fun3)GetProcAddress(hdll,"bind");
		assert(_bind!=0);

		_htons=(fun4)GetProcAddress(hdll,"htons");
		assert(_htons!=0);

		_listen=(fun5)GetProcAddress(hdll,"listen");
		assert(_listen!=0);

		_accept=(fun6)GetProcAddress(hdll,"accept");
		assert(_accept!=0);

		_inet_ntoa=(fun7)GetProcAddress(hdll,"inet_ntoa");
		assert(_inet_ntoa!=0);

		_recv=(fun8)GetProcAddress(hdll,"recv");
		assert(_recv!=0);

		_close=(fun9)GetProcAddress(hdll,"closesocket");
		assert(_close!=0);

		_WSACleanup=(fun10)GetProcAddress(hdll,"WSACleanup");
		assert(_WSACleanup!=0);

		_send=(fun11)GetProcAddress(hdll,"send");
		assert(_send!=0);

		_gethostbyname=(fun12)GetProcAddress(hdll,"gethostbyname");
		assert(_gethostbyname!=0);

		_connect=(fun13)GetProcAddress(hdll,"connect");
		assert(_connect!=0);

		WORD sockver=MAKEWORD(2,2);
		WSADATA wsadata;
		assert(_WSAStartup(sockver,&wsadata)==0);
	}

	~winsockactor(){
		_WSACleanup();
	}
}_winsockactor;

#define socket _socket
#define bind _bind
#define htons _htons
#define listen _listen
#define accept _accept
#define inet_ntoa _inet_ntoa
#define recv _recv
#define close _close
#define send _send
#define gethostbyname _gethostbyname
#define connect _connect

#endif

SOCKET createserver(int port,int lim){
	SOCKET server=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	assert(server!=INVALID_SOCKET);
	struct sockaddr_in sin;
	memset(&sin,0,sizeof(sin));
	sin.sin_family=AF_INET;
	sin.sin_port=htons(port);
#ifdef OS_LINUX
	sin.sin_addr.s_addr=inet_addr("0.0.0.0");
#else
	sin.sin_addr.S_un.S_addr=INADDR_ANY;
#endif
	assert(bind(server,(struct sockaddr*)&sin,sizeof(sin))!=SOCKET_ERROR);
	assert(listen(server,lim)!=SOCKET_ERROR);
	return server;
}

SOCKET getclient(SOCKET server){
	SOCKET client;
	client=accept(server,0,0);
	assert(client!=INVALID_SOCKET);
	return client;
}

SOCKET connecttoserver(const char*hostname,int port){
	SOCKET server;
	struct sockaddr_in name;
	struct hostent*hostinfo;
	server=socket(PF_INET,SOCK_STREAM,0);
	assert(server!=INVALID_SOCKET);
	name.sin_family=AF_INET;
	hostinfo=gethostbyname(hostname);
	assert(hostinfo!=0);
	name.sin_addr=*((struct in_addr*)(hostinfo->h_addr));
	name.sin_port=htons(port);
	assert(connect(server,(struct sockaddr*)&name,sizeof(sockaddr_in))!=SOCKET_ERROR);
	return server;
}

#endif