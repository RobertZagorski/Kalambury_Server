#ifndef CLIENT_H
#define CLIENT_H
#include <memory>

class client
{
public:	

    enum STATUS {DRAW,GUESS,OBSERVE};
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
	/**The method used to retrieve the status of a client*/
	STATUS* getStatus();
    /**The method used to set the status of a client*/
    void setStatus(STATUS*);
private:
	/**The status of a client. Indicates what rights the client has in the game*/
	STATUS* clientStatus;
};
#endif //CLIENT_H
