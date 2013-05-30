#include "serverlogic.h"

serverlogic::serverlogic() : mainserv(), GAMER_MAX_COUNT(new unsigned int(20))
{
    listOfGamers = 0;
    gamestat = serverlogic::OFF;
    patterns[0] = "LOGIN";
    patterns[1] = "WAITING";
    patterns[2] = "JOINED";
    patterns[3] = "DRAW";
    patterns[4] = "CHAT";
    patterns[5] = "GAME";
    patterns[6] = "ITEM";
    patterns[7] = "GUESSED";
    patterns[8] = "SCORE";
    FunctionPointers[0] = &serverlogic::loginmessage;
    FunctionPointers[1] = &serverlogic::waitingmessage;
    FunctionPointers[2] = &serverlogic::joinedmessage;
    FunctionPointers[3] = &serverlogic::drawmessage;
    FunctionPointers[4] = &serverlogic::chatmessage;
    FunctionPointers[5] = &serverlogic::gamemessage;
    FunctionPointers[6] = &serverlogic::itemmessage;
    FunctionPointers[7] = &serverlogic::guessedmessage;
    FunctionPointers[8] = &serverlogic::scoremessage;
}

serverlogic::~serverlogic()
{
}

bool serverlogic::createGame(clientPtr clnt)
{
    listOfGamers = new std::list<clientPtr>();
    listOfGamers->push_back(clnt);
    clnt->setStatus(new client::STATUS(client::DRAW));
    serverlog::CONSOLE con = serverlog::NO_CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("New player registered", *(&con));
    return true;
}

void serverlogic::destroyGame()
{
    if (!listOfGamers->empty())
    {
        listOfGamers->clear();
        serverlog::CONSOLE con = serverlog::CONSOLE_OUTPUT;
        serverlog::getlog().loginfo("The game has been destroyed", *(&con));
    }
}

bool serverlogic::addGamer(clientPtr clnt)
{
    if (listOfGamers == 0)
    {
        //There are new players and haven't been before
        return (createGame(clnt));
    }
    else if (listOfGamers->empty())
    {
        // new incoming client
        return (createGame(clnt));
    }
    else if (listOfGamers->size()<(*GAMER_MAX_COUNT))
    {
        //if (gamestat == OFF)
        //{
            //GAME Hasn't started 
            listOfGamers->push_back(clnt);
            clnt->setStatus(new client::STATUS(client::GUESS));
            return true;
        //}
        //else
        //{
            //If a game has already started
        //    char data[] = "WAIT FOR NEW GAME";
        //    evbuffer_add((clnt->out_buffer), data, strlen(data));
        //    return false;
        //}
    }
    else if (listOfGamers->size() == (*GAMER_MAX_COUNT))
    {
        std::string *data = new std::string("FULL");
        evbuffer_add((clnt->out_buffer), data->c_str(), data->size());
        closeAndFreeClient(clnt);
        delete data;
        return false;
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
    //clientPtr clnt = std::make_shared<client>(*(static_cast<client *>(ctx)));
    clientPtr clnt = boost::make_shared<client>(*static_cast<client*>(ctx));
    serverlog::CONSOLE con = serverlog::NO_CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("Request from a client to be registered", *(&con));
    if (addGamer(clnt))
    {
        //Gamer added successfully
    }
}

void serverlogic::on_read(struct bufferevent *bev, void *arg)
{
    serverlog::CONSOLE con = serverlog::NO_CONSOLE_OUTPUT;
    serverlog::getlog().loginfo("New message from a client", *(&con));
    char *data ;
    ////SprawdŸ treœæ wiadomoœci
    /*A temporary object of evbuffer type where data will be written*/
    struct evbuffer *input;
    input = bufferevent_get_input(bev);

    struct evbuffer *output;
    output = bufferevent_get_output(bev);

    size_t len = evbuffer_get_length(input);
    data = new char[len+1]();
    /**Remove the info from the buffer so that it is empty for next data*/
    evbuffer_remove(input, data, len);
    con = serverlog::CONSOLE_OUTPUT;

    std::string* buffer = new std::string("Message text: ");
    buffer->append(data,len);
    serverlog::getlog().loginfo(buffer->c_str(),*(&con));
    delete buffer;
    
    boost::split(tokenvector, data, boost::is_any_of(" "), boost::token_compress_on);
    for (int i = 0; i<(sizeof(patterns)/sizeof(*patterns));++i){
        if (tokenvector[0].compare(patterns[i]) == 0)
        {
            (this->*FunctionPointers[i])(bev);
            break;
        }
    }
    delete data;
    evbuffer_drain(input, len);
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
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev == (*i)->in_buffer)
        {
            std::string* data = new std::string("Disconnected: ");
            data->append((*i)->getName());
            evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
            serverlog::CONSOLE con = serverlog::NO_CONSOLE_OUTPUT;
            serverlog::getlog().loginfo(data->c_str(), *(&con));
            if (client::DRAW == *((*i)->getStatus()))
            {
                srand ( static_cast<unsigned int>(time(0)) );
                int element = rand()%listOfGamers->size();
                std::list<clientPtr>::iterator pointer = listOfGamers->begin();
                for(int j = 0 ;j < element && pointer != listOfGamers->end() ; j++ , pointer++ ){};
                (*pointer)->setStatus(new client::STATUS(client::DRAW));
            }
            delete data;
            listOfGamers->erase(i);
            con = serverlog::NO_CONSOLE_OUTPUT;
            serverlog::getlog().loginfo("Erased client's data from database", *(&con));
            break;
        }

    }
    if (listOfGamers->size()<=1)
    {
        waitingmessage(bev);
        gamestat = serverlogic::OFF;
        history.clear();
    }
}

