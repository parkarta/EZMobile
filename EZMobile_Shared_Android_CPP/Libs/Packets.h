/*
*****************************************************************
* Packets structures											*
*****************************************************************
*/

#ifndef __PACKETS__
#define __PACKETS__	__PACKETS__

#include "CommonService.h"

#if defined(_MSC_VER)
  #pragma pack(push, 1)
#endif	// Win32
#include "../../../Packets.h"
 


// Server connection procedure
// Encryption algorithm
#define NO_ENCR			0
#define ENCR_AES		1
// Algorithm type
#define ENCR_AES_CBC	1
// Algorithm strength
#define ENCR_AES_128	1
#define ENCR_AES_192	2
#define ENCR_AES_256	4
// Sign
#define NO_SIGN			0
#define SIGN128			1
// Hash algorithm
#define HASH_XOR		1
#define HASH_CRC16		2

#define BoardInfo			\
	union __PACKED_START {	\
	  Board		board;		\
	  uint32_t	board32;	\
	} __PACKED_END;			\
	// u32s		softRev;	\
	// u32s		loaderRev

typedef struct ALIGN(4) __PACKED_START {
	BoardInfo;
	uint8_t		boot;
	uint8_t		check;
} __PACKED_END BoardInfoPacket;

#define CommonDeviceInfo	\
	BoardInfo;				\
	uint16_t	acc16;		\
	uint16_t	port;		\
	union __PACKED_START {	\
	  char		password[4];\
	  uint32_t	password32;	\
	} __PACKED_END;			\
	uint32_t	id;			\
	uint32_t	sn;			\
	MAC			mac

#define EncrData			\
  uint8_t	ike;			\
  uint8_t	encrAlgo;		\
  uint8_t	encrType;		\
  uint8_t	encrStrength;	\
  uint8_t	signType;		\
  uint8_t	hashAlgo;		\
  uint8_t	reserved23[2]

typedef struct ALIGN(4) __PACKED_START {
  CommonDeviceInfo;
  uint16_t	version;
  EncrData;
} __PACKED_END ConnectPacket;

typedef struct ALIGN(4) __PACKED_START {
  EncrData;
} __PACKED_END ACKConnectPacket;

// IKE bits
#define NO_UPDATE	0x00
#define IKE_FIXED	0x01
#define IKE_DH0		0x02

#define AES128CBC	1

typedef struct ALIGN(4) __PACKED_START {
  uint16_t	account;	// Dependent account
  uint16_t	code;		// Event Code (1100-6999)
  uint8_t	part;		// Partition (0-255)
  uint8_t	res5;
  uint16_t	zone;		// Zone (0-999)
  uint16_t	timeout;
  uint16_t	port;
  uint16_t	cellPort;
  uint16_t	primAcc;
  // u32s		cellIP;
} __PACKED_END EventPacket;

#define PasswordHeader		\
	union __PACKED_START {	\
	  char		password[4];\
	  uint32_t	password32;	\
	} __PACKED_END

#define PacketHead

typedef struct ALIGN(4) __PACKED_START {
	PasswordHeader;
	uint8_t		Parameters[];
} __PACKED_END	ReqPacket;

typedef struct ALIGN(4) __PACKED_START {
	uint32_t	ip;
	uint16_t	port;
	uint8_t		mobyComVersionID;
	uint8_t		rpsProtocolVersionID;
} __PACKED_END	CallToPacket;

typedef struct  ALIGN(4) __PACKED_START {
	uint32_t	ethIP;
	uint32_t	cellIP;
	uint16_t	ethPort;
	uint16_t	cellPort;
	uint8_t		channels;
	uint8_t		mobyComVersionID;
	uint8_t		rpsProtocolVersionID;
} __PACKED_END	GetTargetIPPacket;

