#ifndef __PENTAGON_FWT__
#define __PENTAGON_FWT__

#include "interval.h"
#include "pentagon.h"
#include "fwt_kernels.h"
#include <cassert>

#define L1_SIZE (1)

class PentagonFWT : public Pentagon {
public:
    void allocate(int nVars);
    void join(PentagonFWT& other);

    int getNumOfVars()
	{
		return num_of_vars_;
	}

	void setIntervalFor(int var, Interval i)
	{
		intervals_[var] = i;
	}

	void setSubFor(int x, int y)
	{
		sub_[x * num_of_vars_ + y] = 1;
	}

	Interval getIntervalFor(int var)
	{
		return intervals_[var];
	}

	std::set<int> getSubFor(int var);

    bool getSubFor(int x, int y) {
        return !!sub_[x * num_of_vars_ + y];
    }
//private:
    void closure();
    void inferSubFromInterval();
    void inferIntervalFromSub();

    void subJoin(uint8_t*);
    void intervalJoin(Interval* in);
    void subClosure();
	void subLameClosure();

    Interval* intervals_;
    uint8_t* sub_;
    int num_of_vars_;
};


void PentagonFWT::allocate(int num_of_vars)
{
    sub_ = new uint8_t[num_of_vars * num_of_vars];
    for (int i = 0; i < num_of_vars * num_of_vars; i++) {
        sub_[i] = 0;
    }

    intervals_ = new Interval[num_of_vars];

    num_of_vars_ = num_of_vars;
}

std::set<int> PentagonFWT::getSubFor(int var)
{
	std::set<int> r;
	for (int i = 0; i < num_of_vars_; ++i)
		if (sub_[var * num_of_vars_ + i])
			r.insert(i);
	return r;
}

void PentagonFWT::subClosure()
{
    // Delegate to FWT(...)
    //FWI(sub_, sub_, sub_, num_of_vars_, num_of_vars_);
	FWT(sub_, sub_, sub_, num_of_vars_, 8);
}

// Requires the domains to have same number of vars
void PentagonFWT::subJoin(uint8_t* other)
{
    for (int i = 0; i < num_of_vars_ * num_of_vars_; i++) {
        sub_[i] &= other[i];
    }
}

void PentagonFWT::intervalJoin(Interval* in)
{
	for (int i = 0; i < num_of_vars_; i++)
		intervals_[i].join(in[i]);
}

// Maybe merge these into closure
void PentagonFWT::inferIntervalFromSub()
{
	int i,j;
	for (i=0; i<num_of_vars_; ++i)
		for (j=0; j<num_of_vars_; ++j)
			if (sub_[i * num_of_vars_ + j]) {
				intervals_[i].assumeLessThan(intervals_[j]); // <
				intervals_[j].assumeGreaterThan(intervals_[i]); // >
			}
}

void PentagonFWT::inferSubFromInterval()
{
    int i,j;
	for (i=0; i<num_of_vars_; ++i)
		for (j=0; j<num_of_vars_; ++j)
			if (intervals_[i].lessThan(intervals_[j]))
				sub_[i * num_of_vars_ + j] = 1;
}

// Pentagon domain
void PentagonFWT::closure()
{
    inferIntervalFromSub();
    inferSubFromInterval();
    subClosure();
}

void PentagonFWT::join(PentagonFWT& other) 
{
	assert(&other != this);
	assert(other.num_of_vars_ == num_of_vars_);
    // first apply closure
    closure();
    other.closure();
    // then delegate join
    intervalJoin(other.intervals_);
    subJoin(other.sub_);
}


#endif // __PENTAGON_FWT__
