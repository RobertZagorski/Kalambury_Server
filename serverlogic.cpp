#include "serverlogic.h"


serverlogic::serverlogic() : mainserv(), GAMER_MAX_COUNT(new unsigned int(20))
{
    vectorOfGamers = 0;
}

serverlogic::~serverlogic()
{
}

bool serverlogic::createGame(client& clnt)
{
    vectorOfGamers = new std::vector<client>();
    vectorOfGamers->push_back(clnt);
    return true;
}

void serverlogic::destroyGame()
{
    if (!vectorOfGamers->empty())
    {
        vectorOfGamers->clear();
    }
}

bool serverlogic::addGamer(client& clnt)
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

MESSAGE serverlogic::checkForMessageType(char *)
{
    MESSAGE message = MESSAGE::LOGGING;
    return message;
}

void serverlogic::on_accept(struct evconnlistener *listener,evutil_socket_t fd, 
                            struct sockaddr *address, int socklen, void *ctx)
{
    client* clnt = static_cast<client *>(ctx);
    if (addGamer(*clnt))
    {
        ///Gamer added successfully
    }
}

void serverlogic::on_read(struct bufferevent *bev, void *arg)
{
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
    ///Do ka¿dego z innych klientów przeœlij wiadomoœæ
    for (std::vector<client>::iterator i = vectorOfGamers->begin();
         i != vectorOfGamers->end(); ++i)
    {
        /**In case we don't send to the sender*/
        //if (output != i->out_buffer)
        //{
            evbuffer_add((i->out_buffer), data, len);
        
            printf("%.*s\n", len, data);
            //printf("we got some data: %s\n", (*data));
            printf ("The size of data: %d\n", len);
        //}
    }
    delete [] (data);
}

void serverlogic::closeClient(client *clnt) 
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

void serverlogic::closeAndFreeClient(client *clnt) {
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
        free(clnt);
    }
}