void serverlogic::loginmessage(struct bufferevent *bev)
{
    /*for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (tokenvector[1].compare((*i)->getName()))
        {
            std::string* data = new std::string("LOGIN exists");
            evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
            delete data;
        }
    }*/
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev == (*i)->in_buffer)
        {
            char *name_ = new char[tokenvector[1].size()+1]();
            strncpy(name_, tokenvector[1].c_str(), tokenvector[1].size()+1);
            (*i)->setName( name_ );
            break;
        }
    }
    if (gamestat == serverlogic::ON)
    {
        joinedmessage(bev);
    }
    if (listOfGamers->size() == 1 || listOfGamers->size() == 2)
    {
        gamemessage(bev);
    }
}

void serverlogic::gamemessage(struct bufferevent *bev)
{
    std::string* data = new std::string("GAME ");
    if (listOfGamers->size() > 1)
    {
        gamestat = serverlogic::ON;  
        for (std::list<clientPtr>::iterator i = listOfGamers->begin();
            i != listOfGamers->end(); ++i)
        {
                if (client::DRAW == *((*i)->getStatus()))
                {
                    //Adding a drawing player name
                    data->append((*i)->getName());
                    break;
                }
        }
        for (std::list<clientPtr>::iterator i = listOfGamers->begin();
            i != listOfGamers->end(); ++i)
        {
                //Sending game start message
                evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
        }
    }
    else
    {
        this->waitingmessage(bev);
    }             
    delete data;
}

void serverlogic::waitingmessage(struct bufferevent *bev)
{
    //Sending waiting message to every player
    std::string* data = new std::string("WAITING");
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
            i != listOfGamers->end(); ++i)
        {
                evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
        }
    delete data;
}

void serverlogic::joinedmessage(struct bufferevent *bev)
{
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev == (*i)->in_buffer)
        {
            
            std::string* data = new std::string("JOINED ");
            for (std::list<clientPtr>::iterator ptr = listOfGamers->begin();
                 ptr != listOfGamers->end(); ++ptr)
            {
                if (client::DRAW == *((*ptr)->getStatus()))
                {
                    data->append((*ptr)->getName());
                    break;
                }
            }
            //Send joined message
            evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
            delete data;
            //Send history
            boost::thread thread(&serverlogic::sendhistory, this, (*i),(unsigned int) 0);
        }
    }
}

