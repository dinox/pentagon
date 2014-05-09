#ifndef __PENTAGON_BP__
#define __PENTAGON_BP__

#include "interval.h"
#include "pentagon.h"
#include "pentagon_fwt.h"
#include "macros.h"
#include <cassert>
#include <inttypes.h>
#include <stdint.h>

class PentagonBP : public PentagonFWT {
public:
    void allocate(int nVars);
    void join(PentagonBP& other);

    int getNumOfVars()
	{
		return num_of_vars_;
	}

	void setIntervalFor(int var, Interval i)
	{
		intervals_[var] = i;
	}

	void setSubFor(int x, int y) {
		sub_[x * num_of_vars_ + (y / SUB_BITS)] |= 1 << (y % SUB_BITS);
	}

    bool getSubFor(int x, int y) {
        return sub_[(x * num_of_vars_ + (y / SUB_BITS))] & (1 << (y % SUB_BITS));
    }

	Interval getIntervalFor(int var)
	{
		return intervals_[var];
	}

	std::set<int> getSubFor(int var) {
		std::set<int> r;
		for (int i=0;i<num_of_vars_;++i)
			if (getSubFor(var, i))
				r.insert(i);
		return r;
	}

	void FWI(uint8_t* a, uint8_t* b, uint8_t* c, int n, int cols);
	void FWIabc(uint8_t* a, uint8_t* b, uint8_t* c, int n, int cols);
	void FWT(uint8_t* a, uint8_t* b, uint8_t* c, int n, int L1);

//private:
    void closure();
    void inferSubFromInterval();
    void inferIntervalFromSub();

    void subJoin(SUB_TYPE*);
    void intervalJoin(Interval* in);
    void subClosure();
	void subLameClosure();

    Interval* intervals_;
    SUB_TYPE* sub_;
    int num_of_vars_;
};

int round2pow(int v) {
    //TODO: This only works for 8 bit
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v++;
    return v;
}

void PentagonBP::FWI(uint8_t* a, uint8_t* b, uint8_t* c, int n, int cols) {
    int k, i, j, i1, j1;
    for (k = 0; k < n; k++) {
        for (i = 0; i < n; i += UI) {
            for (j = 0; j < n / SUB_BITS; j += UJ) {
                for (i1 = i; i1 < i+UI; i1++) {
                    for (j1 = j; j1 < j+UJ; j1++) {
                         c[i1 * cols + j1] |=
                             a[i1 * cols + k] &
                             b[k * cols + j1];
                    }
                }
            }
        }
    }
}

void PentagonBP::FWIabc(uint8_t* a, uint8_t* b, uint8_t* c, int n, int cols) {
    int k, i, j, i1, j1, k1;
    for (j = 0; j < n; j += UJ) {
        for (i = 0; i < n; i += UI) {
            for (k = 0; k < n; k += UK) {
                for (k1 = k; k1 < k+UI; k1++) {
                    for (i1 = i; i1 < i+UI; i1++) {
                        for (j1 = j; j1 < j+SUB_BITS; j1++) {
                            c[i1 * cols + (j1 / SUB_BITS)] |=
                                a[i1 * cols + k] &
                                b[k * cols + (j1 / SUB_BITS)];
                        }
                    }
                }
            }
        }
    }
}

void PentagonBP::FWT(uint8_t* a, uint8_t* b, uint8_t* c, int n, int L1) {
    int k, i, j;
    int M = n / L1;

    for (k = 0; k < M; k++) {
        // phase 1
        // TODO: Make sure you can do like this (probably not)
        FWI(a+k*n+k, b+k*n+k, c+k*n+k, L1, n);

        // phase 2
        for (j = 0; j < M; j++) {
            if (j != k) {
                FWI(a+L1*(k*n+k), b+L1*(k*n+j), c+L1*(k*n+j), L1, n);
            }
        }

        // phase 3
        for (i = 0; i < M; i++) {
            if (i != k) {
                FWI(a+L1*(i*n+k), b+L1*(k*n+k), c+L1*(i*n+k), L1, n);
            }
        }

        // phase 4
        for (int i = 0; i < M; i++) {
            if (i != k) {
                for (int j = 0; j < M; j++) {
                    if (j != k) {
                        FWIabc(a+L1*(i*n+k), b+L1*(k*n+j), c+L1*(i*n+j), L1, n);
                    }
                }
            }
        }
    }
}

void PentagonBP::allocate(int num_of_vars)
{
    num_of_vars = round2pow(num_of_vars);
    sub_ = new SUB_TYPE[num_of_vars * num_of_vars / SUB_BITS];
    for (int i = 0; i < num_of_vars * num_of_vars / SUB_BITS; i++) {
        sub_[i] = 0;
    }

    intervals_ = new Interval[num_of_vars];

    num_of_vars_ = num_of_vars;
}

void PentagonBP::subClosure()
{
    // Delegate to FWT(...)
    FWI(sub_, sub_, sub_, num_of_vars_, num_of_vars_);
}

// Requires the domains to have same number of vars
void PentagonBP::subJoin(SUB_TYPE* other)
{
    for (int i = 0; i < num_of_vars_ * num_of_vars_ / SUB_BITS; i++) {
        sub_[i] &= other[i];
    }
}

void PentagonBP::intervalJoin(Interval* in)
{
	for (int i = 0; i < num_of_vars_; i++)
		intervals_[i].join(in[i]);
}

// Maybe merge these into closure
void PentagonBP::inferIntervalFromSub()
{
	int i,j;
	for (i=0; i<num_of_vars_; ++i)
		for (j=0; j<num_of_vars_; ++j)
			if (getSubFor(i,j)) {
				intervals_[i].assumeLessThan(intervals_[j]); // <
				intervals_[j].assumeGreaterThan(intervals_[i]); // >
			}
}

void PentagonBP::inferSubFromInterval()
{
    int i,j;
	for (i=0; i<num_of_vars_; ++i)
		for (j=0; j<num_of_vars_; ++j)
			if (intervals_[i].lessThan(intervals_[j]))
				setSubFor(i, j);
}

// Pentagon domain
void PentagonBP::closure()
{
    inferIntervalFromSub();
    inferSubFromInterval();
    subClosure();
}

void PentagonBP::join(PentagonBP& other) 
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


#endif // __PENTAGON_BP__
