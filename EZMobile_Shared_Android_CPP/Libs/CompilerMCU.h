#ifndef __COMPILER__MCU
#define __COMPILER__MCU
#pragma once

#include <stdint.h>
#include <intvar.h>

#ifdef __PACKED_START
  #undef	__PACKED_START
  #undef	__PACKED_END
#endif
#define	__PACKED_START
#pragma warning(disable : 4200)
#pragma warning(disable : 4201)
#pragma warning(disable : 4214)
#define	__PACKED_END

#ifdef ALIGN
  #undef	ALIGN
#endif
#define	ALIGN(n)	alignas(n)

#ifdef __builtin_assume_aligned
  #undef __builtin_assume_aligned
#endif
#define __builtin_assume_aligned(x, n)	(x)

#if defined(_WIN64)
	#define	WSIZE	64
	#define	WHSIZE	32
	typedef unsigned long long	NATIVE;
	typedef long long			NATIVE_INT;
	typedef unsigned int		HNATIVE;
#else // _WIN32
	#define	WSIZE	32
	#define	WHSIZE	16
	typedef unsigned int	NATIVE;
	typedef unsigned short	HNATIVE;
#endif

#define MAX_UINT	(~((NATIVE)0))
#define MAX_INT		((NATIVE_INT)(MAX_UINT >> 1))
#define MIN_INT		(~MAX_INT)

#endif
