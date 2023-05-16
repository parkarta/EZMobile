/*
**************************************************************
**															**
** MobyCom.c - MobyCom protocol								**
**															**
**************************************************************
** Dependencies:											**
**************************************************************
** History													**
** V1.0.0.0 (22/01/2017) Initial release					**
** V1.1.0.0 (27/11/19) Increment FrameID in client mode		**
** V1.2.0.0 (15/04/20) Channel is not subdivided			**
** V1.3.0.0 (15/04/20) Channel is subdivided				**
**															**
**************************************************************
*/
#define	__MOBY_COMM__	1

#include "CRC.h"
#include "MobyCom.h"
#ifndef _DISABLE_ENCRYPTION
#include "aes.h"
#endif
//#include <Trace.h>

#ifndef _DISABLE_ENCRYPTION
  static inline uint16_t UpdateLength(uint16_t Length) {
	uint16_t length = Length;
	if (length == 0)
		length = AES_BLOCK_SIZE;
	else {
		if (length & 1)
			length = Length + 1;
		length += 0x000F;
		length &= 0xFFF0;
	}
	return length;
  }
#else
  static inline uint16_t UpdateLength(uint16_t Length) {
	if (Length & 1)
		return Length + 1;
	else
		return Length;
  }
#endif

uint16_t FinishMobyComFrame(  MobyComFrame * frame
							, uint16_t Length
						#ifndef _DISABLE_ENCRYPTION
							, uint8_t * pKey
						#endif
							) {
#ifndef _DISABLE_ENCRYPTION
	uint32_t	IV[AES_BLOCK_SIZE/sizeof(uint32_t)];
#endif
MobyComFrame * ph = (MobyComFrame *) __builtin_assume_aligned(frame, 4);
	uint16_t length = Length;
	ph->Length = Length;		// The length of the data field
	if ((ph->Flags.server == 0) && (ph->Flags.repeated == 0))
		++ph->FrameID;			// Since V1.1
#ifndef _DISABLE_ENCRYPTION
	if (pKey != 0) {
		ph->Flags.encrypted = 1;
		length = UpdateLength(Length);
	} else
#endif
	{
		ph->Flags.encrypted = 0;
		if (length & 1)
			length++;
	}
	uint16_t * pCRC = (uint16_t *)(&ph->Data[length]);
	*pCRC = CRC16((uint8_t *) ph, length + sizeof(MobyComFrame));
#ifndef _DISABLE_ENCRYPTION
	if (pKey != 0) {
		uint32_t id1 = ph->ClientID;
		uint32_t id2 = (ph->FrameID << 16) | ph->FrameID;
		IV[0] = id1;
		IV[1] = id2;
		IV[2] = id1;
		IV[3] = id2;
//		TRACE("IV = %08X %08X %08X %08X\n", IV[0], IV[1], IV[2], IV[3]);
		EncryptCBC_AES(ph->Data, ph->Data, (uint8_t *) pKey, (uint8_t *) IV, length);
	}	// if (ph->Flags.encrypted && (Key != 0) && !ph->Flags.encrSpecial)
#endif
	return length + sizeof(MobyComFrame) + 2;
}	// uint16_t FillMobyComFrame(MobyComFrame * frame, uint16_t length)

#if 0
#ifndef _DISABLE_ENCRYPTION
uint16_t EncryptMobyComFrame(MobyComFrame * frame, uint8_t * Key) {
MobyComFrame * ph = (MobyComFrame *) __builtin_assume_aligned(frame, 4);
	uint16_t Length = ph->Length;
	if (ph->Flags.encrypted && (Key != 0) && !ph->Flags.encrSpecial) {
		Length = UpdateLength(Length);
		uint32_t id1 = ph->ClientID;
		uint32_t id2 = (ph->FrameID << 16) | ph->FrameID;
		IV[0] = id1;
		IV[1] = id2;
		IV[2] = id1;
		IV[3] = id2;
//		TRACE("IV = %08X %08X %08X %08X\n", IV[0], IV[1], IV[2], IV[3]);
		EncryptCBC_AES(ph->Data, ph->Data, (uint8_t *) Key, (uint8_t *) IV, Length);
	} else	// if (ph->Flags.encrypted)
		if (Length & 1)
			Length++;
	return Length;
}	// uint16_t EncryptMobyComFrame(MobyComFrame * const ph, uint16_t length, uint8_t * Key)
#endif
#endif

uint8_t CheckMobyComFrame(MobyComFrame * frame
					#ifndef _DISABLE_ENCRYPTION
						, uint8_t * Key
					#endif
						 ) {
MobyComFrame * ph = (MobyComFrame *) __builtin_assume_aligned(frame, 4);
uint16_t Length = ph->Length;

  #ifndef _DISABLE_ENCRYPTION
	uint32_t	IV[AES_BLOCK_SIZE/sizeof(uint32_t)];
	if (ph->Flags.encrypted && (Key != 0)) {
		Length = UpdateLength(Length);
		// Decrypt
		uint32_t id1 = ph->ClientID;
		uint32_t id2 = (ph->FrameID << 16) | ph->FrameID;
		IV[0] = id1;
		IV[1] = id2;
		IV[2] = id1;
		IV[3] = id2;
//		TRACE("IV = %08X %08X %08X %08X\n", IV[0], IV[1], IV[2], IV[3]);
		DecryptCBC_AES(ph->Data, ph->Data, Key, (uint8_t *) IV, Length);
	} else
  #endif
		if (Length & 1)
			Length++;
	if (ph->Flags.encrSpecial == 0) {
		// Check CRC
		uint16_t crc = CRC16((uint8_t *)ph, Length + sizeof(MobyComFrame) + 2);
		if (crc != 0) {
	  #if !defined(MOBY)
			if (ph->ProtVer < 2)
				return ERROR_BAD_CRC_0;
			else
	  #endif
				return ERROR_BAD_CRC;
		}
	}
	return 0;
}	// uint16_t CheckMobyComFrame(MobyComFrame * const ph, uint8_t Key)
