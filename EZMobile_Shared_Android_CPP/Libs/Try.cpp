//#include <iostream>
//#include <string>
//using namespace std;
//
// 
//#include "Packets.h"
//#include "BigNumbers.hpp"
//#include "aes.h"
//#include "CRC.h" 
//#include "Trace.h" 
////
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
////#include "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22000.0/shared/wtypes.h"
////#include "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22000.0/ucrt/tchar.h"
////#include "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22000.0/shared/wtypes.h"
//////
//string 		ServerName;
//uint16_t	ServerPort;
//uint32_t	ID;
//string 	Pin;
//bool		btnConnect = true;
//uint8_t		Key[AES_KEY_SCHEDULE_SIZE];// alignas(sizeof(size_t));
//uint32_t	Exp[EXP32WORDS];
//uint32_t	PubKey[B32WORDS];
//
//uint8_t		Inputs;
//bool		bGsm;
//bool		bTamper;
//uint8_t		nPGM;
//uint8_t		TargetID;
//StatesStruct State;
////
//#define _T(x)       __T(x)
//#define _TEXT(x)    __T(x)
//
// 
// 
//
//const string ErrorStrings[] = {
//								  "Server socket timeout"			// 0
//								, "Server error"				// 1
//								, "Client is offline"				// 2
//								, "Communication error"				// 3
//								, "Communicator does not request"	// 4
//								, "Connection success"				// 5
//								, "Wrong password"					// 6
////								, _T("Moby timeout")					// 7
//};
//
//#define SERVER_TIMEOUT				0
//#define SERVER_ERROR				1
//#define CLIENT_OFFLINE				2
//#define COMMUNICATION_ERROR			3
//#define COMMUNICATOR_NOT_REQUEST	4
//#define CONNECTION_SUCCESS			5
//#define WRONG_PASSWORD				6
//#define MOBY_TIMEOUT				7
//////
////////HICON LedEmpty;
////////HICON LedRed;
////////HICON LedGrey;
//////
//int volatile dummy;
//void set() {
//	ServerName ="moby.ddns.net";
//	ServerPort = 34197;
//	ID = 0x3B9AD993;
//	Pin = "1111";
//}
//#define TX_SIZE		256
//#define RX_SIZE		256
// typedef unsigned int     size_t1;
//    typedef long              ptrdiff_t;
//    typedef long             intptr_t;
//// UDPSocket member functions
//	uint8_t	rxBuffer[RX_SIZE]; //alignas(sizeof(size_t1));
//	uint8_t	txBuffer[TX_SIZE]; //alignas(sizeof(size_t1));
//	uint8_t	MonrxBuffer[RX_SIZE];// alignas(sizeof(size_t1));
//	uint8_t	MontxBuffer[TX_SIZE];// alignas(sizeof(size_t1));
////
////UDPSocket	MobySocket;
////UDPSocket	StateSocket;
////
//
//
//int procConnect() {
//	uint32_t	timeout = 3000;
//	int		retValue;
//	// Ask Server about Moby address
//	MobyComFrame* pTX = (MobyComFrame*)txBuffer;
//	pTX->ProtID = MobyComID;
//	pTX->ProtVer = MobyComVer;
//	pTX->AppVerID = MOBILE_APP_VERSION_ID;
//	pTX->Flags.v = 0;
//	pTX->ClientID = ID;
//	pTX->Channel.v = NET_ETHERNET;		//! Change for cellular application
//	pTX->FrameID = 1;
//	pTX->Cmnd = CMND_GET_CONNECT;
//	pTX->ThrdIDsrc = COMMAND_ID;
//	pTX->ThrdIDdest = TARG_LISTEN;
//	int length = FinishMobyComFrame(pTX, 0, 0);
//	//ResetEvent(hMoby);
//	MobySocket.SendTo(pTX, length, ServerPort, ServerName);
//	DWORD res = WaitForSingleObject(hMoby, 5000);
//	if (res != WAIT_OBJECT_0) {
//		retValue = SERVER_TIMEOUT;
//		goto endConnection;
//	}
//	MobyComFrame* pRX = (MobyComFrame*)rxBuffer;
//	if (pRX->Cmnd != CMND_GET_CONNECT) {
//		retValue = SERVER_ERROR;
//		goto endConnection;
//	}
//	GetTargetIPPacket* pTarget = (GetTargetIPPacket*)pRX->Data;
//	u32s ip;
//	uint16_t port;
//	if (pTarget->channels & NET_ETHERNET) {
//		ip.v = pTarget->ethIP;
//		port = pTarget->ethPort;
//	}
//	else if (pTarget->channels & NET_CELL) {
//		ip.v = pTarget->cellIP;
//		port = pTarget->cellPort;
//	}
//	else {
//		retValue = CLIENT_OFFLINE;
//		goto endConnection;
//	}
//	MobyAddress.sin_addr.S_un.S_un_b.s_b1 = ip.b0;
//	MobyAddress.sin_addr.S_un.S_un_b.s_b2 = ip.b1;
//	MobyAddress.sin_addr.S_un.S_un_b.s_b3 = ip.b2;
//	MobyAddress.sin_addr.S_un.S_un_b.s_b4 = ip.b3;
//	MobyAddress.sin_port = port;
//	TRACE("Moby is at %u.%u.%u.%u:%u\n", ip.b0, ip.b1, ip.b2, ip.b3, ntohs(port));
//
//	// Connect directly
//	pTX->Flags.altID = 1;
//	pTX->ClientID = EZMobileID;
//	pTX->ThrdIDsrc = COMMAND_ID;
//	pTX->ThrdIDdest = TARG_LISTEN;
//	pTX->FrameID = 0;
//
//	ResetEvent(hMoby);
//	ResetEvent(hConnect);
//	MobySocket.Send(CMND_CONNECT, 0);
//	res = WaitForSingleObject(hMoby, timeout);
//	if (res == WAIT_OBJECT_0) {
//		pTX->ThrdIDdest = pRX->ThrdIDsrc;
//		if (pRX->Cmnd == CMND_ACK) {
//			TRACE(" ACK is received from Moby, waiting for connect %ds...", timeout);
//			if (pRX->Length > 2) {
//				timeout = *((uint16_t*)&pRX->Data[2]);
//			}
//			res = WaitForSingleObject(hConnect, timeout);
//			if (res != WAIT_OBJECT_0) {
//				TRACE("Connect not received\n");
//				retValue = COMMUNICATOR_NOT_REQUEST;
//				goto exit;
//			}
//			else {
//				TRACE("Received\n");
//			}
//		}
//		else {
//			TRACE(" ACK is not received\n");
//			retValue = COMMUNICATOR_NOT_REQUEST;
//			goto exit;
//		}
//	}
//	else {
//		retValue = COMMUNICATOR_NOT_REQUEST;
//		goto exit;
//	}
//	// Wait for connection
//	pTX->Flags.server = 1;
//	TargetID = pRX->ThrdIDsrc;
//	pTX->ThrdIDdest = TargetID;
//
//	uint8_t ConnectionChannel = pRX->Channel.v;
//	if (ConnectionChannel & NET_ETHERNET) {
//		timeout = 5000;
//	}
//	else if (ConnectionChannel & NET_CELL) {
//		if (ConnectionChannel & TECH_LTE)
//			timeout = 5000;
//		else if (ConnectionChannel & TECH_GPRS)
//			timeout = 15000;
//		else
//			goto commError;
//	}
//	else {
//	commError:
//		retValue = COMMUNICATION_ERROR;
//		goto exit;
//	}
//	// Request to connect ?
//	if (pRX->Cmnd == CMND_KEY) {
//		KeyPacket* pKeyPacket = (KeyPacket*)pTX->Data;
//		for (int i = 0; i < B32WORDS; i++)
//			pKeyPacket->key[i] = PubKey[i];
//		ResetEvent(hMoby);	
//		MobySocket.Send(CMND_KEY, sizeof(KeyPacket));
//		pKeyPacket = (KeyPacket*)pRX->Data;
//		Expm((BigNumb&)pTX->Data, (BigNumb&)pKeyPacket->key, (const NATIVE*)Exp);
//		// Calculate key
//		uint32_t* ps = (uint32_t*)pTX->Data;
//		uint32_t* pd = (uint32_t*)Key;
//		uint32_t h = 0;
//		for (int i = AES_KEY_LENGTH / 32; i > 0; --i) {	// Outer cycle number equals to the exponent words
//			for (int j = (PUB_KEY_LENGTH / 32) / (AES_KEY_LENGTH / 32); j > 0; --j) {	// Inner loop
//				h ^= *ps++;
//			}
//			*pd++ = h;
//		}
//		TRACE("Key:%02X %02X %02X %02X %02X %02X %02X %02X\n\t%02X %02X %02X %02X %02X %02X %02X %02X\n",
//			Key[0], Key[1], Key[2], Key[3],
//			Key[4], Key[5], Key[6], Key[7],
//			Key[8], Key[9], Key[10], Key[11],
//			Key[12], Key[13], Key[14], Key[15]
//		);
//		MobySocket.pKey = Key;
//		KeyExpansion(MobySocket.pKey);
//		res = WaitForSingleObject(hMoby, timeout);
//		TRACE("CMND_KEY sent. Received: R=%X Type=%X Cmnd=%02X\n", res, pRX->FrameType, pRX->Cmnd);
//		if ((res != WAIT_OBJECT_0) || (pRX->Cmnd != CMND_PASSWORD)) {
//			retValue = COMMUNICATION_ERROR;
//			goto exit;
//		}
//		pTX->Flags.encrypted = 1;
//	}
//	else if (pRX->Cmnd != CMND_PASSWORD) {
//		retValue = COMMUNICATION_ERROR;
//		goto exit;
//	}
//
//	ReqPacket* pReq = (ReqPacket*)pTX->Data;
//	TRACE("Password sent ");
//	for (int i = 0; i < 4; i++) {
//		pReq->password[i] = Pin[i];
//		TRACE("%c", Pin[i]);
//	}
//	TRACE("\n");
//	ResetEvent(hMoby);
//	MobySocket.Send(CMND_PASSWORD, sizeof(ReqPacket));
//	res = WaitForSingleObject(hMoby, timeout);
//	TRACE("Wait for Details. Received: R=%X Type=%X Cmnd=%02X\n", res, pRX->FrameType, pRX->Cmnd);
//	if (res == WAIT_OBJECT_0) {
//		if (pRX->Cmnd == CMND_DETAILS) {
//			retValue = CONNECTION_SUCCESS;
//			theApp.m_pMainWnd->PostMessage(WM_ON_CONNECT, (WPARAM)pRX, 0);
//			// Connected
//			MobySocket.Send(CMND_ACK, 0);
//			TRACE("ACK is sent\n");
//
//			pTX->Flags.server = 0;
//			TRACE("MoBy connected\n");
//			return 0;
//		}
//		else {
//			retValue = WRONG_PASSWORD;
//		}
//	}
//	else {
//		retValue = COMMUNICATION_ERROR;
//	}
//
//exit:
//	MobySocket.Send(CMND_BREAK, 0);
//endConnection:
//	theApp.m_pMainWnd->PostMessage(WM_SHOW_MESSAGE, (WPARAM)retValue, 0);
//	theApp.m_pMainWnd->PostMessage(WM_MOBY_DISCONNECTED, (WPARAM)retValue, 0);
//	return retValue;
//
//}	// UINT __cdecl procConnect(LPVOID param)
//
//
//
//class UDPSocket : public CSocket {
//public:
//	UDPSocket() : CSocket(), pKey(0) {}
//	virtual ~UDPSocket() {}
//	SOCKADDR_IN	Address;
//	int			AddrLength;
//	uint8_t *	pKey;
//	void		Send(const uint8_t Cmnd, uint16_t length);
//	void		Send(uint16_t length);
//protected:
//	virtual void OnReceive(int nErrorCode);
//};