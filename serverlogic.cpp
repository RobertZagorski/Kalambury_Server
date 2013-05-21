
#include "serverlogic.h"


serverlogic::serverlogic() : mainserv(), GAMER_MAX_COUNT(new unsigned int(20))
{
    vectorOfGamers = 0;
}

serverlogic::~serverlogic()
{
}

bool serverlogic::createGame(clientPtr clnt)
{
    vectorOfGamers = new std::list<clientPtr>();
    vectorOfGamers->push_back(clnt);
    serverlog::CONSOLE con = serverlog::NO_CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("New player registered", *(&con));
    return true;
}

void serverlogic::destroyGame()
{
    if (!vectorOfGamers->empty())
    {
        vectorOfGamers->clear();
        serverlog::CONSOLE con = serverlog::CONSOLE_OUTPUT;
        serverlog::getlog().loginfo("The game has been destroyed", *(&con));
    }
}

bool serverlogic::addGamer(clientPtr clnt)
{
    if (vectorOfGamers == 0)
    {
        return (createGame(clnt));
    }
    else if (vectorOfGamers->empty())
    {
        return (createGame(clnt));
    }
    else if (vectorOfGamers->size()<(*GAMER_MAX_COUNT))
    {
        vectorOfGamers->push_back(clnt);
        return true;
    }
    else return false;
}

serverlogic::MESSAGE serverlogic::checkForMessageType(char *)
{
    serverlogic::MESSAGE message = serverlogic::LOGGING;
    return message;
}

void serverlogic::on_accept(struct evconnlistener *listener,evutil_socket_t fd, 
                            struct sockaddr *address, int socklen, void *ctx)
{
    clientPtr clnt = std::make_shared<client>(*(static_cast<client *>(ctx)));
    serverlog::CONSOLE con = serverlog::NO_CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("Request from a client to be registered", *(&con));
    if (addGamer(clnt))
    {
        ///Gamer added successfully
    }
}

//void serverlogic::multicast(clientPtr clnt, evbuffer &output, char &data, size_t &len)
//{
//    
//    /**In case we don't send to the sender*/
//        if ((&output) != clnt->out_buffer)
//        {
//        
//            printf("%.*s\n", len, data);
//            //printf("we got some data: %s\n", (*data));
//            printf ("The size of data: %d\n", len);
//        }
//}

void serverlogic::on_read(struct bufferevent *bev, void *arg)
{
    serverlog::CONSOLE con = serverlog::NO_CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("New message from a client", *(&con));
    char *data = (char *) arg;
    ////SprawdŸ treœæ wiadomoœci
    /*A temporary object of evbuffer type where data will be written*/
    struct evbuffer *input;
    input = bufferevent_get_input(bev);

    struct evbuffer *output;
    output = bufferevent_get_output(bev);

    size_t len = evbuffer_get_length(input);
    data = new char[len]();
    /**Remove the info from the buffer so that it is empty for next data*/
    evbuffer_remove(input, data, len);
    con = serverlog::CONSOLE_OUTPUT;
    char *buffer = new char[len+20]();
    sprintf(buffer, "Message text: %s", data);
    serverlog::getlog().loginfo(buffer,*(&con));
    delete [] buffer;
    ///Do ka¿dego z innych klientów przeœlij wiadomoœæ
    //
    //   U¿ycie REGEX do rozpoznania jaki to tekst
    //
    //
    for (std::list<clientPtr>::iterator i = vectorOfGamers->begin();
         i != vectorOfGamers->end(); ++i)
    {
        /**In case we don't send to the sender*/
        if (output != (*i)->out_buffer)
        {
            evbuffer_add(((*i)->out_buffer), data, len);
            con = serverlog::NO_CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("Sent to client", *(&con));
        }
    }
    delete [] (data);
}

void serverlogic::closeClient(clientPtr clnt) 
{
    if (clnt != NULL) {
        if (clnt->c_socket >= 0) {
            #ifdef OS_Windows
                /* Windows code */
		        closesocket(clnt->c_socket );
            #else
                /* GNU/Linux code */
		        close(clnt->c_socket );
            #endif
            clnt->c_socket = -1;
        }
    }
}

void serverlogic::closeAndFreeClient(clientPtr clnt) {
    if (clnt != NULL) {
        serverlogic::closeClient(clnt);
        if (clnt->in_buffer != NULL) {
            bufferevent_free(clnt->in_buffer);
            clnt->in_buffer = NULL;
        }
        if (clnt->evbase != NULL) {
            event_base_free(clnt->evbase);
            clnt->evbase = NULL;
        }
        if (clnt->out_buffer != NULL) {
            evbuffer_free(clnt->out_buffer);
            clnt->out_buffer = NULL;
        }
    }
}

void serverlogic::on_error(struct bufferevent* bev, short what, void* arg)
{
    for (std::list<clientPtr>::iterator i = vectorOfGamers->begin();
         i != vectorOfGamers->end(); ++i)
    {
        if (bev == (*i)->in_buffer)
        {
            
            char data[] = "Disconnected";
            evbuffer_add(((*i)->out_buffer), data, strlen(data));
            //closeAndFreeClient( (*i) );
            vectorOfGamers->erase(i);            
            serverlog::CONSOLE con = serverlog::NO_CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("Erased client's data from database", *(&con));
            break;
        }

    }
}