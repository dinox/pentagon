#ifndef __PENTAGON_MACROS__
#define __PENTAGON_MACROS__

#include <stdint.h>

// this should be <= L2 cache size / 8 (bit packing) / 3 (matrices)
#ifndef L1_SIZE
#define L1_SIZE (4096)
#endif

// sweet spot
#ifndef UALL
#define UALL 16
#endif

// UK doesn't matter
#ifndef UK
#define UK (UALL)
#endif

// UI should be equal to UJ
#ifndef UI
#define UI (UALL)
#endif

#ifndef UJ
#define UJ (UALL)
#endif

#define CPU_FREQ 2200000000ll

// PAY ATTENTION HERE
#define OP_COUNT(nVars, nJoins) ((4*(nVars)*(nVars)*(nVars) + 10*(nVars)*(nVars))*nJoins)

// ------------ macros for BP implementation ------------

#define SUB_TYPE uint32_t
#define SUB_BITS (32)
#define EXPAND_LOWEST_BIT(a) (((a)&1)*(-1))

// ------------ AVX / SSE macros for SIMD implementation ------------
#ifdef AVX
#define SSE
#endif

#ifdef SSE

#include "immintrin.h"
#include "emmintrin.h"
#include "simd_shift_left.h"
#define SIMD_TYPE __m128i
#define SIMD_BITS (128)

#define SIMD_INT_TYPE uint32_t
#define SIMD_INT_BITS (32)
#define SIMD_INT_COUNT (4)

// a : int
#define SIMD_SET_ALL(a) _mm_set1_epi32(a)

// a : int
#define SIMD_FROM_INT(a) _mm_setr_epi32(a,0,0,0)
// a : SIMD_TYPE
#define SIMD_IS_NONZERO(a) _mm_test_all_ones(a)
// a : SIMD_TYPE, b : SIMD_TYPE
#define SIMD_AND(a, b) _mm_and_si128((a), (b))
// a : SIMD_TYPE, b : SIMD_TYPE
#define SIMD_OR(a, b) _mm_or_si128((a), (b))

// --> DONT use these!!
// a : SIMD_TYPE, b : int
//#define SIMD_SHIFT_RIGHT(a, b) _mm_srli_epi64((a), (b) % 64)
// a : SIMD_TYPE, b : int
//#define SIMD_SHIFT_LEFT(a, b) _mm_slli_epi64((a), (b) % 64)

// a: int, returns: SIMD_TYPE
#define SIMD_SET_BIT_MASK(a) _xm_shift_left(_mm_setr_epi32(1,0,0,0), (a))

// a: SIMD_TYPE, b : int, return: int
#define SIMD_EXTRACT_BIT(a, b) (!_mm_test_all_zeros((a), SIMD_SET_BIT_MASK(b)))

#define SIMD_BIT_TO_MASK(a) _mm_cmpeq_epi32(_mm_setr_epi32((a),(a),(a),(a)), _mm_setr_epi32(1,1,1,1))

#else

#define SIMD_TYPE uint32_t
#define SIMD_BITS (32)

#define SIMD_INT_TYPE uint32_t
#define SIMD_INT_BITS (32)
#define SIMD_INT_COUNT (1)

#define SIMD_SET_ALL(a) (a)

#define SIMD_FROM_INT(a) (a)

#define SIMD_IS_NONZERO(a) (!!(a))
#define SIMD_AND(a, b) ((a) & (b))
#define SIMD_OR(a, b) ((a) | (b))
//#define SIMD_SHIFT_RIGHT(a, b) ((a) >> (b))
//#define SIMD_SHIFT_LEFT(a, b) ((a) << (b))


#define SIMD_EXTRACT_BIT(a, b) (((a) >> (b)) & 1)
#define SIMD_SET_BIT_MASK(a) (1 << (a))
#define SIMD_BIT_TO_MASK(a) (-1*(a))

#endif

#ifdef AVX

#define SIMD_E_TYPE __m256i
#define SIMD_E_BITS (256)

#define SIMD_E_SET_ALL(a) _mm256_set1_epi32(a)

#define SIMD_E_AND(a, b) _mm256_castpd_si256(_mm256_and_pd(_mm256_castsi256_pd(a), _mm256_castsi256_pd(b)))
#define SIMD_E_OR(a, b) _mm256_castpd_si256(_mm256_or_pd(_mm256_castsi256_pd(a), _mm256_castsi256_pd(b)))
#endif
// ------------ END SIMD macros --------------------


#endif // __PENTAGON_MACROS__
