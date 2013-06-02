#ifndef SERVERLOGIC_H
#define SERVERLOGIC_H

#include <list>
#include <vector>
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include "mainserv.h"
#include "serverlog.h"


/**The class implementing the logic of a server. It maintains the list of connected clients, multicasting the messages*/
class serverlogic : public mainserv
{

public:
    /**
      * The status of a game
      */
    enum GAMESTATUS {ON,OFF};
	
    serverlogic();
	~serverlogic();
	
    /**
      * Method inherited from base class - mainserv. 
      */
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
    
    /**
     * Setting a player name when a client has sent an appropriate message
     */
    void loginmessage(struct bufferevent *);
    
    /**
     * Called when others need to wait for players
     */
    void waitingmessage(struct bufferevent *);
    
    /**
     * Called when a client joins game
     */
    void joinedmessage(struct bufferevent *);

     /**
     * Called when a client sends draw message
     */
    void drawmessage(struct bufferevent *);

    /**
     * Called when a client sends chat message
     */
    void chatmessage(struct bufferevent *);

    /**
     * called when a drawing player has left a game and new player 
     */
    void gamemessage(struct bufferevent *);

    /**
     * Checking a password when a client has sent an appropriate message
     */
    void itemmessage(struct bufferevent *);

    /**
     * Called when a client has won a game and now will be drawing. The message 
      * with a name of player is sent
     */
    void guessedmessage(struct bufferevent *);

    /**
     * Sending a message with a score table
     */
    void scoremessage(struct bufferevent *);
    
    /**
      * Function invoked when client reguests for sending the history of a game
      */
    void historymessage (struct bufferevent *);


    void clearmessage(struct bufferevent *);

private:
    /**The patterns of messages got from clients*/
    std::string patterns [9];
    /**Function Pointers to functions invoked when message comes*/
    void (serverlogic::*FunctionPointers[9])(struct bufferevent *);
    /**A vector holding parts of incoming message*/
    std::vector<std::string> tokenvector;
    /**A vector of clients holding references to them*/
	std::list<clientPtr> *listOfGamers;
    /**History of DRAW messages*/
    std::vector<std::string> history;
    /**A maximum number of gamers that simultaneously can play*/
    const unsigned int *GAMER_MAX_COUNT;
    /**A string holding actual password clients need to guess*/
    std::string pass;
    /**A enum indicating whether the game has started or not*/
    GAMESTATUS gamestat;
    
    boost::function<void (serverlogic*, struct bufferevent *)> invokefunction;
};
#endif //SERVERLOGIC_H
