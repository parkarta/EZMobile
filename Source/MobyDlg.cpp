
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
#include <Trace.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString		ServerName;
uint16_t	ServerPort;
uint32_t	ID;
CStringA	Pin;
bool		btnConnect = true;
uint8_t		Key[AES_KEY_SCHEDULE_SIZE] alignas(sizeof(size_t));
uint32_t	Exp[EXP32WORDS];
uint32_t	PubKey[B32WORDS];

uint8_t		Inputs;
bool		bGsm;
bool		bTamper;
uint8_t		nPGM;
uint8_t		TargetID;
StatesStruct State;

HANDLE hBreakMonitor;
HANDLE hResetInterval;

CWinThread * MobyMonitor = NULL;
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
	CloseHandle(hMonitor);
	CloseHandle(hMoby);
	CloseHandle(hConnect);
	CloseHandle(hBreakMonitor);
	CloseHandle(hResetInterval);
}	// MobyDlg::~MobyDlg()

void MobyDlg::DoDataExchange(CDataExchange* pDX) {
	__super::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER, ServerName);
	DDX_Text(pDX, IDC_EDIT_PORT, ServerPort);
	DDX_Control(pDX, IDC_EDIT_ID, EditID);
	DDX_TextFix(pDX, IDC_EDIT_ID, ID);
	DDX_TextA(pDX, IDC_EDIT_PIN, Pin);
	DDX_Control(pDX, IDI_PGM1, IconPgm[0]);
	DDX_Control(pDX, IDI_PGM2, IconPgm[1]);
	DDX_Control(pDX, IDI_PGM3, IconPgm[2]);
	DDX_Control(pDX, IDI_PGM4, IconPgm[3]);
	DDX_Control(pDX, IDI_INP1, IconInp[0]);
	DDX_Control(pDX, IDI_INP2, IconInp[1]);
	DDX_Control(pDX, IDI_INP3, IconInp[2]);
}	// void MobyDlg::DoDataExchange(CDataExchange* pDX)

BEGIN_MESSAGE_MAP(MobyDlg, DialogExt)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &MobyDlg::OnBnClickedButtonConnect)
	ON_MESSAGE(WM_ON_CONNECT, OnMobyConnect)
	ON_MESSAGE(WM_MOBY_DISCONNECTED, OnMobyDisconnect)
	ON_MESSAGE(WM_SHOW_MESSAGE, ShowMessage)
	ON_MESSAGE(WM_SET_STATE, SetState)
	ON_MESSAGE(WM_KEY_READY, OnKeyReady)
	ON_MESSAGE(WM_COMMAND_EXECUTED, OnCommandExecuted)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_PGM1, IDC_BUTTON_PGM4, &OnBnClickedPgm)
END_MESSAGE_MAP()

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

LRESULT MobyDlg::ShowMessage(WPARAM wParam, LPARAM lParam) {
	AfxMessageBox(ErrorStrings[wParam]);
	return 0;
}	// LRESULT MobyDlg::ShowMessage(WPARAM wParam, LPARAM lParam)

// MobyDlg message handlers

HICON LedEmpty;
HICON LedRed;
HICON LedGrey;

int volatile dummy;

UINT __cdecl procKeyGeneration() {
	static LONGLONG hrFreq, hrCount0, hrCount1;
	TRACE("Generating key... ");
	if (!QueryPerformanceFrequency((LARGE_INTEGER *)&hrFreq))
		return 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&hrCount0);
	for (int i = 0; i < B32WORDS; i++) {
		UINT32 b = 0;
		for (int j = 0; j < 32; j++) {
			for (int k = 0; k < 1000000; k++)
				dummy++;
			QueryPerformanceCounter((LARGE_INTEGER *)&hrCount1);
			b <<= 1;
			b |= (UINT32)((hrCount1 - hrCount0) & 1);
			hrCount0 = hrCount1;
		}
		PubKey[i] = b;
	}	// for (int i = 0; i < EXPBYTES; i++)
	TRACE("Part 1... ");
	// Hashing to obtain 256-bit exponent from 1024-bit random number
	uint32_t h = 0;
	uint32_t *pRand = &PubKey[0];
	uint32_t *pExp	= &Exp[0];
	for (int i = EXP_LENGTH/32; i > 0; --i) {	// Outer cycle number equals to the exponent words
		for (int j = (PUB_KEY_LENGTH/32)/(EXP_LENGTH/32); j > 0; --j) {	// Inner loop
			h ^= *pRand++;
		}
		*pExp++ = h;
	}
	Exp2m((BigNumb&)PubKey, (const NATIVE *) Exp);
	TRACE("Done\n");
	theApp.m_pMainWnd->PostMessage(WM_KEY_READY, 0, 0);
	return 0;
}	// UINT __cdecl procKeyGeneration()

