#ifndef _BIGNUMBERS_H_
#define	_BIGNUMBERS_H_	_BIGNUMBERS_H_

#define	BNBITS		1024
#define	BNBYTES		(BNBITS/8)
#define	EXPBITS		256
#define	EXPBYTES	(EXPBITS/8)

#pragma warning(disable : 4201)
 

//#if defined(_WIN64)
//	#define	WSIZE	64
//	#define	WHSIZE	32
//	typedef unsigned long long	NATIVE;
//	typedef unsigned int		HNATIVE1;
//#elif defined (_WIN32)
//	#define	WSIZE	32
//	#define	WHSIZE	16
//	typedef unsigned int	NATIVE;
//	typedef unsigned short	HNATIVE1;
//#else
	#define	WSIZE	16
	#define	WHSIZE	8
	typedef	unsigned		NATIVE;
	 typedef unsigned short	HNATIVE1;
//#endif

#ifndef BYTE
  typedef unsigned char	BYTE;
#endif

typedef unsigned short	WORD16;
typedef unsigned int	WORD32;

#define	EXPWORDS	(EXPBITS/WSIZE)
#define	EXPHWORDS	(EXPBITS/WHSIZE)
#define	EXP32WORDS	(EXPBITS/32)
#define	WNUMB		(BNBITS/WSIZE)
#define	WHNUMB		(BNBITS/WHSIZE)
#define	BNWORDS		(BNBITS/WSIZE)
#define	BNHWORDS	(BNBITS/WHSIZE)
#define	HWORDS		(BNBITS/WHSIZE)
#define B16WORDS	(BNBITS/16)
#define B32WORDS	(BNBITS/32)

union BigNumb {
	WORD16	v16[B16WORDS];
	WORD32	v32[B32WORDS];
	HNATIVE1	h[HWORDS];
	NATIVE	v[BNWORDS];
	BYTE	b[BNBYTES];
	void	Set(const NATIVE w);
	const	BigNumb& operator=(const NATIVE w) {Set(w); return *this;}
	BigNumb& operator=(const BigNumb& w);
	NATIVE& operator[](int i) {return v[i];}
	const NATIVE operator[](int i) const {return v[i];}
	const NATIVE operator()(int i) const {return (NATIVE) h[i];}
};

union DBigNumb {
	BigNumb	N[2];
	struct {
		BigNumb	Low;
		BigNumb	High;
	};
	WORD16	v16[2*B16WORDS];
	NATIVE	v[2*WNUMB];
	HNATIVE1	h[2*WHNUMB];
	BYTE	b[2*BNBYTES];
};

void Exp2m(BigNumb& A, const NATIVE * exp);
void Expm(BigNumb& a, BigNumb& b, const NATIVE * exp);

#endif	// _BIGNUMBERS_H_
