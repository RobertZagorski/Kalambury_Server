#ifndef DRAWOBSERVER_H
#define DRAWOBSERVER_H
///An observer interface
#include <string>

/**An observer interface */
class drawobserver
{
public:
    /**The default constructor. Does nothing.*/
    drawobserver() {}
    /**The default deontructor*/
    virtual ~drawobserver() {}
    /** The method that needs to be implemented by classes that inherits that interface
      * It updated the state of a drawing
      * \param text_ the text the observer must be notified with*/
    virtual void update(const std::string &text_) = 0;
    /**The method used to clear the the history vector*/
    virtual void clearobserversmemory() = 0;
    /**The merging of a history
      * \return the combined data*/
    virtual std::string historymerging() = 0;
};
#endif //DRAWOBSERVER_H