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

		// 初始化虚拟输入法托盘。
		static bool InitTrayIcon();
		// 修改虚拟输入法托盘的图标和提示信息。
		static bool ModifyTrayicon(HICON hIcon, tstring strTip);
		// 删除虚拟输入法托盘。
		static bool RemoveTrayIcon();
		// 鼠标左击虚拟输入法托盘图标引发的事件。
		static void DoContextMenu();
		// 鼠标右击虚拟输入法托盘图标引发的事件。
		static void RDoContextMenu();
		// 鼠标左双击虚拟输入法托盘图标引发的事件。
		static void OnLButtonDblClk();
		// 虚拟输入法托盘回调函数。
		static LONG WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		// 初始化输入法托盘窗体。
		static bool InitWindow();
		// 创建输入法托盘窗体。
		static bool CreateILTray();
		// 销毁输入法托盘窗体。
		static bool DeleteILTray();

		static const UINT WM_TRAY_NOTIFY = (WM_APP + 1000);
	//private:
		// NOTIFYICONDATA Structure: Contains information that the system needs to process taskbar status area messages.
		static NOTIFYICONDATA nid;
		static HWND hWnd_il;
		// 当前 RDP Connector 窗口句柄。
};