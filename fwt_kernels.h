#include <stdint.h>
#include <stdio.h>

void FWI(uint8_t* a, uint8_t* b, uint8_t* c, int n, int cols) {
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

void FWIabc(uint8_t* a, uint8_t* b, uint8_t* c, int n, int cols) {
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

void FWT(uint8_t* a, uint8_t* b, uint8_t* c, int n, int L1) {
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

