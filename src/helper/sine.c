#include "sine.h"

// stolen from psn00bsdk
// which is stolen from
// https://www.coranac.com/2009/07/sines/

// why do the psn00bsdk devs think that constants will kill them.
// that's what optimizing compilers were made for, guise.

static inline int _isin(int qN, int x) {
	static const int qA = 12;
	static const int B = 19900, C = 3516;

	int c = x << (30 - qN);        // Semi-circle info into carry.
	x -= 1 << qN;                  // sine -> cosine calc

	x <<= (31 - qN);               // Mask with PI
	x >>= (31 - qN);               // Note: SIGNED shift! (to qN)
	x *= x;
	x >>= (2 * qN - 14);           // x=x^2 To Q14

	int y = B - (x * C >> 14);     // B - x^2*C
	y = (1 << qA) - (x * y >> 16); // A - x^2*(B-x^2*C)

	return (c >= 0) ? y : (-y);
}

// add 2 to these (multiplying by 4) cuz a sine wave has 4 parts yknow.
// that makes l 2^12 = 4096, and h 2^17=131072
#define qN_l 10
#define qN_h 15

int isin(int x) { return _isin(qN_l, x); }
int icos(int x) { return _isin(qN_l, x + (1 << qN_l)); }

int hisin(int x) { return _isin(qN_h, x); }
int hicos(int x) { return _isin(qN_h, x + (1 << qN_h)); }

#undef qN_l
#undef qN_h
