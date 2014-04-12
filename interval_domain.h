#ifndef INTERVAL_DOMAIN
#define INTERVAL_DOMAIN

#include <map>
#include "interval.h"

using namespace std;

class SubDomain;

class IntervalDomain 
{
    friend class SubDomain;
public:
    typedef map<int, Interval> IntervalMap;

    void join(IntervalDomain& other);

    void inferFromSub(SubDomain& sub);

    inline Interval getForVar(int var) 
    {
        return var_map_[var];
    }

    void setForVar(int var, Interval i) 
    {
        var_map_[var] = i;
    }
    
private:
    IntervalMap var_map_;
};

#endif // INTERVAL_DOMAIN
