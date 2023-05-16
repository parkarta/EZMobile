
// EZMobileDlg.h : header file
//

#pragma once
#include <DialogExt.h>
#include <HexEdit.h>

#define WM_ON_CONNECT		(WM_USER + 1)
#define WM_SHOW_MESSAGE		(WM_ON_CONNECT + 1)
#define WM_SET_STATE		(WM_SHOW_MESSAGE + 1)
#define WM_MOBY_DISCONNECTED (WM_SET_STATE + 1)

union IOState {
  struct {
	uint16_t	Inp;
	uint8_t		TamperJamming;
	uint8_t		Rssi;
	uint32_t	Pgm;
  };
  uint64_t		w64;
};

// CEZMobileDlg dialog
class MobyDlg : public DialogExt {
// Construction
public:
	MobyDlg(CWnd* pParent = nullptr);	// standard constructor
	~MobyDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOBY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
//	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	CHexEdit	EditID;
	LRESULT		OnMobyConnect(WPARAM, LPARAM);
	LRESULT		ShowMessage(WPARAM, LPARAM);
	LRESULT		SetState(WPARAM, LPARAM);
	afx_msg void OnBnClickedButtonConnect();
public:
	CStatic IconPgm1ics;
	CStatic IconPgm2ics;
	CStatic IconPgm3ics;
	CStatic IconPgm4ics;
	CStatic IconInp1ics;
	CStatic IconInp2ics;
	CStatic IconInp3ics;
};
