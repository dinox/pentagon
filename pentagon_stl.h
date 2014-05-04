#ifndef __PENTAGON_STL__
#define __PENTAGON_STL__

#include <algorithm>
#include <map>
#include <set>
#include "pentagon.h"

using namespace std;

/*
 * SUB class
 * All variables are indexed by an int
 * In the methods we name parameters so that x < y
 */

class SubDomain
{
public:
    typedef map<int, set<int> > RelationsMap;
    typedef set<int> VariableSet;
    typedef pair<int, VariableSet> VariableSetPair;

    void closure()
    {
    	// Floyd-Warshall
    	for (VariableSet::iterator k = allVars.begin(); k != allVars.end(); ++k) {
    		VariableSet& iset = backward_rel[*k];
    		VariableSet& jset = forward_rel[*k];
    		for (VariableSet::iterator i = iset.begin(); i != iset.end(); ++i)
    			for (VariableSet::iterator j = jset.begin(); j != jset.end(); ++j) {
    				forward_rel[*i].insert(*j);
    				backward_rel[*j].insert(*i);
    			}
    	}
	}

    void join(SubDomain& other)
    {
    	for (RelationsMap::iterator it = forward_rel.begin(); it != forward_rel.end(); ++it) {
			VariableSet& o = other.forward_rel[it->first];
			VariableSet tmp;
			set_intersection(it->second.begin(), it->second.end(), o.begin(), o.end(), std::inserter(tmp, tmp.begin()));
			it->second = tmp;
    	}

    	for (RelationsMap::iterator it = backward_rel.begin(); it != backward_rel.end(); ++it) {
			VariableSet& o = other.backward_rel[it->first];
			VariableSet tmp;
			set_intersection(it->second.begin(), it->second.end(), o.begin(), o.end(), std::inserter(tmp, tmp.begin()));
			it->second = tmp;
		}
/*
    	for (RelationsMap::iterator it = backward_rel.begin(); it != backward_rel.end(); ++it) {
			if (other.backward_rel.find(it->first) == other.backward_rel.end()) {
				backward_rel.erase(it);
				allVars.erase(it->first);
			} else {
				VariableSet& o = other.backward_rel[it->first];
				VariableSet tmp;
				set_intersection(it->second.begin(), it->second.end(), o.begin(), o.end(), std::inserter(tmp, tmp.begin()));
				it->second = tmp;
			}
		}
*/
    }

    // Adds x < y to the map
    void lessThan(int x, int y)
    {
        forward_rel[x].insert(y);
        backward_rel[y].insert(x);

        allVars.insert(x);
        allVars.insert(y);
    }

    // Returns true if x < y
    inline bool isLessThan(int x, int y)
    {
        return forward_rel[x].count(y) > 0;
    }

    RelationsMap forward_rel, backward_rel;
    VariableSet allVars;
};

class PentagonSTL : public Pentagon
{
public:
	typedef pair<int, Interval> IntervalPair;
	typedef map<int, Interval> IntervalMap;

    void allocate(int nVars) {
    	num_of_vars_ = nVars;
    }

	int getNumOfVars() {
		return num_of_vars_;
	}

	void setIntervalFor(int var, Interval i) {
		intervals_[var] = i;
	}
	void setSubFor(int x, int y) {
		sub_.lessThan(x, y);
	}

	Interval getIntervalFor(int var) {
		return intervals_[var];
	}
	std::set<int> getSubFor(int var) {
		return sub_.forward_rel[var];
	}

	void join(PentagonSTL& other);

private:
    void closure();
	void inferSubFromInterval();
	void inferIntervalFromSub();

	void intervalJoin(IntervalMap& other);

	IntervalMap intervals_;
	SubDomain sub_;
	int num_of_vars_;
};

void PentagonSTL::inferIntervalFromSub()
{
	for (SubDomain::RelationsMap::iterator it = sub_.forward_rel.begin(); it != sub_.forward_rel.end(); ++it) {
		for (SubDomain::VariableSet::iterator ii = it->second.begin(); ii != it->second.end(); ++ii) {
			intervals_[it->first].assumeLessThan(intervals_[*ii]);
			intervals_[*ii].assumeGreaterThan(intervals_[it->first]);
		}
	}

	//these two should be equivalent

	/*
    for (IntervalMap::iterator it = intervals_.begin(); it != intervals_.end(); ++it) {
        SubDomain::VariableSet& ss = sub_.forward_rel[it->first];

        for (SubDomain::VariableSet::iterator ii = ss.begin(); ii != ss.end(); ++ii) {
            it->second.assumeLessThan(intervals_[*ii]); // x < y
            intervals_[*ii].assumeGreaterThan(it->second); // y > x
        }
    }
    */

}

void PentagonSTL::inferSubFromInterval()
{
    IntervalMap::iterator it1, it2;

    for (it1 = intervals_.begin(); it1 != intervals_.end(); ++it1)
        for (it2 = intervals_.begin(); it2 != intervals_.end(); ++it2)
            if (it1->second.lessThan(it2->second))
                sub_.lessThan(it1->first, it2->first);
}

void PentagonSTL::intervalJoin(IntervalMap& other)
{
    // non-mapped variables are assumed to be bottom!
    IntervalMap::iterator ii, jj;

    for (IntervalMap::iterator ii = intervals_.begin(); ii != intervals_.end(); ++ii)
    {
        jj = other.find(ii->first);
        if (jj != other.end())
            ii->second.join(jj->second);
    }

    for (IntervalMap::iterator ii = other.begin(); ii != other.end(); ++ii)
    {
        if (intervals_.find(ii->first) == intervals_.end())
        	intervals_.insert(*ii);
    }
}

void PentagonSTL::closure()
{
    inferIntervalFromSub();
    inferSubFromInterval();
    sub_.closure();
}

void PentagonSTL::join(PentagonSTL& other)
{
    // first apply closure
    closure();
    other.closure();
    // then delegate join
    intervalJoin(other.intervals_);
    sub_.join(other.sub_);
}

#endif
