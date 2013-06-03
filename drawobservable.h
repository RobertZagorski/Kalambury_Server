#ifndef DRAWOBSERVABLE_H
#define DRAWOBSERVABLE_H
///*An interface implemented by observable drawing object*
#include <string>

/**An interface that has to be implemented by class that wants to be observed by 
  * drawobserver*/
class drawobservable
{
public:
    drawobservable() {}
    virtual ~drawobservable() {}
    /**Register observer*/
    virtual void registerobserver(drawobserver *obs) = 0;
    /**Detach observer when it is not needed anymore*/
    virtual void detachobserver() = 0;
    /**Notify observer when it is needed*/
    virtual void notifyobserver(const std::string &data) = 0;
};
#endif //DRAWOBSERVABLE_H