#include "client.h"


client::client() {}

client::client(const client &clnt)
{
    this->clientStatus = clnt.clientStatus;
    this->c_socket = clnt.c_socket;
    this->evbase = clnt.evbase;
    this->in_buffer = clnt.in_buffer;
    this->out_buffer = clnt.out_buffer;
}

client::~client() {}


client::STATUS* client::getStatus()
{
	return clientStatus;
}

void client::setStatus(STATUS* status_)
{
	this->clientStatus = status_;
}