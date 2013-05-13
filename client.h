#ifndef CLIENT_H
#define CLIENT_H

enum STATUS {DRAW,GUESS,OBSERVE};
class client
{
private:
	/*The status of a client. Indicates what rights the client has in the game*/
	STATUS* clientStatus;
public:	
	/* The client's socket. */
    int c_socket;

	/* The event_base for this client. */
    struct event_base *evbase;

    /* The bufferedevent for this client. */
    struct bufferevent *in_buffer;

    /* The output buffer for this client. */
    struct evbuffer *out_buffer;
	/*The default constructor*/
	client();
	/*The default deconstructor*/
	~client();
	/*The method used to retrieve the status of a client*/
	STATUS* getStatus(); 
};
#endif //CLIENT_H
