#ifndef __PENTAGON_SIMD__
#define __PENTAGON_SIMD__

#include "pentagon_fwt.h"

class PentagonSIMD : public PentagonFWT {
public:
    void allocate(int nVars);
    void join(PentagonSIMD& other);

    int getNumOfVars() {
		return num_of_vars_;
	}

	void setIntervalFor(int var, Interval i) {
		intervals_[var] = i;
	}

	void setSubFor(int x, int y) {
		ss[x * cols_ * SIMD_INT_COUNT + (y / SUB_BITS)] |= (1 << (y % SUB_BITS));
	}

    bool getSubFor(int x, int y) {
    	return ((ss[(x * cols_ * SIMD_INT_COUNT + (y / SUB_BITS))] >> (y % SUB_BITS)) & 1);
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

	void FWI(SIMD_TYPE* a, SIMD_TYPE* b, SIMD_TYPE* c, int n, int cols);
	void FWIabc(SIMD_TYPE* a, SIMD_TYPE* b, SIMD_TYPE* c, int n, int cols);
	void FWT(SIMD_TYPE* a, int n);

//private:
    void closure();
    void inferSubFromInterval();
    void inferIntervalFromSub();

    void subJoin(SIMD_TYPE*);
    void intervalJoin(Interval* in);
    void subClosure();

    SIMD_INT_TYPE* ss;
    Interval* intervals_;
    SIMD_TYPE* sub_;
    int num_of_vars_;
    int cols_;
};

void PentagonSIMD::FWI(SIMD_TYPE* a, SIMD_TYPE* b, SIMD_TYPE* c, int n, int cols) {
	int inner_cols = n / SIMD_BITS;
	int k, i, j, i1, j1;
    // 2*N^3 OPS
	for (k = 0; k < n; ++k) {
		for (i = 0; i < n; i += UI) {
			for (j = 0; j < inner_cols; j += UJ) {
#ifdef AVX
                for (i1 = i; i1 < i+UI; ++i1) {
					SIMD_INT_TYPE a_i1_k = ((SIMD_INT_TYPE*)a)[i1 * cols * SIMD_INT_COUNT + (k / SIMD_INT_BITS)];
					SIMD_INT_TYPE bitmask = ((a_i1_k >> (k % SIMD_INT_BITS)) & 1) * (-1);
                    SIMD_E_TYPE mask = SIMD_E_SET_ALL(bitmask);
					for (j1 = j; j1 < j+UJ; j1 += 2) {
                        SIMD_E_TYPE *b_e = (SIMD_E_TYPE*) b,
                                    *c_e = (SIMD_E_TYPE*) c;
                        SIMD_E_TYPE rh = SIMD_E_AND(mask, b_e[(k * cols + j1) / 2]);
                        SIMD_E_TYPE c_e_i1_j1 = c_e[(i1 * cols + j1) / 2];
						c_e[(i1 * cols + j1) / 2] = SIMD_E_OR(c_e_i1_j1, rh);
					}
                }
#else

#ifdef SIMD_UNROLL_FWI // unroll UJ by 4 and UI by 2

                int addr_a, addr_b, addr_c0, addr_c1;
                SIMD_INT_TYPE tmp0, tmp1;
                SIMD_TYPE a0, a1;
                SIMD_TYPE b0, b1, b2, b3;
                SIMD_TYPE c00, c01, c02, c03, c10, c11, c12, c13;

                for (i1 = i; i1 < i+UI; i1 += 2) {
                	// load A
                	addr_a = i1 * cols * SIMD_INT_COUNT + (k / SIMD_INT_BITS);
                	tmp0 = ((SIMD_INT_TYPE*)a)[addr_a];
                	tmp1 = ((SIMD_INT_TYPE*)a)[addr_a + cols * SIMD_INT_COUNT];
                	tmp0 >>= (k % SIMD_INT_BITS);
                	tmp1 >>= (k % SIMD_INT_BITS);
                	a0 = SIMD_SET_ALL(EXPAND_LOWEST_BIT(tmp0));
                	a1 = SIMD_SET_ALL(EXPAND_LOWEST_BIT(tmp1));

                	for (j1 = j; j1 < j+UJ; j1 += 4) {
                		// load B
                		addr_b = k * cols + j1;
                		b0 = b[addr_b];
                		b1 = b[addr_b+1];
                		b2 = b[addr_b+2];
                		b3 = b[addr_b+3];
                		// load C
                		addr_c0 = i1 * cols + j1;
                		c00 = c[addr_c0];
                		c01 = c[addr_c0+1];
                		c02 = c[addr_c0+2];
                		c03 = c[addr_c0+3];
                		addr_c1 = addr_c0 + cols;
                		c10 = c[addr_c1];
						c11 = c[addr_c1+1];
						c12 = c[addr_c1+2];
						c13 = c[addr_c1+3];

						// compute
						c00 = SIMD_OR(c00, SIMD_AND(a0, b0));
						c01 = SIMD_OR(c01, SIMD_AND(a0, b1));
						c02 = SIMD_OR(c02, SIMD_AND(a0, b2));
						c03 = SIMD_OR(c03, SIMD_AND(a0, b3));
						c10 = SIMD_OR(c10, SIMD_AND(a1, b0));
						c11 = SIMD_OR(c11, SIMD_AND(a1, b1));
						c12 = SIMD_OR(c12, SIMD_AND(a1, b2));
						c13 = SIMD_OR(c13, SIMD_AND(a1, b3));

						// store
						c[addr_c0] = c00;
						c[addr_c0+1] = c01;
						c[addr_c0+2] = c02;
						c[addr_c0+3] = c03;
						c[addr_c1] = c10;
						c[addr_c1+1] = c11;
						c[addr_c1+2] = c12;
						c[addr_c1+3] = c13;
                	}
                }
#else
                for (i1 = i; i1 < i+UI; ++i1) {
                    SIMD_INT_TYPE a_i1_k = ((SIMD_INT_TYPE*)a)[i1 * cols * SIMD_INT_COUNT + (k / SIMD_INT_BITS)];
                    SIMD_INT_TYPE bitmask = ((a_i1_k >> (k % SIMD_INT_BITS)) & 1) * (-1);
                    SIMD_TYPE mask = SIMD_SET_ALL(bitmask);
                    for (j1 = j; j1 < j+UJ; ++j1) {
                        SIMD_TYPE rh = SIMD_AND(mask, b[k * cols + j1]);
                        SIMD_TYPE c_i1_j1 = c[i1 * cols + j1];
                        c[i1 * cols + j1] = SIMD_OR(c_i1_j1, rh);
                    }
                }
#endif

#endif
            }
        }
    }
}

void PentagonSIMD::FWIabc(SIMD_TYPE*__restrict__ a, SIMD_TYPE*__restrict__ b, SIMD_TYPE*__restrict__ c, int n, int cols) {
	assert( (a != b) && (a != c) );
	int inner_cols = n / SIMD_BITS;
	int i, j, k, i1, j1, k1;
	for (i = 0; i < n; i += UI) {
		for (j = 0; j < inner_cols; j += UJ) {
			for (k = 0; k < n; ++k) {
#ifdef AVX
				for (i1 = i; i1 < i+UI; ++i1) {
					SIMD_INT_TYPE a_i1_k = ((SIMD_INT_TYPE*)a)[i1 * cols * SIMD_INT_COUNT + (k / SIMD_INT_BITS)];
					SIMD_INT_TYPE bitmask = ((a_i1_k >> (k % SIMD_INT_BITS)) & 1) * (-1);
					SIMD_E_TYPE mask = SIMD_E_SET_ALL(bitmask);
					for (j1 = j; j1 < j+UJ; j1 += 2) {
						SIMD_E_TYPE *b_e = (SIMD_E_TYPE*) b,
									*c_e = (SIMD_E_TYPE*) c;
						SIMD_E_TYPE rh = SIMD_E_AND(mask, b_e[(k * cols + j1) / 2]);
						SIMD_E_TYPE c_e_i1_j1 = c_e[(i1 * cols + j1) / 2];
						c_e[(i1 * cols + j1) / 2] = SIMD_E_OR(c_e_i1_j1, rh);
					}
				}
#else

#ifdef UNROLL_FWT

				for (i1 = i; i1 < i+UI; ++i1) {
					for (j1 = j; j1 < j+UJ; ++j1) {
						SIMD_TYPE c0 = c[i1 * cols + j1];
						for (k1 = k; k1 < k+UK; k1 += 4) {
							SIMD_TYPE b0,b1,b2,b3,t0,t1,t2;
							SIMD_TYPE a0,a1,a2,a3;
							SIMD_INT_TYPE tmp, addr_b;
							// load
							tmp = ((SIMD_INT_TYPE*)a)[i1 * cols * SIMD_INT_COUNT + (k1 / SIMD_INT_BITS)];
							tmp >>= (k1 % SIMD_INT_BITS);
							a0 = SIMD_SET_ALL(EXPAND_LOWEST_BIT(tmp));
							tmp >>= 1;
							a1 = SIMD_SET_ALL(EXPAND_LOWEST_BIT(tmp));
							tmp >>= 1;
							a2 = SIMD_SET_ALL(EXPAND_LOWEST_BIT(tmp));
							tmp >>= 1;
							a3 = SIMD_SET_ALL(EXPAND_LOWEST_BIT(tmp));

							addr_b = k1 * cols + j1;
							b0 = b[addr_b]; addr_b += cols;
							b1 = b[addr_b]; addr_b += cols;
							b2 = b[addr_b]; addr_b += cols;
							b3 = b[addr_b];

							// compute

							b0 = SIMD_AND(b0, a0);
							b1 = SIMD_AND(b1, a1);
							b2 = SIMD_AND(b2, a2);
							b3 = SIMD_AND(b3, a3);

							t0 = SIMD_OR(b0, b1);
							t1 = SIMD_OR(b2, b3);

							t0 = SIMD_OR(t0, t1);
							c0 = SIMD_OR(c0, t0);
						}
						// store
						c[i1 * cols + j1] = c0;
					}
				}
#else
				for (i1 = i; i1 < i+UI; ++i1) {
                    SIMD_INT_TYPE a_i1_k = ((SIMD_INT_TYPE*)a)[i1 * cols * SIMD_INT_COUNT + (k / SIMD_INT_BITS)];
                    SIMD_INT_TYPE bitmask = ((a_i1_k >> (k % SIMD_INT_BITS)) & 1) * (-1);
                    SIMD_TYPE mask = SIMD_SET_ALL(bitmask);
                    for (j1 = j; j1 < j+UJ; ++j1) {
                        SIMD_TYPE rh = SIMD_AND(mask, b[k * cols + j1]);
                        SIMD_TYPE c_i1_j1 = c[i1 * cols + j1];
                        c[i1 * cols + j1] = SIMD_OR(c_i1_j1, rh);
                    }
                }
#endif

#endif
			}
		}
	}
}

void PentagonSIMD::FWT(SIMD_TYPE* a, int n) {
	int k, i, j;
    int M = n / L1_SIZE;

    int cols = n / SIMD_BITS;
    int skip = L1_SIZE / SIMD_BITS;

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

void PentagonSIMD::allocate(int num_of_vars)
{
    //num_of_vars = round2pow(num_of_vars);

	assert( num_of_vars % L1_SIZE == 0 );
	assert( L1_SIZE % (SIMD_BITS * UJ) == 0 );
#ifdef SIMD_UNROLL
	assert( (UI % 2 == 0) && (UJ % 4 == 0) );
#endif

	num_of_vars_ = num_of_vars;
	cols_ = num_of_vars / SIMD_BITS;

#ifdef AVX
    assert( UJ % 2 == 0);
    sub_ = (SIMD_TYPE*) (new SIMD_E_TYPE[num_of_vars * cols_]);
#else
    sub_ = new SIMD_TYPE[num_of_vars * cols_];
#endif
    for (int i = 0; i < num_of_vars * cols_; i++) {
        sub_[i] = SIMD_FROM_INT(0);
    }
    ss = (SIMD_INT_TYPE*)sub_;

    intervals_ = new Interval[num_of_vars];
}

void PentagonSIMD::subClosure()
{
    // Delegate to FWT(...)
    //FWI(sub_, sub_, sub_, num_of_vars_, cols_);
	FWT(sub_, num_of_vars_);
}

// Requires the domains to have same number of vars
void PentagonSIMD::subJoin(SIMD_TYPE* other)
{
    for (int i = 0; i < num_of_vars_ * cols_; i++) {
        sub_[i] = SIMD_AND(sub_[i], other[i]);
    }
}

void PentagonSIMD::intervalJoin(Interval* in)
{
	for (int i = 0; i < num_of_vars_; i++)
		intervals_[i].join(in[i]);
}

// Maybe merge these into closure
void PentagonSIMD::inferIntervalFromSub()
{
	int i,j;
	for (i=0; i<num_of_vars_; ++i)
		for (j=0; j<num_of_vars_; ++j)
			if (getSubFor(i,j)) {
				intervals_[i].assumeLessThan(intervals_[j]); // <
				intervals_[j].assumeGreaterThan(intervals_[i]); // >
			}
}

void PentagonSIMD::inferSubFromInterval()
{
    int i,j;
	for (i=0; i<num_of_vars_; ++i)
		for (j=0; j<num_of_vars_; ++j)
			if (intervals_[i].lessThan(intervals_[j]))
				setSubFor(i, j);
}

// Pentagon domain
void PentagonSIMD::closure()
{
    inferIntervalFromSub();
    inferSubFromInterval();
    subClosure();
}

void PentagonSIMD::join(PentagonSIMD& other) 
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


#endif // __PENTAGON_SIMD__
