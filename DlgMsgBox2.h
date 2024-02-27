#pragma once


// CDlgMsgBox2 对话框

class CDlgMsgBox2 : public CDialog
{
	DECLARE_DYNAMIC(CDlgMsgBox2)

public:
	CDlgMsgBox2(int mode, CString strtitle, CString strinfo, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMsgBox2();
	void FreezeWnd(int flag = 0);
	void DrawRoundCorner();
	void setinfo(CString strv);
// 对话框数据
	enum { IDD = IDD_DLG_MSGBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnCls();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnTransaprentBk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCltCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);

	ATL::CImage m_imgBackGround;
	CDC m_dcBackGround;
	HGDIOBJ m_hOldBG;
	CWndShadow m_Shadow;

	CStatic* m_plab_title;
	CButtonEx2 m_btn_ok;
	CButtonEx2 m_btn_cancel;
	CButtonEx2 m_btn_cls;

	int m_mode;
	CString m_strtitle;
	CString m_strinfo;
	RECT m_li_rct;
	int m_SelfResize;
	int m_tohide;
};
