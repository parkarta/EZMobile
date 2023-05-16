/*
*********************************************************************
* CommonService.h													*
* Common definitions for MoBy firmware and PC programs				*
*********************************************************************
*/

#ifndef	__CommonService_h__
#define	__CommonService_h__

#include <stdint.h> 
#include "Models.h"

#if defined(_MSC_VER)
  #include <CompilerMCU.h>
  #pragma pack(push, 1)
#endif

#include "MobyCom.h"

#define SERVER_PROTOCOL_VERSION		0x0200
#define SERVER_PROTOCOL_VERSION_ID	2
#define MOBY_PROTOCOL_VERSION_ID	2
#define RPS_PROTOCOL_VERSION_ID		2
#define UPGRADE_PROTOCOL_VERSION	0x0200
#define UPGRADE_PROTOCOL_VERSION_ID	2
#define PP_PROTOCOL_VERSION_ID		2
#define MOBILE_APP_VERSION_ID		2

#define TARG_BOOT					1

#ifdef __cplusplus
struct ConnectionType : public ChannelStruct {
	ConnectionType() noexcept			{v = 0;}
	ConnectionType(uint8_t x) noexcept	{v = x;}
	ConnectionType(ConnectionType& conn) noexcept	{v = conn.v;}
	operator uint8_t() noexcept			{return v;}
};
#else
  #define ConnectionType	ChannelStruct
#endif

#define	MaxPGMs		4

typedef struct __PACKED_START {
  union {
	// // u32s		Inp;
	struct {
	  uint8_t	Inp1	: 1;
	  uint8_t	Inp2	: 1;
	  uint8_t	Inp3	: 1;
	};
  };
  union {
	// u32s		Pgm;
	struct {
	  uint8_t	Pgm1;
	  uint8_t	Pgm2;
	  uint8_t	Pgm3;
	  uint8_t	Pgm4;
	};
  };
  union {
	struct {
	  uint8_t	Tamper	: 1;
	  uint8_t	Jamming	: 1;
	};
	uint8_t		TamperJam;
  };
  uint8_t		Rssi;
  uint8_t		res10;
  uint8_t		res11;
} __PACKED_END	StatesStruct;

#define MskINP1			1
#define MskINP2			2
#define MskINP3			4
#define MskPGM1			1
#define MskPGM2			2
#define MskPGM3			4
#define MskPGM4			8
#define MskTamper		0x01
#define MskJamming		0x02
// For v < 2.0
#define MskINP1old		1
#define MskINP2old		2
#define MskPGM1old		4
#define MskPGM2old		8
#define MskPGM3old		0x10
#define MskPGM4old		0x20
#define MskTamperold	0x40
#define MskJammingold	0x80
#define MskINP3old		0x0100

typedef struct __PACKED_START {
	union {
	  struct __PACKED_START {
		uint8_t		device;
		uint8_t		model;
		uint8_t		hwRev;
		uint8_t		hwSubRev;
	  } __PACKED_END;
	  uint32_t		v;
	};
} __PACKED_END	Board;

typedef struct __PACKED_START {
	uint32_t	sSign;
	uint32_t	Length;
	Board		HW;
	uint32_t	FirmVersion;
	uint32_t	LoaderVer;
	uint32_t	eSign;
} __PACKED_END	DeviceStruct;

typedef union __PACKED_START {
	struct __PACKED_START {
		bool	twoCalls	: 1;
		bool	autoUpdate	: 1;
		bool	enableTest	: 1;
		bool	checkUpdates: 1;
		bool	bistable1	: 1;
		bool	bistable2	: 1;
		bool	bistable3	: 1;
		bool	bistable4	: 1;
		bool	waitDial	: 1;
		bool	answerTone	: 1;
		bool	inp1bypass	: 1;
		bool	inp2bypass	: 1;
		bool	inp3bypass	: 1;
	} __PACKED_END;
	uint16_t	v;
} __PACKED_END MobyModesStruct;

