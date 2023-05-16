#pragma once

#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <MobyCom.h>
#include <afxsock.h>

#define TX_SIZE		256
#define RX_SIZE		256

// UDPSocket command target

class UDPSocket : public CSocket {
public:
	UDPSocket() : CSocket(), pKey(0) {}
	virtual ~UDPSocket() {}
	SOCKADDR_IN	Address;
	int			AddrLength;
	uint8_t *	pKey;
	void		Send(const uint8_t Cmnd, uint16_t length);
protected:
	virtual void OnReceive(int nErrorCode);
};

extern UDPSocket	MobySocket;
extern UDPSocket	StateSocket;
extern SOCKADDR_IN	MobyAddress;
extern UINT			UDPPort;
extern HANDLE		hConnect;
extern HANDLE		hMonitor;
extern HANDLE		hReceived;
extern uint8_t		rxBuffer[TX_SIZE] alignas(sizeof(size_t));
extern uint8_t		txBuffer[RX_SIZE] alignas(sizeof(size_t));
extern uint8_t		MonrxBuffer[] alignas(sizeof(size_t));
extern uint8_t		MontxBuffer[] alignas(sizeof(size_t));

#endif

