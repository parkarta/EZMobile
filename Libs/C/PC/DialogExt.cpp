
#include <stdafx.h>
#include "DialogExt.h"
#include <Trace.h>

using namespace std;

IMPLEMENT_DYNAMIC(DialogExt, CDialogEx)
IMPLEMENT_DYNAMIC(PropertyPageExt, CPropertyPage)

void SetFrameStrings(CWnd * wnd, DialogItemInfo * pItem) {
CString s;
	for (; pItem->ItemID != 0; pItem++) {
		if (pItem->StringID != 0) {
			s.LoadString(pItem->StringID);
			wnd->SetDlgItemText(pItem->ItemID, s);
		}
	}
}	// void SetFrameStrings(CWnd * wnd, DialogItemInfo * pItem)

void OffsetChildWindow(CWnd* child, int dx, int dy, int dw, int dh) {
	if (!child) return;
	if (!::IsWindow(child->GetSafeHwnd())) return;
	if (!child->GetParent()) return;

	// Find child window's coordinates relative to top-left of parent
	CRect rc;
	child->GetWindowRect(&rc);
	CPoint p(0, 0);
	ScreenToClient(child->GetParent()->GetSafeHwnd(), &p);
	rc.OffsetRect(p);

	// Prevent negative size
	if ((rc.Width() + dw) < 0) dw = -rc.Width();
	if ((rc.Height() + dh) < 0) dh = -rc.Height();

	child->SetWindowPos(0, rc.left + dx, rc.top + dy, rc.Width() + dw, rc.Height() + dh, SWP_NOZORDER);
	child->Invalidate(FALSE);
}	// void OffsetChildWindow(CWnd* child, int dx, int dy, int dw, int dh)

void AFXAPI DDX_TextA(CDataExchange* pDX, int nIDC, char * value, int nMaxLen) {
	ASSERT(nMaxLen != 0);
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	if (pDX->m_bSaveAndValidate) {
		int nLen = ::GetWindowTextLength(hWndCtrl);
		if (nLen > nMaxLen) {
			TRACE("Text in control ID %d is too long. Call DDV_MaxChars()!\n", nIDC);
			nLen = nMaxLen;
		}
		CStringA s;
		::GetWindowTextA(hWndCtrl, s.GetBufferSetLength(nLen), nLen+1);
		for (int i = 0; i < nLen; i++)
			*value++ = (char) (s[i] & 0xFF);
		*value = '\0';
		s.ReleaseBuffer();
	} else {
		::SetWindowTextA(hWndCtrl, value);
	}
}	// void AFXAPI DDX_TextA(CDataExchange* pDX, int nIDC, char value, int nMax)

void AFXAPI DDX_TextAFix(CDataExchange* pDX, int nIDC, char * value, int nLen) {
CStringA s("", nLen);
	ASSERT(nLen != 0);
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	if (pDX->m_bSaveAndValidate) {
		const int Len = ::GetWindowTextLength(hWndCtrl);
		if (Len > (nLen + 1)) {
			TRACE("Text in control ID %d is too long. Call DDV_MaxChars()!\n", nIDC);
		}
		::GetWindowTextA(hWndCtrl, s.GetBufferSetLength(nLen), nLen + 1);
		for (int i = 0; i < nLen; i++)
			*value++ = s[i];
	} else {
		s.CopyChars(s.GetBuffer(nLen), nLen, value, nLen);
		::SetWindowTextA(hWndCtrl, s);
	}
	s.ReleaseBuffer(nLen);
}	// void AFXAPI DDX_TextA(CDataExchange* pDX, int nIDC, char value, int nMax)

void AFXAPI DDX_TextA(CDataExchange* pDX, int nIDC, CStringA& value) {
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	if (pDX->m_bSaveAndValidate) {
	#ifdef UNICODE
		CString s;
		DDX_Text(pDX, nIDC, s);
		value = s;
	#else
		DDX_Text(pDX, nIDC, value);
	#endif
	} else {
		SetWindowTextA(hWndCtrl, value);
	}
}	// void AFXAPI DDX_TextA(CDataExchange* pDX, int nIDC, char value, int nMax)

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, uint16_t& value) {
UINT n;
	if (pDX->m_bSaveAndValidate) {
		DDX_Text(pDX, nIDC, n);
		if (n > 0xFFFF) {
			pDX->Fail();        // throws exception
		}
		value = (uint16_t)n;
	} else {
		n = (UINT)value;
		DDX_Text(pDX, nIDC, n);
	}
}	// void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, UINT16& value)

void AFXAPI DDX_TextFix(CDataExchange* pDX, int nIDC, uint16_t& value) {
	if (pDX->m_bSaveAndValidate) {
		DialogExt const * const pDlg = (DialogExt *) pDX->m_pDlgWnd;
		UINT n = pDlg->GetDlgItemHex(nIDC);
		if (n > 0xFFFF) {
			pDX->Fail();        // throws exception
		}
		value = (uint16_t)n;
	} else {
		CString s;
		s.Format(_T("%04X"), value);
		DDX_Text(pDX, nIDC, s);
	}
}	// void AFXAPI DDX_TextFix(CDataExchange* pDX, int nIDC, uint16_t& value)