#define	twoCallsPos		0
#define autoUpdatePos	1
#define enableTestPos	2
#define checkUpdatesPos	3
#define	bistable1Pos	4
#define	bistable2Pos	5
#define	bistable3Pos	6
#define	bistable4Pos	7
#define	waitDialPos		8
#define	answerTonePos	9
#define inp1bypassPos	10
#define inp2bypassPos	11
#define inp3bypassPos	12

typedef union __PACKED_START {
	struct __PACKED_START {
		bool	UseUDP		: 1;		// 1 = Use TCP, 0 - use UDP
		bool	StaticIP	: 1;		// Static IP?
		uint8_t Encryption	: 3;		// Encryption type
		uint8_t	res			: 3;
	} __PACKED_END;
	uint16_t	v;
} __PACKED_END IPConfig;
#define	DEF_IP_CONFIG	0x0003

typedef union __PACKED_START {
	struct __PACKED_START {
		bool Ethernet1	: 1;
		bool Ethernet2	: 1;
		bool Gprs1		: 1;
		bool Gprs2		: 1;
		bool Gsm		: 1;
	} __PACKED_END;
	uint8_t    v;
} __PACKED_END ChannelsSet;

#define CH_ETHERNET1_OFFS	0
#define CH_ETHERNET2_OFFS	1
#define CH_GPRS1_OFFS		2
#define CH_GPRS2_OFFS		3
#define CH_GSM_OFFS			4

#define CH_ETHERNET1		0x01
#define CH_ETHERNET2		0x02
#define CH_GPRS1			0x04
#define CH_GPRS2			0x08
#define CH_GSM				0x10

typedef enum {
	  None			= 0
	, OnEthFailure	= 1
	, OnGprsFailure	= 2
	, OnJamming		= 3
	, OnInput1		= 4
	, OnInput2		= 5
	, OnInput3		= 6
	, OnTamper		= 7
} PGMPurposes;

extern PGMPurposes	PgmPurpose[];

typedef union __PACKED_START {
	uint8_t	b[6];
	struct __PACKED_START {
		uint32_t	v32;
		uint16_t	v16;
	} __PACKED_END;
} __PACKED_END MAC;

// typedef  u32s	IpAddr;

typedef	struct __PACKED_START {
	uint32_t	sign;
	uint8_t		device;
	uint8_t		des;
	uint8_t		model;
	uint8_t		hwRev;
	// u32s		softRev;
	uint8_t		loaderRev;
	uint8_t		settingsRev;
	uint8_t		res[2];
} __PACKED_END DevInfo;

// Communication protocols
#define NoReport			0
#define RepMobyCom			1
#define RepDC09				2
#define RepMulticom			3

#define APN_LENGTH			32
#define GSM_USER_LENGTH		16
#define GSM_PASS_LENGTH		16

typedef union __PACKED_START {
	struct __PACKED_START {
		bool	doubleSIM	: 1;
		bool				: 1;
		bool	blocked1	: 1;
		bool	blocked2	: 1;
	} __PACKED_END;
	uint16_t	v;
} __PACKED_END	GPRSModes;

typedef struct __PACKED_START {
	char	apn[APN_LENGTH];
	char	name[GSM_USER_LENGTH];
	char	password[GSM_PASS_LENGTH];
} __PACKED_END GPRS_ID;