LRESULT MobyDlg::OnKeyReady(WPARAM, LPARAM) {
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(true);
	return 0;
}	// LRESULT MobyDlg::OnKeyReady(WPARAM, LPARAM)

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
	hMonitor		= CreateEvent(NULL, FALSE, FALSE, NULL);
	hMoby			= CreateEvent(NULL, FALSE, FALSE, NULL);
	hConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	hBreakMonitor	= CreateEvent(NULL, FALSE, FALSE, NULL);
	hResetInterval	= CreateEvent(NULL, FALSE, FALSE, NULL);

	LedEmpty = LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_ICON_EMPTY));
	LedGrey  = LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_ICON_GREY));
	LedRed	 = LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_ICON_RED));
	nPGM	 = 0;
	Inputs	 = 0;
	SetState(0, 0);

	AfxBeginThread((AFX_THREADPROC) procKeyGeneration, 0);
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

UINT __cdecl procCommand(LPVOID par) {
int	length = PtrToInt(par);
MobyComFrame * pTX = (MobyComFrame *) txBuffer;
	pTX->ThrdIDdest	= 0;
	pTX->ThrdIDsrc	= 2;
	MobySocket.Send(length);
	TRACE("Command is sent... res = ");
	DWORD res = WaitForSingleObject(hMoby, 3000);
	TRACE("%d ", res);
	if (res == WAIT_OBJECT_0) {
		TRACE("Reset monitor");
		SetEvent(hResetInterval);
	}
	TRACE("\n");
	theApp.m_pMainWnd->PostMessage(WM_COMMAND_EXECUTED, (WPARAM) res, 0);
	return 0;
}	// UINT __cdecl procCommand(LPVOID par)

void MobyDlg::OnBnClickedPgm(UINT id) {
uint8_t	pgmIndex;
CWinThread * thrdCommand;
	pgmIndex = (uint8_t) (id - IDC_BUTTON_PGM1);
	if (pgmIndex >= MAX_PGMS) {
		AfxMessageBox(_T("Wrong PGM number"), MB_OK | MB_ICONERROR);
		return;
	}
	// Prepare the buffer
	MobyComFrame * pTX	= (MobyComFrame *) txBuffer;
	PgmPacket * pCmnd	= (PgmPacket *) pTX->Data;
	pTX->Cmnd	= CMND_PGM;
	pCmnd->id	= pgmIndex;
	thrdCommand = AfxBeginThread((AFX_THREADPROC)procCommand
								, (LPVOID)sizeof(PgmPacket)
								, THREAD_PRIORITY_NORMAL
								, 0
								, 0
								, NULL);
	TRACE("thrdCommand Pgm %p\n", thrdCommand);
//	if (thrdCommand != NULL) {
//		BeginWaitCursor();
//	}
}	// void MobyDlg::OnBnClickedPgm(UINT id)

LRESULT MobyDlg::OnCommandExecuted(WPARAM wparam, LPARAM lparam) {
	if (wparam == WAIT_OBJECT_0) {
		MobyComFrame * pRX = (MobyComFrame *) rxBuffer;
		PgmReplyPacket *pReply = (PgmReplyPacket *) pRX->Data;
		if (!FrameError(pRX)) {
			if (pRX->Cmnd == CMND_PGM) {
				uint8_t pgmIndex = pReply->id;
				if (pReply->state == 1) {
					IconPgm[pgmIndex].SetIcon(LedRed);
				} else {
					IconPgm[pgmIndex].SetIcon(LedEmpty);
				}
				UpdateData(false);
			} else {	// not CMND_PGM
				AfxMessageBox(_T("Communication error"), MB_OK | MB_ICONERROR);
			}
		} else {	// if (Reply->FrameType == FRAME_COMMAND)
			if (FrameError(pRX)) {
				if (pRX->Error == ERROR_UNIMPLEMENTED)
					AfxMessageBox(_T("Command is not implemented"), MB_OK | MB_ICONEXCLAMATION);
				else if (pRX->Error == ERROR_BAD_PARAM)
					AfxMessageBox(_T("Bad parameter"), MB_OK | MB_ICONERROR);
				else
					AfxMessageBox(_T("Error"), MB_OK | MB_ICONEXCLAMATION);
			} else {
				AfxMessageBox(_T("Communication error"), MB_OK | MB_ICONERROR);
			}
		}
	} else
		AfxMessageBox(_T("Moby doesn't respond"), MB_OK | MB_ICONERROR);
	return 0;
}	// LRESULT MobyDlg::OnCommandExecuted(WPARAM wparam, LPARAM lparam)

