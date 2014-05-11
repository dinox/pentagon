#ifndef __PENTAGON_FWT__
#define __PENTAGON_FWT__

#include "pentagon_dense.h"
#include <cassert>
#include <stdint.h>

class PentagonFWT : public PentagonDM {
public:
    void allocate(int nVars);
    void join(PentagonFWT& other);

    inline int getNumOfVars()
	{
		return num_of_vars_;
	}

    inline void setIntervalFor(int var, Interval i)
	{
		intervals_[var] = i;
	}

	inline void setSubFor(int x, int y)
	{
		sub_[x * num_of_vars_ + y] = 1;
	}

	inline Interval getIntervalFor(int var)
	{
		return intervals_[var];
	}

	std::set<int> getSubFor(int var);

    inline bool getSubFor(int x, int y) {
        return !!sub_[x * num_of_vars_ + y];
    }

	void FWI(uint8_t* a, uint8_t* b, uint8_t* c, int n, int cols);
	void FWIabc(uint8_t* a, uint8_t* b, uint8_t* c, int n, int cols);
	void FWT(uint8_t* a, int n);

private:
    void closure();
    void inferSubFromInterval();
    void inferIntervalFromSub();

    void subJoin(uint8_t*);
    void intervalJoin(Interval* in);
    void subClosure();

    Interval* intervals_;
    uint8_t* sub_;
    int num_of_vars_;
};

void PentagonFWT::FWI(uint8_t* a, uint8_t* b, uint8_t* c, int n, int cols) {
    int k, i, j, i1, j1;
    for (k = 0; k < n; k++) {
        for (i = 0; i < n; i += UI) {
            for (j = 0; j < n; j += UJ) {
                for (i1 = i; i1 < i+UI; i1++) {
                    for (j1 = j; j1 < j+UJ; j1++) {
                        c[i1*cols + j1] = c[i1*cols + j1] || (a[i1*cols + k] && b[k*cols + j1]);
                    }
                }
            }
        }
    }
}

void PentagonFWT::FWIabc(uint8_t* a, uint8_t* b, uint8_t* c, int n, int cols) {
    int k, i, j, i1, j1, k1;
    for (i = 0; i < n; i += UI) {
    	for (j = 0; j < n; j += UJ) {
            for (k = 0; k < n; k += UK) {
                for (k1 = k; k1 < k+UK; k1++) {
                    for (i1 = i; i1 < i+UI; i1++) {
                        for (j1 = j; j1 < j+UJ; j1++) {
                            c[i1*cols + j1] = c[i1*cols + j1] || (a[i1*cols + k1] && b[k1*cols + j1]);
                        }
                    }
                }
            }
        }
    }
}

void PentagonFWT::FWT(uint8_t* a, int n) {
    int k, i, j;
    int M = n / L1_SIZE;

    for (k = 0; k < M; k++) {
        // phase 1
        FWI(a+L1_SIZE*(k*n+k), a+L1_SIZE*(k*n+k), a+L1_SIZE*(k*n+k), L1_SIZE, n);

        // phase 2
        for (j = 0; j < M; j++) {
            if (j != k) {
                FWI(a+L1_SIZE*(k*n+k), a+L1_SIZE*(k*n+j), a+L1_SIZE*(k*n+j), L1_SIZE, n);
            }
        }

        // phase 3
        for (i = 0; i < M; i++) {
            if (i != k) {
                FWI(a+L1_SIZE*(i*n+k), a+L1_SIZE*(k*n+k), a+L1_SIZE*(i*n+k), L1_SIZE, n);
            }
        }

        // phase 4
        for (int i = 0; i < M; i++) {
            if (i != k) {
                for (int j = 0; j < M; j++) {
                    if (j != k) {
                        FWIabc(a+L1_SIZE*(i*n+k), a+L1_SIZE*(k*n+j), a+L1_SIZE*(i*n+j), L1_SIZE, n);
                    }
                }
            }
        }
    }
}


void PentagonFWT::allocate(int num_of_vars)
{
	assert(num_of_vars % L1_SIZE == 0);
	assert(L1_SIZE % UI == 0);
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
	FWT(sub_, num_of_vars_);
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
