#ifndef PENTAGON
#include "interval_domain.h"
#include "sub_domain.h"

class Pentagon {
public:
    typedef pair<int, Interval> IntervalPair;

    void closure();

    void join(Pentagon& other);
    
    void addIntervalFor(int var, Interval i) {
        intervals_.setForVar(var, i);
    }

    void addSubFor(int x, int y) {
        sub_.lessThan(x, y);
    }
private:
    IntervalDomain intervals_;
    SubDomain sub_;
};

#endif // PENTAGON_DOMAIN
