#ifndef SERVERLOG_H
#define SERVERLOG_H

#include <iostream>
#include <fstream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/thread/mutex.hpp>

class serverlog
{
public:
    static enum CONSOLE {NO_CONSOLE_OUTPUT,CONSOLE_OUTPUT};
    ~serverlog(void);
    static serverlog& getlog();
    void loginfo(char *info, CONSOLE &todo);
    void logtofile(char * info);
    static boost::mutex& GetMutex();
private:
    static serverlog* serverlog_;
    serverlog(void);
    serverlog(const serverlog&);
    std::ofstream logfile;
    /**Date connected fields*/
    boost::posix_time::ptime currenttime;
    boost::local_time::local_time_facet* output_facet;
};
#endif //SERVERLOG_H
