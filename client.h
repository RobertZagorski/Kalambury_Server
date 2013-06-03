#ifndef CLIENT_H
#define CLIENT_H
///The class of a client.

/**The class of a client. It mainly keeps information about a client
  * like a socket descriptor, name of a client, points he has  
  */
class client
{
public:	
    /**The status of a player. Used to verify what rights a player has during a game*/
    enum STATUS {
                 DRAW, /**< The client is now drawing. Only one client can have this status*/
                 GUESS, /**< The client is guessing*/
                 OBSERVE /**< The client is now observing the game */
                };
	/**The client's socket. */
    int c_socket;
	/**The event_base for this client. */
    struct event_base *evbase;
    /**The bufferedevent for this client. */
    struct bufferevent *in_buffer;
    /**The output buffer for this client. */
    struct evbuffer *out_buffer;
	/**The default constructor*/
	client();
    /**copy constructor*/
    client (const client &);
	/*The default deconstructor*/
	~client();
	/**The method used to retrieve the status of a client
      * \return STATUS* the status of a client
      */
	STATUS* getStatus();
    /**The method used to set the status of a client
      * \param status_ the new status of a client*/
    void setStatus(STATUS* status_);
    /**The method used to retrieve the name of a client
      * \return the name of a client*/
	char* getName();
    /**The method used to set the name of a client
      * \param name_ the new name of a client*/
    void setName(char *name_);
    /**The method used to retrieve the number of points of a client
      * \return the number of points of a client*/
	unsigned int getPoints();
    /**The method used to add point to client's score*/
    void addPoints();
    /**The method used to set the number of points of a client
      * \param points the points of a client*/
    void setPoints(const int &points_);
private:
	/**The status of a client. Indicates what rights the client has in the game*/
	STATUS *clientStatus_;
    /**The name of a client*/
    char *name_;
    /**Number of points of a client*/
    unsigned int points_;
};
#endif //CLIENT_H
