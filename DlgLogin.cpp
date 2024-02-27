// DlgLogin.cpp : 实现文件
//

#include "stdafx.h"
#include "KdDeskTC.h"
#include "KdDeskTCDlg.h"
#include ".\dlglogin.h"
#include "DlgMsgBox2.h"

CFont* CreateFontEx(int nHeight, int nWeight);
HFONT createfontex(int nheight, int nweight);
int chk_toml();
extern TCHAR buff_inipath[MAX_PATH];
extern TCHAR buff_workdir[MAX_PATH];
extern CLTUSRLGN lpfn_CltUsrLgn;
extern GETDVCLST lpfn_GetDvcLst;
extern SUBCLDSKID lpfn_SubCldskId;
extern char g_devid[64];
char BufOutErr[256] = { 0 };
char BufOutCrt[4096] = { 0 };
char BufOutUsrId[128] = { 0 };
char BufOutKey[128] = { 0 };
//extern CKdDeskTCDlg* g_pdlg_dsk;
CDlgLogin* g_pdlg_lgn = NULL;
extern vector<DVCITEM> g_vec_dvc;
extern vector<DVCITEM> g_vec_dvc_tmp;
extern CKdDeskTCDlg* g_pdlg_dsk;
char cldskid[128] = { 0 };
int set_flagex(CString strv);
int AddDevice(char* pszId, char* pszPwd, char* pszRmk, char* pszKey, char* pszOs)
{
	DVCITEM dvcitem;
	dvcitem.strid = a2t(pszId).c_str();
	dvcitem.strpwd = a2t(pszPwd).c_str();
	if (0 == strcmp(pszId, cldskid)) {
		dvcitem.strrmk.Format(_T("%s(本设备)"), a2t(pszRmk).c_str());
	} else {
	dvcitem.strrmk = a2t(pszRmk).c_str();
	}
	dvcitem.strkey = a2t(pszKey).c_str();
	if (0 == strcmp(pszOs, "Windows")) {
		dvcitem.platform = 0;
	} else {
		dvcitem.platform = 1;
	}
	LogMsg(_T("AddDevice, %s, %s, %s, %s, %s."), a2t(pszId).c_str(), a2t(pszPwd).c_str(), a2t(pszRmk).c_str(), a2t(pszKey).c_str(), a2t(pszOs).c_str());
	g_vec_dvc_tmp.push_back(dvcitem);
	return 0;
}
DWORD WINAPI threadproc_login(LPVOID lpParameter)
{
	g_pdlg_lgn->m_inlogin = 1;
	char buftmp[128];
	char bufrmk[256] = { 0 };
	char bufpwd[128] = { 0 };
	char BufOutExpi[128] = { 0 };
	char BufOutRmk[256] = { 0 };
	int OutUsrNum = 2;
	int isrmkupd = 1;
	CStringA strtmpa;
	SYSTEMTIME st;
	GetLocalTime(&st);
	wsprintfA(buftmp, "%04d-%02d-%02d %02d:%02d:%02d", (int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour, (int)st.wMinute, (int)st.wSecond);
	try {
		chk_toml();
		strtmpa.Format("%s\\result.ini", t2a(buff_workdir).c_str());
		GetPrivateProfileStringA("result", "id", "", cldskid, 128, strtmpa);
		int ret__ = lpfn_CltUsrLgn(buftmp, t2a(g_pdlg_lgn->m_strusr.GetBuffer()).c_str(), t2a(g_pdlg_lgn->m_strpwd.GetBuffer()).c_str(), g_devid, 
			!(g_pdlg_lgn->m_mode),//g_pdlg_lgn->m_autolgn, 
			BufOutCrt, BufOutUsrId, AddDevice, BufOutErr);
		if (200 == ret__) {
			if (0 == g_pdlg_lgn->m_mode && 0 != BufOutCrt[0]) {
				if (g_pdlg_lgn->m_autolgn) {
					theApp.WriteProfileString(_T("KdDeskTC"), _T("Certificate"), a2t(BufOutCrt).c_str());
				}
				theApp.WriteProfileString(_T("KdDeskTC"), _T("Certificate2"), a2t(BufOutCrt).c_str());
			}
			DWORD nSize = 32;
			if (!GetComputerNameExA(ComputerNameNetBIOS, bufrmk, &nSize)) {
			}
			ret__ = lpfn_SubCldskId(buftmp, BufOutUsrId, cldskid, bufrmk, bufpwd, isrmkupd, g_devid, BufOutExpi, &OutUsrNum, BufOutRmk, BufOutKey, BufOutErr);
			if (200 == ret__) {
			}
		}
		::PostMessage(g_pdlg_lgn->m_hWnd, WM_CLT_COMMAND, 10, ret__);
	} catch (...) {
		::PostMessage(g_pdlg_lgn->m_hWnd, WM_CLT_COMMAND, 20, 0);
	}
__cleanup:
	g_pdlg_lgn->m_inlogin = 0;
	return 0;
}
int g_in_getdvclst = 0;
DWORD WINAPI threadproc_getdvclst(LPVOID lpParameter)
{
	g_in_getdvclst = 1;
	char buftmp[128];
	char bufrmk[256] = { 0 };
	char bufpwd[128] = { 0 };
	char BufOutExpi[128] = { 0 };
	char BufOutRmk[256] = { 0 };
	int OutUsrNum = 2;
	int isrmkupd = 1;
	CStringA strtmpa;
	SYSTEMTIME st;
	GetLocalTime(&st);
	wsprintfA(buftmp, "%04d-%02d-%02d %02d:%02d:%02d", (int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour, (int)st.wMinute, (int)st.wSecond);
	try {
		int ret__ = lpfn_GetDvcLst(buftmp, BufOutUsrId, AddDevice, BufOutErr);
		if (200 == ret__) {
		}
		::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 70, ret__);
	} catch (...) {
		LogFat(_T("threadproc_getdvclst, unexpected error."));
	}
__cleanup:
	g_in_getdvclst = 0;
	return 0;
}
// CDlgLogin 对话框

IMPLEMENT_DYNAMIC(CDlgLogin, CDialog)
CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogin::IDD, pParent)
{
	m_SelfResize = -1;
	m_inlogin = 0;
	m_autolgn = 0;
	m_tohide = 0;
	m_mode = 0;
}

