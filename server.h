#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <string>
#include <stdio.h>

///*The logic of a server*/
//#include "serverlogic.h"
#include "mainserv.h"
#include "serverlog.h"
#include <boost\make_shared.hpp>

/* Main server class. Responsible for implementation of functions needed by Libevent 
 * to properly handle connections. Implements only basic functions of a server, such as
 * sending and writing data and registering new clients but do not archive the data
 * about them. This is the responsibility of a class [serverlogic]: @ref serverlogic "serverlogic"
 */
class baseserver : public mainserv
{
public:
	//
	//Default ,private Constructor. It starts the server, opens socket.
	//
	baseserver(mainserv *servlogic, const unsigned short &port_);
	/*
	 * Default constructor, also must be private,
	 * as the pattern of Singleton is implemented
	 */
	baseserver();
	/*
     Default deconstructor. Closes all open sockets and connections 
     and kills the server
     */
	~baseserver();
	/**
     * Called by libevent when there is data to read.
	 */
    virtual void on_read(struct bufferevent *bev, void *arg);
	/**
     * Called by libevent when the write buffer reaches 0.  We only
     * provide this because libevent expects it, but we don't use it.
     */
	void on_write(struct bufferevent *bev, void *arg);
    /**
     * Called by libevent when there is an error on the underlying socket
     * descriptor.
     */
	void on_error(struct bufferevent* bev, short what, void* arg);
	/**
     * This function will be called by libevent when there is a connection
     * ready to be accepted.
     */
    virtual void on_accept(struct evconnlistener *listener,evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx);
	/**
	 * Function closing a client when an error occurs. 
	 * Platform independent
	 */
	virtual void closeClient(clientPtr clnt);
	/**
	 * Function closing and releasing everything connected with 
	 * a client when an error occurs.
	 */
	virtual void closeAndFreeClient(clientPtr clnt);
private:
    mainserv *serverlogic_;
	const unsigned short* PORT;/**Port number to listen to*/
    struct event_base *base;/**The base event of the server registered by Libevent*/
    struct evconnlistener *listener;/**A stuct of evconnlistener type responsible for binding client with socket*/
    evutil_socket_t socketOutput;
    struct sockaddr_in *listenAddress;
};
#endif //SERVER_H