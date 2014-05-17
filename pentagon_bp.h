#ifndef __PENTAGON_BP__
#define __PENTAGON_BP__

#include "pentagon_fwt.h"

class PentagonBP : public PentagonFWT {
public:
    void allocate(int nVars);
    void join(PentagonBP& other);

    int getNumOfVars() {
		return num_of_vars_;
	}

	void setIntervalFor(int var, Interval i) {
		intervals_[var] = i;
	}

	void setSubFor(int x, int y) {
		sub_[x * cols_ + (y / SUB_BITS)] |= (1 << (y % SUB_BITS));
	}

    bool getSubFor(int x, int y) {
        return ((sub_[(x * cols_ + (y / SUB_BITS))] >> (y % SUB_BITS)) & 1);
    	//return (sub_[(x * cols_ + (y / SUB_BITS))] & (1 << (y % SUB_BITS))); // erik's variant works too :)
    }

	Interval getIntervalFor(int var) {
		return intervals_[var];
	}

	std::set<int> getSubFor(int var) {
		std::set<int> r;
		for (int i=0;i<num_of_vars_;++i)
			if (getSubFor(var, i))
				r.insert(i);
		return r;
	}

	void FWI(SUB_TYPE* a, SUB_TYPE* b, SUB_TYPE* c, int n, int cols);
	void FWIabc(SUB_TYPE* a, SUB_TYPE* b, SUB_TYPE* c, int n, int cols);
	void FWT(SUB_TYPE* a, int n);

private:
    void closure();
    void inferSubFromInterval();
    void inferIntervalFromSub();

    void subJoin(SUB_TYPE*);
    void intervalJoin(Interval* in);
    void subClosure();

    Interval* intervals_;
    SUB_TYPE* sub_;
    int num_of_vars_;
    int cols_;
};

int round2pow(int v) {
    //TODO: This only works for 8 bit
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

void PentagonBP::FWI(SUB_TYPE* a, SUB_TYPE* b, SUB_TYPE* c, int n, int cols) {
	int inner_cols = n / SUB_BITS;
	int k, i, j, i1, j1;
	int t;
	for (k = 0; k < n; ++k) {
		for (i = 0; i < n; i += UI) {
			for (j = 0; j < inner_cols; j += UJ) {
				for (i1 = i; i1 < i+UI; ++i1) {
					t = EXPAND_LOWEST_BIT(a[i1 * cols + (k / SUB_BITS)] >> (k % SUB_BITS));
					for (j1 = j; j1 < j+UJ; ++j1) {
						c[i1 * cols + j1] |= t & b[k * cols + j1];
					}
				}
			}
		}
	}
}

void PentagonBP::FWIabc(SUB_TYPE*__restrict__ a, SUB_TYPE*__restrict__ b, SUB_TYPE*__restrict__ c, int n, int cols) {
	assert( (a != b) && (a != c) );
	int inner_cols = n / SUB_BITS;
	int i, j, k, i1, j1, k1;
	int t;
	for (i = 0; i < n; i += UI) {
		for (j = 0; j < inner_cols; j += UJ) {
			for (k = 0; k < n; k += UK) {
				for (i1 = i; i1 < i+UI; ++i1) {
					for (j1 = j; j1 < j+UJ; ++j1) {
						for (k1 = k; k1 < k+UK; ++k1) {
							t = EXPAND_LOWEST_BIT(a[i1 * cols + (k1 / SUB_BITS)] >> (k1 % SUB_BITS));
							c[i1 * cols + j1] |= t & b[k1 * cols + j1];
						}
					}
				}
			}
		}
	}
}

void PentagonBP::FWT(SUB_TYPE* a, int n) {
	int k, i, j;
    int M = n / L1_SIZE;

    int cols = n / SUB_BITS;
    int skip = L1_SIZE / SUB_BITS;

    for (k = 0; k < M; k++) {
		// phase 1
    	FWI(a + k*L1_SIZE*cols + k*skip, a + k*L1_SIZE*cols + k*skip, a + k*L1_SIZE*cols + k*skip, L1_SIZE, cols);
    	//FWI(a+L1_SIZE*(k*n+k), a+L1_SIZE*(k*n+k), a+L1_SIZE*(k*n+k), L1_SIZE, n);

		// phase 2
		for (j = 0; j < M; j++) {
			if (j != k) {
				FWI(a + k*L1_SIZE*cols + k*skip, a + k*L1_SIZE*cols + j*skip, a + k*L1_SIZE*cols + j*skip, L1_SIZE, cols);
				//FWI(a+L1_SIZE*(k*n+k), a+L1_SIZE*(k*n+j), a+L1_SIZE*(k*n+j), L1_SIZE, n);
			}
		}

		// phase 3
		for (i = 0; i < M; i++) {
			if (i != k) {
				FWI(a + i*L1_SIZE*cols + k*skip, a + k*L1_SIZE*cols + k*skip, a + i*L1_SIZE*cols + k*skip, L1_SIZE, cols);
				//FWI(a+L1_SIZE*(i*n+k), a+L1_SIZE*(k*n+k), a+L1_SIZE*(i*n+k), L1_SIZE, n);
			}
		}

		// phase 4
		for (int i = 0; i < M; i++) {
			if (i != k) {
				for (int j = 0; j < M; j++) {
					if (j != k) {
						FWIabc(a + i*L1_SIZE*cols + k*skip, a + k*L1_SIZE*cols + j*skip, a + i*L1_SIZE*cols + j*skip, L1_SIZE, cols);
						//FWIabc(a+L1_SIZE*(i*n+k), a+L1_SIZE*(k*n+j), a+L1_SIZE*(i*n+j), L1_SIZE, n);
					}
				}
			}
		}
	}
}

void PentagonBP::allocate(int num_of_vars)
{
    //num_of_vars = round2pow(num_of_vars);

	assert( num_of_vars % L1_SIZE == 0 );
	assert( L1_SIZE % (SUB_BITS * UJ) == 0 );

	num_of_vars_ = num_of_vars;
	cols_ = num_of_vars / SUB_BITS;

    sub_ = new SUB_TYPE[num_of_vars * cols_];
    for (int i = 0; i < num_of_vars * cols_; i++) {
        sub_[i] = 0;
    }

    intervals_ = new Interval[num_of_vars];


}

void PentagonBP::subClosure()
{
    // Delegate to FWT(...)
    //FWI(sub_, sub_, sub_, num_of_vars_, cols_);
	FWT(sub_, num_of_vars_);
}

// Requires the domains to have same number of vars
void PentagonBP::subJoin(SUB_TYPE* other)
{
    for (int i = 0; i < num_of_vars_ * cols_; i++) {
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
