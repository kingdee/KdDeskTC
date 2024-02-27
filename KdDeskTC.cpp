// KdDeskTC.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "KdDeskTC.h"
#include "KdDeskTCDlg.h"
#include "DlgLogin.h"
#include "DlgMsgBox2.h"
#include <Lm.h>
#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "Version.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern int GetCurrDir(TCHAR *pBuff);
extern long file_size(FILE* stream);

TCHAR g_full_log_name[MAX_PATH] = { 0 };	// full name of the log(program debug message) file name.
TCHAR g_full_err_name[MAX_PATH] = { 0 };    // full name of the err file name.
TCHAR g_full_fat_name[MAX_PATH] = { 0 };    // full name of the fat file name.
static const TCHAR* g_ini_section = _T("KdDeskTC"); // Section of the config file.

// full path of the current directory.
TCHAR buff_workdir[MAX_PATH] = { 0 };
// full path of the config file.
TCHAR buff_inipath[MAX_PATH] = { 0 };
CHAR g_buff_path[MAX_PATH] = { 0 };
TCHAR g_history_path[MAX_PATH] = { 0 };

// 是否输出调试信息。
int g_isdebug = 0;
int active_hwnd(HWND hwnd);
void active_hwnd_ex(HWND hwnd);
int g_to_exit = 0;
TCHAR g_usr_win[64] = { 0 };
int g_usr_type = 0; // [0:user][1:system].
int g_language = 1;
int g_clseye = 1;
char g_devid[64] = { 0 };
CLTUSRLGN lpfn_CltUsrLgn = NULL;
GETDVCLST lpfn_GetDvcLst = NULL;
GETADVERTLST lpfn_GetAdvertLst = NULL;
DELCLDSKID lpfn_DelCldskId = NULL;
SUBCLDSKID lpfn_SubCldskId = NULL;
SUBLOG lpfn_SubLog = NULL;
HANDLE g_mapping_cmw = NULL;
CLDSKCLTWND* g_lp_cmw = NULL;
TCHAR szDesktopPath[MAX_PATH] = { 0 };
TCHAR g_dskshortcut[MAX_PATH] = { 0 };
TCHAR g_uninstshortcut[MAX_PATH] = { 0 };
TCHAR g_sm_sc_base[MAX_PATH] = { 0 };
TCHAR g_sm_sc_main[MAX_PATH] = { 0 };
TCHAR g_sm_sc_uninst[MAX_PATH] = { 0 };
DWORD WINAPI threadproc_startsvc(LPVOID lpParameter);
#include "utility.h"
int uninstallex();
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
BOOL IsWow64()
{
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
	BOOL bIsWow64 = FALSE;
	if (NULL != fnIsWow64Process) {
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
			return FALSE; // error handle
		}
	}
	return bIsWow64;
}
BOOL g_is_win_x64 = FALSE;
LPFN_WOW64DISABLEWOW64FSREDIRECTION lpfn_Wow64DisableWow64FsRedirection = NULL;
LPFN_WOW64REVERTWOW64FSREDIRECTION lpfn_Wow64RevertWow64FsRedirection = NULL;
void GetWow64FunPtr() {
	lpfn_Wow64DisableWow64FsRedirection = (LPFN_WOW64DISABLEWOW64FSREDIRECTION)GetProcAddress(GetModuleHandle(_T("kernel32")), "Wow64DisableWow64FsRedirection");
	lpfn_Wow64RevertWow64FsRedirection = (LPFN_WOW64REVERTWOW64FSREDIRECTION)GetProcAddress(GetModuleHandle(_T("kernel32")), "Wow64RevertWow64FsRedirection");
	LogMsg(_T("GetWow64FunPtr, lpfn_Wow64DisableWow64FsRedirection:%p, lpfn_Wow64RevertWow64FsRedirection:%p."), lpfn_Wow64DisableWow64FsRedirection, lpfn_Wow64RevertWow64FsRedirection);
}
string GetRegistryStringEx(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName) {
	char buftmp[1024] = { 0 };
	HKEY hkey = NULL;
	if (RegOpenKeyExA(hKey, lpSubKey, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hkey) == ERROR_SUCCESS) {
		DWORD cbData = sizeof(buftmp);
		if (RegQueryValueExA(hkey, lpValueName, 0, NULL, (LPBYTE)buftmp, &cbData) == ERROR_SUCCESS) {
		}
	}
	return buftmp;
}
BOOL SetRegistryStringEx(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName, string strValue) {
	BOOL bRes = FALSE;
	HKEY hkey = NULL;
	if (::RegCreateKeyExA(hKey, lpSubKey, 0, NULL, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, NULL, &hkey, NULL) == ERROR_SUCCESS) {
		if (::RegSetValueExA(hkey, lpValueName, NULL, REG_SZ, (const BYTE*)strValue.c_str(), strValue.length() + 1) == ERROR_SUCCESS) {
			bRes = TRUE;
		}
	}
	::RegCloseKey(hkey);
	return bRes;
}
int GetRegistryIntEx(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName) {
	int ntmp = 0;
	HKEY hkey = NULL;
	if (RegOpenKeyExA(hKey, lpSubKey, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hkey) == ERROR_SUCCESS) {
		DWORD cbData = sizeof(int);
		if (RegQueryValueExA(hkey, lpValueName, 0, NULL, (LPBYTE)&ntmp, &cbData) == ERROR_SUCCESS) {
		}
	}
	return ntmp;
}
BOOL SetRegistryIntEx(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName, int nValue) {
	BOOL bRes = FALSE;
	HKEY hkey = NULL;
	if (::RegCreateKeyExA(hKey, lpSubKey, 0, NULL, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, NULL, &hkey, NULL) == ERROR_SUCCESS) {
		if (::RegSetValueExA(hkey, lpValueName, NULL, REG_DWORD, (const BYTE*)&nValue, sizeof(int)) == ERROR_SUCCESS) {
			bRes = TRUE;
		}
	}
	::RegCloseKey(hkey);
	return bRes;
}
void DelRegValueEx(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName) {
	char buftmp[1024] = { 0 };
	HKEY hkey = NULL;
	if (RegOpenKeyExA(hKey, lpSubKey, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hkey) == ERROR_SUCCESS) {
		if (RegDeleteValueA(hkey, lpValueName) == ERROR_SUCCESS) {
		}
	}
}
void AdjustTokenPrivilegesForNT()
{
	HANDLE htoken; 
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &htoken)) {
		TOKEN_PRIVILEGES tp;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (!AdjustTokenPrivileges(htoken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0)) {
			//alwayslogmsg("CDlgMsgBox->OnKtCommand, AdjustTokenPrivileges fail.");
		}
		CloseHandle(htoken);
	}
}
BOOL IsAdminCurUsr() {
	DWORD ret__ = FALSE, utmp;
	USER_INFO_1* ui1_ptr = NULL;
	utmp = NetUserGetInfo(NULL, g_usr_win, 1, (LPBYTE*)&ui1_ptr);
	if (NERR_Success != utmp) {
		// 经验证，获取SYSTEM用户信息失败。
		LogErr(_T("IsAdminCurUsr, NetUserGetInfo(%s) fail."), g_usr_win);
		ret__ = TRUE;
		goto __cleanup;
	}
	if (USER_PRIV_ADMIN == ui1_ptr->usri1_priv) {
		ret__ = TRUE;
	}
	LogMsg(_T("IsAdminCurUsr, %s, usri1_priv:%u."), g_usr_win, ui1_ptr->usri1_priv);
__cleanup:
	if (NULL != ui1_ptr) {
		NetApiBufferFree(ui1_ptr);
		ui1_ptr = NULL;
	}
	return ret__;
}
HWND find_clt_info()
{ 
	HWND hwnd_brs = NULL;
	CString strtmp;
	strtmp.Format(_T("KdDeskTC.8506ECDC-C7A9-4005-B340-9D29C95B1610"));
	HANDLE mapping_cmw = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, strtmp);
	if (mapping_cmw != NULL) {
		CLDSKCLTWND* lp_cmw = (CLDSKCLTWND*)MapViewOfFile(mapping_cmw, FILE_MAP_WRITE, 0, 0, sizeof(CLDSKCLTWND));
		if (lp_cmw == NULL) {
			LogErr(_T("find_clt_info, MapViewOfFile fail."));
		} else {
			hwnd_brs = (HWND)lp_cmw->hwnd;
		}
		UnmapViewOfFile(lp_cmw);
		CloseHandle(mapping_cmw);
	} else {
		//LogMsg(_T("find_clt_info, OpenFileMapping fail."));
	}
	return hwnd_brs;
}
void sav_clt_info(HWND hwnd)
{
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	CString strtmp;
	strtmp.Format(_T("KdDeskTC.8506ECDC-C7A9-4005-B340-9D29C95B1610"));
	g_mapping_cmw = CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, sizeof(CLDSKCLTWND), strtmp);
	if (g_mapping_cmw == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
	} else {
		g_lp_cmw = (CLDSKCLTWND*)MapViewOfFile(g_mapping_cmw, FILE_MAP_WRITE, 0, 0, sizeof(CLDSKCLTWND));
		if (!g_lp_cmw) {
		} else {
			g_lp_cmw->hwnd = (UINT32)hwnd;
		}
	}
}
void cls_clt_info() {
	if (g_lp_cmw) {
	UnmapViewOfFile(g_lp_cmw);
	g_lp_cmw = NULL;
	}
	if (g_mapping_cmw) {
	CloseHandle(g_mapping_cmw);
	g_mapping_cmw = NULL;
	}
}
int create_guid()
{
	::CoInitialize(NULL);
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid)) {
		_snprintf(g_devid, sizeof(g_devid), "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}
	return 0;
}
void CreateDesktopShortcut(int flag = 0)
{
	CString strtmp;
	strtmp.Format(_T("%s\\KdDeskTCtrl.exe"), buff_workdir);
	if (1 == flag) {
		return;
	}
	if (::PathFileExists(g_dskshortcut)) {
		return;
	}
	//tstring strShortcutPath = szDesktopPath;
	tstring strIconPath = buff_workdir;
	strIconPath += _T("\\logo.ico");
	
	CString tipText;
	tipText.Format(_T("%sV%s"), _T("金蝶远程协助"), CURVER);
	theApp.CreateShortcut(strtmp, g_dskshortcut, tipText.GetBuffer(), _T(""), strIconPath.c_str(), 0);
}
void CreateUninstShortcut(int flag = 0)
{
	CString strtmp;
	strtmp.Format(_T("%s\\KdDeskTCtrl.exe"), buff_workdir);
	if (1 == flag) {
		return;
	}
	if (::PathFileExists(g_uninstshortcut)) {
		return;
	}
	//tstring strShortcutPath = szDesktopPath;
	tstring strIconPath = buff_workdir;
	strIconPath += _T("\\logo.ico");
	
	CString tipText;
	tipText.Format(_T("%sV%s"), _T("金蝶远程协助"), CURVER);
	theApp.CreateShortcut(strtmp, g_uninstshortcut, tipText.GetBuffer(), _T("1"), strIconPath.c_str(), 0);
}
void CreateStartMenuShortcut()
{
	CString strtmp;
	strtmp.Format(_T("%s\\KdDeskTCtrl.exe"), buff_workdir);
	if (::PathFileExists(g_sm_sc_main)) {
		return;
	}
	tstring strIconPath = buff_workdir;
	strIconPath += _T("\\logo.ico");
	
	CString tipText;
	tipText.Format(_T("%sV%s"), _T("金蝶远程协助"), CURVER);
	theApp.CreateShortcut(strtmp, g_sm_sc_main, tipText.GetBuffer(), _T(""), strIconPath.c_str(), 0);

	theApp.CreateShortcut(strtmp, g_sm_sc_uninst, tipText.GetBuffer(), _T("1"), strIconPath.c_str(), 0);
}
tstring GetCurPrcFle()
{
	TCHAR buff_path_tmp[MAX_PATH];
	GetModuleFileName(NULL, buff_path_tmp, MAX_PATH - 1);
	tstring str_path_tmp = buff_path_tmp;
	int iSlashPos = (int)(str_path_tmp.rfind('\\'));
	if (iSlashPos != string::npos) {
		str_path_tmp = str_path_tmp.substr(iSlashPos+1);
	}
	//LogMsg(_T("GetCurPrcFle|current process file name:%s"), str_path_tmp.c_str());
	return str_path_tmp;
}
int ReadConfig()
{
	CString StrTmp;
	GetCurrDir(buff_workdir);
	::SetCurrentDirectory(buff_workdir);
	wsprintf(buff_inipath, _T("%s\\Config.ini"), buff_workdir);
	wsprintf(g_history_path, _T("%s\\history.ini"), buff_workdir);
	tstring strtmp = GetCurPrcFle();
	int pos__ = (int)(strtmp.rfind('.'));
	if (-1 != pos__) {
		wsprintf(g_full_log_name, _T("%s\\%s.log"), buff_workdir, strtmp.substr(0, pos__).c_str());
		wsprintf(g_full_err_name, _T("%s\\%s-Error.log"), buff_workdir, strtmp.substr(0, pos__).c_str());
		wsprintf(g_full_fat_name, _T("%s\\%s-Fatal.log"), buff_workdir, strtmp.substr(0, pos__).c_str());
	}

	// whether to show the debug information or not.
	g_isdebug = GetPrivateProfileInt(g_ini_section, _T("debug"), 0, buff_inipath);
	g_clseye = GetPrivateProfileInt(g_ini_section, _T("clseye"), 1, buff_inipath);

	DWORD nSize = 64;
	::GetUserName(g_usr_win, &nSize); // max 20 byte.
	return 0;
}
BOOL set_foreground_wnd(HWND hwnd)
{
	BOOL ret__ = FALSE;
	HWND fgwnd = ::GetForegroundWindow();
	if (fgwnd != hwnd) {
		DWORD tid_c = ::GetCurrentThreadId();
		DWORD tid_f = ::GetWindowThreadProcessId(fgwnd, NULL);
		::AttachThreadInput(tid_c, tid_f, TRUE);
		for (int i = 0; i < 3; i++) {
			if (TRUE == SetForegroundWindow(hwnd)) {
				ret__ = TRUE;
				break;
			} else {
				//LogMsg(_T("set_foreground_wnd, SetForegroundWindow(%u) fail, i:%d."), (UINT)hwnd, i);
			}
			Sleep(1);
		}
		::AttachThreadInput(tid_c, tid_f, FALSE);
	} else {
		ret__ = TRUE;
	}
	return ret__;
}
void addfirewall(TCHAR* rulename, TCHAR* exename)
{
	LogMsg(_T("threadproc_firewall, bgn."));
	TCHAR cmdline[1024] = { 0 };
	//TCHAR workdir[MAX_PATH] = { 0 };
	wsprintf(cmdline, _T("netsh.exe advfirewall firewall delete rule name=\"%s.i\""), 
		rulename
		);
	exec_wait_ex(cmdline);
	wsprintf(cmdline, _T("netsh.exe advfirewall firewall delete rule name=\"%s.o\""), 
		rulename
		);
	exec_wait_ex(cmdline);

	wsprintf(cmdline, _T("netsh advfirewall firewall add rule name=\"%s.i\" dir=in program=\"%s\\%s\" action=allow"), 
		rulename, 
		buff_workdir, 
		exename
		);
	exec_wait_ex(cmdline);
	wsprintf(cmdline, _T("netsh advfirewall firewall add rule name=\"%s.o\" dir=out program=\"%s\\%s\" action=allow"), 
		rulename, 
		buff_workdir, 
		exename
		);
	exec_wait_ex(cmdline);
	LogMsg(_T("threadproc_firewall, end."));
}
DWORD WINAPI threadproc_addfirewall(LPVOID lpParameter)
{
	addfirewall(_T("kingdee.kddesktctrl"), _T("KdDeskTCtrl.exe"));
	addfirewall(_T("kingdee.kddesktc"), _T("KdDeskTC.exe"));
	return 0;
}
void HiDpiAwrChkUpd(char* path, char* v) {
	DWORD dwType, cbData;
	char bufv[MAX_PATH] = { 0 };
	cbData = sizeof(bufv); // bytes of bufv.
	SHGetValueA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", path, &dwType, bufv, &cbData);
	if (0 != strcmp(bufv, v)) {
		dwType = REG_SZ;
		strcpy(bufv, v);
		SHSetValueA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", path, dwType, bufv, (strlen(bufv)+1)*sizeof(char));
	}
}
// CKdDeskTCApp

BEGIN_MESSAGE_MAP(CKdDeskTCApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKdDeskTCApp 构造

CKdDeskTCApp::CKdDeskTCApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_runasadmin = 0;
	m_command = 0;
}

// 唯一的一个 CKdDeskTCApp 对象

CKdDeskTCApp theApp;

// CKdDeskTCApp 初始化

BOOL CKdDeskTCApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	CWndShadow::Initialize(AfxGetInstanceHandle());
	CWndShadow::m_mode = 1;
	CNewMenu::m_fnt_mn = 1;
	CButtonEx2::m_mode = 1;
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	//SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	TCHAR buftmp[MAX_PATH];
	CString strtmp;
	CStringA strtmpa;
	WSADATA wsaData;
	// ...initiates use of WS2_32.DLL by a process.
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	ReadConfig();
	m_pszProfileName = _tcsdup(buff_inipath);

	g_is_win_x64 = IsWow64();

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, szDesktopPath))) {
	}
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_PROGRAMS, NULL, 0, buftmp))) {
	}
	wsprintf(g_dskshortcut, _T("%s\\金蝶远程协助.lnk"), szDesktopPath);
	wsprintf(g_uninstshortcut, _T("%s\\Uninstall.lnk"), buff_workdir);
	wsprintf(g_sm_sc_base, _T("%s\\金蝶远程协助"), buftmp);
	if (!PathFileExists(g_sm_sc_base)) {
		if (FALSE == CreateDirectory(g_sm_sc_base, NULL)) {
		}
	}
	wsprintf(g_sm_sc_main, _T("%s\\金蝶远程协助\\金蝶远程协助.lnk"), buftmp);
	wsprintf(g_sm_sc_uninst, _T("%s\\金蝶远程协助\\Uninstall.lnk"), buftmp);
	CreateDesktopShortcut();
	CreateUninstShortcut();
	CreateStartMenuShortcut();
	HINSTANCE hinstlicense = LoadLibrary(_T("KtLicenseDll.dll"));
	if (NULL != hinstlicense) {
		lpfn_CltUsrLgn = (CLTUSRLGN)GetProcAddress(hinstlicense, "CltUsrLgn");
		lpfn_GetDvcLst = (GETDVCLST)GetProcAddress(hinstlicense, "GetDvcLst");
		lpfn_GetAdvertLst = (GETADVERTLST)GetProcAddress(hinstlicense, "GetAdvertLst");
		lpfn_DelCldskId = (DELCLDSKID)GetProcAddress(hinstlicense, "DelCldskId");
		lpfn_SubCldskId = (SUBCLDSKID)GetProcAddress(hinstlicense, "SubCldskId");
		lpfn_SubLog = (SUBLOG)GetProcAddress(hinstlicense, "SubLog");
		LogMsg(_T("KtLicenseDll.dll, CltUsrLgn:%p, GetDvcLst:%p, GetAdvertLst:%p, DelCldskId:%p, SubCldskId:%p, SubLog:%p."), 
			lpfn_CltUsrLgn, lpfn_GetDvcLst, lpfn_GetAdvertLst, lpfn_DelCldskId, lpfn_SubCldskId, lpfn_SubLog
			);
	} else {
		LogErr(_T("LoadLibrary(KtLicenseDll.dll) fail."));
	}

	strtmpa = this->GetRegistryString("devid", "");
	if (strtmpa.IsEmpty()) {
		create_guid();
		theApp.WriteRegistryString("devid", g_devid);
	} else {
		strcpy(g_devid, strtmpa);
	}
	GetModuleFileNameA(NULL, g_buff_path, MAX_PATH - 1);
	CDlgMsgBox2 dlgmsgbox(1, _T("提示"), _T(""));
	CKdDeskTCDlg dlg;
	CDlgLogin dlglogin;
	int numArgs = 0;
	LPWSTR* parms = CommandLineToArgvW(::GetCommandLineW(), &numArgs);
	m_runasadmin = IsPrcRunAsAdmin();
	//if (0 == m_silent_mode) {
		if (m_runasadmin) {
		} else {
			//HANDLE hthread = ::CreateThread(NULL, 0, threadproc_consent, NULL, 0, NULL);
			//::CloseHandle(hthread);
			int ret__ = (int)ShellExecute(NULL, _T("runas"), a2t(g_buff_path).c_str(), (2 <= numArgs ? parms[1] : NULL), NULL, SW_SHOWNORMAL);
			// 经测试发现，ShellExecute为阻塞模式。
			if (32 < ret__) { // If the function succeeds, it returns a value greater than 32.
			} else {
			}
			//LogErr(_T("CCloudesktopApp->InitInstance, ShellExecute(%s), ret__:%d."), a2t(g_buff_path).c_str(), ret__);
			goto __cleanup;
		}
	if (NULL != parms) {
		if (2 <= numArgs) {
			int cmd = _wtoi(parms[1]);
			if (1 == cmd) {
				CDlgMsgBox2 dlgmsgbox(2, _TXTLNG(_T("提示"), _T("Prompt")), _T("确定要卸载KdDeskTC？"));
				INT_PTR nResponse = dlgmsgbox.DoModal();
				if (nResponse == IDOK) {
					uninstallex();
				}
				return FALSE;
			} else if (2 == cmd) {
				m_command = 2;
				//HiDpiAwrChkUpd(g_buff_path, "~ HIGHDPIAWARE");
				::SHDeleteValueA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", g_buff_path);
				//strtmpa.Format("%s\\KdDeskTC.exe", t2a(buff_workdir).c_str());
				//HiDpiAwrChkUpd((char*)strtmpa.GetString(), "~ HIGHDPIAWARE");
				goto __cleanup;
			}
		}
	}

		HWND cltwnd = find_clt_info();
		if (NULL != cltwnd && IsWindow(cltwnd)) {
			if (!IsWindowVisible(cltwnd)) {
				//PostMessage(cltwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
				PostMessage(cltwnd, WM_CLT_COMMAND, 30, 7);
				goto __cleanup;
			} else {
				//active_hwnd(cltwnd);
				PostMessage(cltwnd, WM_CLT_COMMAND, 30, 8);
				goto __cleanup;
			}
		}
		strtmp = a2t(GetRegistryStringEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\KdDeskTC", "path")).c_str();
		if (0 != strtmp.CompareNoCase(buff_workdir)) {
			HANDLE hthread = ::CreateThread(NULL, 0, threadproc_addfirewall, NULL, 0, NULL);
			CloseHandle(hthread);
			SetRegistryStringEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\KdDeskTC", "path", t2a(buff_workdir).c_str());
		}
	//}
	dlgmsgbox.m_tohide = 1;
	INT_PTR nResponse = dlgmsgbox.DoModal();
	if (nResponse == IDOK) {
	} else if (nResponse == IDCANCEL) {
		goto __cleanup;
	}
	threadproc_startsvc(NULL);
	nResponse = dlglogin.DoModal();
	if (nResponse == IDOK) {
	} else if (nResponse == IDCANCEL) {
		goto __cleanup;
	}
	
	m_pMainWnd = &dlg;
	nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}
	cls_clt_info();
