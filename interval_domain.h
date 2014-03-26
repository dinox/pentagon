#ifndef INTERVAL_DOMAIN
#define INTERVAL_DOMAIN

#include "interval.h"

class IntervalDomain {
public:
    typedef map<int, Interval> IntervalMap;

    IntervalDomain () {
    }

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
