#include <stdint.h>

// These are just so my syntax checker shuts up
#ifndef UK
#define UK (1)
#endif

#ifndef UJ
#define UJ (1)
#endif

#ifndef UI
#define UI (1)
#endif

void FWI(uint8_t* A, uint8_t* B, uint8_t* C, int N, int cols) {
    int k, i, j, i1, j1;
    for (k = 0; k < N; k++) {
        for (i = 0; i < N; i += UI) {
            for (j = 0; j < N; j += UJ) {
                for (i1 = i; i1 < i+UI-1; i1++) {
                    for (j1 = j; j1 < j+UJ-1; j1++) {
                        C[i1*cols + j1] = C[i1*cols + j1] || (A[i1*cols + k] && B[k*cols + j1]);
                    }
                }
            }
        }
    }
}

void FWIabc(uint8_t* A, uint8_t* B, uint8_t* C, int N, int cols) {
    int k, i, j, i1, j1, k1;
    for (j = 0; j < N; j += UJ) {
        for (i = 0; i < N; i += UI) {
            for (k = 0; k < N; k += UK) {
                for (k1 = k; k1 < k+UI-1; k1++) {
                    for (i1 = i; i1 < i+UI-1; i1++) {
                        for (j1 = j; j1 < j+UJ-1; j1++) {
                            C[i1*cols + j1] = C[i1*cols + j1] || (A[i1*cols + k1] && B[k1*cols + j1]);
                        }
                    }
                }
            }
        }
    }
}

void FWT(uint8_t* A, uint8_t* B, uint8_t* C, int N, int L1) {
    int k, i, j;
    int M = N / L1;

    for (k = 0; k < M; k++) {
        // phase 1
        // TODO: Make sure you can do like this (probably not)
        FWI(A+k*N+k, B+k*N+k, C+k*N+k, L1, N);

        // phase 2
        for (j = 0; j < M; j++) {
            if (j != k) {
                FWI(A+k*N+k, B+k*N+j, C+k*N+j, L1, N);
            }
        }

        // phase 3
        for (i = 0; i < M; i++) {
            if (i != k) {
                FWI(A+k*N+k, B+k*N+k, C+k*N+k, L1, N);
            }
        }

        // phase 4
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                FWIabc(A+k*N+k, B+k*N+k, C+k*N+k, L1, N);
            }
        }
    }
}