CDlgLogin::~CDlgLogin()
{
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_CLS, m_btn_cls);
	DDX_Control(pDX, IDC_BTN_VLINE3, m_btn_vline3);
	DDX_Control(pDX, IDC_BTN_VLINE4, m_btn_vline4);
	DDX_Control(pDX, IDC_BTN_HLP2, m_btn_hlp2);
	DDX_Control(pDX, IDC_EDT_USR, m_edt_usr);
	DDX_Control(pDX, IDC_EDT_PWD, m_edt_pwd);
	DDX_Control(pDX, IDOK, m_btn_lgn);
	DDX_Control(pDX, IDC_BTN_FGTPWD, m_btn_fgtpwd);
	DDX_Control(pDX, IDC_BTN_REGUSR, m_btn_regusr);
	DDX_Control(pDX, IDC_BTN_AGR, m_btn_agr);
	DDX_Control(pDX, IDC_BTN_PRI, m_btn_pri);
	DDX_Control(pDX, IDC_CHK_AUTOLGN, m_chk_autolgn);
	DDX_Control(pDX, IDC_CHK_AGREE, m_chk_agree);
	DDX_Control(pDX, IDC_BTN_LOGOLB3, m_btn_logolb3);
}

BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_CLS, OnBnClickedBtnCls)
	ON_MESSAGE(WM_TRANSPARENT_BK_CTOP, OnTransaprentBk)
	ON_MESSAGE(WM_CLT_COMMAND, OnCltCommand)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_BTN_FGTPWD, OnBnClickedBtnFgtpwd)
	ON_BN_CLICKED(IDC_BTN_REGUSR, OnBnClickedBtnRegusr)
	ON_BN_CLICKED(IDC_CHK_AUTOLGN, OnBnClickedChkAutolgn)
	ON_BN_CLICKED(IDC_BTN_HLP2, OnBnClickedBtnHlp2)
	ON_BN_CLICKED(IDC_BTN_AGR, OnBnClickedBtnAgr)
	ON_BN_CLICKED(IDC_BTN_PRI, OnBnClickedBtnPri)