void AFXAPI DDX_TextFix(CDataExchange* pDX, int nIDC, uint32_t& value) {
	if (pDX->m_bSaveAndValidate) {
		DialogExt const * const pDlg = (DialogExt *) pDX->m_pDlgWnd;
		value = pDlg->GetDlgItemHex(nIDC);
	} else {
		CString s;
		s.Format(_T("%08X"), value);
		DDX_Text(pDX, nIDC, s);
	}
}	// void AFXAPI DDX_TextFix(CDataExchange* pDX, int nIDC, UINT32& value)

UINT GetDlgItemHex(const CDialog * pDlg, int id) {
TCHAR	str[9];
TCHAR *	p;
UINT	result = 0;
UINT8	d;

	int n = pDlg->GetDlgItemText(id, (TCHAR *)&str[0], 9);
	if (n > 8) {
		TRACE("Too long value in control ID %d\n");
	}
	for (p = &str[0]; n > 0; n--) {
		result <<= 4;
		d = (UINT8) *p++;
		if ((d >= '0') && (d <= '9'))
			d -= '0';
		else if ((d >= 'A') && (d <= 'F'))
			d -= 'A' - 0xA;
		else if ((d >= 'a') && (d <= 'f'))
			d -= 'a' - 0xA;
		else if (d == 0)
			break;
		else
			TRACE("Not hex digit in control ID %d\n");
		result += d;
	}	// for (int i = 0; i < n; i++)
	return result;
}	// UINT GetDlgItemHex(const CDialog * pDlg, int id)

void SetDlgItemHex16(HWND h, int nID, uint16_t nValue) {
	CString s;
	s.Format(_T("%04X"), nValue);
	SetDlgItemText(h, nID, s);
}	// void DialogExt::SetThisDlgItemHex16(int nID, uint16_t nValue)

void SetDlgItemHex32(HWND h, int nID, uint32_t nValue) {
	CString s;
	s.Format(_T("%08X"), nValue);
	SetDlgItemText(h, nID, s);
}	// void DialogExt::SeThistDlgItemHex32(int nID, uint32_t nValue)

/////////////////////////////////////////////////////////////////////////////
// Listboxes, comboboxes

void AFXAPI DDX_LBString(CDataExchange* pDX, int nIDC, TCHAR * value) {
	pDX->PrepareCtrl(nIDC);
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
	if (pDX->m_bSaveAndValidate) {
		const int nIndex = (int)::SendMessage(hWndCtrl, LB_GETCURSEL, 0, 0L);
		if (nIndex != -1) {
			const int nLen = (int)::SendMessage(hWndCtrl, LB_GETTEXTLEN, nIndex, 0L);
			::SendMessage(hWndCtrl, LB_GETTEXT, nIndex, (LPARAM)(LPVOID)value);
		} else {	// no selection
			*value = _T('\0');
		}
	} else {	// set current selection based on data string
		::SendMessage(hWndCtrl, LB_SELECTSTRING, (WPARAM)-1, (LPARAM)(LPCTSTR)value);
	}
}	// void AFXAPI DDX_LBString(CDataExchange* pDX, int nIDC, TCHAR * value)

void AFXAPI DDX_LBStringExact(CDataExchange* pDX, int nIDC, TCHAR * value) {
	pDX->PrepareCtrl(nIDC);
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
	if (pDX->m_bSaveAndValidate)
		DDX_LBString(pDX, nIDC, value);
	else {
		// set current selection based on data string
		const int i = (int)::SendMessage(hWndCtrl, LB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(LPCTSTR)value);
		if (i >= 0)
			// select it
			SendMessage(hWndCtrl, LB_SETCURSEL, i, 0L);
	}
}	// void AFXAPI DDX_LBStringExact(CDataExchange* pDX, int nIDC, TCHAR * value)

void AFXAPI DDX_CBString(CDataExchange* pDX, int nIDC, TCHAR * value) {
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);

	if ((::GetWindowLong(hWndCtrl, GWL_STYLE) & CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST)
		pDX->PrepareEditCtrl(nIDC);
	else
		pDX->PrepareCtrl(nIDC);

	if (pDX->m_bSaveAndValidate) {
		// just get current edit item text (or drop list static)
		const int nLen = ::GetWindowTextLength(hWndCtrl);
		if (nLen > 0)
			// get known length
			::GetWindowText(hWndCtrl, value, nLen+1);
		else	// for drop lists GetWindowTextLength does not work - assume max of 255 characters
			::GetWindowText(hWndCtrl, value, 255+1);
	} else {	// set current selection based on model string
		::SendMessage(hWndCtrl, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)(LPCTSTR)value);
	}
}	// void AFXAPI DDX_CBString(CDataExchange* pDX, int nIDC, TCHAR * value)

