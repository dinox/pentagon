#ifndef INTERVAL_DOMAIN
#define INTERVAL_DOMAIN

#include <map>
#include "interval.h"

using namespace std;

class IntervalDomain 
{
public:
    typedef map<int, Interval> IntervalMap;

    void join(IntervalDomain& other);

    inline Interval getForVar(int var) 
    {
        return var_map_[var];
    }

    void setForVar(int var, Interval i) 
    {
        var_map_[var] = i;
    }

    IntervalMap var_map_;
};

// Interval domain
void IntervalDomain::join(IntervalDomain& other)
{
    // non-mapped variables are assumed to be bottom!
    IntervalMap::iterator ii, jj;

    for (IntervalMap::iterator ii = var_map_.begin(); ii != var_map_.end(); ++ii)
    {
        jj = other.var_map_.find(ii->first);
        if (jj != other.var_map_.end())
            ii->second.join(jj->second);
    }

    for (IntervalMap::iterator ii = other.var_map_.begin(); ii != other.var_map_.end(); ++ii)
    {
        if (var_map_.find(ii->first) == var_map_.end())
            var_map_.insert(*ii);
    }
}

#endif // INTERVAL_DOMAIN
