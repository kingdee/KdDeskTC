#pragma once


// CDlgMenu2 对话框

class CDlgMenu2 : public CDialog
{
	DECLARE_DYNAMIC(CDlgMenu2)

public:
	CDlgMenu2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMenu2();
	void DrawRoundCorner();
	void DisplayTxtByLang();
	void RefreshMenu();
// 对话框数据
	enum { IDD = IDD_DLG_MENU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnTransaprentBk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnCltCommand(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnSetup();

	ATL::CImage m_imgBackGround;
	CDC m_dcBackGround;
	HGDIOBJ m_hOldBG;
	CWndShadow m_Shadow;

	CButtonEx2 m_btn_open;
	CButtonEx2 m_btn_setup;
	vector<CWnd*> m_vec_btn;

	int m_flag;
	POINT m_pt;
};
