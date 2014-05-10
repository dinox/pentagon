#include "immintrin.h"
#include "emmintrin.h"

#define F1(n) _mm_slli_si128(x, n/8)
#define F2(n) _mm_slli_epi64(_mm_slli_si128(x, n/8), n%8)
#define F5(n) _mm_or_si128(_mm_slli_epi64(x, n), _mm_srli_epi64(_mm_slli_si128(x, 8), 64-n))
#define F6(n) _mm_or_si128(_mm_slli_epi64(_mm_slli_si128(x, n/8), n%8), _mm_srli_epi64(_mm_slli_si128(x, 8+n/8), 64-n%8))

// Variable shift left
// You see why we shall find another solution!

__m128i _xm_shift_left(__m128i x, int bits) {
    switch(bits) {
        case 0: break;
        case 1: x=F5(1); break;
        case 2: x=F5(2); break;
        case 3: x=F5(3); break;
        case 4: x=F5(4); break;
        case 5: x=F5(5); break;
        case 6: x=F5(6); break;
        case 7: x=F5(7); break;
        case 8: x=F1(8); break;
        case 9: x=F6(9); break;
        case 10: x=F6(10); break;
        case 11: x=F6(11); break;
        case 12: x=F6(12); break;
        case 13: x=F6(13); break;
        case 14: x=F6(14); break;
        case 15: x=F6(15); break;
        case 16: x=F1(16); break;
        case 17: x=F6(17); break;
        case 18: x=F6(18); break;
        case 19: x=F6(19); break;
        case 20: x=F6(20); break;
        case 21: x=F6(21); break;
        case 22: x=F6(22); break;
        case 23: x=F6(23); break;
        case 24: x=F1(24); break;
        case 25: x=F6(25); break;
        case 26: x=F6(26); break;
        case 27: x=F6(27); break;
        case 28: x=F6(28); break;
        case 29: x=F6(29); break;
        case 30: x=F6(30); break;
        case 31: x=F6(31); break;
        case 32: x=F1(32); break;
        case 33: x=F6(33); break;
        case 34: x=F6(34); break;
        case 35: x=F6(35); break;
        case 36: x=F6(36); break;
        case 37: x=F6(37); break;
        case 38: x=F6(38); break;
        case 39: x=F6(39); break;
        case 40: x=F1(40); break;
        case 41: x=F6(41); break;
        case 42: x=F6(42); break;
        case 43: x=F6(43); break;
        case 44: x=F6(44); break;
        case 45: x=F6(45); break;
        case 46: x=F6(46); break;
        case 47: x=F6(47); break;
        case 48: x=F1(48); break;
        case 49: x=F6(49); break;
        case 50: x=F6(50); break;
        case 51: x=F6(51); break;
        case 52: x=F6(52); break;
        case 53: x=F6(53); break;
        case 54: x=F6(54); break;
        case 55: x=F6(55); break;
        case 56: x=F1(56); break;
        case 57: x=F6(57); break;
        case 58: x=F6(58); break;
        case 59: x=F6(59); break;
        case 60: x=F6(60); break;
        case 61: x=F6(61); break;
        case 62: x=F6(62); break;
        case 63: x=F6(63); break;
        case 64: x=F1(64); break;
        case 65: x=F2(65); break;
        case 66: x=F2(66); break;
        case 67: x=F2(67); break;
        case 68: x=F2(68); break;
        case 69: x=F2(69); break;
        case 70: x=F2(70); break;
        case 71: x=F2(71); break;
        case 72: x=F1(72); break;
        case 73: x=F2(73); break;
        case 74: x=F2(74); break;
        case 75: x=F2(75); break;
        case 76: x=F2(76); break;
        case 77: x=F2(77); break;
        case 78: x=F2(78); break;
        case 79: x=F2(79); break;
        case 80: x=F1(80); break;
        case 81: x=F2(81); break;
        case 82: x=F2(82); break;
        case 83: x=F2(83); break;
        case 84: x=F2(84); break;
        case 85: x=F2(85); break;
        case 86: x=F2(86); break;
        case 87: x=F2(87); break;
        case 88: x=F1(88); break;
        case 89: x=F2(89); break;
        case 90: x=F2(90); break;
        case 91: x=F2(91); break;
        case 92: x=F2(92); break;
        case 93: x=F2(93); break;
        case 94: x=F2(94); break;
        case 95: x=F2(95); break;
        case 96: x=F1(96); break;
        case 97: x=F2(97); break;
        case 98: x=F2(98); break;
        case 99: x=F2(99); break;
        case 100: x=F2(100); break;
        case 101: x=F2(101); break;
        case 102: x=F2(102); break;
        case 103: x=F2(103); break;
        case 104: x=F1(104); break;
        case 105: x=F2(105); break;
        case 106: x=F2(106); break;
        case 107: x=F2(107); break;
        case 108: x=F2(108); break;
        case 109: x=F2(109); break;
        case 110: x=F2(110); break;
        case 111: x=F2(111); break;
        case 112: x=F1(112); break;
        case 113: x=F2(113); break;
        case 114: x=F2(114); break;
        case 115: x=F2(115); break;
        case 116: x=F2(116); break;
        case 117: x=F2(117); break;
        case 118: x=F2(118); break;
        case 119: x=F2(119); break;
        case 120: x=F1(120); break;
        case 121: x=F2(121); break;
        case 122: x=F2(122); break;
        case 123: x=F2(123); break;
        case 124: x=F2(124); break;
        case 125: x=F2(125); break;
        case 126: x=F2(126); break;
        case 127: x=F2(127); break;
        default: x = _mm_setzero_si128();
    }
    return x;
}
