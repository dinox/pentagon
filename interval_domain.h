#ifndef INTERVAL_DOMAIN
#define INTERVAL_DOMAIN

#include "interval.h"
#include "sub_domain.h"

class IntervalDomain {
public:
    typedef map<int, Interval> IntervalMap;

    // I use references here because we dont want to copy whole interval domain
    //TODO: Implement
    void join(IntervalDomain *other, IntervalDomain *dest);

    //TODO: Implement
    void closure(SubDomain sub);

    Interval getForVar(int var) {
        return var_map_[var];
    }

    void setForVar(int var, Interval i) {
        var_map_[var] = i;
    }


private:
    IntervalMap var_map_;
};

#endif // INTERVAL_DOMAIN
