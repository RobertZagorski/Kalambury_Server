#include "client.h"


client::client() 
{
    this->points_ = 0;
    this->setStatus(new client::STATUS(client::GUESS));
    this->name_ = new char[1]();
}

client::client(const client &clnt)
{
    this->clientStatus_ = new STATUS(*clnt.clientStatus_);
    this->c_socket = clnt.c_socket;
    this->evbase = clnt.evbase;
    this->in_buffer = clnt.in_buffer;
    this->out_buffer = clnt.out_buffer;
    //this->name_ = new char (*clnt.name_);
}

client::~client() 
{
    delete name_;
}


client::STATUS* client::getStatus()
{
	return clientStatus_;
}

void client::setStatus(STATUS* status_)
{
	this->clientStatus_ = status_;
}

char* client::getName()
{
	return name_;
}

void client::setName(char* name)
{
	this->name_ = name;
}

unsigned int client::getPoints()
{
	return points_;
}

void client::addPoints()
{
	this->points_+=1;
}

void client::setPoints(const int &points)
{
    this->points_ = points;
}