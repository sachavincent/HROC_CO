#ifndef CACHE_M_HPP
#define CACHE_M_HPP

#include <set>
#include "observer.hpp"
#include "object.hpp"

class CacheManager : public Observer
{
private:
    CacheManager(){};

public:
    ~CacheManager() override {}

    static CacheManager &getInstance()
    {
        static CacheManager instance;
        return instance;
    }

    inline const std::vector<Object *> &getVisibleObjects() const { return _visibleObjects; }

    void update(Observable *o, bool visible)
    {
        Object *obj = dynamic_cast<Object *>(o);
        if (visible)
            _visibleObjects.insert(obj);
        else
            _visibleObjects.erase(obj);
    }

private:
    std::set<Object *> _visibleObjects;
};
#endif