END_MESSAGE_MAP()

// CDlgLogin 消息处理程序
void CDlgLogin::FreezeWnd(int flag) {
	RECT rctmp;
	/*this->GetWindowRect(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, this->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_LAB_TITLE)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_LAB_TITLE)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_LAB_TITLE)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_LAB_MSG)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_LAB_MSG)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_LAB_MSG)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_EDT_USR)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_EDT_USR)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_EDT_USR)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_EDT_PWD)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_EDT_PWD)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_EDT_PWD)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_CHK_AUTOLGN)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_CHK_AUTOLGN)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_CHK_AUTOLGN)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_FGTPWD)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_FGTPWD)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_BTN_FGTPWD)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_VLINE4)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_VLINE4)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_BTN_VLINE4)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_REGUSR)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_REGUSR)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_BTN_REGUSR)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDOK)->GetClientRect(&rctmp);
	this->GetDlgItem(IDOK)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDOK)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_LAB_AGR)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_LAB_AGR)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_LAB_AGR)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_AGR)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_AGR)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_BTN_AGR)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_LAB_AGR2)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_LAB_AGR2)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_LAB_AGR2)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_PRI)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_PRI)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgLogin->FreezeWnd, GetDlgItem(IDC_BTN_PRI)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);*/
	this->GetWindowRect(&rctmp);
	BOOL ischanged = 0;
	m_SelfResize = 0;
	if (450 != rctmp.right-rctmp.left) {
	rctmp.right = rctmp.left + 450;
	ischanged = 1;
	}
	if (374 != rctmp.bottom-rctmp.top) {
	rctmp.bottom = rctmp.top + 374;
	ischanged = 1;
	}
	if (ischanged) {
		m_SelfResize = 1;
		this->MoveWindow(&rctmp);
	}
	if (0 == flag) {
		int hoff = 20;
		GetDlgItem(IDC_LAB_TITLE)->MoveWindow(6, 8, 210, 24);
		GetDlgItem(IDC_BTN_LOGOLB3)->MoveWindow(120, 40, 106, 24);
		GetDlgItem(IDC_LAB_MSG)->MoveWindow(84, 64+hoff, 282, 20);
		GetDlgItem(IDC_EDT_USR)->MoveWindow(84, 98+hoff, 282, 40);
		GetDlgItem(IDC_EDT_PWD)->MoveWindow(84, 152+hoff, 282, 40);
		GetDlgItem(IDC_CHK_AUTOLGN)->MoveWindow(84, 206+hoff, 123, 16);
		GetDlgItem(IDC_BTN_FGTPWD)->MoveWindow(228, 202+hoff, 60, 23);
		GetDlgItem(IDC_BTN_VLINE4)->MoveWindow(303, 206+hoff, 9, 20);
		GetDlgItem(IDC_BTN_REGUSR)->MoveWindow(309, 202+hoff, 60, 23);
		GetDlgItem(IDOK)->MoveWindow(84, 234+hoff, 285, 23);
		//GetDlgItem(IDC_LAB_AGR)->MoveWindow(84, 283+hoff, 114, 16);
		GetDlgItem(IDC_CHK_AGREE)->MoveWindow(84, 283+hoff, 114, 16);
		GetDlgItem(IDC_BTN_AGR)->MoveWindow(207, 281+hoff, 60, 23);
		GetDlgItem(IDC_LAB_AGR2)->MoveWindow(281, 283+hoff, 15, 16);
		GetDlgItem(IDC_BTN_PRI)->MoveWindow(297, 281+hoff, 60, 23);
		if (1 == m_mode) {
			GetDlgItem(IDC_EDT_USR)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHK_AUTOLGN)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTN_FGTPWD)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTN_VLINE4)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTN_REGUSR)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CHK_AGREE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTN_AGR)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LAB_AGR2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTN_PRI)->ShowWindow(SW_HIDE);
			//this->SetDlgItemText(IDOK, _T("确定"));
		}
	}
}
BOOL CDlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	this->FreezeWnd();
	CString strtmp;
	this->SetWindowText(_T("KdDeskTC"));
	this->SetDlgItemText(IDC_LAB_TITLE, _TXTLNG(_T("KdDeskTC"), _T("KdDeskTC")));
	CFont* fntptr = CreateFontEx(18, FW_NORMAL);//FW_BOLD);
	((CStatic*)this->GetDlgItem(IDC_LAB_TITLE))->SetFont(fntptr);
	fntptr = CreateFontEx(20, FW_BOLD);
	((CStatic*)this->GetDlgItem(IDC_LAB_MSG))->SetFont(fntptr);
	g_pdlg_lgn = this;
	RECT rct;
	this->GetWindowRect(&rct);

	m_imgBackGround.Destroy();
	m_imgBackGround.Create(rct.right-rct.left, rct.bottom-rct.top, 32);
	CrossImageEx(m_imgBackGround, 255, 255, 255, 255, 255, 255);

	CClientDC dc(this);
	m_dcBackGround.CreateCompatibleDC(&dc);
	m_hOldBG = m_dcBackGround.SelectObject(m_imgBackGround);

	m_btn_cls.LoadImages(
		_T("SkinPic/cls_nor.png"),
		_T("SkinPic/cls_hot.png"),
		_T("SkinPic/cls_pre.png"),
		_T("SkinPic/cls_nor.png"),
		_T("SkinPic/cls_nor.png"));
	m_btn_vline3.LoadImages(
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"));
	m_btn_vline4.m_frc_w = 3;
	m_btn_vline4.m_frc_h = 15;
	m_btn_vline4.LoadImages(
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"));
	m_btn_hlp2.LoadImages(
		_T("SkinPic/hlp_nor.png"),
		_T("SkinPic/hlp_hot.png"),
		_T("SkinPic/hlp_pre.png"),
		_T("SkinPic/hlp_nor.png"),
		_T("SkinPic/hlp_nor.png"));
	m_btn_lgn.LoadImages(
		_T("SkinPic/login_nor.png"),
		_T("SkinPic/login_hot.png"),
		_T("SkinPic/login_pre.png"),
		_T("SkinPic/login_nor.png"),
		_T("SkinPic/login_nor.png"));
	m_btn_fgtpwd.LoadImages(
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"));
	m_btn_regusr.LoadImages(
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"));
	m_btn_agr.LoadImages(
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"));
	m_btn_pri.LoadImages(
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"),
		_T("SkinPic/lnkbtn4_255.png"));
	m_btn_logolb3.LoadImages(
		_T("SkinPic/logo2.png"),
		_T("SkinPic/logo2.png"),
		_T("SkinPic/logo2.png"),
		_T("SkinPic/logo2.png"),
		_T("SkinPic/logo2.png"));

	HFONT hfont = createfontex(20, FW_NORMAL);
	m_btn_fgtpwd.SetTextFont(hfont);
	m_btn_fgtpwd.m_crTextColor = RGB(75, 75, 75);
	m_btn_fgtpwd.m_crTextColor_hvr = RGB(54, 129, 255);
	m_btn_fgtpwd.m_crTextColor_pre = RGB(37, 111, 236);
	m_btn_fgtpwd.m_h_align = DT_RIGHT;

	m_btn_regusr.SetTextFont(hfont);
	m_btn_regusr.m_crTextColor = RGB(75, 75, 75);
	m_btn_regusr.m_crTextColor_hvr = RGB(54, 129, 255);
	m_btn_regusr.m_crTextColor_pre = RGB(37, 111, 236);
	m_btn_regusr.m_h_align = DT_LEFT;

	HFONT hfont17 = createfontex(17, FW_NORMAL);
	m_btn_agr.SetTextFont(hfont17);
	m_btn_agr.m_crTextColor = RGB(0, 112, 249);
	m_btn_agr.m_crTextColor_hvr = RGB(54, 129, 255);
	m_btn_agr.m_crTextColor_pre = RGB(37, 111, 236);
	m_btn_agr.m_h_align = DT_RIGHT;

	m_btn_pri.SetTextFont(hfont17);
	m_btn_pri.m_crTextColor = RGB(0, 112, 249);
	m_btn_pri.m_crTextColor_hvr = RGB(54, 129, 255);
	m_btn_pri.m_crTextColor_pre = RGB(37, 111, 236);
	m_btn_pri.m_h_align = DT_LEFT;

	CFont* fntptr17 = CreateFontEx(17, FW_NORMAL);
	//((CStatic*)this->GetDlgItem(IDC_LAB_AGR))->SetFont(fntptr17);
	((CStatic*)this->GetDlgItem(IDC_LAB_AGR2))->SetFont(fntptr17);

	fntptr = CreateFontEx(20, FW_NORMAL);
	m_edt_usr.SetFont(fntptr);
	m_edt_usr.SetMargin(CRect(30, 7, 3, 3));
	m_edt_usr.SetShowType(SHOW_TYPE_NORMAL);
	m_edt_usr.m_crEdgeOuterNormal = RGB(225, 225, 225);
	m_edt_usr.m_crEdgeOuterHighLight = RGB(0, 112, 249);
	m_edt_usr.m_crEdgeInner = RGB(255, 255, 255);
	m_edt_usr.m_csttxtclr = 1;
	m_edt_usr.m_CstTxtClr = RGB(0, 0, 0);
	m_edt_usr.SetTipText(_TXTLNG(_T("金蝶云账号"), _T("Enter phone number")));
	m_edt_usr.LoadImages(_T("SkinPic/usr.png"));

	m_edt_pwd.SetFont(fntptr);
	m_edt_pwd.SetMargin(CRect(30, 7, 3, 3));
	m_edt_pwd.SetShowType(SHOW_TYPE_PASS);
	m_edt_pwd.m_crEdgeOuterNormal = RGB(225, 225, 225);
	m_edt_pwd.m_crEdgeOuterHighLight = RGB(0, 112, 249);
	m_edt_pwd.m_crEdgeInner = RGB(255, 255, 255);
	m_edt_pwd.m_csttxtclr = 1;
	m_edt_pwd.m_CstTxtClr = RGB(0, 0, 0);
	m_edt_pwd.SetTipText(_TXTLNG(_T("请输入密码"), _T("Enter password")));
	m_edt_pwd.LoadImages(_T("SkinPic/pwd.png"));

	m_edt_usr.SetLimitText(24);
	m_edt_pwd.SetLimitText(24);

	//HFONT hfont = createfontex(16, FW_NORMAL);
	m_chk_autolgn.SetTextFont(hfont);
	m_chk_autolgn.SetTextColor(RGB(39, 39, 39));
	m_chk_autolgn.LoadImages(
		_T("SkinPic/cb_unchked.png"), 
		_T("SkinPic/cb_chked.png"),
		_T("SkinPic/disable_unchecked.png"), 
		_T("SkinPic/disable_checked.png"));

	m_chk_agree.SetTextFont(hfont);
	m_chk_agree.SetTextColor(RGB(39, 39, 39));
	m_chk_agree.LoadImages(
		_T("SkinPic/cb_unchked.png"), 
		_T("SkinPic/cb_chked.png"),
		_T("SkinPic/disable_unchecked.png"), 
		_T("SkinPic/disable_checked.png"));

	int offset = 8;
	this->GetClientRect(&rct);
	RECT rct_tmp;
	this->GetDlgItem(IDC_BTN_CLS)->GetClientRect(&rct_tmp);
	this->GetDlgItem(IDC_BTN_CLS)->ClientToScreen(&rct_tmp);
	this->ScreenToClient(&rct_tmp);
	int w = rct_tmp.right - rct_tmp.left;
	int h = rct_tmp.bottom - rct_tmp.top;
	rct_tmp.right = rct.right - rct.left - offset;
	rct_tmp.left = rct_tmp.right - w;
	rct_tmp.top = 8;
	rct_tmp.bottom = rct_tmp.top + h;
	this->GetDlgItem(IDC_BTN_CLS)->MoveWindow(&rct_tmp);

	offset += w + 4;
	this->GetDlgItem(IDC_BTN_VLINE3)->GetClientRect(&rct_tmp);
	this->GetDlgItem(IDC_BTN_VLINE3)->ClientToScreen(&rct_tmp);
	this->ScreenToClient(&rct_tmp);
	w = rct_tmp.right - rct_tmp.left;
	h = rct_tmp.bottom - rct_tmp.top;
	rct_tmp.right = rct.right - rct.left - offset;
	rct_tmp.left = rct_tmp.right - w;
	rct_tmp.top = 12;
	rct_tmp.bottom = rct_tmp.top + h;
	this->GetDlgItem(IDC_BTN_VLINE3)->MoveWindow(&rct_tmp);

	offset += w + 12;
	this->GetDlgItem(IDC_BTN_HLP2)->GetClientRect(&rct_tmp);
	this->GetDlgItem(IDC_BTN_HLP2)->ClientToScreen(&rct_tmp);
	this->ScreenToClient(&rct_tmp);
	w = rct_tmp.right - rct_tmp.left;
	h = rct_tmp.bottom - rct_tmp.top;
	rct_tmp.right = rct.right - rct.left - offset;
	rct_tmp.left = rct_tmp.right - w;
	rct_tmp.top = 13;
	rct_tmp.bottom = rct_tmp.top + h;
	this->GetDlgItem(IDC_BTN_HLP2)->MoveWindow(&rct_tmp);

	DrawRoundCorner();

	GetDlgItem(IDC_EDT_USR)->GetWindowRect(&m_rct_edtusr);
	ScreenToClient(&m_rct_edtusr);
	GetDlgItem(IDC_EDT_PWD)->GetWindowRect(&m_rct_edtpwd);
	ScreenToClient(&m_rct_edtpwd);
	m_autolgn = theApp.GetProfileInt(_T("KdDeskTC"), _T("AutoLgn"), 0);
	int logout = theApp.GetProfileInt(_T("KdDeskTC"), _T("Logout"), 0);
	if (0 == m_mode && 0 == m_autolgn && 0 == logout) {
		theApp.WriteProfileString(_T("KdDeskTC"), _T("Certificate"), _T(""));
		theApp.WriteProfileString(_T("KdDeskTC"), _T("Pwd"), _T(""));
	}
	m_strusr = theApp.GetProfileString(_T("KdDeskTC"), _T("User"), _T(""));
	if (!m_strusr.IsEmpty()) {
	this->SetDlgItemText(IDC_EDT_USR, m_strusr);
	}
	if (0 == m_mode) {
	m_strpwd = theApp.GetProfileString(_T("KdDeskTC"), _T("Pwd"), _T(""));
	if (!m_strpwd.IsEmpty()) {
	//this->SetDlgItemText(IDC_EDT_PWD, m_strpwd);
		m_edt_pwd.SetText(m_strpwd);
	}
	strtmp = theApp.GetProfileString(_T("KdDeskTC"), _T("Certificate"), _T(""));
	if (!strtmp.IsEmpty()) {
		strcpy(BufOutCrt, t2a(strtmp.GetBuffer()).c_str());
	}
	((CButton*)this->GetDlgItem(IDC_CHK_AUTOLGN))->SetCheck(m_autolgn);
	if (1 == m_autolgn && 0 == logout && !m_strusr.IsEmpty() && 0 != BufOutCrt[0]) {
		m_tohide = 1;
		HANDLE handle = CreateThread(NULL, 0, threadproc_login, NULL, 0, NULL);
		CloseHandle(handle);
	}
	if (1 == logout) {
		theApp.WriteProfileInt(_T("KdDeskTC"), _T("Logout"), 0);
	}
	}
	m_Shadow.YFCreate(GetSafeHwnd());
	this->SetTimer(TIMER_REDRAW_EVENT, TIMER_REDRAW_ELAPSE, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgLogin::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strtmp;
	if (1 == this->m_inlogin) return;
	if (m_edt_usr.GetText().IsEmpty()) {
		m_edt_usr.m_crEdgeOuterNormal = RGB(255, 90, 90);
	}
	if (m_edt_pwd.GetText().IsEmpty()) {
		m_edt_pwd.m_crEdgeOuterNormal = RGB(255, 90, 90);
	}
	if (m_edt_usr.GetText().IsEmpty() || m_edt_pwd.GetText().IsEmpty()) {
		InvalidateRect(&m_rct_edtusr);
		InvalidateRect(&m_rct_edtpwd);
		CDlgMsgBox2 dlgmsgbox(0, _TXTLNG(_T("提示"), _T("Prompt")), (0 == m_mode ? _T("用户名与密码不能为空！") : _T("密码不能为空！")));
		dlgmsgbox.DoModal();
		return;
	}
	if (0 == m_mode && !m_chk_agree.GetCheck()) {
		CDlgMsgBox2 dlgmsgbox(0, _TXTLNG(_T("提示"), _T("Prompt")), _T("请阅读《用户协议》与《客户隐私》并同意！"));
		dlgmsgbox.DoModal();
		return;
	}
	//OnOK();
	this->m_strusr = m_edt_usr.GetText();
	if (this->m_strpwd != m_edt_pwd.GetText()) {
		this->m_strpwd = m_edt_pwd.GetText();
		if (0 == m_mode) {
		theApp.WriteProfileString(_T("KdDeskTC"), _T("Certificate"), _T(""));
		BufOutCrt[0] = 0;
		}
	}
	if (0 == m_mode) {
	m_autolgn = ((CButton*)this->GetDlgItem(IDC_CHK_AUTOLGN))->GetCheck();
	theApp.WriteProfileString(_T("KdDeskTC"), _T("User"), m_strusr);
	for (int i = 0; i < m_strpwd.GetLength(); i++) {
		strtmp += _T("#");
	}
	if (m_autolgn) {
	theApp.WriteProfileString(_T("KdDeskTC"), _T("Pwd"), strtmp);
	}
	theApp.WriteProfileString(_T("KdDeskTC"), _T("Pwd2"), strtmp);
	theApp.WriteProfileInt(_T("KdDeskTC"), _T("AutoLgn"), m_autolgn);
	}
	HANDLE handle = CreateThread(NULL, 0, threadproc_login, NULL, 0, NULL);
	CloseHandle(handle);
}
void CDlgLogin::DrawRoundCorner()
{
	CRgn rgn;
	CRect rct;
	GetWindowRect(&rct);
	rct -= rct.TopLeft();
	rgn.CreateRoundRectRgn(rct.left, rct.top, rct.right, rct.bottom, 16, 16);
	SetWindowRgn(rgn, TRUE);
}
void CDlgLogin::OnBnClickedBtnCls()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(WM_SYSCOMMAND, SC_CLOSE);
}

void CDlgLogin::WinHelp(DWORD dwData, UINT nCmd)
{
	// TODO: 在此添加专用代码和/或调用基类
	return;
	CDialog::WinHelp(dwData, nCmd);
}

void CDlgLogin::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CDlgLogin::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDC MemDC;
	CBitmap bmp;
	CBitmap *pOldBmp;

	CRect rcClient;
	GetClientRect(rcClient);

	MemDC.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height());
	pOldBmp = MemDC.SelectObject(&bmp);

	MemDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &m_dcBackGround, 0, 0, SRCCOPY);

	pDC->BitBlt(
		0, 0,
		rcClient.Width(), rcClient.Height(),
		&MemDC,
		0, 0,
		SRCCOPY
		);

	MemDC.SelectObject(pOldBmp);

	if (MemDC.GetSafeHdc() != NULL)
	{
		MemDC.DeleteDC();
	}
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

HBRUSH CDlgLogin::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	switch(pWnd->GetDlgCtrlID()) {
		case IDC_LAB_TITLE: {
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(111,121,140));
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);   
			break;
		}
		case IDC_LAB_MSG: {
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(39,39,39));
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
			break;
		}
		//case IDC_LAB_AGR: 
		case IDC_LAB_AGR2: 
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(121,121,121));
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
			break;
		}
		default: {
			break;
		}
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CDlgLogin::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (TIMER_REDRAW_EVENT == nIDEvent) {
		::RedrawWindow(this->m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
		this->KillTimer(TIMER_REDRAW_EVENT);
		//set_foreground_wnd(this->m_hWnd);
	}
	CDialog::OnTimer(nIDEvent);
}
LRESULT CDlgLogin::OnCltCommand(WPARAM wParam, LPARAM lParam)
{
	CString strtmp;
	switch (wParam) {
		case 10: {
			if (200 == lParam) {
				if (0 == m_mode) {
				g_vec_dvc = g_vec_dvc_tmp;
				}
				OnOK();
			} else {
				set_flagex(_T("0"));
				strtmp.Format(_T("登录失败！%s"), a2t(BufOutErr).c_str());
				CDlgMsgBox2 dlgmsgbox(0, _T("提示"), strtmp);
				dlgmsgbox.DoModal();
				m_edt_pwd.SetText(_T(""));
				if (0 == m_mode) {
				//theApp.WriteProfileString(_T("KdDeskTC"), _T("User"), _T(""));
				theApp.WriteProfileInt(_T("KdDeskTC"), _T("AutoLgn"), 0);
				theApp.WriteProfileString(_T("KdDeskTC"), _T("Certificate"), _T(""));
				BufOutCrt[0] = 0;
				}
				if (1 == m_tohide) {
				m_tohide = 0;
				this->ShowWindow(SW_SHOW);
				}
			}
			break;
		}
		case 20: {
			set_flagex(_T("0"));
			CDlgMsgBox2 dlgmsgbox(0, _TXTLNG(_T("提示"), _T("Prompt")), _T("接口调用异常！"));
			dlgmsgbox.DoModal();
			if (0 == m_mode) {
			//theApp.WriteProfileString(_T("KdDeskTC"), _T("User"), _T(""));
			theApp.WriteProfileInt(_T("KdDeskTC"), _T("AutoLgn"), 0);
			theApp.WriteProfileString(_T("KdDeskTC"), _T("Certificate"), _T(""));
			BufOutCrt[0] = 0;
			}
			if (1 == m_tohide) {
			m_tohide = 0;
			this->ShowWindow(SW_SHOW);
			}
			break;
		}
	}
	return 0;
}
LRESULT CDlgLogin::OnTransaprentBk(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hwnd = (HWND)lParam;
	CTransparentHelper::OnTranparentControl(m_dcBackGround.GetSafeHdc(), (WPARAM)hdc, (LPARAM)hwnd);
	return TRUE;
}
void CDlgLogin::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (0 == m_SelfResize) { // 防止死循环。
	this->FreezeWnd(1);
	} else if (1 == m_SelfResize) {
		m_SelfResize = 0;
	}
	// TODO: 在此处添加消息处理程序代码
}
void CDlgLogin::OnBnClickedBtnFgtpwd()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, _T("open"), _T("https://cloud.kingdee.com/passport/#/password_find"), _T(""), NULL, SW_SHOWNORMAL);
}

