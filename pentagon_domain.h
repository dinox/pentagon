#ifndef PENTAGON
#include "interval_domain.h"
#include "sub_domain.h"

class Pentagon {
public:
    typedef pair<int, Interval> IntervalPair;

    Pentagon();
    Pentagon(IntervalDomain, SubDomain);

    void closure();
    void inferSubFromInterval();
    void inferIntervalFromSub();
    void join(Pentagon& other);

    void addIntervalFor(int var, Interval i) {
        intervals_.setForVar(var, i);
    }

    void addSubFor(int x, int y) {
        sub_.lessThan(x, y);
    }

    IntervalDomain intervals_;
    SubDomain sub_;
};

Pentagon::Pentagon() {
}

Pentagon::Pentagon(IntervalDomain i, SubDomain s) {
    intervals_ = i;
    sub_ = s;
}

// Maybe merge these into closure
void Pentagon::inferIntervalFromSub()
{
    for (IntervalDomain::IntervalMap::iterator it = intervals_.var_map_.begin(); it != intervals_.var_map_.end(); ++it)
    {
        SubDomain::VariableSet& ss = sub_.relations_[it->first];

        for (SubDomain::VariableSet::iterator ii = ss.begin(); ii != ss.end(); ++ii)
        {
            it->second.assumeLessThan(intervals_.var_map_[*ii]); // x < y
            intervals_.var_map_[*ii].assumeGreaterThan(it->second); // y > x
        }
    }
}

void Pentagon::inferSubFromInterval()
{
    IntervalDomain::IntervalMap& m = intervals_.var_map_;

    IntervalDomain::IntervalMap::iterator it1, it2;

    for (it1 = m.begin(); it1 != m.end(); ++it1)
        for (it2 = m.begin(); it2 != m.end(); ++it2) 
            if (it1->second.lessThan(it2->second))
                sub_.lessThan(it1->first, it2->first);
}



// Pentagon domain
void Pentagon::closure()
{
    inferIntervalFromSub();
    inferSubFromInterval();
    sub_.closure();
}

void Pentagon::join(Pentagon& other) 
{
    // first apply closure
    closure();
    other.closure();
    // then delegate join
    intervals_.join(other.intervals_);
    sub_.join(other.sub_);
}

#endif // PENTAGON_DOMAIN
