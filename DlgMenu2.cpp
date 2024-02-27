// DlgMenu2.cpp : 实现文件
//

#include "stdafx.h"
#include "KdDeskTC.h"
#include ".\dlgmenu2.h"
#include ".\kddesktcdlg.h"
HFONT createfontex(int nheight, int nweight);
CFont* CreateFontEx(int nHeight, int nWeight);
BOOL set_foreground_wnd(HWND hwnd);
extern CKdDeskTCDlg* g_pdlg_dsk;
// CDlgMenu2 对话框

IMPLEMENT_DYNAMIC(CDlgMenu2, CDialog)
CDlgMenu2::CDlgMenu2(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMenu2::IDD, pParent)
{
}

CDlgMenu2::~CDlgMenu2()
{
}

void CDlgMenu2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_OPEN, m_btn_open);
	DDX_Control(pDX, IDC_BTN_SETUP, m_btn_setup);
}

BEGIN_MESSAGE_MAP(CDlgMenu2, CDialog)
	ON_MESSAGE(WM_TRANSPARENT_BK_CTOP, OnTransaprentBk)
	ON_MESSAGE(WM_CLT_COMMAND, OnCltCommand)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_SETUP, OnBnClickedBtnSetup)
END_MESSAGE_MAP()

// CDlgMenu2 消息处理程序

