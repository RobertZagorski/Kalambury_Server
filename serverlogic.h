#ifndef SERVERLOGIC_H
#define SERVERLOGIC_H

#include <vector>

#include "mainserv.h"


enum MESSAGE {LOGGING,DRAWING,CHAT,BACK};
class serverlogic : public mainserv
{
public:
	serverlogic();
	~serverlogic();
	
    virtual void on_accept(struct evconnlistener *listener,evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx);
    
    virtual void on_read(struct bufferevent *bev, void *arg);
    /**
	 * Function closing a client when an error occurs. 
	 * Platform independent
	 */
	virtual void closeClient(client *clnt);
	/**
	 * Function closing and releasing everything connected with 
	 * a client when an error occurs.
	 */
	virtual void closeAndFreeClient(client *clnt);
	/*
	 * This method creates a new game when a first gamer joins.
	 */
	bool createGame(client& clnt);
    /*
     * Destroying the game when the last user has logged out
     */
    void destroyGame();

    /*
     * This function add a Gamer to a vector, but also check if the maximum
     * count of gamers hasn't been exceeded. In that case it generates error sent to the client
     */
    bool addGamer(client& clnt);

    /* Checks the type of a message looking at its beginning and returns 
     * one of the four known status types described in enum MESSAGE
     */
    MESSAGE checkForMessageType(char *);
private:
	std::vector<client> *vectorOfGamers;
    const unsigned int *GAMER_MAX_COUNT;
};
#endif //SERVERLOGIC_H