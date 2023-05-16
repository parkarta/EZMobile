/*
*************************************************************************************
* Socket																			*
*************************************************************************************
*/
#include "stdafx.h"
#include "MobyDlg.h"
#include "..\..\CommonService.h"
#include "Socket.h"

UDPSocket	MobySocket;
UDPSocket	StateSocket;
HANDLE		hConnect;
HANDLE		hMonitor;
HANDLE		hReceived;
SOCKADDR_IN	MobyAddress;
extern bool		Connected;

// UDPSocket member functions
uint8_t	rxBuffer[RX_SIZE] alignas(sizeof(size_t));
uint8_t	txBuffer[TX_SIZE] alignas(sizeof(size_t));
uint8_t	MonrxBuffer[RX_SIZE] alignas(sizeof(size_t));
uint8_t	MontxBuffer[TX_SIZE] alignas(sizeof(size_t));

void UDPSocket::OnReceive(int nErrorCode) {
//	SOCKADDR_IN	conAddress;
	if (!nErrorCode) {
		AddrLength = sizeof(SOCKADDR_IN);
		int N = ReceiveFrom(rxBuffer, RX_SIZE, (SOCKADDR *) &Address, &AddrLength);	// retrive message
		if ((N > 0) && (rxBuffer[0] == MobyComID)) {
			MobyComFrame * pRX = (MobyComFrame *) rxBuffer;
			// Decrypt and check CRC
			uint8_t frameError = CheckMobyComFrame(pRX, (uint8_t*)pKey);
			if (frameError == 0) {
				if (pRX->ProtVer >= 2) {
					if (pRX->ThrdIDdest == 2) {
						SetEvent(hReceived);
					} else if (pRX->ThrdIDdest == 3) {
						for (int i = 0; i < N; i++)
							MonrxBuffer[i] = rxBuffer[i];
						SetEvent(hMonitor);
					} else if (pRX->ThrdIDdest <= 1) {
						SetEvent(hConnect);
					} else {
						frameError = ERROR_BAD_PARAM;
					}
				} else {
					frameError = ERROR_BAD_VERSION;
				}
			}	// if (frameError == 0)
		} else {	// if ((N > 0) && (rxBuffer[0] == MobyComID))
			if (N < 0) {
				LPVOID	messBuffer;
				DWORD error = GetLastError();
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &messBuffer, 100, NULL);
				AfxMessageBox((LPTSTR) messBuffer);
			}
		}
	}	// if (!nErrorCode)
	CSocket::OnReceive(nErrorCode);
}	// void UDPSocket::OnReceive(int nErrorCode)

void UDPSocket::Send(const uint8_t Cmnd, uint16_t Length) {
MobyComFrame * pTX;
	if (Cmnd == CMND_STATE) {
		pTX = (MobyComFrame *) MontxBuffer;
		ResetEvent(hMonitor);
	} else {
		pTX = (MobyComFrame *)txBuffer;
		ResetEvent(hReceived);
	}
	pTX->Cmnd = Cmnd;
	uint16_t length = FinishMobyComFrame(pTX, Length, pKey);
	CSocket::SendTo(pTX, length, (const SOCKADDR*) &MobyAddress, sizeof(SOCKADDR));
}	// void UDPSocket::Send(const uint8_t Cmnd, uint16_t Length)