typedef struct ALIGN(4) __PACKED_START {
	CommonDeviceInfo;			// 0-33
	uint8_t		res34[2];
	StatesStruct State;			// 34-43
	char		provider[16];	// 44
	char		apn[16];		// 60
	union {
	  uint8_t	res76;
	  uint8_t	inp3;			// 76
	};
								// 77
} __PACKED_END	DetailsPacket;

typedef struct ALIGN(4) __PACKED_START {
	CommonDeviceInfo;			// 0-33
	uint8_t		inp1;			// +34
	uint8_t		inp2;			// +35
	uint8_t		pgm[MaxPGMs];	// +36
	uint8_t		tamper;			// +40
	uint8_t		rssi;			// +41
	uint8_t		jamming;		// +42
	uint8_t		res;			// +43
	char		provider[16];	// 44
	char		apn[16];		// 60
	uint8_t		inp3;			// 76
								// 77
} __PACKED_END	DetailsPacket1;

typedef struct ALIGN(4) __PACKED_START {
	StatesStruct State;			// 0-33
} __PACKED_END	StatePacket;

typedef struct ALIGN(4) __PACKED_START {
	CommonDeviceInfo;			// 0-33
	struct {					// +34
	  uint8_t	checkOnly	: 1;
	  uint8_t	boot		: 1;
	};
} __PACKED_END	FirmwareReqPacket;

typedef struct ALIGN(4) __PACKED_START {
	uint32_t	version;
} __PACKED_END	FirmwareInfoPacket;

typedef struct ALIGN(4) __PACKED_START {
	uint32_t	id;
	uint32_t	sn;
	MAC			mac;
} __PACKED_END	BoardIDPacket;

typedef struct ALIGN(4) __PACKED_START {
	SettingsStruct	Set;
} __PACKED_END	SettingsPacket;
#define SetOffset	0

typedef struct ALIGN(4) __PACKED_START {
	uint32_t	IP;
	uint16_t	Port;
} __PACKED_END	FirmServerPacket;

typedef struct __PACKED_START {
	uint8_t		id;
	uint8_t		state;
} __PACKED_END	PgmPacket;

typedef struct __PACKED_START {
	uint8_t		id;
	uint8_t		state;
} __PACKED_END	PgmReplyPacket;

typedef struct ALIGN(4) __PACKED_START {
	uint32_t	key[32];
} __PACKED_END	KeyPacket;

typedef struct ALIGN(4) __PACKED_START {
	uint32_t	sign[4];
} __PACKED_END	KeyControlPacket;

typedef struct ALIGN(4) __PACKED_START {
	uint16_t	seq;		// 0
	uint16_t	percent;	// 2
	uint32_t	address;	// 4
	uint16_t	length;		// 8
	uint16_t	res[3];		// 10
	uint8_t		image[];	// 16
} __PACKED_END	FirmwarePacket;

typedef struct __PACKED_START {
	uint16_t	seq;
	uint16_t	percent;
} __PACKED_END	FirmwareAckPacket;

typedef struct ALIGN(4) __PACKED_START {
	uint32_t	address;
	uint16_t	length;
	uint16_t	res[5];
} __PACKED_END	BlockReqPacket;

typedef struct ALIGN(4) __PACKED_START {
	uint32_t	address;
	uint16_t	length;
	uint16_t	res[5];
	uint8_t		image[];
} __PACKED_END	ProgramBlockPacket;

typedef struct ALIGN(4) __PACKED_START {
	uint16_t	percent;
} __PACKED_END	FirmwareReportPacket;

typedef struct ALIGN(4) __PACKED_START {
	uint16_t	currSeq;
	uint16_t	rcvdSeq;
} __PACKED_END	OutSeqPacket;

typedef struct __PACKED_START {
	MAC			mac;
} __PACKED_END	MacPacket;

typedef struct __PACKED_START {
	uint16_t	time;
} __PACKED_END	LinePacket;

#if defined(_MSC_VER)
  #pragma pack(pop)
#endif

#endif
