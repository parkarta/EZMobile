
// References:
// [1] "Handbook of Applied	Cryptography" by Alfred J. Menezes, Paul C. van Oorschot, Scott A.Vanstone

#define _BIGNUMBERS_	_BIGNUMBERS_
#pragma warning(disable : 4505)

//#include "stdafx.h"
#ifdef WIN32
  #include <ForceLib.h>
#endif
#include "BigNumbers.hpp"
#include "Trace.h"

using namespace std;

typedef union {
	NATIVE	v;
	HNATIVE1	h[2];
}	WORDv;

// G(p) group id = 2 (RFC 2409) (G = 2)
static const BigNumb	P = {{	// Little endian format
		0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x5381, 0xECE6, 0x6651, 0x4928,
		0x1FE6, 0x7C4B, 0x2411, 0xAE9F, 0x9FA5, 0x5A89, 0x6BFB, 0xEE38,
		0xB7ED, 0xF406, 0x5CB6, 0x0BFF, 0xED6B, 0xA637, 0x42E9, 0xF44C,
		0x7EC6, 0x625E, 0xB576, 0xE485, 0xC245, 0x6D51, 0x356D, 0x4FE1,
		0x1437, 0xF25F, 0x0A6D, 0x302B, 0x431B, 0xCD3A, 0x19B3, 0xEF95,
		0x04DD, 0x8E34, 0x0879, 0x514A, 0x9B22, 0x3B13, 0xBEA6, 0x020B,
		0xCC74, 0x8A67, 0x4E08, 0x2902, 0x1CD1, 0x80DC, 0x628B, 0xC4C6,
		0xC234, 0x2168, 0xDAA2, 0xC90F, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
}};

static const BigNumb	mP = {{	// C = 2^1024 - P
		0x0001, 0x0000, 0x0000, 0x0000, 0xAC7E, 0x1319, 0x99AE, 0xB6D7,
		0xE019, 0x83B4, 0xDBEE, 0x5160, 0x605A, 0xA576, 0x9404, 0x11C7,
		0x4812, 0x0BF9, 0xA349, 0xF400, 0x1294, 0x59C8, 0xBD16, 0x0BB3,
		0x8139, 0x9DA1, 0x4A89, 0x1B7A, 0x3DBA, 0x92AE, 0xCA92, 0xB01E,
		0xEBC8, 0x0DA0, 0xF592, 0xCFD4, 0xBCE4, 0x32C5, 0xE64C, 0x106A,
		0xFB22, 0x71CB, 0xF786, 0xAEB5, 0x64DD, 0xC4EC, 0x4159, 0xFDF4,
		0x338B, 0x7598, 0xB1F7, 0xD6FD, 0xE32E, 0x7F23, 0x9D74, 0x3B39,
		0x3DCB, 0xDE97, 0x255D, 0x36F0, 0x0000, 0x0000, 0x0000, 0x0000
	}};

static const BigNumb	R2 = {{		// R^2 mod P
		0x1F42, 0x6003, 0xF055, 0x86B8, 0xE3B2, 0xC7B5, 0x1890, 0xC52E,
		0x172A, 0x1893, 0x7232, 0x7CAE, 0x89E3, 0x9699, 0x58A4, 0x55DA,
		0x09B5, 0x267F, 0xFBFA, 0x1F69, 0x0803, 0x01B8, 0x5D0D, 0x64E9,
		0x1C6D, 0x00C5, 0x5E84, 0x0D95, 0x37F2, 0x63E6, 0xAEA8, 0xA513,
		0x158C, 0x74CE, 0x1526, 0x0CB0, 0x5E02, 0xEB65, 0x91E4, 0xE127,
		0x41C7, 0x3100, 0xD1E7, 0x0092, 0x80C2, 0xC388, 0x8FEC, 0x010D,
		0x648D, 0xA6B7, 0xE34C, 0xB990, 0x16FB, 0xE986, 0x67CC, 0x4325,
		0x698D, 0x5A74, 0x28C7, 0x0345, 0x3EAD, 0x3C30, 0xC282, 0x6415
	}};

/*
*****************************************************************
* void Set(BigNumb * const a, const UINT v)						*
* Set small initial	value										*
*****************************************************************
*/
void BigNumb::Set(const NATIVE w) {
int i;
	v[0] = w;
	for (i = 1; i < BNWORDS; i++)
		v[i] = 0;
}	// void Set(const NATIVE v)

