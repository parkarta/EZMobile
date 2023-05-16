#pragma once

#include <stdint.h>
#include <afxdialogex.h>
#include <afxdlgs.h>

#define TEMP_STR_LEN	64

struct DialogItemInfo {
	UINT	ItemID;
	UINT	StringID	= 0;
	UINT	HelpID		= 0;
};

extern void SetFrameStrings(CWnd * wnd, DialogItemInfo * pItem);

void AFXAPI DDX_TextA(CDataExchange* pDX, int nIDC, char * value, int nMax);
void AFXAPI DDX_TextAFix(CDataExchange* pDX, int nIDC, char * value, int nLen);
void AFXAPI DDX_TextA(CDataExchange* pDX, int nIDC, CStringA& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, uint16_t& value);
void AFXAPI DDX_TextFix(CDataExchange* pDX, int nIDC, uint16_t& value);
void AFXAPI DDX_TextFix(CDataExchange* pDX, int nIDC, uint32_t& value);
void SetDlgItemHex16(HWND h, int nID, uint16_t nValue);
void SetDlgItemHex32(HWND h, int nID, uint32_t nValue);
UINT GetDlgItemHex(const CDialog * pDlg, int nID);
void AFXAPI DDX_LBString(CDataExchange* pDX, int nIDC, TCHAR * value);
void AFXAPI DDX_LBStringExact(CDataExchange* pDX, int nIDC, TCHAR * value);
void AFXAPI DDX_CBString(CDataExchange* pDX, int nIDC, TCHAR * value);
void AFXAPI DDX_CBStringExact(CDataExchange* pDX, int nIDC, TCHAR * value);

inline void SetDlgItemTextX(HWND h, int nID, CStringA str)	{::SetDlgItemTextA(h, nID, str);}
inline void SetDlgItemTextX(HWND h, int nID, LPCSTR str)	{::SetDlgItemTextA(h, nID, str);}
inline void SetDlgItemTextX(HWND h, int nID, CStringW str)	{::SetDlgItemTextW(h, nID, str);}
inline void SetDlgItemTextX(HWND h, int nID, LPCWSTR str)	{::SetDlgItemTextW(h, nID, str);}

void OffsetChildWindow(CWnd* child, int dx, int dy, int dw, int dh);

class DialogExt : public CDialogEx {
	DECLARE_DYNAMIC(DialogExt)
public:
	DialogExt() : CDialogEx() {}
	DialogExt(int idd, CWnd * pParent = NULL) : CDialogEx(idd, pParent) {}
	DialogExt(LPCTSTR lpszTemplateName, CWnd* pParentWnd/*= NULL*/) : CDialogEx(lpszTemplateName, pParentWnd/*= NULL*/) {}
	UINT16		GetDlgItemInt16u(int nID) const						{return __super::GetDlgItemInt(nID, NULL, FALSE) & 0xFFFF;}
	UINT		GetDlgItemHex(int nID) const						{return ::GetDlgItemHex(this, nID);}
//	void		SetDlgItemInt(int nID, uint16_t nValue)				{__super::SetDlgItemInt(nID, nValue, FALSE);}
	void		SetThisDlgItemPos(int nID, uint32_t nValue)			{ __super::SetDlgItemInt(nID, nValue, FALSE);}
//	void		SetDlgItemInt(int nID, int nValue, bool sign = false) {__super::SetDlgItemInt(nID, nValue, sign);}
	void		SetThisDlgItemHex16(int nID, uint16_t nValue)		{::SetDlgItemHex16(m_hWnd, nID, nValue);}
	void		SetThisDlgItemHex32(int nID, uint32_t nValue)		{::SetDlgItemHex32(m_hWnd, nID, nValue);}
	void		SetThisDlgItemTextA(int nID, CStringA str) noexcept {::SetDlgItemTextA(m_hWnd, nID, str);}
	void		SetThisDlgItemTextA(int nID, LPCSTR str) noexcept	{::SetDlgItemTextA(m_hWnd, nID, str);}
	void		SetThisDlgItemTextW(int nID, CStringW str) noexcept	{::SetDlgItemTextW(m_hWnd, nID, str);}
	void		SetThisDlgItemTextW(int nID, LPCWSTR str) noexcept	{::SetDlgItemTextW(m_hWnd, nID, str);}
	void		SetThisWindowTextA(CStringA str) noexcept			{::SetWindowTextA(m_hWnd, str);}
	void		SetThisWindowTextW(CStringW str) noexcept			{::SetWindowTextW(m_hWnd, str);}
	void		SetThisWindowTextA(LPCSTR str) noexcept				{::SetWindowTextA(m_hWnd, str);}
	void		SetThisWindowTextW(LPCWSTR str) noexcept			{::SetWindowTextW(m_hWnd, str);}
	void		SetThisWindowTextX(LPCSTR str) noexcept				{SetThisWindowTextA(str);}
	void		SetThisWindowTextX(CStringA str)					{SetThisWindowTextA(str);}
	void		SetThisWindowTextX(LPCWSTR str) noexcept			{SetThisWindowTextW(str);}
	void		SetThisWindowTextX(CStringW str)					{SetThisWindowTextW(str);}
	void		SetDlgItemTextX(int nID, CStringA str) noexcept		{::SetDlgItemTextA(m_hWnd, nID, str);}
	void		SetDlgItemTextX(int nID, LPCSTR str) noexcept		{::SetDlgItemTextA(m_hWnd, nID, str);}
	void		SetDlgItemTextX(int nID, CStringW str) noexcept		{::SetDlgItemTextW(m_hWnd, nID, str);}
	void		SetDlgItemTextX(int nID, LPCWSTR str) noexcept		{::SetDlgItemTextW(m_hWnd, nID, str);}
protected:
};