__cleanup:
	g_to_exit = 1;
	// The WSACleanup function terminates use of the WS2_32.DLL.
	WSACleanup();
	if (2 == m_command) {
	::WinExec(g_buff_path, SW_SHOWNORMAL);
	}
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}
CString CKdDeskTCApp::__encrypt(CString S)
{
	string stra = t2a(S.GetBuffer());
	stra = ccryptoex::__encrypt(stra);
	return a2t(stra).c_str();
}
CString CKdDeskTCApp::__decrypt(CString S)
{
	string stra = t2a(S.GetBuffer());
	stra = ccryptoex::__decrypt(stra);
	return a2t(stra).c_str();
}
BOOL CKdDeskTCApp::WriteRegistryString(LPCSTR lpszEntry, CStringA strValue)
{
	BOOL bRes = FALSE;
	HKEY hkey_ctbs = NULL;
	if (::RegCreateKeyA(HKEY_LOCAL_MACHINE, "Software\\kiscloudex", &hkey_ctbs) == ERROR_SUCCESS) {
	}
	if (NULL != hkey_ctbs) {
		DWORD type = REG_SZ;
		if (::RegSetValueExA(hkey_ctbs, lpszEntry, NULL, type, (const BYTE*)strValue.GetBuffer(), (strValue.GetLength() + 1) * sizeof(CHAR)) == ERROR_SUCCESS) {
			bRes = TRUE;
		}
		::RegCloseKey(hkey_ctbs);
	}
	return bRes;
}

