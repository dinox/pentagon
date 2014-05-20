#include "timer.h"
#include<iostream>

#define SIMD_AND_OR(c, a, b) __asm__ __volatile__("pand $0, $1 \n\tpor $0, $2)" : "=r" (c) : "r" (a), "r" (b));

int main() {
    Timer t(128 * 16000000ull);
    t.start();
    for (int i = 0; i < 1000000; i++) {
        __asm__ __volatile__ (
             "pand %xmm0, %xmm0 \n\t" \
             "pand %xmm1, %xmm1 \n\t" \
             "pand %xmm2, %xmm2 \n\t" \
             "pand %xmm3, %xmm3 \n\t" \
             "pand %xmm4, %xmm4 \n\t" \
             "pand %xmm5, %xmm5 \n\t" \
             "pand %xmm6, %xmm6 \n\t" \
             "pand %xmm7, %xmm7 \n\t" \
             "pand %xmm8, %xmm8 \n\t" \
             "pand %xmm9, %xmm9 \n\t" \
             "pand %xmm10, %xmm10 \n\t" \
             "pand %xmm11, %xmm11 \n\t" \
             "pand %xmm12, %xmm12 \n\t" \
             "pand %xmm13, %xmm13 \n\t" \
             "pand %xmm14, %xmm14 \n\t" \
             "pand %xmm15, %xmm15 \n\t" \
            );
    }
    t.stop();
    t.print_perf();
    t.print_cycles();
    return 0;
}
