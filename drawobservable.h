#ifndef DRAWOBSERVABLE_H
#define DRAWOBSERVABLE_H
///An interface implemented by observable drawing object
#include <string>

/**An interface that has to be implemented by class that wants to be observed by 
  * drawobserver*/
class drawobservable
{
public:
    /**The default constructor. Does nothing*/
    drawobservable() {}
    /**The default virtual deconstructor*/
    virtual ~drawobservable() {}
    /**Register observer
      * \param obs the observer to be registered*/
    virtual void registerobserver(drawobserver *obs_) = 0;
    /**Detach observer when it is not needed anymore*/
    virtual void detachobserver() = 0;
    /**Notify observer when it is needed
      * \param data the text the observer must be notified with*/
    virtual void notifyobserver(const std::string &data_) = 0;
};
#endif //DRAWOBSERVABLE_H