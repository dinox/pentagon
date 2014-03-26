#ifndef PENTAGON
#include "interval_domain.h"
#include "sub_domain.h"

class Pentagon {
public:
    typedef pair<int, Interval> IntervalPair;

    // Constructor
    Pentagon() {
    }

    // Destructor
    ~Pentagon() {
    }

    //TODO: Think about this. Then implement
    void closure() {
        // Close interval domain by using sub domain
        // ...
        // Infer new sub relations from interval domain
        // ...
        // Close sub domain transitivily
        // ...
    }

    //TODO: Implement
    Pentagon join(Pentagon *other) {
        Pentagon ret;
        return ret;
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
