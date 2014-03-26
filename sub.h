#ifndef SUB
#define SUB

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
struct Sub {
    map<int, set<int>*> *relations;

    // Constructor
    Sub() {
        relations = new map<int, set<int>*>();
    }

    // Destructor
    ~Sub() {
        map<int, set<int>*>::iterator it;
        for (it = relations->begin(); it != relations->end(); it++)
            delete it->second;
        delete relations;
    }

    // Adds x < y to the map
    void less_than(int x, int y) {
        if (!relations->count(x))
            relations->insert(pair<int, set<int>*>(x, new set<int>()));
        relations->at(x)->insert(y);
    }

    // Returns true if x < y
    bool is_less_than(int x, int y) {
        return relations->at(x)->count(y);
    }

    // Removes x < y from the map
    void not_less_than(int x, int y) {
        relations->at(x) && relations->at(x)->erase(y);
    }
};

#endif // SUB
