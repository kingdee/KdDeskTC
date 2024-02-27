/************************************************************************
* Include Declaration													*
************************************************************************/
#include "StdAfx.h"
#include ".\trayicon.h"
#include ".\kddesktcdlg.h"
#include "wm_private.h"
extern CKdDeskTCDlg* g_pdlg_dsk;
HWND CTrayIcon::hWnd_il = NULL;
NOTIFYICONDATA CTrayIcon::nid;
// Class CTrayIcon constructor.
CTrayIcon::CTrayIcon(void)
{}
// Class CTrayIcon destructor.
CTrayIcon::~CTrayIcon(void)
{}

// ��ʼ���������뷨���̡�
bool CTrayIcon::InitTrayIcon()
{
	bool bResult = false;
	nid.cbSize = sizeof(NOTIFYICONDATA) ;
	nid.hWnd = hWnd_il;
	// uID: Application-defined identifier of the taskbar icon. Values from 0 to 12 are reserved and should not be used.
	nid.uID = 13;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = WM_TRAY_NOTIFY;  

	// Sends a message to the taskbar's status area.
	if (Shell_NotifyIcon(NIM_ADD, &nid))
	{
		bResult = true;
	}

	return bResult;
}

// �޸��������뷨���̵�ͼ�����ʾ��Ϣ��
bool CTrayIcon::ModifyTrayicon(HICON hIcon, tstring strTip)
{
	bool bResult = false;
	//nid.uFlags = NIF_GUID;
	if (!strTip.empty()) {
		//nid.uFlags |= NIF_TIP;
		_tcscpy(nid.szTip, strTip.c_str());
	}
	nid.hIcon = hIcon;
	//nid.uFlags |= NIF_ICON;
	// Sends a message to the taskbar's status area.
	bResult = Shell_NotifyIcon(NIM_MODIFY, &nid);
	return bResult;
}

// ɾ���������뷨���̡�
bool CTrayIcon::RemoveTrayIcon()
{
	bool bResult = false;
	if (Shell_NotifyIcon(NIM_DELETE, &nid))
	{
		bResult = true;
	}

	return bResult;
}

// �������������뷨����ͼ���������¼���
void CTrayIcon::DoContextMenu()
{//alwayslogmsg("g_tstflg:%d, g_acccnt:%d.", g_tstflg, g_acccnt);
	BOOL is_db_click = FALSE;
	static DWORD lc_prev_time = 0;
	if (!lc_prev_time) {
		lc_prev_time = GetTickCount();
	} else {
		DWORD curr_time = GetTickCount();
		if ((curr_time - lc_prev_time) <= 500) { // double click.
			is_db_click = TRUE;
		}
		lc_prev_time = curr_time;
	}
	if (is_db_click) {
		//OnLButtonDblClk();
		return;
	}
	/*if (NULL != g_pdlg_cld) {
		::PostMessage(g_pdlg_cld->m_hWnd, WM_CLD_COMMAND, 200, 0);
	}*/
	return;
}

// ����һ��������뷨����ͼ���������¼���
void CTrayIcon::RDoContextMenu()
{
	CString strtmp;
	BOOL is_db_click = FALSE;
	static DWORD rc_prev_time = 0;
	if (!rc_prev_time) {
		rc_prev_time = GetTickCount();
	} else {
		DWORD curr_time = GetTickCount();
		if ((curr_time - rc_prev_time) <= 500) { // double click.
			is_db_click = TRUE;
		}
		rc_prev_time = curr_time;
	}
	if (is_db_click) {
		//OnLButtonDblClk();
		return;
	}
	g_pdlg_dsk->popup_menu_ex1();
}

// �����˫���������뷨����ͼ���������¼���
void CTrayIcon::OnLButtonDblClk()
{
	if (NULL != g_pdlg_dsk) {
		::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 30, 0);
	}
}

// �������뷨���̻ص�������
LONG WINAPI CTrayIcon::MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
	switch (uMsg) {
		case WM_TRAY_NOTIFY: {
			switch (lParam) {
				case WM_LBUTTONDOWN: {
					DoContextMenu();
					break;
				}
				case WM_RBUTTONDOWN: {
					RDoContextMenu();
					break;
				}
				case WM_LBUTTONDBLCLK: {
					OnLButtonDblClk();
					break;
				}
				case WM_RBUTTONDBLCLK: {
					OnLButtonDblClk();
					break;
				}
			}
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam); 
} 

// ��ʼ�����뷨���̴��塣
bool CTrayIcon::InitWindow()
{
	hWnd_il = g_pdlg_dsk->m_pdlg_trayex->m_hWnd;
	if (!hWnd_il)
	{
		return false;
	}

	return true;
}

// �������뷨���̴��塣
bool CTrayIcon::CreateILTray()
{
	bool bResult = false;
	bResult = InitWindow();
	bResult = InitTrayIcon();
	return bResult;
}

// �������뷨���̴��塣
bool CTrayIcon::DeleteILTray()
{
	bool bResult = false;
	bResult = RemoveTrayIcon();
	//bResult = DestroyWindow(hWnd_il);
	return bResult;
}