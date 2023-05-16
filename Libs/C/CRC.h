/*
*********************************************************************
* CRC-16 Checksum calculation in accordance with ANSI X3.28			*
* Table Look-up algorithms from RFC 1662 App.C.2					*
* Generator polynomial x^16+x^15+x^2+1								*
*********************************************************************
*/

#ifndef __CRC16_ANSI_H
#define __CRC16_ANSI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern const uint16_t crcTABLE[];

/*
 ************************************************************************
 * uint16 CRC16as() - One step of CRC-16 calculation					*
 *																		*
 * Input:																*
 * int16 crc:	previous value of CRC									*
 * byte	 data:	new data byte											*
 * Return value:														*
 *   uint16	CRC-16 as 16-bit word										*
 *																		*
 * The algorithm uses table look-up method. Precalculated values are	*
 * in the table crcTABLE[256]											*
 ************************************************************************
 */
static inline
uint16_t CRC16as(uint16_t crc, const uint8_t b) {
	return (crc >> 8) ^ (crcTABLE[b ^ (crc & 0x00FFu)]);
}	// uint16_t CRC16t(uint16 crc, const uint8_t data)

/*
 ************************************************************************
 * uint16 CRCai16() - Calculates CRC-16 on the buffered data based on	*
 *					  initial CRC value 								*
 *																		*
 * Input:																*
 * byte *	data:	pointer to the array of bytes CRC of which shall	*
 *					be calculated										*
 * int		len:	the length of the array	(in bytes)					*
 *																		*
 * Return value:														*
 *   uint16	CRC-16 as 16-bit word										*
 *																		*
 ************************************************************************
 */
static inline
uint16_t CRC16ai(uint16_t crc, const uint8_t * data, int len) {
	for (; len > 0; len--)
		crc = CRC16as(crc, *data++);
	return crc;
}	// uint16_t CRC16ai(uint16 crc, const uint8_t * data, int len)

/*
 ************************************************************************
 * Calculate a CRC-16 ANSI given the buffred data			.			*
 * Initial value: 0xFFFF												*
 * Output inversion: YES												*
 ************************************************************************
 */
static inline
uint16_t CRC16a(const uint8_t * pData, int len) {
	return ~CRC16ai(0xFFFF, pData, len);
}	// uint16 CRC16a(BYTE * pData, int len)

/*
 ************************************************************************
 * Calculate a CRC-16 SIA given the buffred data			.			*
 * Initial value: 0														*
 * Output inversion: NO													*
 ************************************************************************
 */
static inline
uint16_t CRC16sia(const uint8_t * pData, int len) {
	return CRC16ai(0, pData, len);
}	// uint16_t CRC16a(BYTE * pData, int len)

static inline
uint16_t CRC16(const uint8_t * pData, int len) {
	return CRC16ai(0, pData, len);
}	// uint16_t CRC16(BYTE * pData, int len)

#ifdef __cplusplus
}
#endif

#endif
