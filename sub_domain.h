#ifndef SUB_DOMAIN
#define SUB_DOMAIN

#include <map>
#include <set>

using namespace std;

class IntervalDomain;

/*
 * SUB class
 * All variables are indexed by an int
 * In the methods we name parameters so that x < y
 * TODO: Maybe also store y > x so we can get that in
 *       an efficient way.
 */
class SubDomain {
    friend class IntervalDomain;
public:
    typedef map<int, set<int> > RelationsMap;
    typedef set<int> VariableSet;
    typedef pair<int, VariableSet> VariableSetPair;

    // Destructor
    // I don't think that we need a destructor here

    void inferFromInterval(IntervalDomain& i);
    
    void closure();

    void join(SubDomain& other);

    // Adds x < y to the map
    void lessThan(int x, int y)
    {
        if (!relations_.count(x))
            relations_.insert(VariableSetPair(x, VariableSet()));
        relations_.at(x).insert(y);
    }

    // Returns true if x < y
    bool isLessThan(int x, int y) 
    {
        return relations_.at(x).count(y);
    }

    // Removes x < y from the map
    void notLessThan(int x, int y)
    {
        if (relations_.count(x))
            relations_.at(x).erase(y);
    }
    
    VariableSet& getVarSet(int var)
    {
        return relations_[var];
    }
    
private:
    RelationsMap relations_;
};

#endif // SUB_DOMAIN
