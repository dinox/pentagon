#ifndef PENTAGON_DM
#define PENTAGON_DM
#include "interval_domain.h"
#include <new>
#include <stdio.h>

class PentagonDM {
public:
    typedef pair<int, Interval> IntervalPair;

    PentagonDM();
    PentagonDM(IntervalDomain, int);

    void allocate(int num_of_vars);
    void closure();
    void inferSubFromInterval();
    void inferIntervalFromSub();
    void join(PentagonDM& other);

    void subJoin(int*);
    void subClosure();

    void addIntervalFor(int var, Interval i) {
        intervals_.setForVar(var, i);
    }

    void addSubFor(int x, int y) {
        sub_[x * num_of_vars_ + y] = 1;
    }

    int getSubFor(int x, int y) {
        return sub_[x * num_of_vars_ + y];
    }

    IntervalDomain intervals_;
    int* sub_;
    int num_of_vars_;
};

PentagonDM::PentagonDM() {
}

PentagonDM::PentagonDM(IntervalDomain i, int num_of_vars) {
    intervals_ = i;
    allocate(num_of_vars);
}

void PentagonDM::allocate(int num_of_vars) {
    sub_ = new int[num_of_vars * num_of_vars];
    for (int i = 0; i < num_of_vars * num_of_vars; i++) {
        sub_[i] = 0;
    }
    num_of_vars_ = num_of_vars;
}

void PentagonDM::subClosure() {
    for (int i = 0; i < num_of_vars_; i++) {
        for (int j = i+1; j < num_of_vars_; j++) {
            if (sub_[i * num_of_vars_ + j]) {
                for (int k = j+1; k < num_of_vars_; k++) {
                    if (sub_[j * num_of_vars_ + k]) {
                        addSubFor(i, k);
                    }
                }
            }
        }
    }
}

// Requires the domains to have same number of vars
void PentagonDM::subJoin(int* other) {
    for (int i = 0; i < num_of_vars_ * num_of_vars_; i++) {
        sub_[i] &= other[i];
    }
}

// Maybe merge these into closure
void PentagonDM::inferIntervalFromSub()
{
    for (IntervalDomain::IntervalMap::iterator it = intervals_.var_map_.begin(); it != intervals_.var_map_.end(); ++it)
    {
        int* ss = sub_ + it->first * num_of_vars_;

        for (int i = 0; i < num_of_vars_; i++) {
            if (ss[i]) {
                it->second.assumeLessThan(intervals_.var_map_[i]); // x < y
                intervals_.var_map_[i].assumeGreaterThan(it->second); // y > x
            }
        }
    }
}

void PentagonDM::inferSubFromInterval()
{
    IntervalDomain::IntervalMap& m = intervals_.var_map_;

    IntervalDomain::IntervalMap::iterator it1, it2;

    for (it1 = m.begin(); it1 != m.end(); ++it1)
        for (it2 = m.begin(); it2 != m.end(); ++it2) 
            if (it1->second.lessThan(it2->second))
                addSubFor(it1->first, it2->first);
}



// Pentagon domain
void PentagonDM::closure()
{
    inferIntervalFromSub();
    inferSubFromInterval();
    subClosure();
}

void PentagonDM::join(PentagonDM& other) 
{
    // first apply closure
    closure();
    other.closure();
    // then delegate join
    intervals_.join(other.intervals_);
    subJoin(other.sub_);
}

#endif // PENTAGON_DOMAIN
