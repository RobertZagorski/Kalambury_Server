#include "historyobserver.h"


historyobserver::historyobserver(void)
{
}


historyobserver::~historyobserver(void)
{
}

void historyobserver::update(const std::string &data)
{
    history.push_back(data);
}

std::string historyobserver::historymerging()
{
    std::string hist;
    hist.append("HISTORY ");
    BOOST_FOREACH( std::string text, history )
    {
       hist.append(text);
    }
    return hist;
}

void historyobserver::clearobserversmemory()
{
    history.clear();
}