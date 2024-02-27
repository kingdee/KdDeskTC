// DlgMsgBox2.cpp : 实现文件
//

#include "stdafx.h"
#include "KdDeskTC.h"
#include ".\dlgmsgbox2.h"

CFont* CreateFontEx(int nHeight, int nWeight);
HFONT createfontex(int nheight, int nweight);
DWORD WINAPI threadproc_dninstini(LPVOID lpParameter);
extern int g_in_dninstexe;
// CDlgMsgBox2 对话框

IMPLEMENT_DYNAMIC(CDlgMsgBox2, CDialog)
CDlgMsgBox2::CDlgMsgBox2(int mode, CString strtitle, CString strinfo, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMsgBox2::IDD, pParent)
{
	this->m_mode = mode;
	this->m_strtitle = strtitle;
	this->m_strinfo = strinfo;
	m_SelfResize = -1;
	m_tohide = 0;
}

CDlgMsgBox2::~CDlgMsgBox2()
{
}

void CDlgMsgBox2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btn_ok);
	DDX_Control(pDX, IDCANCEL, m_btn_cancel);
	DDX_Control(pDX, IDC_BTN_CLS, m_btn_cls);
}


BEGIN_MESSAGE_MAP(CDlgMsgBox2, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_CLS, OnBnClickedBtnCls)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_TRANSPARENT_BK_CTOP, OnTransaprentBk)
	ON_MESSAGE(WM_CLT_COMMAND, OnCltCommand)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


// CDlgMsgBox2 消息处理程序
void CDlgMsgBox2::FreezeWnd(int flag) {
	RECT rctmp;
	/*this->GetWindowRect(&rctmp);
	LogMsg(_T("CDlgMsgBox->FreezeWnd, this->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_LAB_TITLE)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_LAB_TITLE)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgMsgBox->FreezeWnd, GetDlgItem(IDC_LAB_TITLE)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_LAB_INFO)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_LAB_INFO)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgMsgBox->FreezeWnd, GetDlgItem(IDC_LAB_INFO)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDOK)->GetClientRect(&rctmp);
	this->GetDlgItem(IDOK)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgMsgBox->FreezeWnd, GetDlgItem(IDOK)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDCANCEL)->GetClientRect(&rctmp);
	this->GetDlgItem(IDCANCEL)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CDlgMsgBox->FreezeWnd, GetDlgItem(IDCANCEL)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);*/
	this->GetWindowRect(&rctmp);
	BOOL ischanged = 0;
	m_SelfResize = 0;
	if (377 != rctmp.right-rctmp.left) {
	rctmp.right = rctmp.left + 377;
	ischanged = 1;
	}
	if (165 != rctmp.bottom-rctmp.top) {
	rctmp.bottom = rctmp.top + 165;
	ischanged = 1;
	}
	if (ischanged) {
		m_SelfResize = 1;
		this->MoveWindow(&rctmp);
	}
	if (0 == flag) {
	GetDlgItem(IDC_LAB_TITLE)->MoveWindow(8, 7, 333, 24);//(5, 7, 333, 26);
	GetDlgItem(IDC_LAB_INFO)->MoveWindow(8, 49, 365, 60);
	GetDlgItem(IDOK)->MoveWindow(113, 117, 75, 23);
	GetDlgItem(IDCANCEL)->MoveWindow(189, 117, 75, 23);
	}
}
BOOL CDlgMsgBox2::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	this->FreezeWnd();
	CString strtmp;
	GetDlgItem(IDC_LAB_INFO)->GetWindowRect(&m_li_rct); 
	ScreenToClient(&m_li_rct);
	m_plab_title = (CStatic*)this->GetDlgItem(IDC_LAB_TITLE);

	CFont* fntptr = CreateFontEx(19, FW_BOLD);
	m_plab_title->SetFont(fntptr);
	fntptr = CreateFontEx(18, FW_NORMAL);
	((CStatic*)this->GetDlgItem(IDC_LAB_INFO))->SetFont(fntptr);
	// 经测试发现，对话框关闭将会销毁字体句柄。
	HFONT hfont = createfontex(14, FW_NORMAL);
	m_btn_ok.SetTextFont(hfont);
	m_btn_cancel.SetTextFont(hfont);

	this->SetDlgItemText(IDC_LAB_TITLE, m_strtitle);
	this->SetDlgItemText(IDC_LAB_INFO, m_strinfo);
	//this->SetDlgItemText(IDOK, _TXTLNG(_T("确定"), _T("OK")));
	//this->SetDlgItemText(IDCANCEL, _TXTLNG(_T("取消"), _T("Cancel")));
	this->SetDlgItemText(IDOK, _TXTLNG(_T(""), _T("")));
	this->SetDlgItemText(IDCANCEL, _TXTLNG(_T(""), _T("")));

	RECT rct, rctmp;
	this->GetWindowRect(&rct);

	m_imgBackGround.Destroy();
	m_imgBackGround.Create(rct.right-rct.left, rct.bottom-rct.top, 32);
	CrossImageEx(m_imgBackGround, 255, 255, 255, 255, 255, 255);

	CClientDC dc(this);
	m_dcBackGround.CreateCompatibleDC(&dc);
	m_hOldBG = m_dcBackGround.SelectObject(m_imgBackGround);

	m_btn_ok.LoadImages(
		_T("SkinPic/ok_nor.png"),
		_T("SkinPic/ok_hot.png"),
		_T("SkinPic/ok_pre.png"),
		_T("SkinPic/ok_nor.png"),
		_T("SkinPic/ok_nor.png"));

	m_btn_cancel.LoadImages(
		_T("SkinPic/cancel_nor.png"),
		_T("SkinPic/cancel_hot.png"),
		_T("SkinPic/cancel_pre.png"),
		_T("SkinPic/cancel_nor.png"),
		_T("SkinPic/cancel_nor.png"));

	m_btn_cls.LoadImages(
		_T("SkinPic/cls_nor.png"),
		_T("SkinPic/cls_hot.png"),
		_T("SkinPic/cls_pre.png"),
		_T("SkinPic/cls_nor.png"),
		_T("SkinPic/cls_nor.png"));

	m_btn_cancel.m_crTextColor = RGB(255, 255, 255);
	m_btn_cancel.m_crTextColor_hvr = RGB(255, 255, 255);

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

	if (0 == this->m_mode) {
		theApp.center_dlgitm(this, IDOK);
		this->GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
	} else if (1 == this->m_mode) {
		theApp.center_dlgitm(this, IDCANCEL);
		this->GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		//m_tohide = 1;
		CloseHandle(CreateThread(NULL, 0, threadproc_dninstini, (LPVOID)m_hWnd, 0, NULL));
	} else if (2 == this->m_mode) {
		theApp.center_dlgitm(this, IDOK, IDCANCEL);
	}

	DrawRoundCorner();

	m_Shadow.YFCreate(GetSafeHwnd());
	this->SetTimer(TIMER_REDRAW_EVENT, TIMER_REDRAW_ELAPSE, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgMsgBox2::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CDlgMsgBox2::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if (1 == m_mode) {
		if (0 == g_in_dninstexe) {
			this->OnOK();
			return;
		}
	}
	OnCancel();
}

