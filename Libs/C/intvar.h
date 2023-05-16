#pragma once
#ifndef __intvar_h__
#define __intvar_h__

#include <stdint.h>
#include <stdlib.h>

typedef struct u8s {
	union {
		uint8_t	v;
		struct {
			uint8_t	bit0  : 1;
			uint8_t	bit1  : 1;
			uint8_t	bit2  : 1;
			uint8_t	bit3  : 1;
			uint8_t	bit4  : 1;
			uint8_t	bit5  : 1;
			uint8_t	bit6  : 1;
			uint8_t	bit7  : 1;
		};
	};
#ifdef __cplusplus
	u8s() noexcept = default;
	u8s(const uint8_t n) noexcept : v(n) {}
	operator uint8_t&() noexcept	{return v;}
#endif
} u8s, u8v, u8v_t;

typedef struct u16s {
	union {
		uint16_t	v;
		uint8_t		b[2];
		struct {
			uint8_t	b0;
			uint8_t	b1;
		};
		struct {
			uint8_t	bit0  : 1;
			uint8_t	bit1  : 1;
			uint8_t	bit2  : 1;
			uint8_t	bit3  : 1;
			uint8_t	bit4  : 1;
			uint8_t	bit5  : 1;
			uint8_t	bit6  : 1;
			uint8_t	bit7  : 1;
			uint8_t	bit8  : 1;
			uint8_t bit9  : 1;
			uint8_t	bit10 : 1;
			uint8_t	bit11 : 1;
			uint8_t	bit12 : 1;
			uint8_t	bit13 : 1;
			uint8_t	bit14 : 1;
			uint8_t	bit15 : 1;
		};
	};
#ifdef __cplusplus
	u16s() noexcept = default;
	u16s(const uint16_t n) noexcept : v(n) {}
	operator uint16_t() noexcept	{return v;}
	uint8_t Lo() noexcept			{return b0;}
	uint8_t Hi() noexcept			{return b1;}
	uint16_t Swap() noexcept		{return
									  #if defined(__GNUC__)
										__builtin_bswap16(v);
									  #elif defined(_MSC_VER)
										_byteswap_ushort(v);
									  #endif
									}
	uint8_t& operator[](const int i) noexcept	{return b[i];}
#endif
} u16s, u16v, u16v_t;

#ifdef __cplusplus
static inline uint16_t Swap(uint16_t w)	{return
									  #if defined(__GNUC__)
										__builtin_bswap16(w);
									  #elif defined(_MSC_VER)
										_byteswap_ushort(w);
									  #endif
									}
static inline uint16_t Swap(u16s x)	{return Swap(x.v);}
#else
static inline uint16_t Swap16(uint16_t w)	{return
									  #if defined(__GNUC__)
										__builtin_bswap16(w);
									  #elif defined(_MSC_VER)
										_byteswap_ushort(w);
									  #endif
									}
#endif

typedef struct u32s {
	union {
		uint32_t	v;
		uint8_t		b[4];
		struct {
			uint8_t	b0;
			uint8_t	b1;
			uint8_t	b2;
			uint8_t	b3;
		};
		uint16_t	w[2];
		struct {
			uint16_t	w0;
			uint16_t	w1;
		};
		struct {
			uint8_t	bit0  : 1;
			uint8_t	bit1  : 1;
			uint8_t	bit2  : 1;
			uint8_t	bit3  : 1;
			uint8_t	bit4  : 1;
			uint8_t	bit5  : 1;
			uint8_t	bit6  : 1;
			uint8_t	bit7  : 1;
			uint8_t	bit8  : 1;
			uint8_t bit9  : 1;
			uint8_t	bit10 : 1;
			uint8_t	bit11 : 1;
			uint8_t	bit12 : 1;
			uint8_t	bit13 : 1;
			uint8_t	bit14 : 1;
			uint8_t	bit15 : 1;
			uint8_t	bit16 : 1;
			uint8_t	bit17 : 1;
			uint8_t	bit18 : 1;
			uint8_t	bit19 : 1;
			uint8_t	bit20 : 1;
			uint8_t	bit21 : 1;
			uint8_t	bit22 : 1;
			uint8_t	bit23 : 1;
			uint8_t	bit24 : 1;
			uint8_t bit25 : 1;
			uint8_t	bit26 : 1;
			uint8_t	bit27 : 1;
			uint8_t	bit28 : 1;
			uint8_t	bit29 : 1;
			uint8_t	bit30 : 1;
			uint8_t	bit31 : 1;
		};
	};
#ifdef __cplusplus
	u32s() noexcept = default;
	u32s(const uint32_t n) noexcept : v(n) {}
	u32s(const u32s& n) = default;
	operator uint32_t&() noexcept	{return v;}
	uint8_t& operator[](const int i) noexcept {return b[i];}
	uint16_t Lo() noexcept			{return w0;}
	uint16_t Hi() noexcept			{return w1;}
	u32s	 Swap() noexcept		{return
									  #if defined(__GNUC__)
										__builtin_bswap32(v);
									  #elif defined(_MSC_VER)
										_byteswap_ulong(v);
									  #endif
									}
#endif
} u32s, u32v, u32v_t;

