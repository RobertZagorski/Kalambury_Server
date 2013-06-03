#include <iostream>
#include "mainserv.h"
#include "server.h"
#include "serverlogic.h"
#include "serverlog.h"

/**The main function of server program that start it*/
int main(int argc, char *argv[]){
	const unsigned short port = 12034;
    serverlogic *servlogic_ = new serverlogic();
    baseserver* baseserverPtr = new baseserver(servlogic_,port);
	std::cin.get();
    baseserverPtr->~baseserver();
    return 0;
};