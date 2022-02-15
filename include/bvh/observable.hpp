#ifndef OBSERVABLE_HPP
#define OBSERVABLE_HPP

#include "observer.hpp"

class Observable
{
public:
    virtual ~Observable() {}

    virtual void registerObserver(Observer& o) = 0;
    virtual void removeObserver(Observer& o) = 0;
    virtual void notifyObservers() = 0;
};
#endif