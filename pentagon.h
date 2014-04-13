#include "pentagon_domain.h"

// Interval domain
void IntervalDomain::join(IntervalDomain& other)
{
    // non-mapped variables are assumed to be bottom!
    IntervalMap::iterator ii, jj;
    
    for (IntervalMap::iterator ii = var_map_.begin(); ii != var_map_.end(); ++ii)
    {
        jj = other.var_map_.find(ii->first);
        if (jj != other.var_map_.end())
            ii->second.join(jj->second);
    }
    
    for (IntervalMap::iterator ii = other.var_map_.begin(); ii != other.var_map_.end(); ++ii)
    {
        if (var_map_.find(ii->first) == var_map_.end())
            var_map_.insert(*ii);
    }
}

void IntervalDomain::inferFromSub(SubDomain& sub)
{
    for (IntervalMap::iterator it = var_map_.begin(); it != var_map_.end(); ++it)
    {
        SubDomain::VariableSet& ss = sub.relations_[it->first];
        
        for (SubDomain::VariableSet::iterator ii = ss.begin(); ii != ss.end(); ++ii)
        {
            it->second.assumeLessThan(var_map_[*ii]); // x < y
            var_map_[*ii].assumeGreaterThan(it->second); // y > x
        }
    }
}

// Sub domain
void SubDomain::inferFromInterval(IntervalDomain& i)
{
    IntervalDomain::IntervalMap& m = i.var_map_;
    
    IntervalDomain::IntervalMap::iterator it1, it2;
    
    for (it1 = m.begin(); it1 != m.end(); ++it1)
        for (it2 = m.begin(); it2 != m.end(); ++it2) 
            if (it1->second.lessThan(it2->second))
                lessThan(it1->first, it2->first);
}

void SubDomain::closure()
{
    for (RelationsMap::iterator it = relations_.begin(); it != relations_.end(); ++it)
        for (VariableSet::iterator i = it->second.begin(); i != it->second.end(); ++i)
        {
            VariableSet& ss = getVarSet(*i);
            for (VariableSet::iterator j = ss.begin(); j != ss.end(); ++j)
                lessThan(it->first, *j);
        }
}

void SubDomain::join(SubDomain& other)
{
    for (RelationsMap::iterator it = relations_.begin(); it != relations_.end(); ++it)
    {
        VariableSet& o = other.getVarSet(it->first);
        it->second.insert(o.begin(), o.end());
    }
    
    for (RelationsMap::iterator it = other.relations_.begin(); it != other.relations_.end(); ++it)
    {
        VariableSet& o = getVarSet(it->first);
        o.insert(it->second.begin(), it->second.end());
    }
}

// Pentagon domain
void Pentagon::closure()
{
    intervals_.inferFromSub(sub_);
    sub_.inferFromInterval(intervals_);
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
