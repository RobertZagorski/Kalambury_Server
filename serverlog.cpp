#include "serverlog.h"

serverlog* serverlog::serverlog_ = 0L;

serverlog::serverlog()
{
    //Get current time using boost::posix_time library
    currenttime = boost::posix_time::second_clock::local_time();
    output_facet = new boost::local_time::local_time_facet();
    logfile.imbue(std::locale(std::locale::classic(), output_facet));
    output_facet->format("%a %b %d %Y, %H:%M:%s.%f");
}


serverlog::~serverlog()
{
    logfile.close();
    delete output_facet;
}

serverlog& serverlog::getlog()
{
    if(!serverlog_)
    {
        boost::mutex::scoped_lock lock(GetMutex());
        if(!serverlog_)
            serverlog_ = new serverlog();
    }
    return *serverlog_;
}

void serverlog::loginfo(const char *info,const serverlog::CONSOLE &todo)
{
    if (todo == serverlog::CONSOLE_OUTPUT)
    {
        currenttime = boost::posix_time::second_clock::local_time();
        std::cout << currenttime << " : " << info << std::endl;
    }
    //logtofile(info);
}

void serverlog::logtofile(const char *info)
{
    logfile.open ("log.txt", std::ios::out | std::ios::app);
    if (logfile.is_open())
    {
        currenttime = boost::posix_time::second_clock::local_time();
        logfile << currenttime << " : " << info <<"\n";
    }
    logfile.close();
    //else //b³¹d
}

boost::mutex& serverlog::GetMutex()
{
    static boost::mutex mutex;
    return mutex;
}