typedef struct __PACKED_START {
	GPRSModes	GprsModes;				// 0	180
	uint8_t		GPRSSupervisingTime[2];	// 2	182
	union __PACKED_START {
	  struct __PACKED_START {
		char	APN1[APN_LENGTH];				// 4	184
		char	UserName1[GSM_USER_LENGTH];		// 36	216
		char	GSMPassword1[GSM_PASS_LENGTH];	// 52	232
		char	APN2[APN_LENGTH];				// 68	248
		char	UserName2[GSM_USER_LENGTH];		// 100	280
		char	GSMPassword2[GSM_PASS_LENGTH];	// 116	296
	  } __PACKED_END;
	  GPRS_ID	GprsId[2];
	} __PACKED_END;
	uint16_t	EventGSMIniFailure;		// 132	312
	uint16_t	EventETHFailure;		// 134	314
	uint16_t	EventGPRSFailure;		// 136	316
	uint16_t	EventGPRSJamming;		// 138	318
	uint16_t	EventGPRSLow;			// 140	320
	uint16_t	EventGPRSMissing;		// 142	322
	uint16_t	EventSIMMissing;		// 144	324
	uint16_t	reservedEvent;			// 146 148 total	326
#if !defined(_MSC_VER)
	uint8_t		Extention[];
#endif
} __PACKED_END GprsSetStruct;

typedef struct __PACKED_START {			// Aligned on 2
	IPConfig	IPMode;					// 0
	uint16_t	res3;					// 2
	uint16_t	ListeningPort;			// 4
	// u32s		OwnIP;					// 6
	// u32s		NetMask;				// 10
	// u32s		DefGateway;				// 14
	// u32s		DNS1;					// 18
	// u32s		DNS2;					// 22
} __PACKED_END IPStruct;

typedef union __PACKED_START {
  struct __PACKED_START {
	bool		InUse		: 1;
	bool		UseIP		: 1;
	uint8_t		protocol	: 3;
	uint8_t		encryption	: 3;
	uint8_t		staticKey	: 1;
	uint8_t		res09		: 1;
	uint8_t		Tail		: 1;		// Only for DC-09
  } __PACKED_END;
  uint16_t		v;
} __PACKED_END ServerMode;

#define	SERVER_IN_USE_OFFS		0
#define SERVER_IN_USE_MASK		0x0001
#define SERVER_USE_IP_OFFS		1
#define SERVER_USE_IP_MASK		0x0002
#define SERVER_PROTOCOL_OFFS	2
#define SERVER_PROTOCOL_MASK	0x001C
#define SERVER_ENCR_OFFS		6
#define SERVER_ENCR_MASK		0x00E0
#define SERVER_STATIC_KEY_OFFS	8
#define SERVER_STATIC_KEY_MASK	0x0100

#define	MAX_URL	29

typedef struct __PACKED_START {
	ServerMode	mode;					// 0
	uint16_t	port;					// 2
	union __PACKED_START {				// 4
	  char		URL[MAX_URL+1];
	  // u32s		IP;
	} __PACKED_END;
	uint16_t	Time;					// 34
} __PACKED_END	ServerConfig;			// 36

typedef struct ALIGN(4) __PACKED_START {
	char			Password[4];		// 0
	uint16_t		AccountNumber;		// 4
	MobyModesStruct	MoByModes;			// 6
	uint8_t			res8;				// 8
	uint8_t			res9;				// 9
	IPConfig		IPMode;				// 10
	uint8_t			res12;				// 12
	uint8_t			res13;				// 13
	uint16_t		ListeningPort;		// 14
	// u32s			OwnIP;				// 16
	// u32s			NetMask;			// 20
	// u32s			DefGateway;			// 24
	// u32s			DNS1;				// 28
	// u32s			DNS2;				// 32
	ServerConfig	Server[2];			// 36
	union __PACKED_START {
	  char			UpdateURL[MAX_URL+1];
	  // u32s			UpdateIP;			// 108
	} __PACKED_END;
	uint16_t		UpdatePort;			// 138

	// Generated events
	uint16_t		EventUser;			// 140
	uint16_t		EventPartition;		// 142
	uint16_t		EventCallback;		// 144
	uint16_t		EventConnect;		// 146
	uint16_t		EventUnsuccess;		// 148
	uint16_t		EventProgram;		// 150
	uint16_t		EventPanic;			// 152
	uint16_t		EventLine;			// 154
	uint16_t		EventINP1;			// 156
	uint16_t		EventINP2;			// 158
	uint16_t		EventTamper;		// 160
	uint16_t		EventINP3;			// 162
	union __PACKED_START {				// 164
	  uint16_t		EventPGMx[MaxPGMs];
	  struct __PACKED_START {
		uint16_t	EventPGM1;			// 164
		uint16_t	EventPGM2;			// 168
		uint16_t	EventPGM3;			// 170
		uint16_t	EventPGM4;			// 172
	  } __PACKED_END;
	} __PACKED_END;

	union __PACKED_START {				// 172
	  uint16_t	TimePGMx[MaxPGMs];
	  struct __PACKED_START {
		uint16_t	TimePGM1;			// 172
		uint16_t	TimePGM2;			// 174
		uint16_t	TimePGM3;			// 176
		uint16_t	TimePGM4;			// 178
	  } __PACKED_END;
	} __PACKED_END;

	GprsSetStruct	Gprs;

} __PACKED_END SettingsStruct;