CStringA CKdDeskTCApp::GetRegistryString(LPCSTR lpszEntry, LPCSTR lpszDefault)
{
	CStringA strResult = lpszDefault;
	HKEY hkey_ctbs = NULL;
	if (::RegOpenKeyA(HKEY_LOCAL_MACHINE, "Software\\kiscloudex", &hkey_ctbs) == ERROR_SUCCESS) {
	}
	if (NULL != hkey_ctbs) {
		DWORD type = REG_SZ;
		DWORD cbData = 0;
		if (::RegQueryValueExA(hkey_ctbs, lpszEntry, NULL, &type, NULL, &cbData) != ERROR_SUCCESS) {
			return strResult;
		}

		CHAR* buff_value = new CHAR[cbData];
		ZeroMemory(buff_value, cbData);
		if (::RegQueryValueExA(hkey_ctbs, lpszEntry, NULL, &type, (LPBYTE)buff_value, &cbData) == ERROR_SUCCESS) {
			strResult = buff_value;
		}
		
		delete[] buff_value;
		::RegCloseKey(hkey_ctbs);
	}
	return strResult;
}
void CKdDeskTCApp::center_dlgitm(CWnd* pWnd, int idl, int idr)
{
	RECT rctp;
	pWnd->GetWindowRect(&rctp);

	RECT rctl, rctr;
	pWnd->GetDlgItem(idl)->GetClientRect(&rctl);
	pWnd->GetDlgItem(idl)->ClientToScreen(&rctl);
	pWnd->ScreenToClient(&rctl);
	
	int w = rctl.right - rctl.left;
	if (0 < idr) {
	rctl.left = ((rctp.right - rctp.left) - (w * 2 + 2)) / 2;
	} else {
	rctl.left = ((rctp.right - rctp.left) - w) / 2;
	}
	rctl.right = rctl.left + w;
	pWnd->GetDlgItem(idl)->MoveWindow(&rctl);

	if (0 < idr) {
	pWnd->GetDlgItem(idr)->GetClientRect(&rctr);
	pWnd->GetDlgItem(idr)->ClientToScreen(&rctr);
	pWnd->ScreenToClient(&rctr);
	rctr.left = rctl.right + 2;
	rctr.right = rctr.left + w;
	pWnd->GetDlgItem(idr)->MoveWindow(&rctr);
	}
}
HRESULT CKdDeskTCApp::CreateShortcut(LPCTSTR lpszFileName, LPCTSTR lpszShortcutPath, LPCTSTR lpszDesc, LPCTSTR lpszArguments, LPCTSTR lpszIconLocation, int nIconIndex)
{
    HRESULT hRes = E_FAIL;
    DWORD dwRet = 0;
    CComPtr<IShellLink> ipShellLink;
    // buffer that receives the null-terminated string 
    // for the drive and path
    TCHAR szPath[MAX_PATH];    
    // buffer that receives the address of the final 
    //file name component in the path
    LPTSTR lpszFilePart;    
    WCHAR wszTemp[MAX_PATH];
        
    // Retrieve the full path and file name of a specified file
    dwRet = GetFullPathName(lpszFileName, sizeof(szPath) / sizeof(TCHAR), szPath, &lpszFilePart);
    if (!dwRet)
	{
		return hRes;
	}

	// Applications must initialize the COM library before they can call COM library functions.
	::CoInitialize(NULL);
    // Get a pointer to the IShellLink interface
    hRes = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&ipShellLink);
	// Closes the COM library on the current thread, unloads all DLLs loaded by the thread, 
	// frees any other resources that the thread maintains, and forces all RPC connections on the thread to close.
	CoUninitialize();
    if (SUCCEEDED(hRes)) {
        // Get a pointer to the IPersistFile interface
        CComQIPtr<IPersistFile> ipPersistFile(ipShellLink);

        // Set the path to the shortcut target and add the description
        hRes = ipShellLink->SetPath(szPath);
        if (FAILED(hRes))
            return hRes;

        hRes = ipShellLink->SetDescription(lpszDesc);
        if (FAILED(hRes))
            return hRes;

		hRes = ipShellLink->SetArguments(lpszArguments);
		hRes = ipShellLink->SetIconLocation(lpszIconLocation, nIconIndex);

        // IPersistFile is using LPCOLESTR, so make sure that the string is Unicode.
		#if !defined _UNICODE
        MultiByteToWideChar(CP_ACP, 0, lpszShortcutPath, -1, wszTemp, MAX_PATH);
		#else
        wcsncpy(wszTemp, lpszShortcutPath, MAX_PATH);
		#endif

        // Write the shortcut to disk
        hRes = ipPersistFile->Save(wszTemp, TRUE);
    }

    return hRes;
}