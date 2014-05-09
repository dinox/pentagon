#ifndef __PENTAGON__
#define __PENTAGON__

#include <set>
#include "interval.h"

class Pentagon
{
public:
    void allocate(int nVars);

    void setIntervalFor(int var, Interval i);
    void setSubFor(int x, int y);

    virtual int getNumOfVars() = 0;
    virtual Interval getIntervalFor(int var) = 0;
    virtual std::set<int> getSubFor(int var) = 0;
};

// Assumptions about the Sub domain:
// either i < j or j < i or no info at all

#endif // __PENTAGON__