#define PUB_KEY_LENGTH		1024	/* Bits */
#define EXP_LENGTH			256		/* Bits */

typedef struct __PACKED_START {
	uint32_t	pubKey[PUB_KEY_LENGTH/32];	//  0  - 127
	uint32_t	exp[EXP_LENGTH/32];			// 128 - 159
} __PACKED_END KeyStruct;

typedef struct ALIGN(4) {
	uint32_t	pubKey[PUB_KEY_LENGTH/32];	//  0  - 127
	uint32_t	exp[EXP_LENGTH/32];			// 128 - 159
	uint32_t	pcbID[4];		// 160 - 175
	uint32_t	clientID;		// 176 - 179
	uint32_t	SN;				// 180 - 183
	Board		board;			// 184 - 187
	uint32_t	checkSum;		// 188 - 191
	uint32_t	key[2][4];		// 192 - 223
	uint32_t	mcuID[4];		// 224 - 239	Total: 240 bytes
} AuthStruct;

#if defined(_MSC_VER)
  #pragma pack(pop)
#endif

// MoBy commands
#define	COMMAND_ID		2
#define	SOURCE_ID		3
#define	REPLY_ID		3

// Application IDs
		//				1 is reserved
#define PostProgID		2
#define ServerID		3
#define EZcomID			4
#define EZMobileID		5
#define UpdaterID		6
// IDs in the range 7-4F are reserved
typedef struct __PACKED_START {
  uint16_t	AppID;
  uint16_t	reserved;
} AppIDtype;

// Server commands
#define	CMND_FIRMWARE		0x20
#define	CMND_FESTABLISH		0x21
#define	CMND_PROGRESS		0x22
#define	CMND_DOWNLOADED		0x23
#define	CMND_UPDATED		0x24
#define CMND_GET			0x25
#define CMND_PUT			0x26
#define CMND_GET_POST		0x27
#define CMND_PUT_POST		0x28
#define CMND_STATE			0x29
#define CMND_EVENT			0x2A
#define CMND_SET_KEY		0x2B
#define CMND_SET_MAC		0x2C
#define CMND_GET_CONNECT	0x2D
#define	CMND_RESTART		0x2E
#define	CMND_FBREAK			0x2F
#define	CMND_LINE			0x30
#define	CMND_PGM			0x31
#define	CMND_PANEL			0x32
#define CMND_PANEL_RUN		0x33
#define CMND_INP_BYPASS		0x34
#define	CMND_PANEL_OFF		0x3F
#define	CMND_GSM_TEST		0x40
#define CMND_CRIGHT			0x59
#define CMND_KILL			0x5A
#define CMND_POST_PROG		0x5B

// RPS commands
#define	CMND_DOWNLOAD		0x02
#define	CMND_UPLOAD			0x03

// Error codes for the firmware updater
#define ERROR_OUT_SEQ		0xA0
#define ERROR_OUT_SEQ_0		0x20
// Error codes for Panel programming
#define ERROR_NOT_CONN		0xA0
#define ERROR_CONNECTED		0x91
#define ERROR_NOT_CONN_0	0x20
#define ERROR_CONNECTED_0	0x21

#endif