void MobyDlg::OnBnClickedButtonConnect() {
	if (btnConnect) {
		extern UINT __cdecl procConnect(LPVOID param);
		UpdateData();
		CWinThread * p = AfxBeginThread(procConnect, 0);
		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(false);
	} else {
		// Disconnect
		OnMobyDisconnect(0, 0);
	}
}	// void MobyDlg::OnBnClickedButtonConnect()

LRESULT MobyDlg::OnMobyDisconnect(WPARAM, LPARAM) {
	MobyComFrame * pTX = (MobyComFrame *) txBuffer;
	pTX->ThrdIDdest = TargetID;
	MobySocket.Send(CMND_BREAK, 0);

	SetThisDlgItemTextA(IDC_EDIT_MODEL, "");
	SetThisDlgItemTextA(IDC_EDIT_HW, "");
	SetThisDlgItemTextA(IDC_EDIT_FIRMWARE, "");

	nPGM	 = 0;
	Inputs	 = 0;
	SetState(0, 0);

	btnConnect = true;
	SetDlgItemText(IDC_BUTTON_CONNECT, _T("Connect"));
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(true);
	if (MobyMonitor != NULL) {
		SetEvent(hBreakMonitor);
	}
	MobySocket.pKey = 0;
	return 0;
}	// LRESULT MobyDlg::OnMobyDisconnect(WPARAM, LPARAM)

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
	pTX->ThrdIDsrc	= COMMAND_ID;
	pTX->ThrdIDdest	= TARG_LISTEN;
	int length = FinishMobyComFrame(pTX, 0, 0);
	ResetEvent(hMoby);
	MobySocket.SendTo(pTX, length, ServerPort, ServerName);
	DWORD res = WaitForSingleObject(hMoby, 5000);
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
	u32s ip;
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
	TRACE("Moby is at %u.%u.%u.%u:%u\n", ip.b0, ip.b1, ip.b2, ip.b3, ntohs(port));

	// Connect directly
	pTX->Flags.altID= 1;
	pTX->ClientID	= EZMobileID;
	pTX->ThrdIDsrc	= COMMAND_ID;
	pTX->ThrdIDdest	= TARG_LISTEN;
	pTX->FrameID	= 0;

	ResetEvent(hMoby);
	ResetEvent(hConnect);
	MobySocket.Send(CMND_CONNECT, 0);
	res = WaitForSingleObject(hMoby, timeout);
	if (res == WAIT_OBJECT_0) {
		pTX->ThrdIDdest = pRX->ThrdIDsrc;
		if (pRX->Cmnd == CMND_ACK) {
			TRACE(" ACK is received from Moby, waiting for connect %ds...", timeout);
			if (pRX->Length > 2) {
				timeout = *((uint16_t*)&pRX->Data[2]);
			}
			res = WaitForSingleObject(hConnect, timeout);
			if (res != WAIT_OBJECT_0) {
				TRACE("Connect not received\n");
				retValue = COMMUNICATOR_NOT_REQUEST;
				goto exit;
			} else {
				TRACE("Received\n");
			}
		} else {
			TRACE(" ACK is not received\n");
			retValue = COMMUNICATOR_NOT_REQUEST;
			goto exit;
		}
	} else {
		retValue = COMMUNICATOR_NOT_REQUEST;
		goto exit;
	}
	// Wait for connection
	pTX->Flags.server = 1;
	TargetID = pRX->ThrdIDsrc;
	pTX->ThrdIDdest = TargetID;

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
	if (pRX->Cmnd == CMND_KEY) {
		KeyPacket * pKeyPacket = (KeyPacket *) pTX->Data;
		for (int i = 0; i < B32WORDS; i++)
			pKeyPacket->key[i] = PubKey[i];
		ResetEvent(hMoby);
		MobySocket.Send(CMND_KEY, sizeof(KeyPacket));
		pKeyPacket = (KeyPacket *) pRX->Data;
		Expm((BigNumb&)pTX->Data, (BigNumb&)pKeyPacket->key, (const NATIVE *) Exp);
		// Calculate key
		uint32_t * ps = (uint32_t *) pTX->Data;
		uint32_t * pd = (uint32_t *) Key;
		uint32_t h	  = 0;
		for (int i = AES_KEY_LENGTH/32; i > 0; --i) {	// Outer cycle number equals to the exponent words
			for (int j = (PUB_KEY_LENGTH/32)/(AES_KEY_LENGTH/32); j > 0; --j) {	// Inner loop
				h ^= *ps++;
			}
			*pd++ = h;
		}
		TRACE("Key:%02X %02X %02X %02X %02X %02X %02X %02X\n\t%02X %02X %02X %02X %02X %02X %02X %02X\n",
			Key[ 0], Key[ 1], Key[ 2], Key[ 3],
			Key[ 4], Key[ 5], Key[ 6], Key[ 7],
			Key[ 8], Key[ 9], Key[10], Key[11],
			Key[12], Key[13], Key[14], Key[15]
		);
		MobySocket.pKey = Key;
		KeyExpansion(MobySocket.pKey);
		res = WaitForSingleObject(hMoby, timeout);
		TRACE("CMND_KEY sent. Received: R=%X Type=%X Cmnd=%02X\n", res, pRX->FrameType, pRX->Cmnd);
		if ((res != WAIT_OBJECT_0) || (pRX->Cmnd != CMND_PASSWORD)) {
			retValue = COMMUNICATION_ERROR;
			goto exit;
		}
		pTX->Flags.encrypted = 1;
	} else if (pRX->Cmnd != CMND_PASSWORD) {
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
	ResetEvent(hMoby);
	MobySocket.Send(CMND_PASSWORD, sizeof(ReqPacket));
	res = WaitForSingleObject(hMoby, timeout);
	TRACE("Wait for Details. Received: R=%X Type=%X Cmnd=%02X\n", res, pRX->FrameType, pRX->Cmnd);
	if (res == WAIT_OBJECT_0) {
		if (pRX->Cmnd == CMND_DETAILS) {
			retValue = CONNECTION_SUCCESS;
			theApp.m_pMainWnd->PostMessage(WM_ON_CONNECT, (WPARAM) pRX, 0);
			// Connected
			MobySocket.Send(CMND_ACK, 0);
			TRACE("ACK is sent\n");

			pTX->Flags.server = 0;
			TRACE("MoBy connected\n");
			return 0;
		} else {
			retValue = WRONG_PASSWORD;
		}
	} else {
		retValue = COMMUNICATION_ERROR;
	}

exit:
	MobySocket.Send(CMND_BREAK, 0);
endConnection:
	theApp.m_pMainWnd->PostMessage(WM_SHOW_MESSAGE, (WPARAM) retValue, 0);
	theApp.m_pMainWnd->PostMessage(WM_MOBY_DISCONNECTED, (WPARAM) retValue, 0);
	return retValue;

}	// UINT __cdecl procConnect(LPVOID param)

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
	State.Inp = p->State.Inp;
	State.Pgm = p->State.Pgm;
	State.Tamper   = p->State.Tamper;
	State.Jamming |= p->State.Jamming;
	State.Rssi = p->State.Rssi;

	SetState(0, 0);

	btnConnect = false;
	SetDlgItemText(IDC_BUTTON_CONNECT, _T("Disconnect"));
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(true);
	MobyMonitor = AfxBeginThread(procMonitor, 0);
	return 0;
}	// LRESULT MobyDlg::OnMobyConnect(WPARAM wparam, LPARAM lparam)

