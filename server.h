#ifndef SERVER_H
#define SERVER_H
///Main server class.
#include <iostream>
#include <string>
#include <stdio.h>

#include "mainserv.h"
#include "serverlog.h"
#include <boost/make_shared.hpp>

/** Main server class. Responsible for implementation of functions needed by Libevent 
 * to properly handle connections. Implements only basic functions of a server, such as
 * sending and writing data and registering new clients but do not archive the data
 * about them. This is the responsibility of a class [serverlogic]: @ref serverlogic "serverlogic"
 */
class baseserver : public mainserv
{
public:
 
	/*
	 * Default ,private Constructor. It starts the server, opens socket.
	 * \param servlogic the object of the logic of a server implemented in this project
     * \param port_ the port number the server listens on
     */
	baseserver(mainserv *servlogic, const unsigned short &port_);
	/*
	 * Default constructor 
	 */
	baseserver();
	/*
     Default deconstructor. Closes all open sockets and connections 
     and kills the server
     */
	~baseserver();
	/**
     * Called by libevent when there is data to read.
     * \param bev bufferevent connected with a client sending message
     * \param arg user parameter
	 */
    virtual void on_read(struct bufferevent *bev, void *arg);
	/**
     * Called by libevent when the write buffer reaches 0.  We only
     * provide this because libevent expects it, but we don't use it.
     * \param bev the clients
     */
	void on_write(struct bufferevent *bev, void *arg);
    /**
     * Called by libevent when there is an error on the underlying socket
     * descriptor.
     * \param bev client's bufferevent who happened to have an error
     * \param what the kind of error
     * \param arg user argument - the instance of this class
     */
	void on_error(struct bufferevent* bev, short what, void* arg);
	/**
     * This function will be called by libevent when there is a connection
     * ready to be accepted.
     * \param listener the client's listener
     * \param fd the client's socket
     * \param address the address of a client
     * \param socklen the length of a socket
     * \param ctx the user parameter passed to the function - the instance of this class
     */
    virtual void on_accept(struct evconnlistener *listener,evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx);
	/**
	 * Function closing a client when an error occurs. 
	 * Platform independent
     * \param clnt a pointer to the instance of client
	 */
	virtual void closeClient(clientPtr clnt);
	/**
	 * Function closing and releasing everything connected with 
	 * \param clnt a pointer to the instance of client
	 */
	virtual void closeAndFreeClient(clientPtr clnt);
private:
    /**The object of a class resposnbile for implementing the logic of a server*/
    mainserv *serverlogic_;
    /**Port number to listen to*/
	const unsigned short* PORT;
    /**The base event of the server registered by Libevent*/
    struct event_base *base;
    /**A stuct of evconnlistener type responsible for binding client with socket*/
    struct evconnlistener *listener;
    /**The socket of a server*/
    evutil_socket_t socketOutput;
    /**The structure holding the parameters of a socket of server*/
    struct sockaddr_in *listenAddress;
};
#endif //SERVER_H
