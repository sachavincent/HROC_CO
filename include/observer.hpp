#ifndef OBSERVER_HPP
#define OBSERVER_HPP

class Observable;

class Observer
{
public:
    virtual ~Observer() {}

    virtual void update(Observable* o, bool visible) = 0;
};
#endif