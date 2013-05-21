#ifndef SERVERLOGIC_H
#define SERVERLOGIC_H

#include <list>
#include <stdio.h>
#include <boost\bind.hpp>
#include <boost\ref.hpp>
#include "mainserv.h"
#include "serverlog.h"



/**The class implementing the logic of a server. It maintains the list of connected clients, multicasting the messages*/
class serverlogic : public mainserv
{

public:
    enum MESSAGE {LOGGING,DRAWING,CHAT,BACK};
	serverlogic();
	~serverlogic();
	
    virtual void on_accept(struct evconnlistener *listener,evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx);
    
    virtual void on_read(struct bufferevent *bev, void *arg);

    virtual void on_error(struct bufferevent* bev, short what, void* arg);
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
	/**
	 * This method creates a new game when a first gamer joins.
	 */
	bool createGame(clientPtr clnt);
    /**
     * Destroying the game when the last user has logged out
     */
    void destroyGame();

    /**
     * This function add a Gamer to a vector, but also check if the maximum
     * count of gamers hasn't been exceeded. In that case it generates error sent to the client
     */
    bool addGamer(clientPtr clnt);

    /** Checks the type of a message looking at its beginning and returns 
     * one of the four known status types described in enum MESSAGE
     */
    MESSAGE checkForMessageType(char *);
    ///**
    // * Method responsible for multicasting a message to every client except the client who has sent it
    // */
    //void multicast(clientPtr clnt,evbuffer &output,char &data,size_t &len);
private:
	std::list<clientPtr> *vectorOfGamers;
    const unsigned int *GAMER_MAX_COUNT;
};
#endif //SERVERLOGIC_H