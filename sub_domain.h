#ifndef SUB_DOMAIN
#define SUB_DOMAIN

#include<map>
#include<set>

using namespace std;

/*
 * SUB class
 * All variables are indexed by an int
 * In the methods we name parameters so that x < y
 * TODO: Maybe also store y > x so we can get that in
 *       an efficient way.
 */
class SubDomain {
public:
    typedef map<int, set<int> > RelationsMap;
    typedef set<int> VariableSet;
    typedef pair<int, VariableSet> VariableSetPair;

    // Destructor
    ~SubDomain() {
        //RelationsMap::iterator it;
        //for (it = relations_.begin(); it != relations_.end(); it++)
        //    delete it->second;
    }

    void inferFromInterval(const Interval& i) {
        IntervalDomain::IntervalMap& m = i.getIntervalMap();
        
        IntervalDomain::IntervalMap::iterator it1, it2;
        
        for (it1 = m.begin(); it1 != m.end(); ++it1)
            for (it2 = m.begin(); it2 != m.end(); ++it2) 
                if (it1->second.lessThan(it2->second)) {
                    lessThan(it1->first, it2->first);
                }
    }
    
    void closure() {
        for (RelationsMap::iterator it = relations_.begin(); it != relations_.end(); ++it) {
            for (set<int>::iterator i = it->second.begin(); i != it->second.end(); ++i) {
                set<int>& ss = getVarSet(*i);
                for (set<int>::iterator j = ss.begin(); i != ss.end(); ++j)
                    lessThan(it->first, *j);
            }
        }
    }

    void join(const SubDomain& other) {
        
        for (RelationsMap::iterator it = relations_.begin(); it != relations_.end(); ++it) {
            set<int>& o = other.getVarSet(it->first);
            it->second.insert(o.begin(), o.end());
        }
        
        for (RelationsMap::iterator it = other.relations_.begin(); it != other.relations_.end(); ++it) {
            set<int>& o = getVarSet(it->first);
            o.insert(it->second.begin(), it->second.end());
        }
    }

    // Adds x < y to the map
    void lessThan(int x, int y) {
        if (!relations_.count(x))
            relations_.insert(VariableSetPair(x, VariableSet()));
        relations_.at(x).insert(y);
    }

    // Returns true if x < y
    bool isLessThan(int x, int y) {
        return relations_.at(x).count(y);
    }

    // Removes x < y from the map
    void notLessThan(int x, int y) {
        if (relations_.count(x))
            relations_.at(x).erase(y);
    }
    
    set<int>& getVarSet(int var) {
        return &relations_[var];
    }
private:
    RelationsMap relations_;
};

#endif // SUB_DOMAIN