BOOL CDlgMenu2::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	RECT rct;
	this->GetWindowRect(&rct);

	m_imgBackGround.Destroy();
	m_imgBackGround.Create(rct.right-rct.left, rct.bottom-rct.top, 32);
	CrossImageEx(m_imgBackGround, 255, 255, 255);

	CClientDC dc(this);
	m_dcBackGround.CreateCompatibleDC(&dc);
	m_hOldBG = m_dcBackGround.SelectObject(m_imgBackGround);

	m_vec_btn.push_back(&m_btn_open);
	m_vec_btn.push_back(this->GetDlgItem(IDC_PIC_SEP1));
	m_vec_btn.push_back(&m_btn_setup);

	DisplayTxtByLang();
	//经测试发现，17的字体(实际高度14像素)，纵向刚好居中。
	HFONT hfont = createfontex(18, FW_NORMAL);
	RECT rctmp;
	vector<CWnd*>::iterator iter__ = m_vec_btn.begin();
	while (iter__ != m_vec_btn.end()) {
		(*iter__)->GetWindowRect(&rctmp);
		if (1 < rctmp.bottom - rctmp.top) {
		((CButtonEx2*)(*iter__))->LoadImages(
			_T("SkinPic/lnk2_nml.png"),
			_T("SkinPic/lnk2_hvr.png"),
			_T("SkinPic/lnk2_prs.png"),
			_T("SkinPic/lnk2_nml.png"),
			_T("SkinPic/lnk2_nml.png"));
		((CButtonEx2*)(*iter__))->m_crTextColor = RGB(95, 95, 95);
		((CButtonEx2*)(*iter__))->m_crTextColor_hvr = RGB(255, 255, 255);
		((CButtonEx2*)(*iter__))->SetTextFont(hfont);
		((CButtonEx2*)(*iter__))->m_h_align = DT_LEFT;
		}
		++iter__;
	}

	m_Shadow.YFCreate(GetSafeHwnd());
	this->SetTimer(TIMER_REDRAW_EVENT, TIMER_REDRAW_ELAPSE, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CDlgMenu2::DisplayTxtByLang()
{
	if (1 == m_flag) {
	this->SetDlgItemText(IDC_BTN_OPEN, _TXTLNG(_T("    打开主界面"), _T("  Open WeWork")));
	this->SetDlgItemText(IDC_BTN_SETUP, _TXTLNG(_T("    退出"), _T("  Quit")));
	} else if (2 == m_flag) {
	this->SetDlgItemText(IDC_BTN_OPEN, _TXTLNG(_T("    更新验证码"), _T("  Refresh")));
	this->SetDlgItemText(IDC_BTN_SETUP, _TXTLNG(_T("    自定义验证码"), _T("  Customize")));
	} else if (3 == m_flag) {
		this->SetDlgItemText(IDC_BTN_OPEN, _TXTLNG(_T("    删除"), _T("  Remove")));
	//this->SetDlgItemText(IDC_BTN_OPEN, _TXTLNG(_T("    远程控制"), _T("  Remote Control")));
	//this->SetDlgItemText(IDC_BTN_SETUP, _TXTLNG(_T("    文件传输"), _T("  File transfer")));
	}
}
void CDlgMenu2::WinHelp(DWORD dwData, UINT nCmd)
{
	// TODO: 在此添加专用代码和/或调用基类
	return;
	CDialog::WinHelp(dwData, nCmd);
}
void CDlgMenu2::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (TITLEBAR_H >= point.y) {
	//PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	//}
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CDlgMenu2::OnEraseBkgnd(CDC* pDC)
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

HBRUSH CDlgMenu2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	switch(pWnd->GetDlgCtrlID()) {
		default: {
			break;
		}
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CDlgMenu2::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (TIMER_REDRAW_EVENT == nIDEvent) {
		::RedrawWindow(this->m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
		this->KillTimer(TIMER_REDRAW_EVENT);
		//set_foreground_wnd(this->m_hWnd);
	} else if (TIMER_ACTWND_EVENT == nIDEvent) {
		this->KillTimer(TIMER_ACTWND_EVENT);
		set_foreground_wnd(this->m_hWnd);
	}
	CDialog::OnTimer(nIDEvent);
}
LRESULT CDlgMenu2::OnCltCommand(WPARAM wParam, LPARAM lParam)
{
	CString strtmp;
	switch (wParam) {
		case 10: {
			break;
		}
	}
	return 0;
}
LRESULT CDlgMenu2::OnTransaprentBk(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hwnd = (HWND)lParam;
	CTransparentHelper::OnTranparentControl(m_dcBackGround.GetSafeHdc(), (WPARAM)hdc, (LPARAM)hwnd);
	return TRUE;
}
LRESULT CDlgMenu2::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (message) {
		case WM_ACTIVATE: {
			switch (LOWORD(wParam)) {
				case WA_ACTIVE: {
					if (-1 == m_pt.x && -1 == m_pt.y) {
						GetCursorPos(&m_pt);
					}
					//LogMsg(_T("CDlgMenu->WindowProc(WM_ACTIVATE), WA_ACTIVE, x:%d, y:%d."), m_pt.x, m_pt.y);
					break;
				}
				case WA_CLICKACTIVE: {
					//LogMsg(_T("CDlgMenu->WindowProc(WM_ACTIVATE), WA_CLICKACTIVE."));
					break;
				}
				case WA_INACTIVE: {
					POINT pt = { 0 };
					GetCursorPos(&pt);
					if (m_pt.x == pt.x && m_pt.y == pt.y && this->IsWindowVisible()) {
						//set_foreground_wnd(m_hWnd);
						this->SetTimer(TIMER_ACTWND_EVENT, TIMER_ACTWND_ELAPSE, 0);
					} else {
						this->ShowWindow(SW_HIDE);
					}
					//LogMsg(_T("CDlgMenu->WindowProc(WM_ACTIVATE), WA_INACTIVE, actwnd:%u."), (UINT)lParam);
					// [35:12_06 10_03_16_265][thread: 1992]CDlgMenu->WindowProc(WM_ACTIVATE), WA_INACTIVE, actwnd:14418724.
					// [36:12_06 10_03_16_266][thread: 1992]CDlgMenu->WindowProc(WM_ACTIVATE), WA_INACTIVE, actwnd:0.
					break;
				}
			}
			break;
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
void CDlgMenu2::RefreshMenu()
{
	m_pt.x = -1;
	m_pt.y = -1;
	RECT rctp, rctbtn = { 0 };
	this->GetWindowRect(&rctp);
	int x = 2, y = 2, w = 0, h = 0, i = 0;
	set<int> SetNoNeed;
	if (3 == m_flag) {
		SetNoNeed.insert(1);
		SetNoNeed.insert(2);
	}
	for (; i < m_vec_btn.size(); i++) {
		if (SetNoNeed.count(i)) {
			m_vec_btn[i]->ShowWindow(SW_HIDE);
			continue;
		}
		m_vec_btn[i]->ShowWindow(SW_SHOW);
		m_vec_btn[i]->GetWindowRect(&rctbtn);
		w = rctbtn.right - rctbtn.left;
		h = rctbtn.bottom - rctbtn.top;
		rctbtn.left = 2;
		rctbtn.top = y;
		rctbtn.right = rctbtn.left + w;
		rctbtn.bottom = rctbtn.top + h;
		m_vec_btn[i]->MoveWindow(&rctbtn);
		y += h + 2;
	}

	rctp.right = rctp.left + 2 + w + 2;// + 2;
	rctp.bottom = rctp.top + y;// + 2;
	this->MoveWindow(&rctp);
	DrawRoundCorner();
}
void CDlgMenu2::OnBnClickedBtnOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	this->ShowWindow(SW_HIDE);
	if (1 == m_flag) {
		::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 41, 64);
	} else if (2 == m_flag) {
		::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 41, 128);
	} else if (3 == m_flag) {
		::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 41, 1);
	}
}

void CDlgMenu2::OnBnClickedBtnSetup()
{
	// TODO: 在此添加控件通知处理程序代码
	this->ShowWindow(SW_HIDE);
	if (1 == m_flag) {
		::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 41, 6);
	} else if (2 == m_flag) {
		::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 41, 129);
	} else if (3 == m_flag) {
		::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 41, 2);
	}
}
void CDlgMenu2::DrawRoundCorner()
{
	/*CRgn rgn;
	CRect rct;
	GetWindowRect(&rct);
	rct -= rct.TopLeft();
	rgn.CreateRoundRectRgn(rct.left, rct.top, rct.right, rct.bottom, 8, 8);
	SetWindowRgn(rgn, TRUE);*/
}