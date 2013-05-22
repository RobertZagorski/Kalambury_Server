#ifndef MAINSERV_H
#define MAINSERV_H
#include <iostream>
#include <cstring>
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
#include <unistd.h>
#include <err.h>
#elif defined(_WIN32) || defined(WIN32)
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#define OS_Windows
#endif
/*client class*/
#include "client.h"

/**The main base class for the project. It is responsible for delivering an interface all derived classes must implement.*/
class mainserv 
{
public:
    /**The smart pointer to a client object. Used shared_ptr in this case*/
    typedef std::shared_ptr<client> clientPtr;
    /**The default constructor. Does nothing*/
    mainserv();
    /**The virtual deconstructor.*/
    virtual ~mainserv();
    /**Function responsible for handling newly connected connections*/
    virtual void on_accept(struct evconnlistener *listener,
                  evutil_socket_t fd, struct sockaddr *address,
                  int socklen, void *ctx) = 0;
    /**Method responsible for proper handling of incoming messages from previously accepted clients*/
    virtual void on_read(struct bufferevent *bev, void *arg) = 0;
    /**This function is called when an unexptected problem is caused with a socket*/
    virtual void on_error(struct bufferevent* bev, short what, void* arg) = 0;
    /**Closing client's socket*/
	virtual void closeClient(clientPtr clnt) = 0;
	/**Closing client's socket and releasing all buffers connected with it*/
    virtual void closeAndFreeClient(clientPtr clnt) = 0;
};
#endif //MAINSERV_H