class PropertyPageExt : public CPropertyPage {
	DECLARE_DYNAMIC(PropertyPageExt)
public:
	PropertyPageExt() : CPropertyPage() {}
	PropertyPageExt(int idd) : CPropertyPage(idd) {}
	PropertyPageExt(UINT nIDTemplate, UINT nIDCaption = 0) : CPropertyPage(nIDTemplate, nIDCaption) {}
	PropertyPageExt(LPCTSTR lpszTemplateName, UINT nIDCaption = 0) : CPropertyPage(lpszTemplateName, nIDCaption) {}
	UINT16		GetDlgItemInt16u(int nID) const						{return ::GetDlgItemInt(m_hWnd, nID, 0, FALSE) & 0xFFFF;}
	UINT		GetDlgItemHex(int nID) const						{return ::GetDlgItemHex(this, nID);}
//	void		SetDlgItemPos(int nID, uint16_t nValue)				{__super::SetDlgItemInt(nID, nValue, FALSE);}
	void		SetDlgItemPos(int nID, uint32_t nValue)				{ __super::SetDlgItemInt(nID, nValue, FALSE);}
//	void		SetDlgItemInt(int nID, int nValue, bool sign = TRUE) {__super::SetDlgItemInt(nID, nValue, sign);}
	void		SetThisDlgItemHex16(int nID, uint16_t nValue)		{::SetDlgItemHex16(m_hWnd, nID, nValue);}
	void		SetThisDlgItemHex32(int nID, uint32_t nValue)		{::SetDlgItemHex32(m_hWnd, nID, nValue);}
	void		SetThisDlgItemTextA(int nID, CStringA str)			{::SetDlgItemTextA(m_hWnd, nID, str);}
	void		SetThisDlgItemTextA(int nID, LPCSTR str)			{::SetDlgItemTextA(m_hWnd, nID, str);}
	void		SetThisDlgItemTextW(int nID, CStringW str)			{::SetDlgItemTextW(m_hWnd, nID, str);}
	void		SetThisDlgItemTextW(int nID, LPCWSTR str)			{::SetDlgItemTextW(m_hWnd, nID, str);}
	void		SetThisWindowTextA(CStringA str)					{::SetWindowTextA(m_hWnd, str);}
	void		SetThisWindowTextW(CStringW str)					{::SetWindowTextW(m_hWnd, str);}
	void		SetThisWindowTextA(LPCSTR str)						{::SetWindowTextA(m_hWnd, str);}
	void		SetThisWindowTextW(LPCWSTR str)						{::SetWindowTextW(m_hWnd, str);}
	void		SetThisWindowTextX(LPCSTR str)						{SetThisWindowTextA(str);}
	void		SetThisWindowTextX(CStringA str)					{SetThisWindowTextA(str);}
	void		SetThisWindowTextX(LPCWSTR str)						{SetThisWindowTextW(str);}
	void		SetThisWindowTextX(CStringW str)					{SetThisWindowTextW(str);}
	void		SetDlgItemTextX(int nID, CStringA str)				{::SetDlgItemTextA(m_hWnd, nID, str);}
	void		SetDlgItemTextX(int nID, LPCSTR str)				{::SetDlgItemTextA(m_hWnd, nID, str);}
	void		SetDlgItemTextX(int nID, CStringW str)				{::SetDlgItemTextW(m_hWnd, nID, str);}
	void		SetDlgItemTextX(int nID, LPCWSTR str)				{::SetDlgItemTextW(m_hWnd, nID, str);}
protected:
};

class TabCtrlExt : public CTabCtrl {
public:
	TabCtrlExt() noexcept : CTabCtrl() {}
#if defined (_UNICODE)
	long InsertItemX(int nItem, TCITEMA const * const pTabCtrlItem) {ASSERT(::IsWindow(m_hWnd)); return (LONG)::SendMessage(m_hWnd, TCM_INSERTITEMA, nItem, (LPARAM) pTabCtrlItem);}
//	long InsertItemX(int nItem, LPCSTR lpszItem) {ASSERT(ASSERT(::IsWindow(m_hWnd)); return CTabCtrl::InsertItem(TCIF_TEXT, nItem, lpszItem, 0, 0);}
	BOOL SetItemX(int nItem, TCITEMA const * const pTabCtrlItem) {ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_SETITEMA, nItem, (LPARAM) pTabCtrlItem);}
#else
	long InsertItemX(int nItem, TCITEMW* pTabCtrlItem) {ASSERT(::IsWindow(m_hWnd)); return (LONG)::SendMessage(m_hWnd, TCM_INSERTITEMW, nItem, (LPARAM)pTabCtrlItem);}
//	long InsertItemX(int nItem, LPCWSTR lpszItem) {ASSERT(::IsWindow(m_hWnd)); return CTabCtrl::InsertItem(TCIF_TEXT, nItem, lpszItem, 0, 0);}
	BOOL SetItemX(int nItem, TCITEMW* pTabCtrlItem) {ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_SETITEMW, nItem, (LPARAM)pTabCtrlItem);}
#endif
};