LRESULT MobyDlg::SetState(WPARAM wparam, LPARAM lparam) {
	int i;
	uint8_t mask = 1;
	for (i = 0; i < nPGM; i++, mask <<= 1) {
		if ((State.Pgm & mask) == 0)
			IconPgm[i].SetIcon(LedEmpty);
		else
			IconPgm[i].SetIcon(LedRed);
		GetDlgItem(IDI_PGM1 + i)->EnableWindow(TRUE);
		GetDlgItem(IDI_PGM1 + MAX_PGMS + i)->EnableWindow(TRUE);
		GetDlgItem(IDI_PGM1 + 2 * MAX_PGMS + i)->EnableWindow(TRUE);
	}
	for (; i < MAX_PGMS; i++) {
		IconPgm[i].SetIcon(LedGrey);
		GetDlgItem(IDI_PGM1 + i)->EnableWindow(FALSE);
		GetDlgItem(IDI_PGM1 + MAX_PGMS + i)->EnableWindow(FALSE);
		GetDlgItem(IDI_PGM1 + 2 * MAX_PGMS + i)->EnableWindow(FALSE);
	}

	mask = 1;
	for (i = 0; i < Inputs; i++, mask <<= 1) {
		if ((State.Inp & mask) == 0)
			IconInp[i].SetIcon(LedEmpty);
		else
			IconInp[i].SetIcon(LedRed);
		GetDlgItem(IDI_INP1 + i)->EnableWindow(TRUE);
		GetDlgItem(IDI_INP1 + MAX_INPUTS + i)->EnableWindow(TRUE);
		GetDlgItem(IDI_INP1 + 2 * MAX_INPUTS + i)->EnableWindow(TRUE);
	}
	for (; i < MAX_INPUTS; i++) {
		IconInp[i].SetIcon(LedGrey);
		GetDlgItem(IDI_INP1 + i)->EnableWindow(FALSE);
		GetDlgItem(IDI_INP1 + MAX_INPUTS + i)->EnableWindow(FALSE);
		GetDlgItem(IDI_INP1 + 2 * MAX_INPUTS + i)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);
	return 0;
}	// LRESULT MobyDlg::SetState(WPARAM wparam, LPARAM lparam)

