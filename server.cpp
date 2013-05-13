#include "server.h"

    extern "C" void baseserver_on_accept(struct evconnlistener *listener, evutil_socket_t fd, 
                                         struct sockaddr *address, int socklen, void *ctx)
    {
        baseserver* baseserverPtr = (static_cast<baseserver*>(ctx));
        baseserverPtr->on_accept(listener, fd, address, socklen, ctx);
    }
    extern "C" void baseserver_on_read(struct bufferevent *bev, void *arg)
    {
        baseserver* baseserverPtr = (static_cast<baseserver*>(arg));
        baseserverPtr->on_read(bev, arg);
    }
    extern "C" void baseserver_on_write(struct bufferevent *bev, void *arg) 
    {
        baseserver* baseserverPtr = (static_cast<baseserver*>(arg));
        baseserverPtr->on_write(bev, arg);
    }

    extern "C" void baseserver_on_error(struct bufferevent* bev, short what, void* arg)
    {
        baseserver* baseserverPtr = (static_cast<baseserver*>(arg));
        baseserverPtr->on_error(bev, what, arg);
    }

baseserver::baseserver() : mainserv(){};

baseserver::baseserver(mainserv *servlogic, const unsigned short &port_) : 
                       mainserv(), serverlogic_(servlogic), PORT(&port_)
{
    const char reuseaddrOn = 1;
    #ifdef OS_Windows
        //Start up Winsock…
        WSADATA wsadata;
        int error = WSAStartup(0x0202, &wsadata);
        //Did something happen?
        if (error)
            WSACleanup();
        //Did we get the right Winsock version?
        if (wsadata.wVersion != 0x0202)
        {
            WSACleanup(); //Clean up Winsock
        }
    #endif
	/* Create listening socket. */
    socketOutput = socket(AF_INET, SOCK_STREAM, 0);
    if (socketOutput < 0) {
        //B³¹d - implementacja biblioteki boost z b³êdami boost::exception
        int error = WSAGetLastError();
		std::cout << "listen failed " << error << std::endl; 
        #ifdef OS_Windows
            WSACleanup();
        #endif
    }

	listenAddress = new sockaddr_in();
	listenAddress->sin_family = AF_INET;
	listenAddress->sin_addr.s_addr = INADDR_ANY;
    listenAddress->sin_port = htons(*PORT);
    memset(&(listenAddress->sin_zero), 0, sizeof(listenAddress->sin_zero));
    printf("server port = %d\n", ntohs(listenAddress->sin_port));

	/*Set the socket to non-blocking*/
    if (evutil_make_socket_nonblocking(socketOutput) < 0) {
		//B³¹d - implementacja biblioteki boost z b³êdami boost::exception
        //err(1, "failed to set server socket to non-blocking");
    }
	/*creating a base event for server needed by Libevent*/
    if ((base = event_base_new()) == NULL) {
        //perror("Unable to create socket accept event base");
		#ifdef OS_Windows
            /* Windows code */
		    closesocket((socketOutput));
        #else
            /* GNU/Linux code */
		    close(socketOutput);
        #endif
        baseserver::~baseserver();
    }
    printf("Server running.\n");
    /* A listening socket is created, creating a accept event to
     * be notified when a new client is connected. 
     * Also this function binds the socket with client */
    listener = evconnlistener_new_bind(base, baseserver_on_accept, this,
            LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
            (struct sockaddr*)listenAddress, sizeof(*listenAddress));
        if (!listener) {
                perror("Couldn't create listener");
        }
    /* Start the event loop. */
    event_base_dispatch(base);

    event_base_free(base);
    base = NULL;
	#ifdef OS_Windows
        /* Windows code */
		closesocket((socketOutput));
    #else
        /* GNU/Linux code */
	    close(socketOutput);
    #endif
    delete(listenAddress);
    printf("Server shutdown.\n");
}


baseserver::~baseserver()
{
    if (event_base_loopexit(base, NULL)) {
        perror("Error shutting down server");
    }
}

void baseserver::closeClient(client *clnt) 
{
    serverlogic_->closeClient(clnt);
}

void baseserver::closeAndFreeClient(client *clnt) 
{
    serverlogic_->closeAndFreeClient(clnt);
}


void baseserver::on_accept(struct evconnlistener *listener,evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx)
{
	baseserver *callback_ptr = (baseserver *) ctx;
    client *connectedClient;

	/*Create new client*/
	connectedClient = new client();

	if ((connectedClient->out_buffer = evbuffer_new()) == NULL) 
	{
        //warn("client output buffer allocation failed");
        closeAndFreeClient(connectedClient);
        int error = WSAGetLastError();
	std::cout << "listen failed " << error << std::endl; 
        //return;
    }
    
    ///*Accept incoming connection*/
    connectedClient->evbase = event_base_new();
    if ((connectedClient->evbase = evconnlistener_get_base(listener)) == NULL)
    {
        //warn("client event_base creation failed");
        closeAndFreeClient(connectedClient);
        int error = WSAGetLastError();
    	std::cout << "listen failed " << error << std::endl;
    }
    
    /*Complete socket binding*/
	connectedClient->in_buffer = bufferevent_socket_new(
                                 connectedClient->evbase, fd, BEV_OPT_CLOSE_ON_FREE);
    if ((connectedClient->in_buffer) == NULL) 
	{
        closeAndFreeClient(connectedClient);
        int error = WSAGetLastError();
    	std::cout << "listen failed " << error << std::endl;
       //return;
    }
    connectedClient->c_socket = evconnlistener_get_fd(listener);
	
    /*Create the buffered event*/
	bufferevent_setcb(connectedClient->in_buffer, (baseserver_on_read), 
                     (baseserver_on_write), (baseserver_on_error), this);
    
    /* enable it before our callbacks will be called. */
    bufferevent_enable(connectedClient->in_buffer, EV_READ|EV_WRITE);
    connectedClient->out_buffer = bufferevent_get_output(connectedClient->in_buffer);
    serverlogic_->on_accept(listener,fd, address, socklen, connectedClient);
	//////
	////// Reszta kodu dotycz¹ca rejestracji klientów itd.
	//////
	//////
        
}

void baseserver::on_read(struct bufferevent *bev, void *arg)
{
    ///
	/// Tu nale¿y zaimplementowaæ obs³ugê co nale¿y zrobiæ po przyjœciu danych od klienta
	///
    serverlogic_->on_read(bev,arg);
    /* Copy all the data from the input buffer to the output buffer. */
    //evbuffer_add_buffer(output, input);   
}

void baseserver::on_write(struct bufferevent *bev, void *arg) 
{
}

void baseserver::on_error(struct bufferevent* bev, short what, void* arg)
{
    std::cout << "Error on the underlying socket" << std::endl;
    evutil_socket_t socketptr = bufferevent_getfd(bev);
    bufferevent_free(bev);
    ///Jakoœ trzeba zamkn¹æ klienta i usun¹æ go z vectora w serverlogic.
}