void CDlgLogin::OnBnClickedBtnRegusr()
{
	// TODO: 在此添加控件通知处理程序代码
	//ShellExecute(NULL, _T("open"), _T("https://www.jdy.com/reg/?channelCode=JWB0104"), _T(""), NULL, SW_SHOWNORMAL);
	ShellExecute(NULL, _T("open"), _T("https://cloud.kingdee.com/passport/#/auth/reg"), _T(""), NULL, SW_SHOWNORMAL);
}

void CDlgLogin::OnBnClickedChkAutolgn()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CDlgLogin::OnBnClickedBtnHlp2()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, _T("open"), _T("https://kisdoc.kingdee.com/web/#/1452/2832"), _T(""), NULL, SW_SHOWNORMAL);
}
void CDlgLogin::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (1 == m_tohide) {
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
		m_Shadow.ToHideShadow();
	}
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
}
void CDlgLogin::OnBnClickedBtnAgr()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, _T("open"), _T("https://account.kdcloud.com/privacy?id=3515726087781072896"), _T(""), NULL, SW_SHOWNORMAL);
}

void CDlgLogin::OnBnClickedBtnPri()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, _T("open"), _T("https://account.kdcloud.com/privacy?id=3515729708820185088"), _T(""), NULL, SW_SHOWNORMAL);
}
