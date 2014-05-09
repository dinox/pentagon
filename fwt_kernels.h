#include <stdint.h>

#ifndef UJ
#define UJ (1)
#endif

#ifndef UI
#define UI (1)
#endif

void FWI(uint8_t* A, uint8_t* B, uint8_t* C, int N) {
    int k, i, j, i1, j1;
    for (k = 0; k < N; k++) {
        for (i = 0; i < N; i += UI) {
            for (j = 0; j < N; j += UJ) {
                for (i1 = i; i1 < i+UI-1; i1++) {
                    for (j1 = j; j1 < j+UJ-1; j1++) {
                        C[i1*N + j1] = A[i1*N + k] && B[k*N + j1];
                    }
                }
            }
        }
    }
}

void FWIabc(uint8_t* A, uint8_t* B, uint8_t* C, int N) {
    int k, i, j, i1, j1;
    for (k = 0; k < N; k++) {
        for (i = 0; i < N; i += UI) {
            for (j = 0; j < N; j += UJ) {
                for (i1 = i; i1 < i+UI-1; i1++) {
                    for (j1 = j; j1 < j+UJ-1; j1++) {
                        C[i1*N + j1] = A[i1*N + k] && B[k*N + j1];
                    }
                }
            }
        }
    }
}


