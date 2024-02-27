#pragma once


// CDlgLogin 对话框

class CDlgLogin : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLogin();
	void FreezeWnd(int flag = 0);
	void DrawRoundCorner();
// 对话框数据
	enum { IDD = IDD_DLG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnCls();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnTransaprentBk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnCltCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnFgtpwd();
	afx_msg void OnBnClickedBtnRegusr();
	afx_msg void OnBnClickedChkAutolgn();
	afx_msg void OnBnClickedBtnHlp2();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedBtnAgr();
	afx_msg void OnBnClickedBtnPri();

	ATL::CImage m_imgBackGround;
	CDC m_dcBackGround;
	HGDIOBJ m_hOldBG;
	CWndShadow m_Shadow;

	CStatic* m_plab_title;
	//CButtonEx2 m_btn_min;
	CButtonEx2 m_btn_cls;
	CButtonEx2 m_btn_vline3;
	CButtonEx2 m_btn_vline4;
	CButtonEx2 m_btn_hlp2;
	CEditEx2 m_edt_usr;
	CEditEx2 m_edt_pwd;
	CButtonEx2 m_btn_lgn;
	CButtonEx2 m_btn_fgtpwd;
	CButtonEx2 m_btn_regusr;
	CButtonEx2 m_btn_agr;
	CButtonEx2 m_btn_pri;
	CCheckBoxEx m_chk_autolgn;
	CCheckBoxEx m_chk_agree;
	CButtonEx2 m_btn_logolb3;
	RECT m_rct_edtusr;
	RECT m_rct_edtpwd;

	int m_SelfResize;
	int m_inlogin;
	CString m_strusr;
	CString m_strpwd;
	int m_autolgn;
	int m_tohide;
	int m_mode;
};
