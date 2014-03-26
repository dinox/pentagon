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

    //TODO: Implement
    void infer_from_interval(Interval i);

    //TODO: Implement
    void closure(SubDomain *other, SubDomain *dest);

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
private:
    RelationsMap relations_;
};

#endif // SUB_DOMAIN
