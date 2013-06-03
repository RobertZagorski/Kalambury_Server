#ifndef SERVERLOG_H
#define SERVERLOG_H
///A class responsible for logging
#include <iostream>
#include <fstream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/thread/mutex.hpp>

/**Class responsible for logging information connected with server. Based on
  * singleton pattern*/
class serverlog
{
public:
    /**The enum responsible for */
    enum CONSOLE {
                  NO_CONSOLE_OUTPUT, /**< The ouput will not be put on the console */
                  CONSOLE_OUTPUT /**< The ouput will be put on the console */
                 };
    ~serverlog(void);
    static serverlog& getlog();
    void loginfo(const char *info, const CONSOLE &todo);
    void logtofile(const char * info);
    static boost::mutex& GetMutex();
private:
    /**The private serverlog object holding the only one reference to this class object*/
    static serverlog* serverlog_;
    /**Private constructor. Does nothing*/
    serverlog();
    /**The private copy constructor*/
    serverlog(const serverlog&);
    /**The stream that hold a reference to log file*/
    std::ofstream logfile;
    /**The varabiable holding a reference to current time*/
    boost::posix_time::ptime currenttime;
    /**The variable holding a text - regex by which the date should be presented*/
    boost::local_time::local_time_facet* output_facet;
};
#endif //SERVERLOG_H
