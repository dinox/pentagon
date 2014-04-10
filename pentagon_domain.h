#ifndef PENTAGON
#include "interval_domain.h"
#include "sub_domain.h"

class Pentagon {
public:
    typedef pair<int, Interval> IntervalPair;

    void closure() {
        intervals_.closure(sub_);
        sub_.inferFromInterval(intervals_);
        sub_.closure();
    }

    void join(const Pentagon& other) {
        closure();
        other.closure();
        intervals_.join(other.intervals_);
        sub_.join(other.sub_);
    }
    
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
