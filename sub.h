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
class Sub {
public:
    typedef RelationsMap map<int, set<int>*>

    // Constructor
    Sub() {
        relations_ = new RelationsMap();
    }

    // Destructor
    ~Sub() {
        RelationsMap:iterator it;
        for (it = relations_->begin(); it != relations_->end(); it++)
            delete it->second;
        delete relations_;
    }

    // Adds x < y to the map
    void less_than(int x, int y) {
        if (!relations_->count(x))
            relations_->insert(pair<int, set<int>*>(x, new set<int>()));
        relations_->at(x)->insert(y);
    }

    // Returns true if x < y
    bool is_less_than(int x, int y) {
        return relations_->at(x)->count(y);
    }

    // Removes x < y from the map
    void not_less_than(int x, int y) {
        relations_->at(x) && relations_->at(x)->erase(y);
    }
private:
    map<int, set<int>*> *relations_;
};

#endif // SUB
