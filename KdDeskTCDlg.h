// KdDeskTCDlg.h : 头文件
//

#pragma once
#include "DlgMenu2.h"
#include "DlgTrayEx.h"
typedef struct tagDVCITEM {
	CString strid;
	CString strpwd;
	CString strrmk;
	CString strkey;
	int platform;
} DVCITEM, *LPDVCITEM;
typedef struct tagDVCITEM2 {
	CString strid;
	CString strrmk;
	int platform;
} DVCITEM2, *LPDVCITEM2;
typedef struct tagADVITEM {
	int nid;
	CString strimg;
	CString strurl;
	CString strtxt;
} ADVITEM, *LPADVITEM;
// CKdDeskTCDlg 对话框
class CKdDeskTCDlg : public CDialog
{
// 构造
public:
	CKdDeskTCDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CKdDeskTCDlg();
	void FreezeWnd(int flag = 0);
	void DrawRoundCorner();
	CString convert_cldskid(CString stri, BOOL addspc);
	void r_rmtidlst(int flag = 0);
	void w_rmtidlst();
	void show_tab_rmtctr();
	void hide_tab_rmtctr();
	void show_tab_dvclst();
	void hide_tab_dvclst();
	void show_tab_advset();
	void hide_tab_advset();
	void popup_menu_ex1();
	void popup_menu_ex2();
	void popup_menu_ex3();
	void TrackPopupMenuEx(int cmd);
	int GetCurMntrInfo();
	void delitm_rid(int indx);
	int set_pwd(CString strpwd);
	int upd_pwd(CString& strpwd);
	int set_cstdir(CString strcstdir);
	int set_uid(CString struid);
	int IsAutoStart();
	int get_updurl();
	int get_idlst();
	int rmt_cnt(CString strid);
	int rmt_fle(CString strid);
// 对话框数据
	enum { IDD = IDD_KDDESKTC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnMin();
	afx_msg void OnBnClickedBtnCls();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnTransaprentBk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnCltCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnHlp();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// tab rmtctr.
	afx_msg void OnBnClickedBtnLogolb();
	afx_msg void OnBnClickedBtnLogon();
	afx_msg void OnBnClickedBtnRmtctr();
	afx_msg void OnBnClickedBtnDvclst();
	afx_msg void OnBnClickedBtnAdvset();
	afx_msg void OnBnClickedBtnEye();
	afx_msg void OnBnClickedBtnUpdpwd();
	afx_msg void OnCbnSelchangeCmbrid();
	afx_msg void OnBnClickedBtnConr();
	afx_msg void OnBnClickedBtnCopy();
	void OnBnClickedBtnEyeEx();
	// tab advset.
	afx_msg void OnBnClickedChkAutsta();
	afx_msg void OnBnClickedChkAutlog();
	afx_msg void OnBnClickedChkTotray();
	afx_msg void OnBnClickedRadPrv();
	afx_msg void OnBnClickedRadCst();
	afx_msg void OnCbnSelchangeCmbpwdupd();
	afx_msg void OnCbnSelchangeCmbidle();
	afx_msg void OnBnClickedBtnChgdir();
	afx_msg void OnBnClickedBtnOpndir();
	afx_msg void OnBnClickedBtnUpdn();
	afx_msg void OnBnClickedBtnUninst();
	// tab dvclst.
	afx_msg void OnEndtrackSkinList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRclickLstDvc(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickLstDvc2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMLclickLstDvc(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMLclickLstDvc2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemchangedLstdvc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedLstdvc2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnRmtcon();
	afx_msg void OnBnClickedBtnRmtfle();
	ATL::CImage m_imgBackGround;
	CDC m_dcBackGround;
	HGDIOBJ m_hOldBG;
	CWndShadow m_Shadow;

	CStatic* m_plab_title;
	CButtonEx2 m_btn_min;
	CButtonEx2 m_btn_cls;
	CButtonEx2 m_btn_vline2;
	CButtonEx2 m_btn_hlp;
	CButtonEx2 m_btn_logolb;
	CButtonEx2 m_btn_logolb2;
	CButtonEx2 m_btn_avatar;
	CButtonEx2 m_btn_logon;
	CButtonEx2 m_btn_rmtctr;
	CButtonEx2 m_btn_dvclst;
	CButtonEx2 m_btn_advset;
	int m_SelfResize;
	int m_curtab;
	CDlgTrayEx* m_pdlg_trayex;
	RECT m_rct_logolb;
	int m_curadv;

	// tab rmtctr.
	vector<DVCITEM2> m_vec_rmtidlst;
	CStatic* m_plab_curdvc;
	CStatic* m_plab_lid;
	CStatic* m_plab_lpwd;
	CEditEx2* m_pedt_lid;
	CEditEx2* m_pedt_lpwd;
	CButtonEx2* m_pbtn_copy;
	CButtonEx2* m_pbtn_vline;
	CButtonEx2* m_pbtn_eye;
	CButtonEx2* m_pbtn_updpwd;
	CStatic* m_plab_rid;
	CComboBoxDropDown* m_pcmb_rid;
	CButtonEx2* m_pbtn_conr;
	CButtonEx2* m_pbtn_ready;
	CStatic* m_plab_ready;
	int edtlpwd(int flag);
	RECT m_rct_edtlpwd;
	RECT m_rct_btneye;
	int m_in_conr;
	CString m_lid;
	CString m_lpwd;
	DWORD m_tc_dn;

	// tab advset.
	CStatic* m_plab_autsta;
	CCheckBoxEx* m_pchk_autsta;
	CStatic* m_plab_fletrn;
	CStatic* m_plab_defdir;
	CRadioBoxEx* m_prad_prv;
	CRadioBoxEx* m_prad_cst;
	CEditEx2* m_pedt_defdir;
	CButtonEx2* m_pbtn_chgdir;
	CButtonEx2* m_pbtn_opndir;
	CStatic* m_plab_pwdupd;
	CComboBoxDropList* m_pcmb_pwdupd;
	CStatic* m_plab_idle;
	CComboBoxDropList* m_pcmb_idle;
	CStatic* m_plab_clswnd;
	CCheckBoxEx* m_pchk_totray;
	CStatic* m_plab_logset;
	CCheckBoxEx* m_pchk_autlog;
	CStatic* m_plab_about;
	CStatic* m_plab_curver;
	CButtonEx2* m_pbtn_updn;
	CButtonEx2* m_pbtn_uninst;
	CString m_cstdir;
	int m_pwdupd;
	int m_idle;
	CString m_updurl;
	//CString m_newver;

	// tab dvclst.
	CStatic* m_plab_dvc;
	CStatic* m_plab_dvc2;
	CSkinListCtrl* m_plst_dvc;
	CSkinListCtrl* m_plst_dvc2;
	CImageList m_imglst_icon;
	CButtonEx2* m_pbtn_vline3;
	CStatic* m_plab_dvcname;
	CStatic* m_plab_dvccode;
	RECT m_rct_dvcname;
	RECT m_rct_dvccode;
	CButtonEx2* m_pbtn_rmtcon;
	CButtonEx2* m_pbtn_rmtfle;
	
	RECT m_rcMonitor;
	RECT m_rcWork;
	CDlgMenu2* m_pdlg_menu1;
	CDlgMenu2* m_pdlg_menu2;
	CDlgMenu2* m_pdlg_menu3;
};
