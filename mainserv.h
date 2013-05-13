#ifndef MAINSERV_H
#define MAINSERV_H
#include <iostream>
/* Libevent*/
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <signal.h>

#ifdef __unix__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <mutex>
#elif defined(_WIN32) || defined(WIN32)
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#define OS_Windows
#endif
/*client class*/
#include "client.h"

class mainserv 
{
public:
    mainserv();
    virtual ~mainserv();
    virtual void on_accept(struct evconnlistener *listener,
                  evutil_socket_t fd, struct sockaddr *address,
                  int socklen, void *ctx) = 0 ;
    virtual void on_read(struct bufferevent *bev, void *arg) = 0;
	virtual void closeClient(client *clnt) = 0;
	virtual void closeAndFreeClient(client *clnt) = 0;
};
#endif //MAINSERV_H
