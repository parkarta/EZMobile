
// EZMobileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EZMobile.h"
//#include "EZMobileDlg.h"
#include "..\..\CommonService.h"
#include "..\..\Packets.h"
#include <BigNumbers.hpp>
#include <aes.h>
#include <CRC.h>
#include "Socket.h"
#include "MobyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString		ServerName;
uint16_t	ServerPort;
uint32_t	ID;
CStringA	Pin;
uint8_t		Key[AES_KEY_SCHEDULE_SIZE] alignas(sizeof(size_t));
uint32_t	Exp[EXP32WORDS];
uint32_t	PubKey[B32WORDS];

uint8_t MobyProtVersionID;
uint8_t AppProtVersionID;

HANDLE hBreakMonitor;
HANDLE hResetInterval;

CWinThread * MobyMonitor;
UINT __cdecl procMonitor(LPVOID /*param*/);

// MobyDlg dialog

MobyDlg::MobyDlg(CWnd* pParent /*=nullptr*/)
	: DialogExt(IDD_EZMOBILE_DIALOG, pParent) {
	ServerName	= _T("moby.ddns.net");
	ServerPort	= 34197;
	ID			= 0x3B9AD993;
	Pin			= _T("1111");
}	// MobyDlg::MobyDlg(CWnd* pParent /*=nullptr*/)

MobyDlg::~MobyDlg() {
	CloseHandle(hMonitor);
	CloseHandle(hReceived);
	CloseHandle(hConnect);
	MobySocket.Close();
	AfxSocketTerm();
//#if 0
	AFX_MODULE_THREAD_STATE* pThreadState = AfxGetModuleThreadState();
	if (pThreadState->m_plistSocketNotifications != NULL)
		while (!pThreadState->m_plistSocketNotifications->IsEmpty())
			delete pThreadState->m_plistSocketNotifications->RemoveHead();
#ifndef _AFXDLL
	if (pThreadState->m_pmapSocketHandle != NULL) {
		delete pThreadState->m_pmapSocketHandle;
		pThreadState->m_pmapSocketHandle = NULL;
	}
	if (pThreadState->m_pmapDeadSockets != NULL) {
		delete pThreadState->m_pmapDeadSockets;
		pThreadState->m_pmapDeadSockets = NULL;
	}
	if (pThreadState->m_plistSocketNotifications != NULL) {
		delete pThreadState->m_plistSocketNotifications;
		pThreadState->m_plistSocketNotifications = NULL;
	}
#endif
//#endif
	if (MobyMonitor != 0) {
		MobyMonitor->SuspendThread();
		delete MobyMonitor;
		MobyMonitor = NULL;
	}
}	// MobyDlg::~MobyDlg()

void MobyDlg::DoDataExchange(CDataExchange* pDX) {
	__super::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER, ServerName);
	DDX_Text(pDX, IDC_EDIT_PORT, ServerPort);
	DDX_Control(pDX, IDC_EDIT_ID, EditID);
	DDX_TextFix(pDX, IDC_EDIT_ID, ID);
	DDX_TextA(pDX, IDC_EDIT_PIN, Pin);
	DDX_Control(pDX, IDI_PGM1, IconPgm1ics);
	DDX_Control(pDX, IDI_PGM2, IconPgm2ics);
	DDX_Control(pDX, IDI_PGM3, IconPgm3ics);
	DDX_Control(pDX, IDI_PGM4, IconPgm4ics);
	DDX_Control(pDX, IDI_INP1, IconInp1ics);
	DDX_Control(pDX, IDI_INP2, IconInp2ics);
	DDX_Control(pDX, IDI_INP3, IconInp3ics);
}	// void MobyDlg::DoDataExchange(CDataExchange* pDX)

BEGIN_MESSAGE_MAP(MobyDlg, DialogExt)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &MobyDlg::OnBnClickedButtonConnect)
	ON_MESSAGE(WM_ON_CONNECT, OnMobyConnect)
	ON_MESSAGE(WM_SHOW_MESSAGE, ShowMessage)
	ON_MESSAGE(WM_SET_STATE, SetState)
END_MESSAGE_MAP()

LRESULT MobyDlg::ShowMessage(WPARAM wParam, LPARAM lParam) {
	AfxMessageBox((LPCTSTR) wParam);
	return 0;
}	// LRESULT MobyDlg::ShowMessage(WPARAM wParam, LPARAM lParam)