#ifdef __cplusplus
static inline uint32_t Swap(uint32_t w) {return
									  #if defined(__GNUC__)
										__builtin_bswap32(w);
									  #elif defined(_MSC_VER)
										_byteswap_ulong(w);
									  #endif
									}
static inline uint16_t Swap(u32s x)	{return Swap(x.v);}
#else
static inline uint32_t Swap32(uint32_t w) {return
									  #if defined(__GNUC__)
										__builtin_bswap32(w);
									  #elif defined(_MSC_VER)
										_byteswap_ulong(w);
									  #endif
									}
#endif

typedef struct u64s {
	union {
		uint64_t	v;
		uint8_t		b[8];
		struct {
			uint8_t	b0;
			uint8_t	b1;
			uint8_t	b2;
			uint8_t	b3;
			uint8_t	b4;
			uint8_t	b5;
			uint8_t	b6;
			uint8_t	b7;
		};
		uint32_t	w32[2];
		struct {
			uint32_t	w32_0;
			uint32_t	w32_1;
		};
		uint16_t	w16[4];
		struct {
			uint16_t	w16_0;
			uint16_t	w16_1;
			uint16_t	w16_2;
			uint16_t	w16_3;
		};
		struct {
			uint8_t	bit0  : 1;
			uint8_t	bit1  : 1;
			uint8_t	bit2  : 1;
			uint8_t	bit3  : 1;
			uint8_t	bit4  : 1;
			uint8_t	bit5  : 1;
			uint8_t	bit6  : 1;
			uint8_t	bit7  : 1;
			uint8_t	bit8  : 1;
			uint8_t bit9  : 1;
			uint8_t	bit10 : 1;
			uint8_t	bit11 : 1;
			uint8_t	bit12 : 1;
			uint8_t	bit13 : 1;
			uint8_t	bit14 : 1;
			uint8_t	bit15 : 1;
			uint8_t	bit16 : 1;
			uint8_t	bit17 : 1;
			uint8_t	bit18 : 1;
			uint8_t	bit19 : 1;
			uint8_t	bit20 : 1;
			uint8_t	bit21 : 1;
			uint8_t	bit22 : 1;
			uint8_t	bit23 : 1;
			uint8_t	bit24 : 1;
			uint8_t bit25 : 1;
			uint8_t	bit26 : 1;
			uint8_t	bit27 : 1;
			uint8_t	bit28 : 1;
			uint8_t	bit29 : 1;
			uint8_t	bit30 : 1;
			uint8_t	bit31 : 1;
			uint8_t	bit32 : 1;
			uint8_t	bit33 : 1;
			uint8_t	bit34 : 1;
			uint8_t	bit35 : 1;
			uint8_t	bit36 : 1;
			uint8_t	bit37 : 1;
			uint8_t	bit38 : 1;
			uint8_t	bit39 : 1;
			uint8_t	bit40 : 1;
			uint8_t bit41 : 1;
			uint8_t	bit42 : 1;
			uint8_t	bit43 : 1;
			uint8_t	bit44 : 1;
			uint8_t	bit45 : 1;
			uint8_t	bit46 : 1;
			uint8_t	bit47 : 1;
			uint8_t	bit48 : 1;
			uint8_t	bit49 : 1;
			uint8_t	bit50 : 1;
			uint8_t	bit51 : 1;
			uint8_t	bit52 : 1;
			uint8_t	bit53 : 1;
			uint8_t	bit54 : 1;
			uint8_t	bit55 : 1;
			uint8_t	bit56 : 1;
			uint8_t	bit57 : 1;
			uint8_t bit58 : 1;
			uint8_t	bit59 : 1;
			uint8_t	bit60 : 1;
			uint8_t	bit61 : 1;
			uint8_t	bit62 : 1;
			uint8_t	bit63 : 1;
		};
	};
#ifdef __cplusplus
	u64s() noexcept = default;
	u64s(const uint64_t n) noexcept : v(n) {}
	operator uint64_t&() noexcept	{return v;}
	uint8_t& operator[](const int i) noexcept {return b[i];}
	uint32_t Lo() noexcept			{return w32_0;}
	uint32_t Hi() noexcept			{return w32_1;}
	u64s Swap() noexcept			{return
									  #if defined(__GNUC__)
										__builtin_bswap32(v);
									  #elif defined(_MSC_VER)
										_byteswap_uint64(v);
									  #endif
									}
#endif
} u64s, u64v, u64v_t;

#ifdef __cplusplus
static inline uint64_t Swap(uint64_t w) {return
									  #if defined(__GNUC__)
										__builtin_bswap32(w);
									  #elif defined(_MSC_VER)
										_byteswap_uint64(w);
									  #endif
									}
static inline uint64_t Swap(u64s w) {return Swap(w.v);}
#else
static inline uint64_t Swap64(uint64_t w) {return
									  #if defined(__GNUC__)
										__builtin_bswap32(w);
									  #elif defined(_MSC_VER)
										_byteswap_uint64(w);
									  #endif
									}
#endif

#endif
