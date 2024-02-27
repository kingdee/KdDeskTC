// DlgTrayEx.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KdDeskTC.h"
#include ".\dlgtrayex.h"
#include "TrayIcon.h"

// CDlgTrayEx �Ի���

IMPLEMENT_DYNAMIC(CDlgTrayEx, CDialog)
CDlgTrayEx::CDlgTrayEx(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTrayEx::IDD, pParent)
{
}

CDlgTrayEx::~CDlgTrayEx()
{
}

void CDlgTrayEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTrayEx, CDialog)
END_MESSAGE_MAP()


// CDlgTrayEx ��Ϣ�������

BOOL CDlgTrayEx::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgTrayEx::WinHelp(DWORD dwData, UINT nCmd)
{
	// TODO: �ڴ����ר�ô����/����û���
	return;
	CDialog::WinHelp(dwData, nCmd);
}

LRESULT CDlgTrayEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	switch (message) {
		case CTrayIcon::WM_TRAY_NOTIFY: {
			switch (lParam) {
				case WM_LBUTTONDOWN: {
					//LogMsg(_T("CDlgTrayEx->WindowProc(WM_LBUTTONDOWN)."));
					CTrayIcon::DoContextMenu();
					break;
				}
				case WM_RBUTTONDOWN: {
					//LogMsg(_T("CDlgTrayEx->WindowProc(WM_RBUTTONDOWN)."));
					CTrayIcon::RDoContextMenu();
					break;
				}
				case WM_LBUTTONDBLCLK: {
					//LogMsg(_T("CDlgTrayEx->WindowProc(WM_LBUTTONDBLCLK)."));
					CTrayIcon::OnLButtonDblClk();
					break;
				} 
				case WM_RBUTTONDBLCLK: {
					//LogMsg(_T("CDlgTrayEx->WindowProc(WM_RBUTTONDBLCLK)."));
					CTrayIcon::OnLButtonDblClk();
					break;
				}
			}
			break;
		}
		/*case WM_ACTIVATE: {
			if (LOWORD(wParam) == WA_INACTIVE) {
				LogMsg(_T("CDlgTrayEx->WindowProc(WM_ACTIVATE), WA_INACTIVE."));
			}
			break;
		}*/
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
