#include "client.h"


client::client() {}

client::~client() {}


STATUS* client::getStatus()
{
	return clientStatus;
}