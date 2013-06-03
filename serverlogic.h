#ifndef SERVERLOGIC_H
#define SERVERLOGIC_H
///The logic of a server
#include <list>
#include <vector>
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include "mainserv.h"
#include "serverlog.h"
#include "historyobserver.h"
#include "drawobservable.h"
#include "drawobserver.h"

/**The class implementing the logic of a server. It maintains the list of connected clients, multicasting the messages*/
class serverlogic : public mainserv, public drawobservable
{

public:
    /** The status of a game */
    enum GAMESTATUS {
                     ON, /**< The game is now playing */
                     OFF /**< The game is hold */
                    };
	
    /**Constructor, invoked from main. Needed for creating function pointers*/
    serverlogic();

    /**Deconstructor. Releasing everything connected with server*/
	~serverlogic();
	
    /**Function responsible for handling newly connected clients
      * \param listener the client's listener
      * \param fd the client's socket
      * \param address the address of a client
      * \param socklen the length of a socket
      * \param ctx the user parameter passed to the function
      **/
    virtual void on_accept(struct evconnlistener *listener,evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx);
    /**Method responsible for proper handling of incoming messages from 
      *previously accepted clients
      * \param bev bufferevent connected with a client sending message
      * \param arg user parameter
      */
    virtual void on_read(struct bufferevent *bev, void *arg);
    /**This function is called when an unexptected problem is caused with a socket
      * \param bev client's bufferevent who happened to have an error
      * \param what the kind of error
      * \param arg user argument
      */
    virtual void on_error(struct bufferevent* bev, short what, void* arg);
    
    /**Closing client's socket
      * \param clnt a pointer to the instance of client*/
	virtual void closeClient(clientPtr clnt);
	/**Closing client's socket and releasing all buffers connected with it
      * \param clnt a pointer to the instance of client*/
	virtual void closeAndFreeClient(clientPtr clnt);

	/** This method creates a new game when a first gamer joins. 
      * \param the pointer to the client
      * \return true if the game is suceessfully created*/
	bool createGame(clientPtr clnt);
    
    /** Destroying the game when the last user has logged out */
    void destroyGame();

    /** This function add a Gamer to a vector, but also check if the maximum
     * count of gamers hasn't been exceeded. In that case it generates error 
     * sent to the client 
     * \param clnt the pointer to the client
     * \return true if the gamer is suceessfully added*/
    bool addGamer(clientPtr clnt);
    
    /** Setting a player name when a client has sent an appropriate message 
     * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void loginmessage(struct bufferevent * bev);
    
    /** Called when others need to wait for players 
      * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void waitingmessage(struct bufferevent * bev);
    
    /** Called when a client joins game 
      * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void joinedmessage(struct bufferevent * bev);

     /** Called when a client sends draw message 
       * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void drawmessage(struct bufferevent * bev);

    /** Called when a client sends chat message 
      * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void chatmessage(struct bufferevent * bev);

    /** Called when a drawing player has left a game and new player 
      * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void gamemessage(struct bufferevent * bev);

    /** Checking a password when a client has sent an appropriate message 
      * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void itemmessage(struct bufferevent * bev);

    /** Called when a client has won a game and now will be drawing. The message 
      * with a name of player is sent
      * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void guessedmessage(struct bufferevent * bev);

    /** Sending a message with a score table 
      * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void scoremessage(struct bufferevent * bev);
    
    /** Function invoked when client reguests for sending the history of a game 
      * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void historymessage (struct bufferevent * bev);

    /**Function invoked when client wants to draw from the beginning 
      * \param bev the stuct bufferevent associated with a client from whom the message originates*/
    void clearmessage(struct bufferevent * bev);

    /**Register observer
     * \param obs the observer that is to be registered*/
    void registerobserver(drawobserver * obs);

    /**Detach observer when it is not needed anymore*/
    void detachobserver();

    /**Notify observer when it is needed
      * \param the data that needs to be passed to the observer*/
    void notifyobserver(const std::string & data);

    /**Notify observers to clear a history*/
    void clearobserversmemory();
private:
    /**A vector of clients holding references to them*/
	std::list<clientPtr> *listOfGamers;
    /**A vector holding parts of incoming message*/
    std::vector<std::string> tokenvector;
    /**A maximum number of gamers that simultaneously can play*/
    const unsigned int *GAMER_MAX_COUNT;
    /**A string holding actual password clients need to guess*/
    std::string pass;
    /**A enum indicating whether the game has started or not*/
    GAMESTATUS gamestat;
    /**The patterns of messages got from clients*/
    std::string patterns [9];
    /**Function Pointers to functions invoked when message comes*/
    void (serverlogic::*FunctionPointers[9])(struct bufferevent *);
    /**Functor responsible for invoking appropriate function when needed*/
    boost::function<void (serverlogic*, struct bufferevent *)> invokefunction;
    /**A list holding references to observer holding history*/
    std::list<drawobserver *> observers;
};
#endif //SERVERLOGIC_H
