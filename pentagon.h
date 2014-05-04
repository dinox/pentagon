#ifndef __PENTAGON__
#define __PENTAGON__

#include <set>
#include "interval.h"

class Pentagon {
public:
    void allocate(int nVars);
    int getNumOfVars();
    // void join(Pentagon& other);

    void setIntervalFor(int var, Interval i);
    void setSubFor(int x, int y);

    Interval getIntervalFor(int var);
    std::set<int> getSubFor(int var);
};

// Assumptions about the Sub domain:
// either i < j or j < i or no info at all

#endif // __PENTAGON__
