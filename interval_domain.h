#ifndef INTERVAL_DOMAIN
#define INTERVAL_DOMAIN

#include "interval.h"
#include "sub_domain.h"

class IntervalDomain {
public:
    typedef map<int, Interval> IntervalMap;

    // I use references here because we dont want to copy whole interval domain
    void join(IntervalDomain& other) {
        for (IntervalMap::iterator it = var_map_.begin(); it != var_map_.end(); ++it) {
            it->second = it->second.join(other.getForVar(it->first));
        }
    }

    void closure(const SubDomain& sub) {
        for (IntervalMap::iterator it = var_map_.begin(); it != var_map_.end(); ++it) {
            set<int>& ss = sub.getVarSet(it->first);
            
            for (SubDomain::VariableSet::iterator ii=ss.begin(); ii != ss.end(); ++ii)
                ii->second.assumeLessThan(getForVar(*ii));
        }
    }

    inline Interval getForVar(int var) {
        return var_map_[var];
    }

    void setForVar(int var, Interval i) {
        var_map_[var] = i;
    }
    
    IntervalMap& getIntervalMap() {
        return var_map_;
    }


private:
    IntervalMap var_map_;
};

#endif // INTERVAL_DOMAIN
