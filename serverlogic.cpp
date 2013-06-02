#include "serverlogic.h"

serverlogic::serverlogic() : mainserv(), GAMER_MAX_COUNT(new unsigned int(20))
{
    listOfGamers = 0;
    gamestat = serverlogic::OFF;
    patterns[0] = "LOGIN";
    patterns[1] = "JOINED";
    patterns[2] = "HISTORY";
    patterns[3] = "ITEM";
    patterns[4] = "DRAW";
    patterns[5] = "CHAT";
    patterns[6] = "SCORE";
    patterns[7] = "CLEAR";
    FunctionPointers[0] = &serverlogic::loginmessage;
    FunctionPointers[1] = &serverlogic::joinedmessage;
    FunctionPointers[2] = &serverlogic::historymessage;
    FunctionPointers[3] = &serverlogic::itemmessage;
    FunctionPointers[4] = &serverlogic::drawmessage;
    FunctionPointers[5] = &serverlogic::chatmessage;
    FunctionPointers[6] = &serverlogic::scoremessage;
    FunctionPointers[7] = &serverlogic::clearmessage;
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
        listOfGamers->push_back(clnt);
        clnt->setStatus(new client::STATUS(client::GUESS));
        return true;
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

void serverlogic::on_accept(struct evconnlistener *listener,evutil_socket_t fd, 
                            struct sockaddr *address, int socklen, void *ctx)
{
    clientPtr clnt = boost::make_shared<client>(*static_cast<client*>(ctx));
    clnt->setPoints(0);
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
            //(this->*FunctionPointers[i])(bev);
            invokefunction = FunctionPointers[i];
            invokefunction(this, bev);
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
    //Check whether the client doesn't duplicate the name
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev != (*i)->in_buffer)
        {
            if (!tokenvector[1].compare((*i)->getName()))
            {
                std::string* data = new std::string("LOGIN exists");
                evbuffer_add(bufferevent_get_output(bev), data->c_str(), data->size());
                delete data;
            }
        }
    }
    //Set the name of a player
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
    //The condition to check if a player already joins the game
    if (gamestat == serverlogic::ON)
    {
        joinedmessage(bev);
    }
    // The condition for the first or second gamer
    if (listOfGamers->size() == 1 || listOfGamers->size() == 2)
    {
        serverlogic::gamemessage(bev);
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
        //The gamer is alone in the game, must wait for others
        this->waitingmessage(bev);
    }             
    delete data;
}

void serverlogic::waitingmessage(struct bufferevent *bev)
{
    //Sending waiting message to player
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
        }
    }
}

void serverlogic::historymessage(struct bufferevent *bev)
{
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev == (*i)->in_buffer)
        {
            BOOST_FOREACH( std::string text, history )
            {
                evbuffer_add((*i)->out_buffer, text.c_str(), text.size());
            }
        }
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
    //Saving to history
    history.push_back(*data);
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev != (*i)->in_buffer)
        {
            //multicasting draw message
            evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
            
        }
    }
    delete data;
}

void serverlogic::chatmessage(struct bufferevent *bev)
{

    std::string* data = new std::string(tokenvector[0] + " ");
    std::string* password = new std::string("");
    //Copy the data from input buffer
    data->append(" ");
    data->append(tokenvector[1]);
    for (unsigned int i=2; i< tokenvector.size();++i)
    {
        data->append(" ");
        data->append(tokenvector[i]);
        password->append(" ");
        password->append(tokenvector[i]);
    }
    *password = password->substr(1,password->size()-1);
    if ( !password->compare(pass.c_str()) )
    {
        guessedmessage(bev);
        return;
    }
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev != (*i)->in_buffer)
        {
            evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
        }
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
                pass = pass.substr(1,pass.size()-1);
                std::string* data = new std::string("OK");
                evbuffer_add(((*i)->out_buffer), data->c_str(), data->size());
                delete data;
                break;
            }
        }
}

void serverlogic::guessedmessage(struct bufferevent *bev)
{
    std::string data;
    //Change of the status of client that has previously drawn
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
        i != listOfGamers->end(); ++i)
    {
        if (client::DRAW == *((*i)->getStatus()))
        {
            delete (*i)->getStatus();
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
            data.append(" ");
            data.append(pass);
            delete (*i)->getStatus();
            (*i)->setStatus(new client::STATUS(client::DRAW));
            (*i)->addPoints();
            //Sending the info who has guessed the answer to every client
            for (std::list<clientPtr>::iterator it = listOfGamers->begin();
                it != listOfGamers->end(); ++it)
            {
                std::cout << evbuffer_add(((*it)->out_buffer), data.c_str(), data.size()) << std::endl;
            }
            break;
        }
    }
    pass.clear();
    history.clear();
}

void serverlogic::scoremessage(struct bufferevent *bev)
{
    std::multimap<unsigned int,std::string> *mmap =  new std::multimap<unsigned int,std::string>();
    bool inthree = false;
    int position = 0;
    int playerposition = position;
    unsigned int points;
    std::string name;
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        ++position;
        mmap->insert( std::pair<unsigned int,std::string>((*i)->getPoints(),(*i)->getName()) );
        if (bev == (*i)->in_buffer)
        {
            points = (*i)->getPoints();
            name = (*i)->getName();
            playerposition = position;
        }
    }
    int counter = 0;
    //Counting how many players are ahead
    /*for (std::multimap<unsigned int,std::string>::reverse_iterator i = mmap->rbegin();
         i != mmap->find(points); ++i)
    {
        ++counter;
    }*/
    std::string *message = new std::string(tokenvector[0] + " ");
    std::multimap<unsigned int,std::string>::reverse_iterator i = mmap->rbegin();
    //message->append(boost::lexical_cast<std::string>(1));
    //message->append(" ");
    message->append(i->second);
    message->append(" ");
    message->append(boost::lexical_cast<std::string>(i->first));
    message->append(" ");
    points == i->first ? inthree = true : inthree = inthree ;
    ++i;
    //message->append(boost::lexical_cast<std::string>(2));
    //message->append(" ");
    message->append(i->second);
    message->append(" ");
    message->append(boost::lexical_cast<std::string>(i->first));
    message->append(" ");
    (points == i->first) ? inthree = true : inthree = inthree ;
    if (mmap->size() > 2)
    {
        ++i;
        //message->append(boost::lexical_cast<std::string>(3));
        //message->append(" ");
        message->append(i->second);
        message->append(" ");
        message->append(boost::lexical_cast<std::string>(i->first));
        message->append(" ");
        (points == i->first) ? inthree = true : inthree = inthree ;
    }
    ///Sending score to the player
    if (!inthree)
    {
        //message->append(boost::lexical_cast<std::string>(playerposition));
        //message->append(" ");
        message->append(name);
        message->append(" ");
        message->append(boost::lexical_cast<std::string>(points));
        message->append(" ");
    }
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev == (*i)->in_buffer)
        {
            evbuffer_add(((*i)->out_buffer), message->c_str(), message->size());
            break;
        }
    }
    delete message;
}

void serverlogic::clearmessage(struct bufferevent *bev)
{
    for (std::list<clientPtr>::iterator i = listOfGamers->begin();
         i != listOfGamers->end(); ++i)
    {
        if (bev != (*i)->in_buffer)
        {
            evbuffer_add(((*i)->out_buffer), tokenvector[0].c_str(), tokenvector[0].size());
        }
    }
}