static void ProcessState() {
MobyComFrame* pRX = (MobyComFrame *) MonrxBuffer;
StatePacket * p	  = (StatePacket *) &pRX->Data[0];
	State.Inp	  = p->State.Inp;
	State.Pgm	  = p->State.Pgm;
	State.Tamper  =  p->State.Tamper;
	State.Jamming = p->State.Jamming;
	State.Rssi	  = p->State.Rssi;
	theApp.m_pMainWnd->PostMessage(WM_SET_STATE, (WPARAM) 0, (LPARAM) 0);
}	// static bool ProcessState()

UINT __cdecl procMonitor(LPVOID /*param*/) {
HANDLE		wEvent[3];
DWORD		res;
int			count;
MobyComFrame*	pTX = (MobyComFrame *) MontxBuffer;
MobyComFrame*	pRX = (MobyComFrame *) MonrxBuffer;

	TRACE("procMonitor is started\n");
	pTX->ProtID		= MobyComID;
	pTX->ProtVer	= ((MobyComFrame *) rxBuffer)->ProtVer;
	pTX->AppVerID	= ((MobyComFrame *) rxBuffer)->AppVerID;
	pTX->ClientID	= ID;
	pTX->ThrdIDsrc	= COMMAND_ID;
	pTX->Flags.v	= (MobySocket.pKey != 0 ? MskEncrypted : 0);
	pTX->ThrdIDdest	= ((MobyComFrame *) rxBuffer)->ThrdIDsrc;
	pTX->FrameID	= 0;
	DWORD Interval  = 15000;
	goto getState;

	while (true) {
		wEvent[0] = hResetInterval;
		wEvent[1] = hMonitor;
		wEvent[2] = hBreakMonitor;
		ResetEvent(hResetInterval);
		do {
			res = WaitForMultipleObjects(3, wEvent, false, Interval);
		} while (res == WAIT_OBJECT_0);
		if (res == (WAIT_OBJECT_0 + 1)) {
			if (pRX->Cmnd == CMND_STATE) {
				TRACE("CMND_STATE is received\n");
				ProcessState();
				continue;
			} else {
				TRACE("Not State command\n");
				goto getState;
			}
		}
		if (res == (WAIT_OBJECT_0 + 2))
			break;
	  getState:
		wEvent[0] = hMonitor;
		wEvent[1] = hBreakMonitor;
		TRACEMON("CMND_STATE sent\n");
		for (count = 3; count > 0; count--) {
			pRX->Cmnd = 0;
			MobySocket.Send(CMND_STATE, 0);
			res = WaitForMultipleObjects(2, wEvent, false, 2000);
			TRACEMON("Monitor RX: res = %X, Cmnd = %02X State: %02X %02X\n", res, pRX->Cmnd, pRX->Data[0], pRX->Data[1]);
			if (res == WAIT_OBJECT_0) {
				if (pRX->Cmnd == CMND_STATE) {
					ProcessState();
					break;
				} else {
					TRACEMON("Wrong frame\n");
					continue;
				}
			} else if (res == (WAIT_OBJECT_0 + 1)) {
				TRACE("Break event. Disconnect() is called\n");
				goto exitMonitor;
			} else if (res == WAIT_TIMEOUT) {
				TRACE("Monitor timeout\n");
			}
		}
		if (count == 0) {
			theApp.m_pMainWnd->PostMessage(WM_SHOW_MESSAGE, MOBY_TIMEOUT, 0);
			TRACE("Timeout is expired\n");
			theApp.m_pMainWnd->PostMessage(WM_MOBY_DISCONNECTED, 0, 0);
			break;
		}
	}
exitMonitor:
	MobyMonitor = 0;
	TRACE("Exit MobyMonitor\n");
	return 0;
}	// UINT __cdecl procMonitor(LPVOID param)

