/*
 *********************************************************************
 *
 * GCC ARM Compiler specific definitions
 *
 *********************************************************************
 * FileName:        CompilerMCU.h
 * Dependencies:    None
 * Processor:       Cortex-M0+
 * Compiler:        GCC
 * Company:         Elite Eletronic.
 *
 * Date         Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 29/05/2017	Initial release
 *********************************************************************
*/
#ifndef __COMPILER_H
#define __COMPILER_H

#include <stdint.h>

#ifndef uint
  typedef unsigned int	uint;
#endif

#define LowHalf(v)	((v) & 0xFFFF)
#define HighHalf(v)	(((v) >> 16) & 0xFFFF)

#define __Address(f)	((void *)(((uint)(f)) | 1))

// Base RAM and ROM pointer types for given architecture
#define PTR_BASE		unsigned long
#define ROM_PTR_BASE	unsigned long
#define HWORD			uint16_t

#define __prog__
#define __PROG
#define	ROM				const
#ifndef FAR
  #define FAR			far
#endif

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
