
// EZMobileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EZMobile.h"
#include "EZMobileDlg.h"
//#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEZMobileDlg dialog

CEZMobileDlg::CEZMobileDlg(CWnd* pParent /*=nullptr*/) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEZMobileDlg::DoDataExchange(CDataExchange* pDX) {
	__super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEZMobileDlg, DialogExt)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

// CEZMobileDlg message handlers

BOOL CEZMobileDlg::OnInitDialog() {
	__super::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}	// BOOL CEZMobileDlg::OnInitDialog()

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEZMobileDlg::OnPaint() {
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
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}
}	// void CEZMobileDlg::OnPaint()

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEZMobileDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

