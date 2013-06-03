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
    serverlog::CONSOLE con = serverlog::CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("Starting to initiate server", *(&con));
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
        #ifdef OS_Windows
            int error = WSAGetLastError();
            con = serverlog::NO_CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("Socket couldn't be created "+error, *(&con));
            WSACleanup();
        #else
            err(1, "listen failed");
        #endif

    }

	listenAddress = new sockaddr_in();
	listenAddress->sin_family = AF_INET;
	listenAddress->sin_addr.s_addr = INADDR_ANY;
    listenAddress->sin_port = htons(*PORT);
    memset(&(listenAddress->sin_zero), 0, sizeof(listenAddress->sin_zero));
    con = serverlog::CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("Parameters of the socket: ", *(&con));
    char *buffer = new char[20]();
    sprintf(buffer, "Port: %d", ntohs(listenAddress->sin_port));
    serverlog::getlog().loginfo(buffer,*(&con));
    delete [] buffer;

	/*Set the socket to non-blocking*/
    if (evutil_make_socket_nonblocking(socketOutput) < 0) {
		//B³¹d - implementacja biblioteki boost z b³êdami boost::exception
        #ifdef OS_Windows
            /* Windows code */
            con = serverlog::CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("Failed to set socket in non-blocking mode", *(&con));
        #else
            con = serverlog::CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("Failed to set socket in non-blocking mode", *(&con));
        #endif
    }
	/*creating a base event for server needed by Libevent*/
    if ((base = event_base_new()) == NULL) {
        con = serverlog::CONSOLE_OUTPUT;
        serverlog::getlog().loginfo("Unable to create socket accept event base", *(&con));
		#ifdef OS_Windows
            /* Windows code */
		    closesocket((socketOutput));
        #else
            /* GNU/Linux code */
		    close(socketOutput);
        #endif
       //~serverlogic_();
    }
    con = serverlog::CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("Server Running", *(&con));
    /* A listening socket is created, creating a accept event to
     * be notified when a new client is connected. 
     * Also this function binds the socket with client */
    listener = evconnlistener_new_bind(base, baseserver_on_accept, this,
            LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
            (struct sockaddr*)listenAddress, sizeof(*listenAddress));
        if (!listener) {
            con = serverlog::NO_CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("Couldn't create listener", *(&con));
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
    con = serverlog::CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("Server shutdown", *(&con));
}


baseserver::~baseserver()
{
    if (event_base_loopexit(base, NULL)) {
        serverlog::CONSOLE con = serverlog::CONSOLE_OUTPUT;
        serverlog::getlog().loginfo("Error shutting down server", *(&con));
    }
}

void baseserver::closeClient(clientPtr clnt) 
{
    serverlogic_->closeClient(clnt);
}

void baseserver::closeAndFreeClient(clientPtr clnt) 
{
    serverlogic_->closeAndFreeClient(clnt);
}


void baseserver::on_accept(struct evconnlistener *listener,evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx)
{
	baseserver *callback_ptr = (baseserver *) ctx;
    /*Create new client*/
    clientPtr connectedClient = boost::make_shared<client>();
    connectedClient->out_buffer = evbuffer_new();
    serverlog::CONSOLE con = serverlog::CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("New incoming connection", *(&con));
	if (connectedClient->out_buffer == NULL) 
	{
        closeAndFreeClient(connectedClient);
        #ifdef OS_Windows
            /* Windows code */
            int error = WSAGetLastError();
            con = serverlog::CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("failed to create new ev_buffer for the client. Socket Error: "+ error, *(&con));
        #else
            /* GNU/Linux code */
            serverlog::CONSOLE con = serverlog::CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("client bufferevent creation failed", *(&con));
        #endif
    }
    
    ///*Accept incoming connection*/
    //connectedClient->evbase = event_base_new();
    connectedClient->evbase = evconnlistener_get_base(listener);
    if ((connectedClient->evbase = evconnlistener_get_base(listener)) == NULL)
    {
        //warn("client event_base creation failed");
        closeAndFreeClient(connectedClient);
        #ifdef OS_Windows
            /* Windows code */
            int error = WSAGetLastError();
            con = serverlog::CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("Failed to create base evconnlistenner for the client. Socket error: " + error, *(&con));
        #else
            /* GNU/Linux code */
            con = serverlog::CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("failed to create base evconnlistenner for the client", *(&con));
        #endif
    }
    
    /*Complete socket binding*/
	connectedClient->in_buffer = bufferevent_socket_new(
                                 connectedClient->evbase, fd, 
                                 BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS );
    con = serverlog::NO_CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("Associating socket with client", *(&con));
    if ((connectedClient->in_buffer) == NULL) 
	{
        closeAndFreeClient(connectedClient);
        #ifdef OS_Windows
            /* Windows code */
            int error = WSAGetLastError();
            con = serverlog::CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("Failed to associate underlying socket with the client: " + error, *(&con));
        #else
            /* GNU/Linux code */
            con = serverlog::CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("Failed to associate underlying socket with the client: ", *(&con));
        #endif
    }
    connectedClient->c_socket = evconnlistener_get_fd(listener);
	
    /*Create the buffered event*/
	bufferevent_setcb(connectedClient->in_buffer, (baseserver_on_read), 
                     (baseserver_on_write), (baseserver_on_error), this);
    con = serverlog::NO_CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("Setting a buffered event for client", *(&con));
    /* enable it before our callbacks will be called. */
    bufferevent_enable(connectedClient->in_buffer, EV_READ|EV_WRITE);
    connectedClient->out_buffer = bufferevent_get_output(connectedClient->in_buffer);
    serverlogic_->on_accept(listener,fd, address, socklen, static_cast<void *>(connectedClient.get()));
    connectedClient.reset();        
}

void baseserver::on_read(struct bufferevent *bev, void *arg)
{
    serverlogic_->on_read(bev,arg);
}

void baseserver::on_write(struct bufferevent *bev, void *arg) 
{
    evbuffer_drain(bufferevent_get_output(bev), 
                   evbuffer_get_length(bufferevent_get_output(bev)));
}

void baseserver::on_error(struct bufferevent* bev, short what, void* arg)
{
    serverlog::CONSOLE con = serverlog::NO_CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("Error on the underlying socket", *(&con));
    evutil_socket_t socketptr = bufferevent_getfd(bev);
    try {
    serverlogic_->on_error(bev,what, (void *)socketptr);
    } catch (std::exception Exc) {std::cout << Exc.what() << std::endl;}
}