/*
*****************************************************************
* void Copy(BigNumb const * const s, BigNumb * const d)			*
* Set small initial	value										*
*****************************************************************
*/
BigNumb& BigNumb::operator=(const BigNumb& s) {
int	i;
	for (i = 0; i < BNWORDS; i++)
		v[i] = s[i];
	return *this;
}	// void Copy(BigNumb const * const s, BigNumb * const d)

/*
*********************************************************************
* int Comp(BigNumb const * const a1, BigNumb const * const a2)		*
* Compare two positive big numbers									*
*																	*
* Returns:															*
*	 1 if a1 > a2													*
*	 0 if a1 == a2													*
*	-1 if a1 < a2													*
*********************************************************************
*/
static int Comp(BigNumb const * const a1, BigNumb const * const a2) {
int i;
	for (i = BNHWORDS-1; i >= 0 ; i--)
		if (a1->h[i] == a2->h[i])
			continue;
		else if (a1->h[i] > a2->h[i])
			return 1;
		else
			return -1;
	return 0;
}	// int Comp(BigNumb const * const a1, BigNumb const * const a2)

/*
*****************************************************************
* void Add(BigNumb * a1, BigNumb const * a2)					*
* Calculates:													*
*	a1 += a2 	(without modulo reduction)						*
*****************************************************************
*/
static NATIVE Add(BigNumb * const a1, BigNumb const * const a2) {
int		i;
WORDv	c;
	c.v = 0;
	for (i = 0; i < BNHWORDS; i++) {
		c.v += (NATIVE)(a1->h[i]);
		c.v += (NATIVE)(a2->h[i]);
		a1->h[i] = c.h[0];
		c.v >>= WHSIZE;
	}
	return (NATIVE)c.h[1];
}	// UINT Add(BigNumb * a1, BigNumb const * a2)

/*
*****************************************************************
* void Addm(BigNumb * a1, BigNumb const * a2)					*
* Calculates:													*
*	a1 += a2 mod(P)	(a1 = (a1 + a2) mod P)						*
*****************************************************************
*/
static void Addm(BigNumb * const a1, BigNumb const * const a2) {
NATIVE	carry;
	carry = Add(a1, a2);
	// It may be out of G(P) group
	if ((carry > 0) || (Comp(a1, &P) >= 0))
		Add(a1, &mP);
}	// void Addm(BigNumb * a1, BigNumb const * a2)

/*
*****************************************************************
* void MulMont(BigNumb * A, BigNumb const * B, BigNumb * T)		*
* Calculates:													*
*	T = A * B * R^-1 mod P										*
* FIOS method	(A little modified Algorithm 1 of				*
* Johann Großschädl, Karl C. Posch and Stefan Tillich			*
* Architectural Enhancements to Support Digital Signal			*
* Processing and Public-Key Cryptography						*
* Proceedings of the 2nd Workshop on Intelligent Solutions in	*
* Embedded Systems (WISES 2004), pp. 129–143)					*
*****************************************************************
*/
void MulMont(const BigNumb& Mult1, const BigNumb& Mult2, BigNumb& Result) {
//static void MulMont(const BigNumb& Mult1, const BigNumb& Mult2, BigNumb& Result) {
NATIVE	i, j;
HNATIVE1	u;			// m = T[0] + u * A[0] * B[j] where u = -P[0]^(-1) (mod r) = 1 with our P
HNATIVE1	ext;		// It is used instead of T[BNHWORDS]
HNATIVE1	t;			// Multiplication carry < r
WORDv	C;			// Reduction carry < r^2
NATIVE	V;
HNATIVE1	*A = (HNATIVE1 *) &Mult1;
HNATIVE1	*B = (HNATIVE1 *) &Mult2;
HNATIVE1	*T = (HNATIVE1 *) &Result;

	Result = 0;
	ext = 0;
	for (i = 0; i < BNHWORDS; ++i) {
		NATIVE Bi = B[i];

		C.v = T[0] + A[0] * Bi;				// C <= r^2 - r
		t = C.h[1];							// t <= r - 1
		u = C.h[0];
		V = (NATIVE)u * (NATIVE)P.h[0];		// V <= r^2 - 2r + 1
		C.v = V + (NATIVE)C.h[0];			// C <= r^2 - r
											// In asm: must set 0 --> C.h[1]
		// Internal loop
		for (j = 1; j < BNHWORDS; ++j) {
			V = A[j] * Bi;
			C.v >>= WHSIZE;					// C <= r - 1
			C.v += V + t;					// C.v = V + t + C.h[1]
											// C <= r^2 - 1
			t = C.h[1];
			V = T[j] + (NATIVE)u * (NATIVE)P.h[j];	// V <= r^2 - r
			C.h[1] = 0;
			C.v += V;						// C <= r^2 - 1
			T[j-1] = C.h[0];
		}
		C.v >>= WHSIZE;
		C.v += (NATIVE)ext + t;				// C.v = T[BNHWORDS] + C.h[1] + t
		T[BNHWORDS-1] = C.h[0];
		ext = C.h[1];						// T[BNHWORDS] = C.h[1];
	}	// for (i = 0; i < BNHWORDS; ++i)

	if ((C.h[1] > 0) || (Comp((BigNumb *)T, &P) != -1))
		Add((BigNumb *) T, &mP);

}	// static void MulMont(const HNATIVE1 * A, const HNATIVE1 * B, HNATIVE1 * T)