void serverlogic::sendhistory (clientPtr i, unsigned int &j)
{
    if (j < history.size())
    {
        while (evbuffer_get_length(i->out_buffer) > 0) {}
        evbuffer_add(i->out_buffer, history[j].c_str(), history[j].size());
        bufferevent_flush(i->in_buffer, EV_WRITE, BEV_NORMAL);
        boost::thread thread(&serverlogic::sendhistory, this, i, ++j );
    }
}

void serverlogic::drawmessage(struct bufferevent *bev)
{
    std::string* data = new std::string("");
    data->append(tokenvector[0]);
    for (unsigned int i=1; i< tokenvector.size();++i)
    {
        data->append(" ");
        data->append(tokenvector[i]);
    }
    history.push_back(*data);
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev != (*i)->in_buffer)
        {
            evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
            //multicasting draw message
            //Saving to history
        }
    }
    delete data;
}

void serverlogic::chatmessage(struct bufferevent *bev)
{

    std::string* data = new std::string(tokenvector[0] + " ");
    std::string* password = new std::string("");
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev == (*i)->in_buffer)
        {
            //Copy the data from input buffer 
            for (unsigned int i=1; i< tokenvector.size();++i)
            {
                data->append(" ");
                data->append(tokenvector[i]);
            }
            break;
        }
    }
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev != (*i)->in_buffer)
        {
            evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
        }
    }
    for (unsigned int i = 2; i<tokenvector.size();++i)
    {
        password->append(tokenvector[i] + " ");
    }
    if (pass.compare(*password))
    {
        boost::thread thread( boost::bind(&serverlogic::guessedmessage, this, bev));
    }
    delete data;
    delete password;
}

void serverlogic::itemmessage(struct bufferevent *bev)
{
        for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
        {
            if (bev == (*i)->in_buffer)
            {
                for (unsigned int j=1; j < tokenvector.size();++j)
                {
                    pass.append(" ");
                    pass.append(tokenvector[j]);
                }
                std::string* data = new std::string("OK");
                evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
                delete data;
                break;
            }
        }
}

void serverlogic::guessedmessage(struct bufferevent *bev)
{
    while (evbuffer_get_length(
            bufferevent_get_output((listOfGamers->front())->in_buffer)) > 0
            && evbuffer_get_length(
            bufferevent_get_output((listOfGamers->back())->in_buffer)) > 0 ) 
    { std::cout << evbuffer_get_length(bufferevent_get_output(bev)) << std::endl; }
    std::string data;
    //Change of the status of client that has previously drawn
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (client::DRAW == *((*i)->getStatus()))
        {
            (*i)->setStatus(new client::STATUS(client::GUESS));
            break;
        }
    }
    //Check who has guessed the drawing item
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev == (*i)->in_buffer)
        {
            data = "GUESSED ";
            data.append((*i)->getName());
            (*i)->setStatus(new client::STATUS(client::DRAW));
            (*i)->addPoints();
            //Sending the info who has guessed the answer to every client
            for (std::list<clientPtr>::iterator it = listOfGamers->begin();
                it != listOfGamers->end(); ++it)
            {
                evbuffer_add(((*it)->out_buffer), data.c_str(), data.size());
                evbuffer_drain((*it)->out_buffer, data.size());
            }
            boost::thread thread( boost::bind( 
                                &serverlogic::threadgamemessage, this, (*i) ) );
            break;
        }
    }

}

void serverlogic::threadgamemessage(clientPtr i)
{
    while (evbuffer_get_length(i->out_buffer) > 0) {}
    std::string* data2 = new std::string("GAME ");
    data2->append(i->getName());
    evbuffer_add((i->out_buffer), data2->c_str(), data2->size());
    delete data2;
}

void serverlogic::scoremessage(struct bufferevent *bev)
{
    ///Sending score to every player
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev == (*i)->in_buffer)
        {
            bufferevent_read_buffer(bev,(*i)->out_buffer);
            break;
        }
    }
}