void CDlgMsgBox2::OnBnClickedBtnCls()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(WM_SYSCOMMAND, SC_CLOSE); // 触发OnBnClickedCancel()。
}
void CDlgMsgBox2::DrawRoundCorner()
{
	CRgn rgn;
	CRect rct;
	GetWindowRect(&rct);
	rct -= rct.TopLeft();
	rgn.CreateRoundRectRgn(rct.left, rct.top, rct.right, rct.bottom, 16, 16);
	SetWindowRgn(rgn, TRUE);
}
BOOL CDlgMsgBox2::OnEraseBkgnd(CDC* pDC)
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

	// 绘制背景
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
LRESULT CDlgMsgBox2::OnTransaprentBk(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hwnd = (HWND)lParam;
	CTransparentHelper::OnTranparentControl(m_dcBackGround.GetSafeHdc(), (WPARAM)hdc, (LPARAM)hwnd);
	return TRUE;
}
HBRUSH CDlgMsgBox2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	switch(pWnd->GetDlgCtrlID()) {
		case IDC_LAB_TITLE: {
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(22,23,24));
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);   
			break;
		}
		case IDC_LAB_INFO:{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(51, 56, 61));
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);   
			break;
		}
		default: {
			break;
		}
	}
	return hbr;
}
void CDlgMsgBox2::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgMsgBox2::WinHelp(DWORD dwData, UINT nCmd)
{
	// TODO: 在此添加专用代码和/或调用基类
	return;
	CDialog::WinHelp(dwData, nCmd);
}
void CDlgMsgBox2::setinfo(CString strv)
{
	this->m_tohide = 0;
	if (!::IsWindowVisible(this->m_hWnd)) {
		this->ShowWindow(SW_SHOW);
	}
	//TCHAR* strv = (TCHAR*)lParam;
	this->SetDlgItemText(IDC_LAB_INFO, strv);
	InvalidateRect(&m_li_rct);
	UpdateWindow();
}
LRESULT CDlgMsgBox2::OnCltCommand(WPARAM wParam, LPARAM lParam)
{
	CString strtmp;
	switch (wParam) {
		case 10: {
			strtmp = (TCHAR*)lParam;
			setinfo(strtmp);
			break;
		}
		case 11: {
			TCHAR* strv = (TCHAR*)lParam;
			this->SetWindowText(strv);
			this->SetDlgItemText(IDC_LAB_TITLE, strv);
			break;
		}
		case 20: {
			if (0 == lParam) {
				this->OnOK();
			} else {
				this->OnCancel();
			}
			break;
		}
	}
	return 0;
}
void CDlgMsgBox2::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (TIMER_REDRAW_EVENT == nIDEvent) {
		::RedrawWindow(this->m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
		this->KillTimer(TIMER_REDRAW_EVENT);
		// 经测试发现，set_foreground_wnd会导致线程卡顿。
		//set_foreground_wnd(this->m_hWnd);
	}
	CDialog::OnTimer(nIDEvent);
}
void CDlgMsgBox2::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (0 == m_SelfResize) { // 防止死循环。
	this->FreezeWnd(1);
	} else if (1 == m_SelfResize) {
		m_SelfResize = 0;
	}
	// TODO: 在此处添加消息处理程序代码
}
void CDlgMsgBox2::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (1 == m_tohide) {
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
		m_Shadow.ToHideShadow();
	}
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
}