/*
*****************************************************************
* void MulSetMont(BigNumb * X, BigNumb const * Y)				*
* Calculates:													*
*	X = X * Y * R^-1 mod P										*
* It is based on algorithm 14.36 from [1]						*
*****************************************************************
*/
static void MulSetMont(BigNumb& A, const BigNumb& B) {
BigNumb	T;
	T = A;
	MulMont(T, B, A);
}	// void MulSetMont(UINTh * A, const UINTh * B)

/*
*****************************************************************
* void SqrMon(BigNumb * X)										*
* Calculates:													*
*	X = X^2 * R^-1 mod P										*
* It is based on algorithm 14.36 from [1]						*
*****************************************************************
*/
static void SqrMont(BigNumb& X) {
BigNumb	T;
	T = X;
	MulMont(T, T, X);
}

static void FromMont(BigNumb& X) {
BigNumb	T;
	T = 1;
	MulSetMont(X, T);
}

static void ToMont(BigNumb& X) {
BigNumb	T;
	T = X;
	MulMont(T, R2, X);
}

void Exp2m(BigNumb& A, const NATIVE * exp) {
BigNumb b2;
	b2 = 2;
	Expm(A, b2, exp);
}	// void Exp2m(BigNumb * A)

void Expm(BigNumb& A, BigNumb& B, const NATIVE * Exp) {
int		i;

#ifdef _DEBUG
	TRACE("P (Big Endian):\n");
	BYTE * pb = (BYTE *) &P + BNBYTES - 1;
	for (i = BNBYTES - 1; i >= 0; i--)
		TRACE("%02X", *pb--);
	TRACE("\n");
#endif

	ToMont(B);
	for (i = EXPWORDS - 1; i >= 0; i--)
		if (Exp[i] != 0)
			break;
	if (i < 0) {	// Exp == 0, return 1
		A = mP;
		return;
	}
	NATIVE mask = 1ULL << (WSIZE - 1);
	while ((Exp[i] & mask) == 0)
		mask >>= 1;
	A = B;
	mask >>= 1;
	for (; i >= 0; i--, mask = 1ULL << (WSIZE - 1))
		for (; mask != 0; mask >>= 1) {
			SqrMont(A);		// A = A^2
			if (Exp[i] & mask) {
				MulSetMont(A, B);		// A *= B mod P
			}
		}
	FromMont(A);
}	// void Expm(BigNumb * A, BigNumb /*const*/ * B, UINT32 * exp)

#if 0
void Expm(BigNumb& A, BigNumb& B, const NATIVE * Exp) {
int		i;
NATIVE	mask;

	ToMont(B);
	for (i = 0; i < EXPWORDS; i++)
		if (Exp[i] != 0)
			break;
	if (i == EXPWORDS) {	// Exp == 0, return 1
		A = mP;
		return;
	}
	mask = 1ULL << (WSIZE - 1);
	while ((Exp[i] & mask) == 0)
		mask >>= 1;
	A = B;
	mask >>= 1;
	for (; i < EXPWORDS; i++, mask = 1ULL << (WSIZE - 1))
		for (; mask != 0; mask >>= 1) {
			SqrMont(A);		// A = A^2
			if (Exp[i] & mask) {
				MulSetMont(A, B);		// A *= B mod P
			}
		}
	FromMont(A);
}	// void Expm(BigNumb * A, BigNumb /*const*/ * B, UINT32 * exp)
#endif
