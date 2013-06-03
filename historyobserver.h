#ifndef HISTORYOBSERVER_H
#define HISTORYOBSERVER_H
///*An implementation of observer responsible for holding history*

#include <string>
#include <vector>
#include <boost/foreach.hpp>

#include "drawobserver.h"

/**Class holding a reference to history of a drawing. Implemented as a observer pattern*/
class historyobserver : public drawobserver
{
public:
    /**Default constructor*/
    historyobserver();
    /**Default deconstructor*/
    ~historyobserver();
    /**Updating a collection of history - DRAW messages*/
    void update(const std::string &);
    /**Pass a reference to merged history collection*/
    std::string historymerging();
    /**Reset a history*/
    void clearobserversmemory();

private:
    /**History of DRAW messages*/
    std::vector<std::string> history;

};
#endif // HISTORYOBSERVER_H