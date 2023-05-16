#pragma once

class CHexEdit : public CEdit {
public:
	CHexEdit() noexcept : CEdit() {}
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
