/*
*********************************************************************
* MobyCom.h															*
* Definitions for MobyCom protocol									*
*																	*
* Revision history:													*
* V1.1.0.0 29//11/1 Flag 'server' is added (Server/Client mode)		*
*					Flag 'communicator' is removed					*
*																	*
*********************************************************************
*/
#ifndef MobyCom_h
#define MobyCom_h

#ifdef __cplusplus
extern "C" {
#endif

#include "Compiler.h"
#include "../../../../../../Libs/C/MobyCom.h"

#define MobyComID			0x21
#define MobyComVer			0x02

// Encryption Modes
#define NO_ENCRYPTION		0
#define AES128_CBC			1

// Frame types	Deprecated
#define FRAME_NULL			0						// NULL frame
#define FRAME_KEEP_ALIVE	(FRAME_NULL + 1)		// Keep Alive frame
#define FRAME_CONNECT		(FRAME_KEEP_ALIVE + 1)	// Connection establishing frame
#define FRAME_KEY			(FRAME_CONNECT + 1)		// Generate Key
#define FRAME_COMMAND		(FRAME_KEY + 1)			// Command frame
#define FRAME_ERROR			(FRAME_COMMAND + 1)		// Error Frame
#define FRAME_FIRMWARE		(FRAME_ERROR + 1)		// Program update frame
#define FRAME_MEMORY		(FRAME_FIRMWARE + 1)	// Partial memory update frame
#define FRAME_REPORT_MOBYCOM	0x20				// Event message (Report) in MobyCom format
#define FRAME_REPORT_DC05	(FRAME_REPORT_MOBYCOM+1)// Event message (Report) in Ademco Contact ID format (Ademco DC-05)
#define FRAME_REPORT_DC09	(FRAME_REPORT_DC05+1)	// Event message (Report) in Ademco DC-09 format

// Predefined targets
#define TARG_UNDEF			0
#define TARG_LISTEN			0
#define TARG_SERVER			1

// Command codes
#define CMND_NULL			0
#define	CMND_ACK			0x01
#define	CMND_CALLBACK		0x02
#define CMND_CONNECT		0x03
#define	CMND_DETAILS		0x05
#define CMND_TEST			0x07
#define CMND_RESET			0x0B
#define CMND_REPEAT			0x0C
#define	CMND_WAIT			0x0D
#define	CMND_NOP			0x0E
#define	CMND_BREAK			0x0F

// Connect commands
#define CMND_REGISTER		0x10
#define CMND_KEY			0x11
#define	CMND_PASSWORD		0x12
#define CMND_TEST_CONN		0x13
#if !defined(MOBY)
// For version < 2.0
#define CMND_KEY_0			0x04
#define	CMND_PASSWORD_0		0x06
#endif

#define InConnect(p)		(((p)->Cmnd & 0xFC) == 0x10)

// Error codes
#define ERROR_COMM			0x80
#define ERROR_NAK			0x81
#define ERROR_BAD_CRC		0x82
#define ERROR_UNIMPLEMENTED	0x83
#define ERROR_BAD_PASS		0x84
#define ERROR_BAD_PARAM		0x85
#define ERROR_DEV_BUSY		0x86
#define ERROR_HARDWARE		0x87
#define ERROR_COM_TIMEOUT	0x88
#define ERROR_OPEN			0x89
#define ERROR_BAD_VERSION	0x8A
#define ERROR_REJECTED		0x8B
#define ERROR_BAD_CMND		0x8C

#define ERROR_CONNECT		0x90

#define FrameError(p)		((p)->Cmnd >= 0x80)

#if !defined(MOBY)
// For version < 2.0
#define ERROR_COMM_0			0x01
#define ERROR_BAD_CRC_0			0x02
#define ERROR_BAD_CMND_0		0x03
#define ERROR_UNIMPLEMENTED_0	0x04
#define ERROR_BAD_PASS_0		0x05
#define ERROR_BAD_PARAM_0		0x06
#define ERROR_DEV_BUSY_0		0x07
#define ERROR_HARDWARE_0		0x08
#define ERROR_COM_TIMEOUT_0		0x09
#define ERROR_OPEN_0			0x0A
#define ERROR_BAD_VERSION_0		0x0B
#define ERROR_REJECTED_0		0x0C
#define ERROR_NAK_0				0x0D
#define ERROR_BAD_TYPE_0		0x0E
#endif

// Network IDs
#define NET_UNKNOWN			0
#define NET_ETHERNET		1
#define NET_CELL			2
#define GPRS_KIND			1
#define LTE_KIND			2
#define TECH_GPRS			0x10
#define TECH_LTE			0x20

typedef struct {			// To allow using as a base class
  union __PACKED_START {
	struct __PACKED_START {
	  uint8_t id	: 4;
	  uint8_t kind	: 4;
	} __PACKED_END;
	uint8_t	v;
  } __PACKED_END;
} ChannelStruct;

typedef union __PACKED_START {
  struct __PACKED_START {
	uint8_t	minor;
	uint8_t	major;
  } __PACKED_END;
  uint16_t	v;
} __PACKED_END VersionType;

#if defined(_MSC_VER)
  #pragma pack(push, 1)
  #ifdef ALIGN
	#undef ALIGN
  #endif
  #define ALIGN(n)
#endif	// Win32
#ifdef ALIGN
#undef ALIGN
#endif
#define ALIGN(n)
// Field structures
typedef struct {
  union __PACKED_START {
	struct __PACKED_START {
	  uint8_t	encrypted		: 1;
	  uint8_t	encrSpecial		: 1;
	  uint8_t	repeated		: 1;
	  uint8_t	server			: 1;	// V1.1
	  uint8_t	altID			: 1;	// v2.0
	  uint8_t	reserved5		: 1;
	  uint8_t	appdef6			: 1;
	  uint8_t	appdef7			: 1;
	} __PACKED_END;
	uint8_t	v;
  } __PACKED_END;
} MobyComFlags;

	// Masks for MobyComFlags
#define MskEncrypted		(1)
#define MskEncryptSpecial	(1 << 1)
#define MskRepeated			(1 << 2)
#define MskServer			(1 << 3)
#define MskAltID			(1 << 4)
#define MskRes5				(1 << 5)
#define MskAppDef6			(1 << 6)
#define MskAppDef7			(1 << 7)

#define IsServer(flags)		(((MobyComFlags) flags).server != 0)
#define IsClient(flags)		(((MobyComFlags) flags).server == 0)

	// For v < 2
#define ackRequired			repeater
#define MskAckRequired		(1 << 4)

// Frame structures

typedef struct ALIGN(4) __PACKED_START {
  uint8_t		ProtID;			// Protocol ID (MobyComID)
  uint8_t		ProtVer;		// Version of the protocol
  union {
	uint8_t		AppVerID;		// ID of the application protocol version (since v2.0)
	uint8_t		FrameType;		// The type of the Frame (before v.2.0)
  };
  MobyComFlags	Flags;			// Properties of the packet
  union __PACKED_START {
    uint32_t		ClientID;	// ID of the client
	struct __PACKED_START {
	  uint16_t		AppID;
	  uint16_t		Res6;
	} __PACKED_END;
	struct __PACKED_START {
	  uint16_t		Account;
	  VersionType	Version;	// Deprecated since v2.0
	} __PACKED_END;
  } __PACKED_END;
  uint16_t		FrameID;		// Identifies the frame
  ChannelStruct	Channel;		// Network ID
  union {
	uint8_t		Cmnd;			// Command for a command frame
	uint8_t		Error;			// Error code
  };
  uint8_t		ThrdIDsrc;		// Identifies the source thread
  uint8_t		ThrdIDdest;		// Identifies the destination thread
  uint16_t		Length;			// The length of the data field
  uint8_t		Data[];			// Data Field
} __PACKED_END MobyComFrame;	// CRC16 is added at the end of the frame

static inline int FrameErrorVer(const MobyComFrame * const p) {
	if (p->ProtVer >= 2)
		return p->Cmnd >= 0x80;
	else
		return p->FrameType == FRAME_ERROR;
}

uint16_t FinishMobyComFrame(  MobyComFrame * frame
							, uint16_t length
						#ifndef _DISABLE_ENCRYPTION
							, uint8_t * Key
						#endif
							);

uint8_t CheckMobyComFrame(MobyComFrame * frame
					#ifndef _DISABLE_ENCRYPTION
						, uint8_t * Key
					#endif
						 );

#if defined(_MSC_VER)
  #pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif	// MobyCom_h
