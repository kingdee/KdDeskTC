#pragma once
/************************************************************************
* Include Declaration													*
************************************************************************/
#include "resource.h"
#include "common_types.h"
/************************************************************************
* Class CTrayIcon														*
************************************************************************/
class CTrayIcon
{
	private:
		CTrayIcon();
	public:
		~CTrayIcon();

		// ��ʼ���������뷨���̡�
		static bool InitTrayIcon();
		// �޸��������뷨���̵�ͼ�����ʾ��Ϣ��
		static bool ModifyTrayicon(HICON hIcon, tstring strTip);
		// ɾ���������뷨���̡�
		static bool RemoveTrayIcon();
		// �������������뷨����ͼ���������¼���
		static void DoContextMenu();
		// ����һ��������뷨����ͼ���������¼���
		static void RDoContextMenu();
		// �����˫���������뷨����ͼ���������¼���
		static void OnLButtonDblClk();
		// �������뷨���̻ص�������
		static LONG WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		// ��ʼ�����뷨���̴��塣
		static bool InitWindow();
		// �������뷨���̴��塣
		static bool CreateILTray();
		// �������뷨���̴��塣
		static bool DeleteILTray();

		static const UINT WM_TRAY_NOTIFY = (WM_APP + 1000);
	//private:
		// NOTIFYICONDATA Structure: Contains information that the system needs to process taskbar status area messages.
		static NOTIFYICONDATA nid;
		static HWND hWnd_il;
		// ��ǰ RDP Connector ���ھ����
};