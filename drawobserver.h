#ifndef DRAWOBSERVER_H
#define DRAWOBSERVER_H
///*An observer interface *
#include <string>

/**An observer interface */
class drawobserver
{
public:
    drawobserver() {}
    virtual ~drawobserver(void) {}
    /** The method that needs to be implemented by classes that inherits that interface
      * It updated the state of a drawing*/
    virtual void update(const std::string &) = 0;
    virtual void clearobserversmemory() = 0;
    virtual std::string historymerging() = 0;
};
#endif //DRAWOBSERVER_H