// MobyDlg message handlers

HICON LedEmpty;
HICON LedRed;
HICON LedGrey;

BOOL MobyDlg::OnInitDialog() {
	__super::OnInitDialog();

	if (!AfxSocketInit())
		return FALSE;
	if (!MobySocket.Create(34191, SOCK_DGRAM)) {
		DWORD err = GetLastError();
		char errBuffer[200];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &errBuffer, 200, NULL);
		AfxMessageBox((LPCTSTR)errBuffer);
		return FALSE;
	}
	MobyAddress.sin_family = AF_INET;
	hConnect	= CreateEvent(NULL, FALSE, FALSE, NULL);
	hMonitor	= CreateEvent(NULL, FALSE, FALSE, NULL);
	hReceived	= CreateEvent(NULL, FALSE, FALSE, NULL);

	LedEmpty = LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_ICON_EMPTY));
	LedGrey  = LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_ICON_GREY));
	LedRed	 = LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_ICON_RED));
	IconPgm1ics.SetIcon(LedEmpty);
	IconPgm2ics.SetIcon(LedEmpty);
	IconPgm3ics.SetIcon(LedEmpty);
	IconPgm4ics.SetIcon(LedEmpty);
	IconInp1ics.SetIcon(LedEmpty);
	IconInp2ics.SetIcon(LedEmpty);
	IconInp3ics.SetIcon(LedEmpty);

	return TRUE;  // return TRUE  unless you set the focus to a control
}	// BOOL CEZMobileDlg::OnInitDialog()

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void MobyDlg::OnPaint() {
	if (IsIconic()) {
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
//		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}
}	// void MobyDlg::OnPaint()

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
//HCURSOR MobyDlg::OnQueryDragIcon() {
//	return static_cast<HCURSOR>(m_hIcon);
//}

void MobyDlg::OnBnClickedButtonConnect() {
extern UINT __cdecl procConnect(LPVOID param);
	UpdateData();
	CWinThread * p = AfxBeginThread(procConnect, 0);
}

const TCHAR * ErrorStrings[] = {
								  _T("Server socket timeout")			// 0
								, _T("Server error")					// 1
								, _T("Client is offline")				// 2
								, _T("Communication error")				// 3
								, _T("Communicator does not request")	// 4
								, _T("Connection success")				// 5
								, _T("Wrong password")					// 6
								, _T("Moby timeout")					// 7
							  };

#define SERVER_TIMEOUT				0
#define SERVER_ERROR				1
#define CLIENT_OFFLINE				2
#define COMMUNICATION_ERROR			3
#define COMMUNICATOR_NOT_REQUEST	4
#define CONNECTION_SUCCESS			5
#define WRONG_PASSWORD				6
#define MOBY_TIMEOUT				7

UINT __cdecl procConnect(LPVOID param) {
uint32_t	timeout = 3000;
UINT		retValue;
	// Ask Server about Moby address
	MobyComFrame *pTX = (MobyComFrame *) txBuffer;
	pTX->ProtID		= MobyComID;
	pTX->ProtVer	= MobyComVer;
	pTX->AppVerID	= MOBILE_APP_VERSION_ID;
	pTX->Flags.v	= 0;
	pTX->ClientID	= ID;
	pTX->Channel.v	= NET_ETHERNET;		//! Change for cellular application
	pTX->FrameID	= 1;
	pTX->Cmnd		= CMND_GET_CONNECT;
	pTX->ThrdIDsrc	= 1;
	pTX->ThrdIDdest	= TARG_LISTEN;
	int length = FinishMobyComFrame(pTX, 0, 0);
	MobySocket.SendTo(pTX, length, ServerPort, ServerName);
	DWORD res = WaitForSingleObject(hConnect, 5000);
	if (res != WAIT_OBJECT_0) {
		retValue = SERVER_TIMEOUT;
		goto endConnection;
	}
	MobyComFrame * pRX = (MobyComFrame *) rxBuffer;
	if (pRX->Cmnd != CMND_GET_CONNECT) {
		retValue = SERVER_ERROR;
		goto endConnection;
	}
	GetTargetIPPacket * pTarget = (GetTargetIPPacket *) pRX->Data;
	u32v ip;
	uint16_t port;
	if (pTarget->channels & NET_ETHERNET) {
		ip.v = pTarget->ethIP;
		port = pTarget->ethPort;
	} else if (pTarget->channels & NET_CELL) {
		ip.v = pTarget->cellIP;
		port = pTarget->cellPort;
	} else {
		retValue = CLIENT_OFFLINE;
		goto endConnection;
	}
	MobyAddress.sin_addr.S_un.S_un_b.s_b1 = ip.b0;
	MobyAddress.sin_addr.S_un.S_un_b.s_b2 = ip.b1;
	MobyAddress.sin_addr.S_un.S_un_b.s_b3 = ip.b2;
	MobyAddress.sin_addr.S_un.S_un_b.s_b4 = ip.b3;
	MobyAddress.sin_port = port;
//	MobyAddress.sin_port = ntohs(port);

	// Connect directly
	pTX->Flags.altID= 1;
	pTX->ClientID	= EZMobileID;
	pTX->ThrdIDsrc	= COMMAND_ID;
	pTX->ThrdIDdest	= TARG_LISTEN;
	pTX->FrameID	= 0;

	MobySocket.Send(CMND_CONNECT, 0);
	res = WaitForSingleObject(hConnect, timeout);
	if (res == WAIT_OBJECT_0) {
		if (pRX->Cmnd == CMND_ACK) {
			TRACE(" ACK is received from Moby\n");
			if (pRX->Length > 2) {
				timeout = *((uint16_t*)&pRX->Data[2]);
			}
			res = WaitForSingleObject(hConnect, timeout);
		}
	}
	// Wait for connection
	pTX->Flags.server = 1;
	if (res != WAIT_OBJECT_0) {
		retValue = COMMUNICATOR_NOT_REQUEST;
		goto exit;
	}

	uint8_t ConnectionChannel = pRX->Channel.v;
	if (ConnectionChannel & NET_ETHERNET) {
		timeout = 5000;
	} else if (ConnectionChannel & NET_CELL) {
		if (ConnectionChannel & TECH_LTE)
			timeout = 5000;
		else if (ConnectionChannel & TECH_GPRS)
			timeout = 15000;
		else
			goto commError;
	} else {
	  commError:
		retValue = COMMUNICATION_ERROR;
		goto exit;
	}
	// Request to connect ?
	MobyProtVersionID = pRX->ProtVer >= MobyComVer ? MobyComVer : pRX->ProtVer;
	pTX->ProtVer	= MobyProtVersionID;
	AppProtVersionID= pRX->AppVerID;
	pTX->AppVerID	= AppProtVersionID;
	pTX->ThrdIDdest = pRX->ThrdIDsrc;
	if (pRX->Cmnd == CMND_KEY) {
		KeyPacket * pKeyPacket = (KeyPacket *) pTX->Data;
		for (int i = 0; i < B32WORDS; i++)
			pKeyPacket->key[i] = PubKey[i];
		MobySocket.Send(pRX->ProtVer >= 2 ? CMND_KEY : CMND_KEY_0, sizeof(KeyPacket));
		pKeyPacket = (KeyPacket *) pRX->Data;
		Expm((BigNumb&)pTX->Data, (BigNumb&)pKeyPacket->key, (const NATIVE *) Exp);
		// Calculate key
		if (MobyProtVersionID >= 2) {
			uint32_t * ps = (uint32_t *) pTX->Data;
			uint32_t * pd = (uint32_t *) Key;
			uint32_t h	  = 0;
			for (int i = AES_KEY_LENGTH/32; i > 0; --i) {	// Outer cycle number equals to the exponent words
				for (int j = (PUB_KEY_LENGTH/32)/(AES_KEY_LENGTH/32); j > 0; --j) {	// Inner loop
					h ^= *ps++;
				}
				*pd++ = h;
			}
		} else {
			uint8_t  * ps = pTX->Data;
			uint16_t * pd = (uint16_t *) Key;
			for (int i = 0; i < 8; i++) {
				*pd++ = CRC16(ps, 16);
				ps += 16;
			}
		}
		TRACE("Key:%02X %02X %02X %02X %02X %02X %02X %02X\n\t%02X %02X %02X %02X %02X %02X %02X %02X\n",
			Key[0], Key[1], Key[2], Key[3],
			Key[4], Key[5], Key[6], Key[7],
			Key[8], Key[9], Key[10], Key[11],
			Key[12], Key[13], Key[14], Key[15]
		);
		MobySocket.pKey = Key;
		KeyExpansion(MobySocket.pKey);
		res = WaitForSingleObject(hReceived, timeout);
		TRACE("CMND_KEY sent. Received: R=%X Type=%X Cmnd=%02X\n", res, pRX->FrameType, pRX->Cmnd);
		if ((res != WAIT_OBJECT_0) || (pRX->Cmnd != (pRX->ProtVer >= 2 ? CMND_PASSWORD : CMND_PASSWORD_0))) {
			retValue = COMMUNICATION_ERROR;
			goto exit;
		}
		pTX->Flags.encrypted = 1;
	} else if (pRX->Cmnd != (pRX->ProtVer >= 2 ? CMND_PASSWORD : CMND_PASSWORD_0)) {
		retValue = COMMUNICATION_ERROR;
		goto exit;
	}

	ReqPacket * pReq = (ReqPacket *) pTX->Data;
	TRACE("Password sent ");
	for (int i = 0; i < 4; i++) {
		pReq->password[i] = Pin[i];
		TRACE("%c", Pin[i]);
	}
	TRACE("\n");
	MobySocket.Send(pTX->ProtVer >= 2 ? CMND_PASSWORD : CMND_PASSWORD_0, sizeof(ReqPacket));
	res = WaitForSingleObject(hReceived, timeout);
	TRACE("Wait for Details. Received: R=%X Type=%X Cmnd=%02X\n", res, pRX->FrameType, pRX->Cmnd);
	if ((res == WAIT_OBJECT_0) && (pRX->Cmnd == CMND_DETAILS)) {
		retValue = CONNECTION_SUCCESS;
		theApp.m_pMainWnd->PostMessage(WM_ON_CONNECT, (WPARAM) pRX, 0);
	} else {
		if ((res == WAIT_OBJECT_0) && (pRX->FrameType == FRAME_COMMAND))
			retValue = WRONG_PASSWORD;
		else
			retValue = COMMUNICATION_ERROR;
		goto exit;
	}

	// Request to connect
	MobySocket.Send(CMND_ACK, 0);
	TRACE("ACK is sent\n");

	pTX->Flags.server = 0;
	TRACE("MoBy connected\n");
exit:
	if (retValue == CONNECTION_SUCCESS) {
		return 0;
	} else {
		MobySocket.Send(CMND_BREAK, 0);
	}
endConnection:
	theApp.m_pMainWnd->PostMessage(WM_SHOW_MESSAGE, (WPARAM) retValue, 0);
	return retValue;

}	// UINT __cdecl procConnect(LPVOID param)

uint8_t	Inputs;
bool	bGsm;
bool	bTamper;
uint8_t	nPGM;
IOState State;

LRESULT MobyDlg::OnMobyConnect(WPARAM wparam, LPARAM lparam) {
MobyComFrame * pRX = (MobyComFrame *) wparam;
DetailsPacket * p = (DetailsPacket *) pRX->Data;
	uint8_t device = p->board.device;
	uint8_t ModelCode = p->board.model;
	uint32_t softRev = p->softRev.v;
	Inputs	= 2;
	bTamper	= true;
	bGsm	= true;
	nPGM	= 0;
	CStringA	Model;
	if (device == DEV_MOBY) {
		switch (ModelCode) {
		case CODE_MOBY_S:
			Model	= "MoBy-S";
			nPGM	= 2;
			bGsm	= false;
			break;
		case CODE_MOBY_SP:
			Model	= "MoBy-SP";
			Inputs	= 0;
			break;
		case CODE_MOBY_SG:
			Model	= "MoBy-SG";
			nPGM	= 2;
			break;
		case CODE_MOBY_SGE:
			Model	= "MoBy-SGE";
			nPGM	= 2;
			break;
		case CODE_MOBY_U:
			Model	= "MoBy-U";
			nPGM	= 4;
			bGsm	= false;
			break;
		case CODE_MOBY_UG:
			Model	= "MoBy-UG";
			nPGM	= 4;
			break;
		case CODE_MOBY_UGE:
			Model	= "MoBy-UGE";
			nPGM	= 4;
			break;
		case CODE_MOBY_UEC:
			Model	= "MoBy-UEC";
			nPGM	= 3;
			Inputs	= 3;
			break;
		case CODE_MOBY_UGC:
			Model	= "MoBy-UGC";
			nPGM	= 3;
			Inputs	= 3;
			break;
		case CODE_MOBY_R:
			Model	= "MoBy-R";
			Inputs	= 3;
			nPGM	= 3;
			bGsm	= false;
			break;
		case CODE_MOBY_RG:
			Model	= "MoBy-RG";
			Inputs	= 3;
			nPGM	= 3;
			break;
		case CODE_MOBY_RGE:
			Model	= "MoBy-RGE";
			Inputs	= 3;
			nPGM	= 3;
			break;
		default:
			Model	= "Unknown model!";
			ModelCode = UNKNOWN_CODE;
			bTamper	= false;
			bGsm	= false;
			break;
		}	// switch (ModelCode)
	} else {	// if (device == DEV_MOBY)
		Model		= "Unknown Device!";
		ModelCode	= UNKNOWN_CODE;
		bTamper		= false;
		bGsm		= false;
	}
	SetThisDlgItemTextA(IDC_EDIT_MODEL, Model);
	Model.Empty();
	if (ModelCode != UNKNOWN_CODE) {
		Model = (char)(p->board.hwRev + '0');
		Model += (char)('.');
		Model += (char)(p->board.hwSubRev + '0');
	}
	SetThisDlgItemTextA(IDC_EDIT_HW, Model);
	uint32_t SoftRev = p->softRev.v;
	Model.Format("%u.%u.%u.%u", p->softRev.b3, p->softRev.b2, p->softRev.b1, p->softRev.b0);
	SetThisDlgItemTextA(IDC_EDIT_FIRMWARE, Model);

	// Set State
	State.Inp = p->Inp;
	State.Pgm = p->Pgm;
	State.TamperJamming =  p->tamper;
	State.TamperJamming |= p->jamming << 1;
	State.Rssi = p->rssi;

	SetState(0, 0);

	return 0;
}	// LRESULT MobyDlg::OnMobyConnect(WPARAM wparam, LPARAM lparam)

LRESULT MobyDlg::SetState(WPARAM wparam, LPARAM lparam) {
	if (nPGM > 0) {
		if ((State.Pgm & 1) == 0)
			IconPgm1ics.SetIcon(LedEmpty);
		else
			IconPgm1ics.SetIcon(LedRed);
		if (nPGM > 1) {
			if ((State.Pgm & 2) == 0)
				IconPgm2ics.SetIcon(LedEmpty);
			else
				IconPgm2ics.SetIcon(LedRed);
			if (nPGM > 2) {
				if ((State.Pgm & 4) == 0)
					IconPgm3ics.SetIcon(LedEmpty);
				else
					IconPgm3ics.SetIcon(LedRed);
				if (nPGM > 3) {
					if ((State.Pgm & 8) == 0)
						IconPgm4ics.SetIcon(LedEmpty);
					else
						IconPgm4ics.SetIcon(LedRed);
				} else {
					IconPgm4ics.SetIcon(LedGrey);
				}
			} else {
				IconPgm3ics.SetIcon(LedGrey);
				IconPgm4ics.SetIcon(LedGrey);
			}
		} else {
			IconPgm2ics.SetIcon(LedGrey);
			IconPgm3ics.SetIcon(LedGrey);
			IconPgm4ics.SetIcon(LedGrey);
		}
	} else {
		IconPgm1ics.SetIcon(LedGrey);
		IconPgm2ics.SetIcon(LedGrey);
		IconPgm3ics.SetIcon(LedGrey);
		IconPgm4ics.SetIcon(LedGrey);
	}

	if (Inputs > 0) {
		if ((State.Inp & 1) == 0)
			IconInp1ics.SetIcon(LedEmpty);
		else
			IconInp1ics.SetIcon(LedRed);
		if (Inputs > 1) {
			if ((State.Inp & 2) == 0)
				IconInp2ics.SetIcon(LedEmpty);
			else
				IconInp2ics.SetIcon(LedRed);
			if (Inputs > 2) {
				if ((State.Inp & 4) == 0)
					IconInp3ics.SetIcon(LedEmpty);
				else
					IconInp3ics.SetIcon(LedRed);
			} else {
				IconInp3ics.SetIcon(LedGrey);
			}
		} else {
			IconInp2ics.SetIcon(LedGrey);
			IconInp3ics.SetIcon(LedGrey);
		}
	} else {
		IconInp1ics.SetIcon(LedGrey);
		IconInp2ics.SetIcon(LedGrey);
		IconInp3ics.SetIcon(LedGrey);
	}

	UpdateData(FALSE);
	return 0;
}	// LRESULT MobyDlg::SetState(WPARAM wparam, LPARAM lparam)

UINT __cdecl procMonitor(LPVOID /*param*/) {
HANDLE		wEvent[3];
DWORD		res;
int			count = 0;
MobyComFrame*	pTX = (MobyComFrame *) MontxBuffer;
MobyComFrame*	pRX = (MobyComFrame *) MonrxBuffer;

	TRACE("procMonitor is started\n");
	pTX->ProtID		= MobyComID;
	pTX->ProtVer	= MobyProtVersionID;
	pTX->AppVerID	= AppProtVersionID;
	pTX->ClientID	= ID;
	pTX->ThrdIDsrc	= COMMAND_ID;
	pTX->Flags.v	= (MobySocket.pKey != 0 ? MskEncrypted : 0);
	pTX->ThrdIDdest	= ((MobyComFrame *) txBuffer)->ThrdIDsrc;
	pTX->FrameID	= 0;
	hBreakMonitor	= CreateEvent(NULL, false, false, NULL);
	hMonitor		= CreateEvent(NULL, false, false, NULL);
	hResetInterval	= CreateEvent(NULL, false, false, NULL);
	DWORD Interval  = 5000;

	while (true) {
		wEvent[0] = hResetInterval;
		wEvent[1] = hMonitor;
		wEvent[2] = hBreakMonitor;
		ResetEvent(hResetInterval);
		do {
			res = WaitForMultipleObjects(3, wEvent, false, Interval);
		} while (res == WAIT_OBJECT_0);
		if (res == (WAIT_OBJECT_0 + 1))
			goto processState;
		if (res == (WAIT_OBJECT_0 + 2))
			break;
		wEvent[0] = hMonitor;
		wEvent[1] = hBreakMonitor;
		for (count = 3; count > 0; count--) {
//			TRACE("CMND_STATE sent\n");
			pRX->Cmnd = 0;
			MobySocket.Send(CMND_STATE, 0);
			res = WaitForMultipleObjects(2, wEvent, false, 2000);
//			TRACE("Monitor RX: res = %X, Cmnd = %02X State: %02X %02X\n", res, pRX->Cmnd, pRX->Data[0], pRX->Data[1]);
			if (res == WAIT_OBJECT_0) {
			  processState:
				if (pRX->Cmnd == CMND_STATE) {
					StatePacket * p = (StatePacket *) &pRX->Data[0];
					State.Inp = p->Inp;
					State.Pgm = p->Pgm;
					State.TamperJamming =  p->tamper;
					State.TamperJamming |= p->jamming << 1;
					State.Rssi = p->rssi;
					theApp.m_pMainWnd->PostMessage(WM_SET_STATE, (WPARAM) 0, (LPARAM) 0);
					break;
				} else {
					TRACE("Wrong frame\n");
					continue;
				}
			} else if (res == (WAIT_OBJECT_0 + 1)) {
				TRACE("Break event. Disconnect() is called\n");
				goto exitMonitor;
			} else if (res == WAIT_TIMEOUT) {
				TRACE("Monitor timeout\n");
			}
		}	// for (count = 3; count > 0; count--)
		if (count == 0) {
			theApp.m_pMainWnd->PostMessage(WM_SHOW_MESSAGE, MOBY_TIMEOUT, 0);
			TRACE("Timeout is expired\n");
//			theApp.m_pMainWnd->PostMessage(WM_MOBY_DISCONNECTED, 0, 0);
			break;
		}
		TRACE("\n");
	}
exitMonitor:
	CloseHandle(hBreakMonitor);
	CloseHandle(hResetInterval);
	CloseHandle(hMonitor);
	hBreakMonitor = 0;
	MobyMonitor = 0;
	TRACE("Exit MobyMonitor\n");
	return 0;
}	// UINT __cdecl procMonitor(LPVOID param)

