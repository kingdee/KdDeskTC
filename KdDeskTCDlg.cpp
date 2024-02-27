// KdDeskTCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KdDeskTC.h"
#include ".\kddesktcdlg.h"
#include "DlgLogin.h"
#include "DlgMsgBox2.h"
#include "DlgTmpex.h"
#include "TrayIcon.h"
#include <Wininet.h>
#pragma comment(lib, "Wininet.lib")
#define SECURITY_IGNORE_ERROR_MASK  (INTERNET_FLAG_IGNORE_CERT_CN_INVALID   |  \
                                     INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |  \
                                     SECURITY_FLAG_IGNORE_UNKNOWN_CA        |  \
                                     SECURITY_FLAG_IGNORE_REVOCATION    )
CKdDeskTCDlg* g_pdlg_dsk = NULL;
int g_showmainwnd = 1;
int g_frclseye = 0;
BOOL set_foreground_wnd(HWND hwnd);
extern BOOL g_is_win_x64;
extern TCHAR buff_inipath[MAX_PATH];
extern TCHAR buff_workdir[MAX_PATH];
extern TCHAR g_history_path[MAX_PATH];
extern int g_language;
extern int g_clseye;
extern int g_to_exit;
extern char BufOutUsrId[128];
extern char BufOutErr[256];
extern TCHAR g_sm_sc_base[MAX_PATH];
PCTSTR szAcceptTypes[] = { _T("*/*"), NULL };
// tab rmtctr.
#define IDC_LAB_CURDVC 5000
#define IDC_LAB_LID    5001
#define IDC_LAB_LPWD   5002
#define IDC_EDT_LID    5003
#define IDC_EDT_LPWD   5004
#define IDC_BTN_COPY   5005
#define IDC_BTN_VLINE  5006
#define IDC_BTN_EYE    5007
#define IDC_BTN_UPDPWD 5008
#define IDC_LAB_RID    5009
#define IDC_CMB_RID    5010
#define IDC_BTN_CONR   5011
#define IDC_BTN_READY  5013
#define IDC_LAB_READY  5014
// tab advset.
#define IDC_CHK_AUTSTA 5015
#define IDC_LAB_DEFDIR 5016
#define IDC_RAD_PRV	   5017
#define IDC_RAD_CST	   5018
#define IDC_EDT_DEFDIR 5019
#define IDC_BTN_CHGDIR 5020
#define IDC_BTN_OPNDIR 5021
#define IDC_LAB_PWDUPD 5022
#define IDC_CMB_PWDUPD 5023
#define IDC_LAB_AUTSTA 5024
#define IDC_LAB_FLETRN 5025
#define IDC_LAB_CLSWND 5026
#define IDC_CHK_TOTRAY 5027
#define IDC_LAB_ABOUT  5028
#define IDC_LAB_CURVER 5029
#define IDC_BTN_UPDN   5030
#define IDC_BTN_UNINST 5040
#define IDC_LAB_LOGSET 5041
#define IDC_CHK_AUTLOG 5042
#define IDC_LAB_IDLE 5043
#define IDC_CMB_IDLE 5044
// tab dvclst.
#define IDC_LST_DVC     5031
#define IDC_BTN_VLINE3  5032
#define IDC_LAB_DVCNAME 5033
#define IDC_LAB_DVCCODE 5034
#define IDC_BTN_RMTCON	5035
#define IDC_BTN_RMTFLE	5036
#define IDC_LST_DVC2    5037
#define IDC_LAB_DVC     5038
#define IDC_LAB_DVC2    5039
vector<DVCITEM> g_vec_dvc;
vector<DVCITEM> g_vec_dvc_tmp;
vector<ADVITEM> g_vec_adv;
void sav_clt_info(HWND hwnd);
void truncature(CStringW* pstrw, string* pstra, int lmtxta);
int exec_wait_ex(TCHAR* cmdline, TCHAR* curdir = NULL, DWORD timeout = 60*1000, BOOL terminate = 0);
int exec_nowait(TCHAR* cmdline, TCHAR* curdir = NULL, BOOL tohide = TRUE);
extern GETADVERTLST lpfn_GetAdvertLst;
extern DELCLDSKID lpfn_DelCldskId;
extern SUBCLDSKID lpfn_SubCldskId;
TCHAR pathinstexe[MAX_PATH] = { 0 };
TCHAR pathinstini[MAX_PATH] = { 0 };
CString g_str_tip;
CString g_newver;
extern int g_in_getdvclst;
extern TCHAR g_dskshortcut[MAX_PATH];
extern char cldskid[128];
DWORD WINAPI threadproc_getdvclst(LPVOID lpParameter);
CString _TXTLNG(CString strch, CString stren) {
	if (1 == g_language) {
		return strch;
	} else {
		return stren;
	}
}
void GetBJTime(char* timeptr) 
{
	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation(&tzi);

	__time64_t curtime = ::_time64(NULL);
    //_time64(&curtime);
	curtime += (tzi.Bias*60);
	curtime += 8 * 60 * 60;
    tm* tmptr = _localtime64(&curtime);
	tmptr->tm_year += 1900;
	tmptr->tm_mon += 1;
	if (NULL != timeptr) 
	{
		wsprintfA(timeptr, "%04d-%02d-%02d %02d:%02d:%02d", tmptr->tm_year, tmptr->tm_mon, tmptr->tm_mday, tmptr->tm_hour, tmptr->tm_min, tmptr->tm_sec);
	}
}
CFont* CreateFontEx(int nHeight, int nWeight) {
	CFont* fntptr = new CFont();
	//titfnt.CreatePointFont(180, "宋体");
	fntptr->CreateFont(
		nHeight,					// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		nWeight,//FW_NORMAL,		// nWeight
		FALSE,						// bItalic
		FALSE,						// bUnderline
		0,							// cStrikeOut
		ANSI_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,			// nOutPrecision
		CLIP_DEFAULT_PRECIS,		// nClipPrecision
		DEFAULT_QUALITY,			// nQuality
		DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
		_T("微软雅黑")//_T("Microsoft Sans Serif")
		);
	return fntptr;
}
HFONT createfontex(int nheight, int nweight) {
	HFONT hfont	= CreateFont(
		nheight,					// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		nweight,					// nWeight
		FALSE,						// bItalic
		FALSE,						// bUnderline
		0,							// cStrikeOut
		ANSI_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,			// nOutPrecision
		CLIP_DEFAULT_PRECIS,		// nClipPrecision
		DEFAULT_QUALITY,			// nQuality
		DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
		_T("微软雅黑")//_T("Microsoft Sans Serif")
		);
	return hfont;
}
int active_hwnd(HWND hwnd)
{
	int ret__ = 0;
	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	if ((style & WS_MINIMIZE) != WS_MINIMIZE && hwnd != GetForegroundWindow()) {
		set_foreground_wnd(hwnd);
		ret__ = 1;
	} else if ((style & WS_MINIMIZE) == WS_MINIMIZE) {
		SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		set_foreground_wnd(hwnd);
		ret__ = 2;
	}
	BringWindowToTop(hwnd);
	return ret__;
}
void active_hwnd_ex(HWND hwnd)
{
	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	if ((style & WS_MINIMIZE) != WS_MINIMIZE && hwnd != GetForegroundWindow()) {
		//set_foreground_wnd(hwnd);
	} else if ((style & WS_MINIMIZE) == WS_MINIMIZE) {
		SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	for (int i = 0; i < 60; i++) {
		if (set_foreground_wnd(hwnd)) {
			break;
		}
		::Sleep(100);
	}
}
DWORD WINAPI threadproc_actwnd(LPVOID lpParameter)
{
	::Sleep(100);
	active_hwnd((HWND)lpParameter);
	return 0;
}
DWORD WINAPI threadproc_actwnd_ex(LPVOID lpParameter)
{
	active_hwnd_ex((HWND)lpParameter);
	return 0;
}
ULONG CryptTable[0x500];
VOID PrepareCryptTable() {
    ULONG seed = 0x00100001, index1 = 0, index2 = 0, i;

	for (index1 = 0; index1 < 0x100; index1++) {
		for (index2 = index1, i = 0; i < 5; i++, index2 += 0x100) {
            ULONG temp1, temp2;

            seed = (seed * 125 + 3) % 0x2AAAAB;
            temp1 = (seed & 0xFFFF) << 0x10;

            seed = (seed * 125 + 3) % 0x2AAAAB;
            temp2 = (seed & 0xFFFF);

            CryptTable[index2] = (temp1 | temp2);
		}
	}
}

ULONG HashBuffer(PCHAR Buffer, USHORT Length, ULONG iType) {
	ULONG seed1 = 0x7FED7FED;
	ULONG seed2 = 0xEEEEEEEE;
	USHORT i = 0;

    /*for (; i < Length; i++, Buffer++) {
        seed1 = CryptTable[(iType << 8) + *Buffer] ^ (seed1 + seed2);
        seed2 = *Buffer + seed1 + seed2 + (seed2 << 5) + 3;
    }*/

	//
	// 注意：中文字符单字节是负数，所以要转换为正整数。
	//

	for (; i < Length; i++, Buffer++) {
        seed1 = CryptTable[(iType << 8) + (UCHAR)*Buffer] ^ (seed1 + seed2);
        seed2 = (UCHAR)*Buffer + seed1 + seed2 + (seed2 << 5) + 3;
    }

    return seed1;
}
LONG HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2, HASH_C = 3, HASH_D = 4;
int onlbtndblclk_edt(HWND hwnd)
{
	int ret__ = 0;
	TCHAR buftmp[128] = { 0 };
	::GetWindowText(hwnd, buftmp, 128);
	if (NULL != _tcsstr(buftmp, _T("---"))) {
		return ret__;
	}
	::SendMessage(hwnd, EM_SETSEL, 0, -1);
	ret__ = ::SendMessage(hwnd, WM_COPY, 0, 0);
	if (0 < ret__) {
		//g_pdlg_cldsk->set_labl_txt(hwnd, 1);
	}
	return ret__;
}
int onlbtndblclk_edt_r(HWND hwnd)
{
	int ret__ = 0;
	TCHAR buftmp[128] = { 0 };
	::GetWindowText(hwnd, buftmp, 128);
	::SendMessage(hwnd, EM_SETSEL, 0, -1);
	return ret__;
}
void DelItm_Rid(int index)
{
	if (NULL != g_pdlg_dsk) {
		g_pdlg_dsk->delitm_rid(index);
	}
}
BOOL to_stop_service(SC_HANDLE hservice)
{
	SERVICE_STATUS ss;
	DWORD errcode, cbBytesNeeded;
	SERVICE_STATUS_PROCESS ssp;
	BOOL ret__ = QueryServiceStatusEx(hservice, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &cbBytesNeeded);
	//LogMsg(_T("to_stop_service, ret__:%d, ssp.dwCurrentState:%u."), ret__, ssp.dwCurrentState);
	if (ret__ && SERVICE_STOPPED == ssp.dwCurrentState) {
		// 经测试发现，SERVICE_STOPPED并不代表进程已退出。如果进程没退出，但ssp.dwProcessId的值却是0。
		ret__ = TRUE;
		return ret__;
	} else {
		ret__ = FALSE;
	}
	// [27:06_18 18_26_18_769][thread: 5560]to_stop_service, ControlService(SERVICE_CONTROL_STOP), bgn.
	// [28:06_18 18_26_24_769][thread: 5560]to_stop_service, ControlService(SERVICE_CONTROL_STOP), end, ret__:1.
	// 经测试发现，调用ControlService是阻塞返回的。
	//for (int i = 0; i < 2; i++) {
	//LogMsg(_T("to_stop_service, ControlService(SERVICE_CONTROL_STOP), bgn."));
	ret__ = ::ControlService(hservice, SERVICE_CONTROL_STOP, &ss);
	//LogMsg(_T("to_stop_service, ControlService(SERVICE_CONTROL_STOP), end, ret__:%d."), ret__);
	if (FALSE == ret__) {
		errcode = GetLastError();
		LogErr(_T("to_stop_service, ControlService(SERVICE_CONTROL_STOP) fail."));
	} else {
	//	break;
	}
	//::Sleep(300);
	//}
	return ret__;
}
BOOL to_start_service(SC_HANDLE hservice)
{
	DWORD errcode, cbBytesNeeded;
	SERVICE_STATUS_PROCESS ssp;
	BOOL ret__ = QueryServiceStatusEx(hservice, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &cbBytesNeeded);
	//LogMsg(_T("to_start_service, ret__:%d, ssp.dwCurrentState:%u."), ret__, ssp.dwCurrentState);
	if (ret__ && SERVICE_RUNNING == ssp.dwCurrentState) {
		ret__ = TRUE;
		return ret__;
	} else {
		ret__ = FALSE;
	}
	if (!ChangeServiceConfig(hservice,SERVICE_NO_CHANGE,SERVICE_AUTO_START,SERVICE_NO_CHANGE,NULL,NULL,NULL,NULL,NULL,NULL,NULL)) {
	}
	//for (int i = 0; i < 2; i++) {
	// 经测试发现，调用StartService是瞬间返回的。
	// 经测试发现，如果停止服务超时，且进程还未退出，这时StartService会启动一个新进程(挂起状态)并等待老进程退出，如果30秒还未退出，则强制杀掉老进程。
	ret__ = ::StartService(hservice, 0, NULL);
	if (FALSE == ret__) {
		errcode = GetLastError();
		LogErr(_T("to_start_service, StartService fail."));
	} else {
	//	break;
	}
	//::Sleep(300);
	//}
	return ret__;
}
int ControlServiceEx(TCHAR* servicename, int command)
{
	int ret__ = 0;
	BOOL bSuccess;
	SC_HANDLE hmanager = NULL, hservice = NULL;
	//try {
	if (!(hmanager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS))) {
		//LogErr("ControlServiceEx|OpenSCManager fail.");
		ret__ = 1;
		goto __cleanup;
	}
	if (!(hservice = OpenService(hmanager, servicename, SC_MANAGER_ALL_ACCESS))) {
		//LogErr("ControlServiceEx|OpenService(%s) fail.", servicename);
		ret__ = 2;
		goto __cleanup;
	}
	SERVICE_STATUS_PROCESS ssp;
	DWORD cbBytesNeeded = 0;
	bSuccess = QueryServiceStatusEx(hservice, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &cbBytesNeeded);
	if (bSuccess) {
		//SERVICE_STATUS status__;
		BOOL ret__cs = FALSE, ret__ss = FALSE;
		if (10 == command) {
			ret__cs = to_stop_service(hservice);
			ret__ss = to_start_service(hservice);
			if (!ret__ss) {
				ret__ = 4;
			}
			//LogErr("ControlServiceEx|to_stop_service:%d, to_start_service:%d, %s.", ret__cs, ret__ss, servicename);
		} else if (11 == command) {
			//if (0 != QueryServiceStatus__(hmanager, hservice)) {
			ret__cs = to_stop_service(hservice);
			//}
			if (!ret__cs) {
				ret__ = 4;
			}
			//LogErr("ControlServiceEx|to_stop_service:%d, %s.", ret__cs, servicename);
		} else if (12 == command) {
			ret__ss = to_start_service(hservice);
			if (!ret__ss) {
				ret__ = 4;
			}
			//LogErr("ControlServiceEx|to_start_service:%d, %s.", ret__ss, servicename);
		}
	}
	CloseServiceHandle(hservice);
	hservice = NULL;
	/*} catch (...) {
		//LogFat("ControlServiceEx|unexpected fatal error occur.");
		ret__ = 3;
		goto __cleanup;
	}*/
__cleanup:
	if (NULL != hservice) {
	CloseServiceHandle(hservice);
	hservice = NULL;
	}
	if (NULL != hmanager) {
	CloseServiceHandle(hmanager);
	hmanager = NULL;
	}
	return ret__;
}
int get_cfg()
{
	CString strtmp;
	//TCHAR buftmp[MAX_PATH];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --get-cfg"), buff_workdir);
	exec_wait_ex(strtmp.GetBuffer(), buff_workdir);
	return 0;
}
TCHAR path_toml[MAX_PATH] = { 0 };
unsigned __int64 g_lwt_toml = 0;
int chk_toml() {
	int ret__ = 0;
	if (0 == path_toml[0]) {
		SHGetSpecialFolderPath(NULL, path_toml, CSIDL_WINDOWS, FALSE);
		_tcscat(path_toml, _T("\\ServiceProfiles\\LocalService\\AppData\\Roaming\\KdDeskTC\\config\\KdDeskTC.toml"));
	}
	if (PathFileExists(path_toml)) {
		WIN32_FILE_ATTRIBUTE_DATA fad;
		unsigned __int64 LastWriteTime__;
		memset(&fad, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
		if (!GetFileAttributesEx(path_toml, GetFileExInfoStandard, &fad)) {
			LogErr(_T("chk_toml, GetFileAttributesEx fail, %s."), path_toml);
			goto __cleanup;
		}
		ret__ = 1;
		memcpy(&LastWriteTime__, &fad.ftLastWriteTime, sizeof(__int64));
		if (g_lwt_toml != LastWriteTime__) {
			g_lwt_toml = LastWriteTime__;
			get_cfg();
			if (NULL != g_pdlg_dsk) {
			::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 10, 0);
			}
		}
	}
__cleanup:
	return ret__;
}
TCHAR path_peers[MAX_PATH] = { 0 };
unsigned __int64 g_lwt_peers = 0;
int chk_peers() {
	int ret__ = 0;
	if (0 == path_peers[0]) {
		SHGetSpecialFolderPath(NULL, path_peers, CSIDL_APPDATA, FALSE);
		_tcscat(path_peers, _T("\\KdDeskTC\\config\\peers"));
		//LogMsg(_T("chk_peers, %s."), path_peers);
	}
	if (PathFileExists(path_peers)) {
		WIN32_FILE_ATTRIBUTE_DATA fad;
		unsigned __int64 LastWriteTime__;
		memset(&fad, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
		if (!GetFileAttributesEx(path_peers, GetFileExInfoStandard, &fad)) {
			LogErr(_T("path_peers, GetFileAttributesEx fail, %s."), path_peers);
			goto __cleanup;
		}
		ret__ = 1;
		memcpy(&LastWriteTime__, &fad.ftLastWriteTime, sizeof(__int64));
		if (0 == g_lwt_peers) g_lwt_peers = LastWriteTime__;
		if (g_lwt_peers != LastWriteTime__) {
			g_lwt_peers = LastWriteTime__;
			g_pdlg_dsk->get_idlst();
			::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 12, 0);
		}
	}
__cleanup:
	return ret__;
}
int set_flagex(CString strv)
{
	CString strtmp;
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --set-flagex \"%s\""), buff_workdir, strv);
	exec_nowait(strtmp.GetBuffer(), buff_workdir);
	return 0;
}
int set_pwdupd(CString strv, int flag = 0)
{
	CString strtmp;
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --set-pwdupd \"%s\""), buff_workdir, strv);
	if (0 == flag) {
	exec_nowait(strtmp.GetBuffer(), buff_workdir);
	} else {
		exec_wait_ex(strtmp.GetBuffer(), buff_workdir);
	}
	return 0;
}
int set_idle(CString strv)
{
	CString strtmp;
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --set-idle \"%s\""), buff_workdir, strv);
	exec_nowait(strtmp.GetBuffer(), buff_workdir);
	return 0;
}
int reset()
{
	CString strtmp;
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --reset"), buff_workdir);
	exec_wait_ex(strtmp.GetBuffer(), buff_workdir);
	return 0;
}
DWORD WINAPI threadproc_startsvc(LPVOID lpParameter)
{
	TCHAR buftmp[1024];
	TCHAR buftmp2[1024];
	//LogMsg(_T("threadproc_startsvc, bgn."));
	SC_HANDLE hmanager = NULL;
	SC_HANDLE hservice = NULL;
	hmanager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hmanager == NULL) {
		LogFat(_T("threadproc_rstsvc, OpenSCManager fail."));
		goto __cleanup;
	}
    hservice = ::OpenService(hmanager, _T("KdDeskTC"), SC_MANAGER_ALL_ACCESS);
    if (hservice != NULL) {
		//::WinExec("sc.exe start SLSrvController", SW_HIDE);
		//to_stop_service(hservice);
		//::Sleep(2000);
		to_start_service(hservice);
		goto __cleanup;
    }
	//sc.exe create KdDeskTC start= auto DisplayName= "KdDeskTC Service" binPath= "\"$INSTDIR\${PRODUCT_NAME}.exe\" --service"
	wsprintf(buftmp, _T("sc.exe create KdDeskTC start= auto DisplayName= \"KdDeskTC Service\" binPath= \"%s\\KdDeskTC.exe --service\""), buff_workdir);
	if (0 != exec_wait_ex(buftmp, NULL, 9*1000)) {
		goto __cleanup;
	}
	::Sleep(1000);
	ControlServiceEx(_T("KdDeskTC"), 12);
__cleanup:
	if (hservice != NULL) {
	::CloseServiceHandle(hservice);
	hservice = NULL;
	}
	if (hmanager != NULL) {
	::CloseServiceHandle(hmanager);
	hmanager = NULL;
	}
	//LogMsg(_T("threadproc_startsvc, end."));
	return 0;
}
HANDLE event_refresh = NULL;
DWORD WINAPI threadproc_refresh(LPVOID lpParameter)
{
	threadproc_startsvc(NULL);
	set_flagex(_T("1"));
	/*if (0 == theApp.GetProfileInt(_T("KdDeskTC"), _T("autsta"), 0)) {
		if (0 == theApp.GetProfileInt(_T("KdDeskTC"), _T("InitPwdUpd"), 0)) {
			theApp.WriteProfileInt(_T("KdDeskTC"), _T("InitPwdUpd"), 1);
			set_pwdupd(_T("1"), 1);
			::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 90, 1);
		}
	}*/
	if (0 == theApp.GetProfileInt(_T("KdDeskTC"), _T("init"), 0)) {
		theApp.WriteProfileInt(_T("KdDeskTC"), _T("init"), 1);
		reset();
	}
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	DWORD ret_wfso;
	event_refresh = CreateEvent(&sa, FALSE, FALSE, NULL);
	if (NULL == event_refresh) {
		LogFat(_T("threadproc_refresh, CreateEvent fail."));
		goto __cleanup;
	}
	::ResetEvent(event_refresh);
	while (0 == g_to_exit) {
		chk_toml();
		chk_peers();
		ret_wfso = WaitForSingleObject(event_refresh, 5*1000);
		if (ret_wfso == WAIT_OBJECT_0) { // The state of the specified object is signaled.
		} else if (ret_wfso == WAIT_TIMEOUT) { // The time-out interval elapsed, and the object's state is nonsignaled.
			//LogFat(_T("threadproc_refresh, ret_wfso == WAIT_TIMEOUT."));
		} else if (ret_wfso == WAIT_FAILED) { // error occurs.
			LogFat(_T("threadproc_refresh, ret_wfso == WAIT_FAILED."));
			break;
		}
	}
__cleanup:
	CloseHandle(event_refresh);
	event_refresh = NULL;
	return 0;
}
DWORD WINAPI threadproc_refresh2(LPVOID lpParameter)
{
	TCHAR bufcls[256];
	TCHAR buftxt[256];
	RECT dskrct, tmprct;
	HWND hwnd;
	::GetWindowRect(hwnd, &dskrct);
	while (0 == g_to_exit) {
		::Sleep(300);
		if (NULL == hwnd) {
			hwnd = ::FindWindow(NULL, _T("金蝶.远程协助"));
			if (NULL != hwnd) {
			::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 80, 0);
			}
		} else if (!IsWindow(hwnd)) {
			hwnd = NULL;
			::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 80, 1);
		}
	}
	return 0;
}
int g_in_chkupd = 0;
DWORD WINAPI threadproc_chkupd(LPVOID lpParameter)
{
	g_in_chkupd = 1;
	g_pdlg_dsk->get_updurl();
	g_in_chkupd = 0;
	return 0;
}
extern SUBLOG lpfn_SubLog;
HANDLE event_oplog = NULL;
DWORD WINAPI threadproc_oplog(LPVOID lpParameter)
{
	char buftmp[128];
	char bufini[MAX_PATH];
	wsprintfA(bufini, "%s\\oplog.ini", t2a(buff_workdir).c_str());
	char buflclid[128];
	char buflcluid[128];
	char buftype[256];
	char bufrmtid[128];
	char bufrmtuid[128];
	char bufremark[256];

	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	DWORD ret_wfso;
	event_oplog = CreateEvent(&sa, FALSE, FALSE, _T("oplog_eve_7A39D3D4-B09F-4f74-AC7D-E6A1BEA5C8A1"));
	if (NULL == event_oplog) {
		LogFat(_T("threadproc_oplog, CreateEvent fail."));
		goto __cleanup;
	}
	::ResetEvent(event_oplog);
	SYSTEMTIME st;
	while (0 == g_to_exit) {
		ret_wfso = WaitForSingleObject(event_oplog, INFINITE);
		if (ret_wfso == WAIT_OBJECT_0) { // The state of the specified object is signaled.
			LogMsg(_T("threadproc_oplog, WAIT_OBJECT_0."));
		} else if (ret_wfso == WAIT_TIMEOUT) { // The time-out interval elapsed, and the object's state is nonsignaled.
		} else if (ret_wfso == WAIT_FAILED) { // error occurs.
			LogFat(_T("threadproc_oplog, ret_wfso == WAIT_FAILED."));
			break;
		}
		if (::PathFileExistsA(bufini)) {
			GetLocalTime(&st);
			wsprintfA(buftmp, "%04d-%02d-%02d %02d:%02d:%02d", (int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour, (int)st.wMinute, (int)st.wSecond);
			::GetPrivateProfileStringA("oplog", "lclid", "", buflclid, 128, bufini);
			::GetPrivateProfileStringA("oplog", "lcluid", "", buflcluid, 128, bufini);
			::GetPrivateProfileStringA("oplog", "type", "", buftype, 256, bufini);
			::GetPrivateProfileStringA("oplog", "rmtid", "", bufrmtid, 128, bufini);
			::GetPrivateProfileStringA("oplog", "rmtuid", "", bufrmtuid, 128, bufini);
			::GetPrivateProfileStringA("oplog", "remark", "", bufremark, 256, bufini);
			if (NULL != strstr(buftype, "无人值守")) {
				lpfn_SubLog(buftmp, buflclid, buflcluid, buftype, buftmp, bufrmtid, bufrmtuid, bufremark, BufOutErr);
			} else {
			lpfn_SubLog(buftmp, buflclid, buflcluid, (char*)UTF8ToMultiByte(buftype).c_str(), buftmp, bufrmtid, bufrmtuid, (char*)UTF8ToMultiByte(bufremark).c_str(), BufOutErr);
			}
			//DeleteFileEx(a2t(bufini).c_str());
		}
	}
__cleanup:
	CloseHandle(event_oplog);
	event_oplog = NULL;
	return 0;
}
int setv_lm_dword(LPCTSTR pszSubKey, LPCTSTR pszValue, DWORD data, DWORD* retptr = NULL, HWND hwndmsg = NULL) {
	int ret__;
	CString strtmp;
	DWORD type__ = 0;
	DWORD data__ = -1;
	DWORD cbData = sizeof(DWORD);
	SHGetValue(HKEY_LOCAL_MACHINE, pszSubKey, pszValue, &type__, &data__, &cbData);
	ret__ = data__;
	if (data__ != data) {
		type__ = REG_DWORD;
		data__ = data;
		cbData = sizeof(DWORD);
		DWORD ret__ = SHSetValue(HKEY_LOCAL_MACHINE, pszSubKey, pszValue, type__, &data__, cbData);
		if (ERROR_SUCCESS != ret__) {
			if (NULL != retptr) *retptr = ret__;
			if (NULL != hwndmsg) {
				strtmp.Format(_TXTLNG(_T("设置注册表失败(%s)！"), _T("Failed to set registry(%s)!")), pszValue);
				::SendMessage(hwndmsg, WM_CLT_COMMAND, 10, (LPARAM)strtmp.GetBuffer());
			}
		}
	}
	return ret__;
}
HGLOBAL clipboardbuf = NULL;
BOOL copy_to_clipboard(HWND hwnd, string content)
{
	if (OpenClipboard(hwnd)) {
		if (!EmptyClipboard()) {
			LogMsg(_T("copy_to_clipboard|EmptyClipboard fails"));
		}
		if (NULL != clipboardbuf) {
			if (NULL == GlobalFree(clipboardbuf)) {
				LogMsg(_T("copy_to_clipboard|GlobalFree success"));
			} else {
				LogMsg(_T("copy_to_clipboard|GlobalFree fail"));
			}
			clipboardbuf = NULL;
		}
		clipboardbuf = GlobalAlloc(GHND, content.length() + 1);
		char* buffer;
		buffer = (char*)GlobalLock(clipboardbuf);
		strcpy(buffer, content.c_str());
		if (!GlobalUnlock(clipboardbuf) && NO_ERROR == GetLastError()) {
		}
		if (!SetClipboardData(CF_TEXT, clipboardbuf)) {
			LogMsg(_T("copy_to_clipboard|SetClipboardData fails"));
		}
		if (!CloseClipboard()) {
			LogMsg(_T("copy_to_clipboard|CloseClipboard fails"));
		}
	} else {
		LogMsg(_T("copy_to_clipboard|OpenClipboard fails"));
	}
	return TRUE;
}
int dnadvimg(CString srvname, u_short srvport, CString url, int ishttps = TRUE)
{
	char buftmp[1024];
	FILE *fp = NULL;
	DWORD dwFlags, dwByteToRead, dwSizeOfRq, dwBytes;
	HINTERNET hnet = NULL, hcon = NULL, hpst = NULL;//hurl = NULL;
	int errcode = 0, ntmp;
	CString strtmp;

	TCHAR szHeaders[256];
	wsprintf(szHeaders, _T("Content-Type: application/x-www-form-urlencoded;charset=UTF-8/r/nConnection: keep-alive/r/nCache-Control: no-cache/r/n"));

	hnet = InternetOpen(_T("KdDeskTCData"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (NULL == hnet) {
		LogErr(_T("threadproc_dnadvimg, InternetOpen fail."));
		errcode = 1;
		goto __cleanup;
	}
	hcon = InternetConnect(hnet, srvname, srvport, _T(""), _T(""), INTERNET_SERVICE_HTTP, 0, 1);
	if (NULL == hcon) {
		LogErr(_T("dnadvimg, InternetConnect fail, %s:%d."), srvname, (int)srvport);
		errcode = 1;
		goto __cleanup;
	}
	dwFlags = INTERNET_FLAG_DONT_CACHE;
	if (TRUE == ishttps) {
		dwFlags |= (INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
			INTERNET_FLAG_KEEP_CONNECTION |
			INTERNET_FLAG_NO_COOKIES |
			INTERNET_FLAG_NO_UI |
			INTERNET_FLAG_SECURE |
			INTERNET_FLAG_RELOAD);
	}
	hpst = HttpOpenRequest(hcon, _T("GET"), url, HTTP_VERSION, NULL, szAcceptTypes, dwFlags, 1);
	if (NULL == hpst) {
		LogErr(_T("threadproc_dnadvimg, HttpOpenRequest fail, %s:%d."), srvname, (int)srvport);
		errcode = 1;
		goto __cleanup;
	}
	DWORD dwBufLen = sizeof(dwFlags);
	InternetQueryOption(hpst, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBufLen);
	dwFlags |= SECURITY_IGNORE_ERROR_MASK;
	InternetSetOption(hpst, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
	InternetSetOption(hpst, INTERNET_OPTION_IGNORE_OFFLINE, NULL, 0);
	BOOL ret__ = HttpSendRequest(hpst, szHeaders, lstrlen(szHeaders), _T(""), 0);
	if (FALSE == ret__) {
		LogErr(_T("threadproc_dnadvimg, HttpSendRequest fail, %s:%d."), srvname, (int)srvport);
		errcode = 1;
		goto __cleanup;
	}
	dwByteToRead = 0;
	dwSizeOfRq = 4;
	dwBytes = 0;
	if (!HttpQueryInfo(hpst, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL)) {
		dwByteToRead = 0;
	}
	if (dwByteToRead != 200) {
		LogErr(_T("threadproc_dnadvimg, HttpQueryInfo fail, dwByteToRead:%u.", dwByteToRead));
		errcode = 1;
	}
	if (!HttpQueryInfo(hpst, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL)) {
		dwByteToRead = 0;
	}
	DWORD dwNumberOfBytesRead;
	ntmp = url.ReverseFind('/');
	if (-1 == ntmp) {
		errcode = 1;
		goto __cleanup;
	}
	strtmp.Format(_T("%s\\advert\\%s"), buff_workdir, url.Mid(ntmp+1));
	fp = _tfopen(strtmp, _T("wb"));
	if (NULL == fp) {
		LogErr(_T("threadproc_dnadvimg, _tfopen fail - %s."), strtmp);
		errcode = 1;
		goto __cleanup;
	}
	while (0 == g_to_exit) {
		if (!InternetReadFile(hpst, buftmp, 1024, &dwNumberOfBytesRead)) {
			LogErr(_T("threadproc_dnadvimg, InternetReadFile fail."));
			errcode = 1;
			goto __cleanup;
		}
		if (0 == dwNumberOfBytesRead) {
			LogErr(_T("threadproc_dnadvimg, 0 == dwNumberOfBytesRead, to break."));
			break;
		}
		fwrite(buftmp, sizeof(char), dwNumberOfBytesRead, fp);
	}
	if (NULL != fp) {
		fclose(fp);
		fp = NULL;
	}
__cleanup:
	if (NULL != fp) {
		fclose(fp);
		fp = NULL;
	}
	if (NULL != hpst) {
		InternetCloseHandle(hpst);
		hpst = NULL;
	}
	if (NULL != hcon) {
		InternetCloseHandle(hcon);
		hcon = NULL;
	}
	if (NULL != hnet) {
		InternetCloseHandle(hnet);
		hnet = NULL;
	}
	return errcode;
}
int AddAdvert(int nId, char* pszImg, char* pszUrl, char* pszTxt)
{
	ADVITEM advitem;
	advitem.nid = nId;
	advitem.strimg = a2t(pszImg).c_str();
	advitem.strurl = a2t(pszUrl).c_str();
	advitem.strtxt = a2t(pszTxt).c_str();
	LogMsg(_T("AddAdvert, %d, %s, %s."), nId, advitem.strimg, advitem.strurl);
	g_vec_adv.push_back(advitem);
	return 0;
}
// https://kisyun.kingdee.com/view/style/img/login-banner3.jpg
int GetSrvAddr(CString fullurl, CString& srvname, u_short& srvport, CString& url, int& ishttps) {
	TCHAR buftmp[1024];
	_tcscpy(buftmp, fullurl.MakeLower().GetString());
	TCHAR* pcursorbgn = _tcsstr(buftmp, _T("https://"));
	if (NULL != pcursorbgn) {
		ishttps = TRUE;
		pcursorbgn += 8;
	}
	else {
		ishttps = FALSE;
		pcursorbgn = buftmp;
		pcursorbgn += 7;
	}
	TCHAR* pcursor = _tcschr(pcursorbgn, '/');
	if (NULL == pcursor) {
		return 0;
	}
	url = pcursor;
	*pcursor = 0;
	pcursor = _tcschr(pcursorbgn, ':');
	if (NULL == pcursor) {
		srvname = pcursorbgn;
		if (0 == ishttps) {
			srvport = 80;
		}
		else {
			srvport = INTERNET_DEFAULT_HTTPS_PORT;
		}
	}
	else {
		*pcursor = 0;
		pcursor++;
		srvname = pcursorbgn;
		srvport = _ttoi(pcursor);
	}
	LogMsg(_T("GetSrvAddr, %s -> %s:%d, %s, %d."), fullurl, srvname, (int)srvport, url, ishttps);
	return 1;
}
int g_in_advert = 0;
int g_advintvl = 10;
DWORD WINAPI threadproc_advert(LPVOID lpParameter)
{
	g_in_advert = 1;
	int errcode = 0, ntmp;
	TCHAR buftmp[1024];
	CString srvname, url;
	u_short srvport = 0;
	int ishttps = 0;
	wsprintf(buftmp, _T("%s\\advert"), buff_workdir);
	buftmp[_tcslen(buftmp)+1] = 0;
	if (::PathFileExists(buftmp)) {
		SHFILEOPSTRUCT shfos;
		shfos.hwnd   = NULL;
		shfos.wFunc  = FO_DELETE;
		shfos.pFrom  = buftmp;
		shfos.pTo    = NULL;
		shfos.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
		shfos.hNameMappings = NULL;
		if (0 != SHFileOperation(&shfos)) {
			errcode = 1;
			goto __cleanup;
		}
	}
	CreateDirectory(buftmp, NULL);

	SYSTEMTIME st;
	GetLocalTime(&st);
	wsprintf(buftmp, _T("%04d-%02d-%02d %02d:%02d:%02d"), (int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour, (int)st.wMinute, (int)st.wSecond);
	try {
		int ret__ = lpfn_GetAdvertLst(t2a(buftmp).c_str(), BufOutUsrId, AddAdvert, &g_advintvl, BufOutErr);
		if (200 != ret__) {
			errcode = 1;
			goto __cleanup;
		}
	} catch (...) {
		LogFat(_T("threadproc_advert, unexpected error."));
		errcode = 1;
		goto __cleanup;
	}
	if (0 == g_vec_adv.size()) {
		goto __cleanup;
	}
	vector<ADVITEM>::iterator iter__ = g_vec_adv.begin();
	while (iter__ != g_vec_adv.end()) {
		if (1 == GetSrvAddr(iter__->strimg, srvname, srvport, url, ishttps)) {
			dnadvimg(srvname, srvport, url, ishttps);
		}
		++iter__;
	}
	if (1 == g_vec_adv.size()) {
		::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 50, 0);
		goto __cleanup;
	}
	ntmp = 0;
	while (0 == g_to_exit) {
		::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 50, ntmp);
		::Sleep(g_advintvl*1000);
		ntmp++;
		if (ntmp == g_vec_adv.size()) {
			ntmp = 0;
		}
	}
__cleanup:
	g_in_advert = 0;
	return 0;
}
//#define UPDDNSRV_ADDR _T("kisdep.kingdee.com")
//#define UPDDNSRV_PORT 8088
#define UPDDNSRV_ADDR _T("downloads.cmcloud.cn")
#define UPDDNSRV_PORT INTERNET_DEFAULT_HTTPS_PORT
#define UPDDNSRV_URL_INI _T("/kis/kiscloud/stable/kddesk/KdDeskTC_Inst.ini")
#define UPDDNSRV_URL_EXE _T("/kis/kiscloud/stable/kddesk/KdDeskTC_Inst.exe")
//#define UPDDNSRV_URL_INI _T("/kis_test/temp/kddesk/KdDeskTC_Inst.ini")
//#define UPDDNSRV_URL_EXE _T("/kis_test/temp/kddesk/KdDeskTC_Inst.exe")
int g_in_dninstexe = 0;
DWORD WINAPI threadproc_dninstexe(LPVOID lpParameter)
{
	g_in_dninstexe = 1;
	HWND hwnd = (HWND)lpParameter;
	char buftmp[1024];
	FILE *fp = NULL;
	DWORD dwFlags, dwByteToRead, dwSizeOfRq, dwBytes;
	HINTERNET hnet = NULL, hcon = NULL, hpst = NULL;
	int errcode = 0;

	TCHAR szHeaders[256];
	wsprintf(szHeaders, _T("Content-Type: application/x-www-form-urlencoded;charset=UTF-8/r/nHost: api.kingdee.com/r/nConnection: keep-alive/r/nCache-Control: no-cache/r/n"));

	if (::IsWindow(hwnd)) {
		g_str_tip = _T("正在下载更新...");
		::SendMessage(hwnd, WM_CLT_COMMAND, 10, (LPARAM)g_str_tip.GetString());
	}
	wsprintf(pathinstexe, _T("%s\\KdDeskTC_Inst.exe"), buff_workdir);
	if (PathFileExists(pathinstexe)) {
		if (!DeleteFileEx(pathinstexe)) {
			//clean_process(0);
			if (!DeleteFileEx(pathinstexe)) {
			LogErr(_T("threadproc_dninstexe, DeleteFileEx fail, %s."), pathinstexe);
			errcode = 1;
			g_str_tip = _T("删除缓存文件失败！");
			goto __cleanup;
			}
		}
	}

	hnet = InternetOpen(_T("KdDeskTCData"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (NULL == hnet) {
		LogErr(_T("threadproc_dninstexe, InternetOpen fail."));
		errcode = 1;
		g_str_tip = _T("调用InternetOpen失败！");
		goto __cleanup;
	}
	hcon = InternetConnect(hnet, UPDDNSRV_ADDR, UPDDNSRV_PORT, _T(""), _T(""), INTERNET_SERVICE_HTTP, 0, 1);
	if (NULL == hcon) {
		LogErr(_T("threadproc_dninstexe, InternetConnect fail, %s:%d."), t2a(UPDDNSRV_ADDR).c_str(), (int)UPDDNSRV_PORT);
		errcode = 1;
		g_str_tip.Format(_T("调用InternetConnect失败！%s:%d."), t2a(UPDDNSRV_ADDR).c_str(), (int)UPDDNSRV_PORT);
		goto __cleanup;
	}
	dwFlags = INTERNET_FLAG_DONT_CACHE;
	if (8088 != UPDDNSRV_PORT) {
		dwFlags |= (INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
			INTERNET_FLAG_KEEP_CONNECTION |
			INTERNET_FLAG_NO_COOKIES |
			INTERNET_FLAG_NO_UI |
			INTERNET_FLAG_SECURE |
			INTERNET_FLAG_RELOAD);
	}
	hpst = HttpOpenRequest(hcon, _T("GET"), UPDDNSRV_URL_EXE, HTTP_VERSION, NULL, szAcceptTypes, dwFlags, 1);
	if (NULL == hpst) {
		LogErr(_T("threadproc_dninstexe, HttpOpenRequest fail, %s:%d."), t2a(UPDDNSRV_ADDR).c_str(), (int)UPDDNSRV_PORT);
		errcode = 1;
		g_str_tip.Format(_T("调用HttpOpenRequest失败！%s:%d."), t2a(UPDDNSRV_ADDR).c_str(), (int)UPDDNSRV_PORT);
		goto __cleanup;
	}
	DWORD dwBufLen = sizeof(dwFlags);
	InternetQueryOption(hpst, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBufLen);
	dwFlags |= SECURITY_IGNORE_ERROR_MASK;
	InternetSetOption(hpst, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
	InternetSetOption(hpst, INTERNET_OPTION_IGNORE_OFFLINE, NULL, 0);
	BOOL ret__ = HttpSendRequest(hpst, szHeaders, lstrlen(szHeaders), "", 0);
	if (FALSE == ret__) {
		LogErr(_T("threadproc_dninstexe, HttpSendRequest fail, %s:%d."), t2a(UPDDNSRV_ADDR).c_str(), (int)UPDDNSRV_PORT);
		errcode = 1;
		g_str_tip.Format(_T("调用HttpSendRequest失败！%s:%d."), t2a(UPDDNSRV_ADDR).c_str(), (int)UPDDNSRV_PORT);
		goto __cleanup;
	}
	dwByteToRead = 0;
	dwSizeOfRq = 4;
	dwBytes = 0;
	if (!HttpQueryInfo(hpst, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL)) {
		dwByteToRead = 0;
	}
	if (dwByteToRead != 200) {
		LogErr(_T("threadproc_dninstexe, HttpQueryInfo fail, dwByteToRead:%u."), dwByteToRead);
		errcode = 1;
		g_str_tip.Format(_T("调用HttpQueryInfo失败(%u)！"), dwByteToRead);
		//goto __cleanup;
	}
	if (!HttpQueryInfo(hpst, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL)) {
		dwByteToRead = 0;
	}
	DWORD dwNumberOfBytesRead;
	fp = _tfopen(pathinstexe, _T("wb"));
	if (NULL == fp) {
		LogErr(_T("threadproc_dninstexe, fopen fail - %s."), pathinstexe);
		errcode = 1;
		g_str_tip = _T("调用fopen失败！");
		goto __cleanup;
	}
	while (0 == g_to_exit) {
		if (!InternetReadFile(hpst, buftmp, 1024, &dwNumberOfBytesRead)) {
			LogErr(_T("threadproc_dninstexe, InternetReadFile fail."));
			errcode = 1;
			g_str_tip = _T("调用InternetReadFile失败！");
			goto __cleanup;
		}
		if (0 == dwNumberOfBytesRead) {
			LogErr(_T("threadproc_dninstexe, 0 == dwNumberOfBytesRead, to break."));
			break;
		}
		fwrite(buftmp, sizeof(char), dwNumberOfBytesRead, fp);
	}
	if (NULL != fp) {
		fclose(fp);
		fp = NULL;
	}
__cleanup:
	if (NULL != fp) {
		fclose(fp);
		fp = NULL;
	}
	if (NULL != hpst) {
		InternetCloseHandle(hpst);
		hpst = NULL;
	}
	if (NULL != hcon) {
		InternetCloseHandle(hcon);
		hcon = NULL;
	}
	if (NULL != hnet) {
		InternetCloseHandle(hnet);
		hnet = NULL;
	}
	if (0 == g_to_exit) {
		if (0 == errcode) {
			if (::PathFileExists(pathinstexe)) {
				if (::IsWindow(hwnd)) {
					g_str_tip = _T("正在安装更新...");
					::SendMessage(hwnd, WM_CLT_COMMAND, 10, (LPARAM)g_str_tip.GetString());
					::Sleep(2000);
				}
				::WinExec(t2a(pathinstexe).c_str(), SW_SHOWNORMAL);
				g_to_exit = 1;
			}
		} else {
			if (::IsWindow(hwnd)) {
				::SendMessage(hwnd, WM_CLT_COMMAND, 10, (LPARAM)g_str_tip.GetString());
			}
		}
	}
	g_in_dninstexe = 0;
	return 0;
}
int g_in_dninstini = 0;
DWORD WINAPI threadproc_dninstini(LPVOID lpParameter)
{
	g_in_dninstini = 1;
	HWND hwnd = (HWND)lpParameter;
	char buftmp[1024];
	TCHAR buftmp2[256];
	FILE *fp = NULL;
	DWORD dwFlags, dwByteToRead, dwSizeOfRq, dwBytes;
	HINTERNET hnet = NULL, hcon = NULL, hpst = NULL;
	int errcode = 0;

	TCHAR szHeaders[256];
	wsprintf(szHeaders, _T("Content-Type: application/x-www-form-urlencoded;charset=UTF-8/r/nConnection: keep-alive/r/nCache-Control: no-cache/r/n"));

	wsprintf(pathinstini, _T("%s\\KdDeskTC_Inst.ini"), buff_workdir);
	if (PathFileExists(pathinstini)) {
		if (!DeleteFileEx(pathinstini)) {
			//clean_process(0);
			if (!DeleteFileEx(pathinstini)) {
			LogErr(_T("threadproc_dninstini, DeleteFileEx fail, %s."), pathinstini);
			errcode = 1;
			goto __cleanup;
			}
		}
	}

	hnet = InternetOpen(_T("KdDeskTCData"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (NULL == hnet) {
		LogErr(_T("threadproc_dninstini, InternetOpen fail."));
		errcode = 1;
		//g_str_tip = "调用InternetOpen失败！";
		goto __cleanup;
	}
	hcon = InternetConnect(hnet, UPDDNSRV_ADDR, UPDDNSRV_PORT, _T(""), _T(""), INTERNET_SERVICE_HTTP, 0, 1);
	if (NULL == hcon) {
		LogErr(_T("threadproc_dninstini, InternetConnect fail, %s:%d."), t2a(UPDDNSRV_ADDR).c_str(), (int)UPDDNSRV_PORT);
		errcode = 1;
		goto __cleanup;
	}
	dwFlags = INTERNET_FLAG_DONT_CACHE;
	if (8088 != UPDDNSRV_PORT) {
		dwFlags |= (INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
			INTERNET_FLAG_KEEP_CONNECTION |
			INTERNET_FLAG_NO_COOKIES |
			INTERNET_FLAG_NO_UI |
			INTERNET_FLAG_SECURE |
			INTERNET_FLAG_RELOAD);
	}
	hpst = HttpOpenRequest(hcon, _T("GET"), UPDDNSRV_URL_INI, HTTP_VERSION, NULL, szAcceptTypes, dwFlags, 1);
	if (NULL == hpst) {
		LogErr(_T("threadproc_dninstini, HttpOpenRequest fail, %s:%d."), t2a(UPDDNSRV_ADDR).c_str(), (int)UPDDNSRV_PORT);
		errcode = 1;
		goto __cleanup;
	}
	DWORD dwBufLen = sizeof(dwFlags);
	InternetQueryOption(hpst, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBufLen);
	dwFlags |= SECURITY_IGNORE_ERROR_MASK;
	InternetSetOption(hpst, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
	InternetSetOption(hpst, INTERNET_OPTION_IGNORE_OFFLINE, NULL, 0);
	BOOL ret__ = HttpSendRequest(hpst, szHeaders, lstrlen(szHeaders), _T(""), 0);
	if (FALSE == ret__) {
		LogErr(_T("threadproc_dninstini, HttpSendRequest fail, %s:%d."), t2a(UPDDNSRV_ADDR).c_str(), (int)UPDDNSRV_PORT);
		errcode = 1;
		goto __cleanup;
	}
	dwByteToRead = 0;
	dwSizeOfRq = 4;
	dwBytes = 0;
	if (!HttpQueryInfo(hpst, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL)) {
		dwByteToRead = 0;
	}
	if (dwByteToRead != 200) {
		LogErr(_T("threadproc_dninstini, HttpQueryInfo fail, dwByteToRead:%u."), dwByteToRead);
		errcode = 1;
		//goto __cleanup;
	}
	if (!HttpQueryInfo(hpst, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL)) {
		dwByteToRead = 0;
	}
	DWORD dwNumberOfBytesRead;
	fp = _tfopen(pathinstini, _T("wb"));
	if (NULL == fp) {
		LogErr(_T("threadproc_dninstini, fopen fail - %s."), pathinstini);
		errcode = 1;
		goto __cleanup;
	}
	while (0 == g_to_exit) {
		if (!InternetReadFile(hpst, buftmp, 1024, &dwNumberOfBytesRead)) {
			LogErr(_T("threadproc_dninstini, InternetReadFile fail."));
			errcode = 1;
			goto __cleanup;
		}
		if (0 == dwNumberOfBytesRead) {
			LogErr(_T("threadproc_dninstini, 0 == dwNumberOfBytesRead, to break."));
			break;
		}
		fwrite(buftmp, sizeof(char), dwNumberOfBytesRead, fp);
	}
	if (NULL != fp) {
		fclose(fp);
		fp = NULL;
	}
__cleanup:
	if (NULL != fp) {
		fclose(fp);
		fp = NULL;
	}
	if (NULL != hpst) {
		InternetCloseHandle(hpst);
		hpst = NULL;
	}
	if (NULL != hcon) {
		InternetCloseHandle(hcon);
		hcon = NULL;
	}
	if (NULL != hnet) {
		InternetCloseHandle(hnet);
		hnet = NULL;
	}
	if (0 == g_to_exit) {
		if (0 == errcode) {
			if (PathFileExists(pathinstini)) {
				::GetPrivateProfileString(_T("KdDeskTC"), _T("winver"), _T(""), buftmp2, 128, pathinstini);
				if (0 < _tcscmp(buftmp2, CURVER)) {
					LogErr(_T("threadproc_dninstini, oldver:%s, newver:%s."), CURVER, buftmp2);
					g_newver = buftmp2;
					if (NULL != g_pdlg_dsk) {
						::PostMessage(g_pdlg_dsk->m_hWnd, WM_CLT_COMMAND, 60, 0);
					}
					if (1 == ::GetPrivateProfileInt(_T("KdDeskTC"), _T("frcupd"), 0, pathinstini)) {
						threadproc_dninstexe(lpParameter);
					}
				}
			}
		}
		if (::IsWindow(hwnd)) {
			::PostMessage(hwnd, WM_CLT_COMMAND, 20, g_to_exit);
		}
	}
	g_in_dninstini = 0;
	return 0;
}
void clean_process(int flag)
{
	HANDLE hProcess;
	TCHAR pathdos[MAX_PATH] = { 0 };
	TCHAR pathtmp[MAX_PATH];
	memcpy(pathtmp, buff_workdir, 2);
	pathtmp[2] = 0;
	QueryDosDevice(pathtmp, pathdos, MAX_PATH);
	_tcscat(pathdos, buff_workdir+2);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return;
	}
	BOOL ret__tp;
	PROCESSENTRY32 ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	BOOL bReport = Process32First(hSnapshot, &ProcessEntry);
	while (bReport) {
		if (ProcessEntry.th32ProcessID != ::GetCurrentProcessId()) {
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessEntry.th32ProcessID);   
			if (NULL != hProcess) {
				pathtmp[0] = 0;
				GetProcessImageFileName(hProcess, pathtmp, MAX_PATH);
				//alwayslogmsg("clean_process, %s.", pathtmp);
				if (NULL != _tcsstr(pathtmp, pathdos)) {
					ret__tp = TerminateProcess(hProcess, 0);
					//alwayslogmsg("clean_process, TerminateProcess(%d), %s.", ret__tp, pathtmp);
				}
				//if (NULL != hProcess) {
				::CloseHandle(hProcess);
				//}
			}
		}
		bReport = Process32Next(hSnapshot, &ProcessEntry);
	}
	CloseHandle(hSnapshot);
}
BOOL delete_startmenu()
{
	if (!::PathFileExists(g_sm_sc_base)) {
		return TRUE;
	}
	SHFILEOPSTRUCT shfos;
	shfos.hwnd = NULL;
	shfos.wFunc = FO_DELETE;
	shfos.pFrom = g_sm_sc_base;
	shfos.pTo = NULL;
	shfos.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	if (0 != SHFileOperation(&shfos)) {
		//LogFat("DelSrvUpdDir, SHFileOperation(FO_DELETE) fail -> %s.", bufdir);
		return FALSE;
	}
	//LogErr("DelSrvUpdDir, SHFileOperation(FO_DELETE) success -> %s.", bufdir);
	return TRUE;
}
int uninstallex()
{
	char buftmp[MAX_PATH];
	FILE *fp = NULL;

	DeleteFileEx(g_dskshortcut);
	delete_startmenu();
	//SHDeleteKey(HKEY_LOCAL_MACHINE, "Software\\CldPrintApp");
	//SHDeleteValue(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", g_buff_path);
	//::WinExec("sc.exe stop KPCloudGuardian", SW_HIDE);
	ControlServiceEx(_T("KdDeskTC"), 11);
	::WinExec("sc.exe delete KdDeskTC", SW_HIDE);
	clean_process(1);
	sprintf(buftmp, "%s\\uninstall.bat", t2a(buff_workdir).c_str());
	fp = fopen(buftmp, "wt");
	if (NULL == fp) {
		//alwayslogmsg("uninstallex, error occur, fopen(%s) fail.", buftmp);
		goto __cleanup;
	}
	fputs("timeout /t 5 /nobreak > nul", fp);
	fputs("\n", fp);
	sprintf(buftmp, "rmdir /s /q \"%s\"", t2a(buff_workdir).c_str());
	fputs(buftmp, fp);
	fputs("\n", fp);

	fclose(fp);
	fp = NULL;

	sprintf(buftmp, "%s\\uninstall.bat", t2a(buff_workdir).c_str());
	ShellExecuteA(NULL, "open", buftmp, NULL, NULL, SW_HIDE);
	
__cleanup:
	if (NULL != fp) {
		fclose(fp);
		fp = NULL;
	}
	ExitProcess(0);
	return 0;
}
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKdDeskTCDlg 对话框
CKdDeskTCDlg::CKdDeskTCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKdDeskTCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_SelfResize = -1;
	m_curtab = 0;
	m_pdlg_trayex = NULL;
	m_curadv = -1;
	// tab rmtctr.
	m_plab_curdvc = NULL;
	m_plab_lid = NULL;
	m_plab_lpwd = NULL;
	m_pedt_lid = NULL;
	m_pedt_lpwd = NULL;
	m_pbtn_copy = NULL;
	m_pbtn_vline = NULL;
	m_pbtn_eye = NULL;
	m_pbtn_updpwd = NULL;
	m_plab_rid = NULL;
	m_pcmb_rid = NULL;
	m_pbtn_conr = NULL;
	m_pbtn_ready = NULL;
	m_plab_ready = NULL;
	m_in_conr = 0;
	// tab advset.
	m_pchk_autsta = NULL;
	m_plab_defdir = NULL;
	m_prad_prv = NULL;
	m_prad_cst = NULL;
	m_pedt_defdir = NULL;
	m_pbtn_chgdir = NULL;
	m_pbtn_opndir = NULL;
	m_plab_pwdupd = NULL;
	m_pcmb_pwdupd = NULL;
	m_plab_idle = NULL;
	m_pcmb_idle = NULL;
	m_plab_autsta = NULL;
	m_plab_fletrn = NULL;
	m_plab_clswnd = NULL;
	m_pchk_totray = NULL;
	m_plab_logset = NULL;
	m_pchk_autlog = NULL;
	m_plab_about  = NULL;
	m_plab_curver = NULL;
	m_pbtn_updn = NULL;
	m_pbtn_uninst = NULL;
	m_pwdupd = 1;
	m_idle = 1;
	// tab dvclst.
	m_plst_dvc = NULL;
	m_pbtn_vline3 = NULL;
	m_plab_dvcname = NULL;
	m_plab_dvccode = NULL;
	m_pbtn_rmtcon = NULL;
	m_pbtn_rmtfle = NULL;
	m_plst_dvc2 = NULL;
	m_plab_dvc = NULL;
	m_plab_dvc2 = NULL;

	memset(&m_rcMonitor, 0, sizeof(RECT));
	memset(&m_rcWork, 0, sizeof(RECT));
	m_pdlg_menu1 = NULL;
	m_pdlg_menu2 = NULL;
	m_pdlg_menu3 = NULL;
}
CKdDeskTCDlg::~CKdDeskTCDlg()
{
	if (this->m_pdlg_trayex) {
		delete this->m_pdlg_trayex;
		this->m_pdlg_trayex = NULL;
	}
	if (this->m_pdlg_menu1) {
		delete this->m_pdlg_menu1;
		this->m_pdlg_menu1 = NULL;
	}
	if (this->m_pdlg_menu2) {
		delete this->m_pdlg_menu2;
		this->m_pdlg_menu2 = NULL;
	}
	if (this->m_pdlg_menu3) {
		delete this->m_pdlg_menu3;
		this->m_pdlg_menu3 = NULL;
	}
	CTrayIcon::DeleteILTray();
}
void CKdDeskTCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_MIN, m_btn_min);
	DDX_Control(pDX, IDC_BTN_CLS, m_btn_cls);
	DDX_Control(pDX, IDC_BTN_VLINE2, m_btn_vline2);
	DDX_Control(pDX, IDC_BTN_HLP, m_btn_hlp);
	DDX_Control(pDX, IDC_BTN_LOGOLB, m_btn_logolb);
	DDX_Control(pDX, IDC_BTN_LOGOLB2, m_btn_logolb2);
	DDX_Control(pDX, IDC_BTN_AVATAR, m_btn_avatar);
	DDX_Control(pDX, IDC_BTN_LOGON, m_btn_logon);
	DDX_Control(pDX, IDC_BTN_RMTCTR, m_btn_rmtctr);
	DDX_Control(pDX, IDC_BTN_DVCLST, m_btn_dvclst);
	DDX_Control(pDX, IDC_BTN_ADVSET, m_btn_advset);
}

BEGIN_MESSAGE_MAP(CKdDeskTCDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_MIN, OnBnClickedBtnMin)
	ON_BN_CLICKED(IDC_BTN_CLS, OnBnClickedBtnCls)
	ON_MESSAGE(WM_TRANSPARENT_BK_CTOP, OnTransaprentBk)
	ON_MESSAGE(WM_CLT_COMMAND, OnCltCommand)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_BTN_LOGOLB, OnBnClickedBtnLogolb)
	ON_BN_CLICKED(IDC_BTN_LOGON, OnBnClickedBtnLogon)
	ON_BN_CLICKED(IDC_BTN_RMTCTR, OnBnClickedBtnRmtctr)
	ON_BN_CLICKED(IDC_BTN_DVCLST, OnBnClickedBtnDvclst)
	ON_BN_CLICKED(IDC_BTN_ADVSET, OnBnClickedBtnAdvset)
	ON_BN_CLICKED(IDC_BTN_EYE, OnBnClickedBtnEye)
	ON_BN_CLICKED(IDC_BTN_UPDPWD, OnBnClickedBtnUpdpwd)
	ON_BN_CLICKED(IDC_CHK_AUTSTA, OnBnClickedChkAutsta)
	ON_BN_CLICKED(IDC_CHK_AUTLOG, OnBnClickedChkAutlog)
	ON_NOTIFY(HDN_ENDTRACK, 0, OnEndtrackSkinList)
	ON_BN_CLICKED(IDC_BTN_HLP, OnBnClickedBtnHlp)
	ON_NOTIFY(NM_RCLICK, IDC_LST_DVC, OnNMRclickLstDvc)
	ON_NOTIFY(NM_RCLICK, IDC_LST_DVC2, OnNMRclickLstDvc2)
	ON_NOTIFY(NM_CLICK, IDC_LST_DVC, OnNMLclickLstDvc)
	ON_NOTIFY(NM_CLICK, IDC_LST_DVC2, OnNMLclickLstDvc2)
	ON_BN_CLICKED(IDC_CHK_TOTRAY, OnBnClickedChkTotray)
	ON_CBN_SELCHANGE(IDC_CMB_RID, OnCbnSelchangeCmbrid)
	ON_BN_CLICKED(IDC_BTN_CONR, OnBnClickedBtnConr)
	ON_BN_CLICKED(IDC_RAD_PRV, OnBnClickedRadPrv)
	ON_BN_CLICKED(IDC_RAD_CST, OnBnClickedRadCst)
	ON_CBN_SELCHANGE(IDC_CMB_PWDUPD, OnCbnSelchangeCmbpwdupd)
	ON_CBN_SELCHANGE(IDC_CMB_IDLE, OnCbnSelchangeCmbidle)
	ON_BN_CLICKED(IDC_BTN_CHGDIR, OnBnClickedBtnChgdir)
	ON_BN_CLICKED(IDC_BTN_OPNDIR, OnBnClickedBtnOpndir)
	ON_BN_CLICKED(IDC_BTN_UPDN, OnBnClickedBtnUpdn)
	ON_BN_CLICKED(IDC_BTN_RMTCON, OnBnClickedBtnRmtcon)
	ON_BN_CLICKED(IDC_BTN_RMTFLE, OnBnClickedBtnRmtfle)
	ON_BN_CLICKED(IDC_BTN_COPY, OnBnClickedBtnCopy)
	ON_BN_CLICKED(IDC_BTN_UNINST, OnBnClickedBtnUninst)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LST_DVC, OnItemchangedLstdvc)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LST_DVC2, OnItemchangedLstdvc2)
END_MESSAGE_MAP()

void CKdDeskTCDlg::OnBnClickedOk()
{
	if (0 == m_curtab) {
	if (0 != edtlpwd(0)) {
		if (::GetFocus() == m_pedt_lpwd->m_hWnd) {
			m_pbtn_conr->SetFocus();
		}
		return;
	}
	}
	//OnBnClickedBtnConr();
}
// CKdDeskTCDlg 消息处理程序
void CKdDeskTCDlg::FreezeWnd(int flag) {
	RECT rctmp;
	/*this->GetWindowRect(&rctmp);
	LogMsg(_T("CKdDeskTCDlg->FreezeWnd, this->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_LAB_TITLE)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_LAB_TITLE)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CKdDeskTCDlg->FreezeWnd, GetDlgItem(IDC_LAB_TITLE)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_AVATAR)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_AVATAR)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CKdDeskTCDlg->FreezeWnd, GetDlgItem(IDC_BTN_AVATAR)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_LAB_NICKNAME)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_LAB_NICKNAME)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CKdDeskTCDlg->FreezeWnd, GetDlgItem(IDC_LAB_NICKNAME)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_LOGON)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_LOGON)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CKdDeskTCDlg->FreezeWnd, GetDlgItem(IDC_BTN_LOGON)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_RMTCTR)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_RMTCTR)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CKdDeskTCDlg->FreezeWnd, GetDlgItem(IDC_BTN_RMTCTR)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_DVCLST)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_DVCLST)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CKdDeskTCDlg->FreezeWnd, GetDlgItem(IDC_BTN_DVCLST)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_ADVSET)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_ADVSET)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CKdDeskTCDlg->FreezeWnd, GetDlgItem(IDC_BTN_ADVSET)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);
	this->GetDlgItem(IDC_BTN_LOGOLB)->GetClientRect(&rctmp);
	this->GetDlgItem(IDC_BTN_LOGOLB)->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	LogMsg(_T("CKdDeskTCDlg->FreezeWnd, GetDlgItem(IDC_BTN_LOGOLB)->MoveWindow(%d, %d, %d, %d);"), rctmp.left, rctmp.top, rctmp.right-rctmp.left, rctmp.bottom-rctmp.top);*/
	this->GetWindowRect(&rctmp);
	BOOL ischanged = 0;
	m_SelfResize = 0;
	if (752 != rctmp.right-rctmp.left) {
	rctmp.right = rctmp.left + 752;
	ischanged = 1;
	}
	if (500 != rctmp.bottom-rctmp.top) {
	rctmp.bottom = rctmp.top + 500;
	ischanged = 1;
	}
	if (ischanged) {
		m_SelfResize = 1;
		this->MoveWindow(&rctmp);
	}
	if (0 == flag) {
		GetDlgItem(IDC_LAB_TITLE)->MoveWindow(16, 16, 210, 25);
		GetDlgItem(IDC_BTN_AVATAR)->MoveWindow(60, 53, 51, 25);
		GetDlgItem(IDC_LAB_NICKNAME)->MoveWindow(16, 109, 138, 21);
		GetDlgItem(IDC_BTN_LOGON)->MoveWindow(53, 135, 51, 25);
		GetDlgItem(IDC_BTN_RMTCTR)->MoveWindow(16, 186, 88, 25);
		GetDlgItem(IDC_BTN_DVCLST)->MoveWindow(16, 235, 88, 25);
		GetDlgItem(IDC_BTN_ADVSET)->MoveWindow(16, 284, 88, 25);
		GetDlgItem(IDC_BTN_LOGOLB)->MoveWindow(16, 350, 88, 25);
		GetDlgItem(IDC_BTN_LOGOLB2)->MoveWindow(16, 460, 88, 25);
	}
}
BOOL CKdDeskTCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	g_pdlg_dsk = this;
	this->FreezeWnd();
	CString strtmp, strtmp2;
	this->SetWindowText(_T("金蝶远程协助"));//_T("KdDeskTC"));
	strtmp.Format(_T("V%s"), CURVER);
	this->SetDlgItemText(IDC_LAB_TITLE, strtmp);
	CFont* fntptr = CreateFontEx(18, FW_NORMAL);//FW_BOLD);
	((CStatic*)this->GetDlgItem(IDC_LAB_TITLE))->SetFont(fntptr);
	fntptr = CreateFontEx(19, FW_BOLD);//FW_NORMAL);
	((CStatic*)this->GetDlgItem(IDC_LAB_NICKNAME))->SetFont(fntptr);

	RECT rct;
	this->GetWindowRect(&rct);

	m_imgBackGround.Destroy();
	m_imgBackGround.Create(rct.right-rct.left, rct.bottom-rct.top, 32);
	CrossImageLR(m_imgBackGround, 160, 248, 243, 240, 255, 255, 255);

	CClientDC dc(this);
	m_dcBackGround.CreateCompatibleDC(&dc);
	m_hOldBG = m_dcBackGround.SelectObject(m_imgBackGround);

	m_btn_min.LoadImages(
		_T("SkinPic/min_nor.png"),
		_T("SkinPic/min_hot.png"),
		_T("SkinPic/min_pre.png"),
		_T("SkinPic/min_nor.png"),
		_T("SkinPic/min_nor.png"));
	m_btn_cls.LoadImages(
		_T("SkinPic/cls_nor.png"),
		_T("SkinPic/cls_hot.png"),
		_T("SkinPic/cls_pre.png"),
		_T("SkinPic/cls_nor.png"),
		_T("SkinPic/cls_nor.png"));
	m_btn_vline2.LoadImages(
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"),
		_T("SkinPic/vline2.png"));
	m_btn_hlp.LoadImages(
		_T("SkinPic/hlp_nor.png"),
		_T("SkinPic/hlp_hot.png"),
		_T("SkinPic/hlp_pre.png"),
		_T("SkinPic/hlp_nor.png"),
		_T("SkinPic/hlp_nor.png"));
	m_btn_logolb2.LoadImages(
		_T("SkinPic/logo.png"),
		_T("SkinPic/logo.png"),
		_T("SkinPic/logo.png"),
		_T("SkinPic/logo.png"),
		_T("SkinPic/logo.png"));
	m_btn_avatar.LoadImages(
		_T("SkinPic/avatar.png"),
		_T("SkinPic/avatar.png"),
		_T("SkinPic/avatar.png"),
		_T("SkinPic/avatar.png"),
		_T("SkinPic/avatar.png"));
	m_btn_logon.LoadImages(
		_T("SkinPic/lnkbtn4.png"),
		_T("SkinPic/lnkbtn4.png"),
		_T("SkinPic/lnkbtn4.png"),
		_T("SkinPic/lnkbtn4.png"),
		_T("SkinPic/lnkbtn4.png"));
	m_btn_rmtctr.LoadImages(
		_T("SkinPic/rmtctr_pre.png"),
		_T("SkinPic/rmtctr_pre.png"),
		_T("SkinPic/rmtctr_pre.png"),
		_T("SkinPic/rmtctr_pre.png"),
		_T("SkinPic/rmtctr_pre.png"));
	m_btn_dvclst.LoadImages(
		_T("SkinPic/dvclst_nor.png"),
		_T("SkinPic/dvclst_hot.png"),
		_T("SkinPic/dvclst_pre.png"),
		_T("SkinPic/dvclst_nor.png"),
		_T("SkinPic/rmtctr_nor.png"));
	m_btn_advset.LoadImages(
		_T("SkinPic/advset_nor.png"),
		_T("SkinPic/advset_hot.png"),
		_T("SkinPic/advset_pre.png"),
		_T("SkinPic/advset_nor.png"),
		_T("SkinPic/advset_nor.png"));
	HFONT hfont = createfontex(20, FW_BOLD);
	m_btn_logolb.SetTextFont(hfont);
	m_btn_logolb.m_crTextColor = RGB(43, 140, 239);
	m_btn_logolb.m_crTextColor_hvr = RGB(97, 170, 243);
	m_btn_logolb.m_crTextColor_pre = RGB(43, 140, 239);
	HFONT hfont20 = createfontex(20, FW_NORMAL);
	m_btn_logon.SetTextFont(hfont20);
	m_btn_logon.m_crTextColor = RGB(255, 90, 90);
	m_btn_logon.m_crTextColor_hvr = RGB(255, 125, 125);
	m_btn_logon.m_crTextColor_pre = RGB(255, 90, 90);
	m_btn_logolb.m_frc_w = 130;
	m_btn_logolb.m_frc_h = 56;

	m_btn_rmtctr.SetTextFont(hfont20);
	m_btn_rmtctr.m_crTextColor = RGB(1, 112, 249);
	m_btn_rmtctr.m_crTextColor_hvr = RGB(102, 102, 102);
	m_btn_rmtctr.m_crTextColor_pre = RGB(1, 112, 249);

	m_btn_dvclst.SetTextFont(hfont20);
	m_btn_dvclst.m_crTextColor = RGB(102, 102, 102);
	m_btn_dvclst.m_crTextColor_hvr = RGB(102, 102, 102);
	m_btn_dvclst.m_crTextColor_pre = RGB(1, 112, 249);

	m_btn_advset.SetTextFont(hfont20);
	m_btn_advset.m_crTextColor = RGB(102, 102, 102);
	m_btn_advset.m_crTextColor_hvr = RGB(102, 102, 102);
	m_btn_advset.m_crTextColor_pre = RGB(1, 112, 249);

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

	offset += w + 8;
	this->GetDlgItem(IDC_BTN_MIN)->GetClientRect(&rct_tmp);
	this->GetDlgItem(IDC_BTN_MIN)->ClientToScreen(&rct_tmp);
	this->ScreenToClient(&rct_tmp);
	w = rct_tmp.right - rct_tmp.left;
	h = rct_tmp.bottom - rct_tmp.top;
	rct_tmp.right = rct.right - rct.left - offset;
	rct_tmp.left = rct_tmp.right - w;
	rct_tmp.top = 8;
	rct_tmp.bottom = rct_tmp.top + h;
	this->GetDlgItem(IDC_BTN_MIN)->MoveWindow(&rct_tmp);

	offset += w + 4;
	this->GetDlgItem(IDC_BTN_VLINE2)->GetClientRect(&rct_tmp);
	this->GetDlgItem(IDC_BTN_VLINE2)->ClientToScreen(&rct_tmp);
	this->ScreenToClient(&rct_tmp);
	w = rct_tmp.right - rct_tmp.left;
	h = rct_tmp.bottom - rct_tmp.top;
	rct_tmp.right = rct.right - rct.left - offset;
	rct_tmp.left = rct_tmp.right - w;
	rct_tmp.top = 12;
	rct_tmp.bottom = rct_tmp.top + h;
	this->GetDlgItem(IDC_BTN_VLINE2)->MoveWindow(&rct_tmp);

	offset += w + 12;
	this->GetDlgItem(IDC_BTN_HLP)->GetClientRect(&rct_tmp);
	this->GetDlgItem(IDC_BTN_HLP)->ClientToScreen(&rct_tmp);
	this->ScreenToClient(&rct_tmp);
	w = rct_tmp.right - rct_tmp.left;
	h = rct_tmp.bottom - rct_tmp.top;
	rct_tmp.right = rct.right - rct.left - offset;
	rct_tmp.left = rct_tmp.right - w;
	rct_tmp.top = 13;
	rct_tmp.bottom = rct_tmp.top + h;
	this->GetDlgItem(IDC_BTN_HLP)->MoveWindow(&rct_tmp);

	//this->SetDlgItemText(IDC_BTN_LOGOLB, _T("     KdDeskTC"));
	this->SetDlgItemText(IDC_BTN_AVATAR, _T(""));

	DrawRoundCorner();

	show_tab_rmtctr();

	m_Shadow.YFCreate(GetSafeHwnd());
	this->SetTimer(TIMER_REDRAW_EVENT, TIMER_REDRAW_ELAPSE, 0);
	this->SetTimer(TIMER_CHKUPD_EVENT, TIMER_CHKUPD_ELAPSE, 0);

	if (m_imglst_icon.m_hImageList) {
	m_imglst_icon.DeleteImageList();
	}
	m_imglst_icon.Create(32, 32, ILC_MASK | ILC_COLOR32, 1, 1); // 建立32位图像控制
	m_imglst_icon.Add(AfxGetApp()->LoadIcon(IDI_ICO_WIN));
	m_imglst_icon.Add(AfxGetApp()->LoadIcon(IDI_ICO_MAC));

	this->GetCurMntrInfo();

	this->m_pdlg_trayex = new CDlgTrayEx();
	this->m_pdlg_trayex->Create(IDD_DLG_TRAYEX, this->GetDesktopWindow());	
	this->m_pdlg_trayex->ShowWindow(SW_HIDE);
	::SetWindowPos(this->m_pdlg_trayex->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW);
	if (!CTrayIcon::hWnd_il) {
		CTrayIcon::CreateILTray();
		CTrayIcon::ModifyTrayicon(m_hIcon, _TXTLNG(_T("KdDeskTC"), _T("KdDeskTC")).GetBuffer());//AfxGetApp()->LoadIcon(IDI_ICO_SMALL), BufWndTitle);
	}
	strtmp = theApp.GetProfileString(_T("KdDeskTC"), _T("User"), _T(""));
	strtmp2.Format(_T("%s****%s"), strtmp.Left(3), strtmp.Right(4));
	this->SetDlgItemText(IDC_LAB_NICKNAME, strtmp2);
	sav_clt_info(this->m_hWnd);
	::PostMessage(m_hWnd, WM_CLT_COMMAND, 10, 0);
	CloseHandle(CreateThread(NULL, 0, threadproc_advert, NULL, 0, NULL));
	CloseHandle(CreateThread(NULL, 0, threadproc_oplog, NULL, 0, NULL));
	set_uid(a2t(BufOutUsrId).c_str());
	//CDlgTmpex dlgtmpex;
	//dlgtmpex.DoModal();
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CKdDeskTCDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CKdDeskTCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CKdDeskTCDlg::DrawRoundCorner()
{
	CRgn rgn;
	CRect rct;
	GetWindowRect(&rct);
	rct -= rct.TopLeft();
	rgn.CreateRoundRectRgn(rct.left, rct.top, rct.right, rct.bottom, 16, 16);
	SetWindowRgn(rgn, TRUE);
}
void CKdDeskTCDlg::OnBnClickedBtnMin()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CKdDeskTCDlg::OnBnClickedBtnCls()
{
	// TODO: 在此添加控件通知处理程序代码
	if (1 == theApp.GetProfileInt(_T("KdDeskTC"), _T("totray"), 0)) {
		g_showmainwnd = 0;
		::ShowWindow(this->m_hWnd, SW_HIDE);
	} else {
		//if (!IsAutoStart()) {
			set_flagex(_T("0"));
		//}
		SendMessage(WM_SYSCOMMAND, SC_CLOSE);
	}
}

void CKdDeskTCDlg::WinHelp(DWORD dwData, UINT nCmd)
{
	// TODO: 在此添加专用代码和/或调用基类
	return;
	CDialog::WinHelp(dwData, nCmd);
}

void CKdDeskTCDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CKdDeskTCDlg::OnEraseBkgnd(CDC* pDC)
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

HBRUSH CKdDeskTCDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	switch(pWnd->GetDlgCtrlID()) {
		case IDC_LAB_TITLE: 
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(111,121,140));
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);   
			break;
		}
		case IDC_LAB_NICKNAME: 
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(45,62,77));
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
			break;
		}
		case IDC_LAB_CURDVC: 
		case IDC_LAB_RID: 
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(77,77,77));
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
			break;
		}
		case IDC_LAB_LID: 
		case IDC_LAB_LPWD: 
		case IDC_LAB_READY: 
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(141,141,141));
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
			break;
		}
		case IDC_LAB_AUTSTA:
		case IDC_LAB_FLETRN:
		case IDC_LAB_PWDUPD:
		case IDC_LAB_IDLE:
		case IDC_LAB_CLSWND: 
		case IDC_LAB_ABOUT: 
		case IDC_LAB_DVCNAME: 
		case IDC_LAB_DVC:
		case IDC_LAB_DVC2:
		case IDC_LAB_LOGSET:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(75,75,75));
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
			break;
		}
		case IDC_LAB_DEFDIR: 
		case IDC_LAB_CURVER: 
		case IDC_LAB_DVCCODE: 
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(90,90,90));
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

void CKdDeskTCDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (TIMER_REDRAW_EVENT == nIDEvent) {
		::RedrawWindow(this->m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
		this->KillTimer(TIMER_REDRAW_EVENT);
		//set_foreground_wnd(this->m_hWnd);
	} else if (TIMER_CHKUPD_EVENT == nIDEvent) {
		CloseHandle(CreateThread(NULL, 0, threadproc_dninstini, NULL, 0, NULL));
	}
	CDialog::OnTimer(nIDEvent);
}
LRESULT CKdDeskTCDlg::OnCltCommand(WPARAM wParam, LPARAM lParam)
{
	CString strtmp, strtmp2;
	int ntmp;
	TCHAR buftmp[MAX_PATH];
	switch (wParam) {
		case 1: {
			//LogMsg(_T("OnCltCommand(1)."));
			m_tc_dn = ::GetTickCount();
			m_pedt_lpwd->m_tc_dn = m_tc_dn;
			break;
		}
		case 10: {
			strtmp.Format(_T("%s\\result.ini"), buff_workdir);
			GetPrivateProfileString(_T("result"), _T("id"), _T(""), buftmp, 128, strtmp);
			m_lid = buftmp;
			GetPrivateProfileString(_T("result"), _T("pwd"), _T(""), buftmp, 128, strtmp);
			m_lpwd = buftmp;
			GetPrivateProfileString(_T("result"), _T("cstdir"), _T(""), buftmp, MAX_PATH, strtmp);
			m_cstdir = buftmp;
			m_pwdupd = GetPrivateProfileInt(_T("result"), _T("pwdupd"), 1, strtmp);
			m_idle = GetPrivateProfileInt(_T("result"), _T("idle"), 1, strtmp);
			this->SetDlgItemText(IDC_EDT_LID, convert_cldskid(m_lid, TRUE));
			//this->SetDlgItemText(IDC_EDT_LPWD, m_lpwd);
			m_pedt_lpwd->SetText(m_lpwd);
			r_rmtidlst();
			break;
		}
		case 11: {
			strtmp.Format(_T("%s\\result3.ini"), buff_workdir);
			GetPrivateProfileString(_T("result"), _T("updurl"), _T(""), buftmp, MAX_PATH, strtmp);
			m_updurl.Format(_T("%s.exe"), buftmp);
			if (m_pbtn_updn) {
				strtmp.Format(_T("发现新版本V%s"), m_updurl.Right(9).Left(5));// KdDeskTC-2.0.3.exe
				m_pbtn_updn->SetWindowText(strtmp);
				//InvalidateRect(NULL);
			}
			break;
		}
		case 12: {
			r_rmtidlst();
			break;
		}
		case 20: {
			if (1 == m_in_conr) {
				m_in_conr = 0;
				if (0 == lParam) {
					//this->KillTimer(TIMER_CONRTO_EVENT);
				} else if (1 == lParam) {
					//this->KillTimer(TIMER_CONRTO_EVENT);
					w_rmtidlst();
					//r_rmtidlst(1);
				} else if (2 == lParam) {
				}
			}
			break;
		}
		case 30: {
			switch (lParam) {
				case 0: {
					LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
					if ((style & WS_MINIMIZE) == WS_MINIMIZE) {
						//CloseHandle(CreateThread(NULL, 0, threadproc_actwnd, (LPVOID)m_hWnd, 0, NULL));
						active_hwnd(m_hWnd);
					} else {
						g_showmainwnd = !g_showmainwnd;
						if (0 == g_showmainwnd) {
							::ShowWindow(this->m_hWnd, SW_HIDE);
						} else {
							::ShowWindow(this->m_hWnd, SW_SHOW);
							//CloseHandle(CreateThread(NULL, 0, threadproc_actwnd, (LPVOID)m_hWnd, 0, NULL));
							active_hwnd(m_hWnd);
						}
					}
					break;
				}
				case 7: {
					g_showmainwnd = 1;
					::ShowWindow(this->m_hWnd, SW_SHOW);
					//CloseHandle(CreateThread(NULL, 0, threadproc_actwnd, (LPVOID)m_hWnd, 0, NULL));
					active_hwnd(m_hWnd);
					break;
				}
				case 8: {
					//CloseHandle(CreateThread(NULL, 0, threadproc_actwnd, (LPVOID)m_hWnd, 0, NULL));
					//CloseHandle(CreateThread(NULL, 0, threadproc_actwnd_ex, (LPVOID)m_hWnd, 0, NULL));
					active_hwnd(m_hWnd);
					//this->SetTimer(TIMER_ACTWND_EVENT, TIMER_ACTWND_ELAPSE, 0);
					//SwitchToThisWindow(this->m_hWnd, TRUE);
					//this->SetForegroundWindow();
					//this->BringWindowToTop();
					break;
				}
			}
			break;
		}
		case 41: {
			TrackPopupMenuEx(lParam);
			break;
		}
		case 50: {
			m_curadv = lParam;
			strtmp = g_vec_adv[m_curadv].strimg;
			ntmp = strtmp.ReverseFind('/');
			if (-1 == ntmp) {
				break;
			}
			strtmp2.Format(_T("%s\\advert\\%s"), buff_workdir, strtmp.Mid(ntmp+1));
			if (::PathFileExists(strtmp2)) {
			m_btn_logolb.LoadImages(strtmp2);
			m_btn_logolb.GetWindowRect(&m_rct_logolb);
			ScreenToClient(&m_rct_logolb);
			InvalidateRect(&m_rct_logolb);
			}
			break;
		}
		case 60: {
			if (NULL != m_pbtn_updn) {
			strtmp.Format(_T("发现新版本V%s"), g_newver);
			m_pbtn_updn->SetWindowText(strtmp);
			}
			break;
		}
		case 70: {
			if (200 == lParam) {
				ntmp = 0;
				if (g_vec_dvc.size() == g_vec_dvc_tmp.size()) {
					for (int i = 0; i < g_vec_dvc.size(); i++) {
						if (g_vec_dvc[i].strid != g_vec_dvc_tmp[i].strid) {
							ntmp = 1;
							break;
						}
					}
				} else {
					ntmp = 1;
				}
				if (1 == ntmp) {
				g_vec_dvc = g_vec_dvc_tmp;
				if (NULL != m_plst_dvc) {
					m_plst_dvc->DeleteAllItems();
					for (int i = 0; i < g_vec_dvc.size(); i++) {
						m_plst_dvc->InsertItem(i, g_vec_dvc[i].strrmk, g_vec_dvc[i].platform);
					}
				}
				}
			}
			break;
		}
		case 80: {
			if (0 == lParam) {
				OnBnClickedBtnRmtctr();
				g_frclseye = 1;
				//m_pedt_lpwd->EnableWindow(FALSE);
				if (0 == g_clseye) {
				OnBnClickedBtnEyeEx();
				}
				//this->ShowWindow(SW_HIDE);
				if (::GetFocus() == m_pedt_lpwd->m_hWnd) {
					m_pbtn_conr->SetFocus();
				}
			} else if (1 == lParam) {
				g_frclseye = 0;
				//m_pedt_lpwd->EnableWindow(TRUE);
				//this->ShowWindow(SW_SHOW);
				m_tc_dn = 0;
				m_pedt_lpwd->m_tc_dn = 0;
			}
			break;
		}
		case 90: {
			m_pwdupd = lParam;
			//m_pcmb_pwdupd->SetCurSel(m_pwdupd);
			//InvalidateRect(NULL);
			break;
		}
	}
	return 0;
}
void CKdDeskTCDlg::TrackPopupMenuEx(int cmd) {
	CString strtmp;
	switch (cmd) {
		case 1: {
			int selitem = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
			if (-1 != selitem) {
				//this->rmt_cnt(g_vec_dvc[selitem].strid);
				char bufTimestamp[128];
				GetBJTime(bufTimestamp);
				lpfn_DelCldskId(bufTimestamp, BufOutUsrId, t2a(g_vec_dvc[selitem].strid.GetBuffer()).c_str(), BufOutErr);

				int indx__ = 0;
				vector<DVCITEM>::iterator iter__ = g_vec_dvc.begin();
				while (iter__ != g_vec_dvc.end()) {
					if (indx__ == selitem) {
						g_vec_dvc.erase(iter__);
						break;
					}
					++indx__;
					++iter__;
				}
				m_plst_dvc->DeleteItem(selitem);
				break;
			}
			selitem = m_plst_dvc2->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
			if (-1 != selitem) {
				delitm_rid(selitem);
			}
			break;
		}
		case 2: {
			int selitem = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
			if (-1 != selitem) {
				this->rmt_fle(g_vec_dvc[selitem].strid);
			}
			break;
		}
		case 6: {
			//this->OnBnClickedCancel();
			//if (!IsAutoStart()) {
				set_flagex(_T("0"));
			//}
			this->OnCancel();
			break;
		}
		case 64: {
			g_showmainwnd = 1;
			::ShowWindow(this->m_hWnd, SW_SHOW);
			active_hwnd(m_hWnd);
			break;
		}
		case 128: { // refresh verification code.
			this->upd_pwd(strtmp);
			m_pedt_lpwd->SetText(strtmp);
			m_pedt_lpwd->SetFocus();
			break;
		}
		case 129: { // customize verification code.
			edtlpwd(1);
			break;
		}
	}
}
void CKdDeskTCDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (0 == g_showmainwnd) {
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
		m_Shadow.ToHideShadow();
	}
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
}
LRESULT CKdDeskTCDlg::OnTransaprentBk(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hwnd = (HWND)lParam;
	CTransparentHelper::OnTranparentControl(m_dcBackGround.GetSafeHdc(), (WPARAM)hdc, (LPARAM)hwnd);
	return TRUE;
}
void CKdDeskTCDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (0 == m_SelfResize) { // 防止死循环。
	this->FreezeWnd(1);
	} else if (1 == m_SelfResize) {
		m_SelfResize = 0;
	}
	// TODO: 在此处添加消息处理程序代码
}
void CKdDeskTCDlg::OnBnClickedBtnLogolb()
{
	// TODO: 在此添加控件通知处理程序代码
	if (-1 == m_curadv) return;
	if (!g_vec_adv[m_curadv].strurl.IsEmpty()) {
	ShellExecute(NULL, _T("open"), g_vec_adv[m_curadv].strurl, _T(""), NULL, SW_SHOWNORMAL);
	}
}

void CKdDeskTCDlg::OnBnClickedBtnLogon()
{
	// TODO: 在此添加控件通知处理程序代码
	theApp.m_command = 2;
	//theApp.WriteProfileInt(_T("KdDeskTC"), _T("AutoLgn"), 0);
	theApp.WriteProfileInt(_T("KdDeskTC"), _T("Logout"), 1);
	//theApp.WriteProfileString(_T("KdDeskTC"), _T("User"), _T(""));
	//theApp.WriteProfileString(_T("KdDeskTC"), _T("Certificate"), _T(""));
	set_flagex(_T("0"));
	this->OnCancel();
}

void CKdDeskTCDlg::OnBnClickedBtnRmtctr()
{
	// TODO: 在此添加控件通知处理程序代码
	m_curtab = 0;
	m_btn_rmtctr.LoadImages(
		_T("SkinPic/rmtctr_pre.png"),
		_T("SkinPic/rmtctr_pre.png"),
		_T("SkinPic/rmtctr_pre.png"),
		_T("SkinPic/rmtctr_pre.png"),
		_T("SkinPic/rmtctr_pre.png"));
	m_btn_dvclst.LoadImages(
		_T("SkinPic/dvclst_nor.png"),
		_T("SkinPic/dvclst_hot.png"),
		_T("SkinPic/dvclst_pre.png"),
		_T("SkinPic/dvclst_nor.png"),
		_T("SkinPic/rmtctr_nor.png"));
	m_btn_advset.LoadImages(
		_T("SkinPic/advset_nor.png"),
		_T("SkinPic/advset_hot.png"),
		_T("SkinPic/advset_pre.png"),
		_T("SkinPic/advset_nor.png"),
		_T("SkinPic/advset_nor.png"));
	m_btn_rmtctr.m_crTextColor = RGB(1, 112, 249);
	m_btn_dvclst.m_crTextColor = RGB(102, 102, 102);
	m_btn_advset.m_crTextColor = RGB(102, 102, 102);
	show_tab_rmtctr();
	hide_tab_dvclst();
	hide_tab_advset();
	InvalidateRect(NULL);
}

void CKdDeskTCDlg::OnBnClickedBtnDvclst()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD tc_off = ::GetTickCount() - m_tc_dn;
	if (500 > tc_off) {
		CDlgMsgBox2 dlgmsgbox(0, _TXTLNG(_T("提示"), _T("Prompt")), _T("远程模式禁用此功能！"));
		dlgmsgbox.DoModal();
		return;
	}
	m_curtab = 1;
	m_btn_rmtctr.LoadImages(
		_T("SkinPic/rmtctr_nor.png"),
		_T("SkinPic/rmtctr_hot.png"),
		_T("SkinPic/rmtctr_pre.png"),
		_T("SkinPic/rmtctr_nor.png"),
		_T("SkinPic/rmtctr_nor.png"));
	m_btn_dvclst.LoadImages(
		_T("SkinPic/dvclst_pre.png"),
		_T("SkinPic/dvclst_pre.png"),
		_T("SkinPic/dvclst_pre.png"),
		_T("SkinPic/dvclst_pre.png"),
		_T("SkinPic/rmtctr_pre.png"));
	m_btn_advset.LoadImages(
		_T("SkinPic/advset_nor.png"),
		_T("SkinPic/advset_hot.png"),
		_T("SkinPic/advset_pre.png"),
		_T("SkinPic/advset_nor.png"),
		_T("SkinPic/advset_nor.png"));
	m_btn_rmtctr.m_crTextColor = RGB(102, 102, 102);
	m_btn_dvclst.m_crTextColor = RGB(1, 112, 249);
	m_btn_advset.m_crTextColor = RGB(102, 102, 102);
	hide_tab_rmtctr();
	show_tab_dvclst();
	hide_tab_advset();
	InvalidateRect(NULL);
	if (0 == g_in_getdvclst) {
		g_vec_dvc_tmp.clear();
		CloseHandle(CreateThread(NULL, 0, threadproc_getdvclst, NULL, 0, NULL));
	}
}

void CKdDeskTCDlg::OnBnClickedBtnAdvset()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD tc_off = ::GetTickCount() - m_tc_dn;
	if (500 > tc_off) {
		CDlgMsgBox2 dlgmsgbox(0, _TXTLNG(_T("提示"), _T("Prompt")), _T("远程模式禁用此功能！"));
		dlgmsgbox.DoModal();
		return;
	}
	m_curtab = 2;
	m_btn_rmtctr.LoadImages(
		_T("SkinPic/rmtctr_nor.png"),
		_T("SkinPic/rmtctr_hot.png"),
		_T("SkinPic/rmtctr_pre.png"),
		_T("SkinPic/rmtctr_nor.png"),
		_T("SkinPic/rmtctr_nor.png"));
	m_btn_dvclst.LoadImages(
		_T("SkinPic/dvclst_nor.png"),
		_T("SkinPic/dvclst_hot.png"),
		_T("SkinPic/dvclst_pre.png"),
		_T("SkinPic/dvclst_nor.png"),
		_T("SkinPic/rmtctr_nor.png"));
	m_btn_advset.LoadImages(
		_T("SkinPic/advset_pre.png"),
		_T("SkinPic/advset_pre.png"),
		_T("SkinPic/advset_pre.png"),
		_T("SkinPic/advset_pre.png"),
		_T("SkinPic/advset_pre.png"));
	m_btn_rmtctr.m_crTextColor = RGB(102, 102, 102);
	m_btn_dvclst.m_crTextColor = RGB(102, 102, 102);
	m_btn_advset.m_crTextColor = RGB(1, 112, 249);
	hide_tab_rmtctr();
	hide_tab_dvclst();
	show_tab_advset();
	InvalidateRect(NULL);
}
void CKdDeskTCDlg::OnBnClickedBtnEyeEx()
{
	// TODO: 在此添加控件通知处理程序代码
	g_clseye = !g_clseye;
	theApp.WriteProfileInt(_T("KdDeskTC"), _T("clseye"), g_clseye);
	if (1 == g_clseye) {
		m_pedt_lpwd->m_clseye = 1;
	m_pbtn_eye->LoadImages(
		_T("SkinPic/clseye_nor.png"),
		_T("SkinPic/clseye_hot.png"),
		_T("SkinPic/clseye_pre.png"),
		_T("SkinPic/clseye_nor.png"),
		_T("SkinPic/clseye_nor.png"));
	} else {
		m_pedt_lpwd->m_clseye = 0;
	m_pbtn_eye->LoadImages(
		_T("SkinPic/opneye_nor.png"),
		_T("SkinPic/opneye_hot.png"),
		_T("SkinPic/opneye_pre.png"),
		_T("SkinPic/opneye_nor.png"),
		_T("SkinPic/opneye_nor.png"));
	}
	m_pedt_lpwd->SetText(m_pedt_lpwd->GetText());
	InvalidateRect(&m_rct_edtlpwd);
	InvalidateRect(&m_rct_btneye);
}
void CKdDeskTCDlg::OnBnClickedBtnEye()
{
	// TODO: 在此添加控件通知处理程序代码
	if (1 == g_frclseye && 1 == g_clseye) {
		DWORD tc_off = ::GetTickCount() - m_tc_dn;
		//LogMsg(_T("OnBnClickedBtnEye, tc_off:%u."), tc_off);
		if (500 > tc_off) {
			return;
		}
	}
	OnBnClickedBtnEyeEx();
}
void CKdDeskTCDlg::OnBnClickedBtnUpdpwd()
{
	// TODO: 在此添加控件通知处理程序代码
	if (1 == g_frclseye) {
		DWORD tc_off = ::GetTickCount() - m_tc_dn;
		if (500 > tc_off) {
			return;
		}
	}
	popup_menu_ex2();
	
}
void CKdDeskTCDlg::OnBnClickedBtnChgdir()
{
	CString strtmp;
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(BROWSEINFO));
	bi.hwndOwner = this->m_hWnd;
	TCHAR path[MAX_PATH] = { 0 };
	bi.lpszTitle = _T("请选择默认打开的文件夹:");
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pitemidlist = SHBrowseForFolder(&bi);
	if (pitemidlist != NULL) {
		SHGetPathFromIDList(pitemidlist, path);
		this->m_cstdir = path;
		this->SetDlgItemText(IDC_EDT_DEFDIR, m_cstdir);
		set_cstdir(path);
	}
}
void CKdDeskTCDlg::OnBnClickedBtnOpndir()
{
	ShellExecute(NULL, _T("open"), m_cstdir, _T(""), NULL, SW_SHOWNORMAL);
}
void CKdDeskTCDlg::OnBnClickedBtnUpdn()
{
	//if (!m_updurl.IsEmpty()) {
	//ShellExecute(NULL, _T("open"), m_updurl, _T(""), NULL, SW_SHOWNORMAL);
	//}
	CString strtmp;
	if (!g_newver.IsEmpty()) {
		if (0 == g_in_dninstexe) {
			CloseHandle(CreateThread(NULL, 0, threadproc_dninstexe, NULL, 0, NULL));
			strtmp.Format(_T("正在更新..."));
			m_pbtn_updn->SetWindowText(strtmp);
		}
	}
}
void CKdDeskTCDlg::OnBnClickedBtnUninst()
{
	CDlgMsgBox2 dlgmsgbox(2, _TXTLNG(_T("提示"), _T("Prompt")), _T("确定要卸载KdDeskTC？"));
	INT_PTR nResponse = dlgmsgbox.DoModal();
	if (nResponse == IDOK) {
		this->ShowWindow(SW_HIDE);
		uninstallex();
	}
}
void CKdDeskTCDlg::OnBnClickedBtnRmtcon()
{
	int selitem = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 != selitem) {
		this->rmt_cnt(g_vec_dvc[selitem].strid);
		return;
	}
	selitem = m_plst_dvc2->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 != selitem) {
		this->rmt_cnt(m_vec_rmtidlst[selitem].strid);
		return;
	}
}
void CKdDeskTCDlg::OnBnClickedBtnRmtfle()
{
	int selitem = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 != selitem) {
		this->rmt_fle(g_vec_dvc[selitem].strid);
		return;
	}
	selitem = m_plst_dvc2->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 != selitem) {
		this->rmt_fle(m_vec_rmtidlst[selitem].strid);
		return;
	}
}
void CKdDeskTCDlg::OnBnClickedBtnCopy()
{
	copy_to_clipboard(m_hWnd, t2a(m_pedt_lid->GetText().GetBuffer()));
}
CString CKdDeskTCDlg::convert_cldskid(CString stri, BOOL addspc)
{
	int ntmp = stri.Find(_T(" ("));
	if (-1 != ntmp) {
		stri = stri.Left(ntmp);
	}
	stri.Replace(_T(" "), _T(""));
	if (FALSE == addspc) {
	return stri;
	}
	CString stro;
	while (!stri.IsEmpty()) {
		stro += stri.Left(3);
		stro += _T(" ");
		stri.Delete(0, 3);
	}
	stro.TrimRight(_T(" "));
	return stro;
}
void CKdDeskTCDlg::r_rmtidlst(int flag)
{
	CString strtmp, strtmp2;
	TCHAR buftmp[4096];
	m_pcmb_rid->DeleteAllString();
	if (0 == flag) {
	m_pcmb_rid->SetCurText(_T(""));
	}
	//m_vec_rmtidlst.clear();
	vector<DVCITEM2> vec_rmtidlst;
	DVCITEM2 di2;
	//tstring RmtIdLst = theApp.GetProfileString(_T("KdDeskTC"), _T("ridlst"), _T("")).GetBuffer();
	::GetPrivateProfileString(_T("common"), _T("idlst"), _T(""), buftmp, 4096, g_history_path);
	tstring RmtIdLst = buftmp;
	int nPos = 0, nOffset = 0;
	if (!RmtIdLst.empty()) {
		while (true) {
			nPos = RmtIdLst.find(_T(","), nOffset);
			if (nPos != tstring::npos) {
				di2.strid = RmtIdLst.substr(nOffset, nPos - nOffset).c_str();
				::GetPrivateProfileString(di2.strid, _T("hostname"), _T(""), buftmp, 128, g_history_path);
				if (di2.strid == cldskid) {
					di2.strrmk.Format(_T("%s(本设备)"), buftmp);
				} else {
				di2.strrmk = buftmp;
				}
				::GetPrivateProfileString(di2.strid, _T("platform"), _T(""), buftmp, 128, g_history_path);
				if (0 == _tcscmp(buftmp, _T("Windows"))) {
					di2.platform = 0;
				} else {
					di2.platform = 1;
				}
				vec_rmtidlst.push_back(di2);
				nOffset = nPos + 1;
			} else {
				di2.strid = RmtIdLst.substr(nOffset, RmtIdLst.length() - nOffset).c_str();
				::GetPrivateProfileString(di2.strid, _T("hostname"), _T(""), buftmp, 128, g_history_path);
				if (di2.strid == cldskid) {
					di2.strrmk.Format(_T("%s(本设备)"), buftmp);
				} else {
				di2.strrmk = buftmp;
				}
				::GetPrivateProfileString(di2.strid, _T("platform"), _T(""), buftmp, 128, g_history_path);
				if (0 == _tcscmp(buftmp, _T("Windows"))) {
					di2.platform = 0;
				} else {
					di2.platform = 1;
				}
				vec_rmtidlst.push_back(di2);
				break;
			}
		}
	}
	CString rmtid = theApp.GetProfileString(_T("KdDeskTC"), _T("rid"), _T(""));
	if (rmtid.IsEmpty() && 0 < vec_rmtidlst.size()) {
		rmtid = vec_rmtidlst[0].strid;
	}
	if (!rmtid.IsEmpty()) {
		int index_usr = 0;
		vector<DVCITEM2>::const_iterator iter__ = vec_rmtidlst.begin();
		while (iter__ != vec_rmtidlst.end())
		{
			strtmp = this->convert_cldskid(iter__->strid, TRUE);
			strtmp2 = iter__->strrmk.GetString();
			//strtmp2 = strtmp2.Left(15);
			//truncature(&strtmp2, NULL, 15);
			strtmp2.Replace(_T("("), _T(""));
			strtmp2.Replace(_T(")"), _T(""));
			if (!strtmp2.IsEmpty()) {
				strtmp += _T(" (");
				strtmp += strtmp2;
				strtmp += _T(")");
			}
			m_pcmb_rid->InsertString(index_usr, strtmp);
			if (rmtid == iter__->strid)
			{
				m_pcmb_rid->SetCurSel(index_usr);
			}
			++index_usr;
			++iter__;
		}
		if (0 < index_usr) {
			//m_pcmb_rid->InsertString(index_usr, _TXTLNG(_T("清空所有记录"), _T("Clear all records")));
		}
		if (0 == flag) {
			if (m_pcmb_rid->GetCurSel() < 0) {
				m_pcmb_rid->SetCurText(rmtid);
			}
		}
	}
	if (m_vec_rmtidlst.size() != vec_rmtidlst.size()) {
		m_vec_rmtidlst = vec_rmtidlst;
		if (NULL != m_plst_dvc2) {
			m_plst_dvc2->DeleteAllItems();
			for (int i = 0; i < m_vec_rmtidlst.size(); i++) {
				m_plst_dvc2->InsertItem(i, m_vec_rmtidlst[i].strrmk, m_vec_rmtidlst[i].platform);
			}
			if (0 < m_vec_rmtidlst.size()) {
				if (1 == m_curtab && !m_plab_dvc->IsWindowVisible()) {
					m_plab_dvc->ShowWindow(SW_SHOW);
					m_plst_dvc2->ShowWindow(SW_SHOW);
					m_plab_dvc2->MoveWindow(200,250+10,200,20);
					m_plst_dvc->MoveWindow(200,280,190,180);
				}
			}
		}
	}
}
void CKdDeskTCDlg::w_rmtidlst()
{
	CString srid, srpwd, strtmp;
	this->m_pcmb_rid->GetCurText(srid);
	srid = this->convert_cldskid(srid, FALSE);

	/*BOOL isnewid = TRUE;
	tstring str_rid_tmp = srid.GetBuffer();
	vector<tstring>::iterator iter__ = m_vec_rmtidlst.begin();
	while (iter__ != m_vec_rmtidlst.end()) {
		if (str_rid_tmp == *iter__) {
			if (iter__ == m_vec_rmtidlst.begin()) {
			isnewid = FALSE;
			} else {
			m_vec_rmtidlst.erase(iter__);
			}
			break;
		}
		++iter__;
	}
	if (TRUE == isnewid) {
		int counter = 0;
		iter__ = m_vec_rmtidlst.begin();
		while (iter__ != m_vec_rmtidlst.end()) {
			str_rid_tmp += _T("|");
			str_rid_tmp += *iter__;
			++counter;
			if (16 == counter) {
				break;
			}
			++iter__;
		}
		
		strtmp = str_rid_tmp.c_str();
		strtmp.TrimRight('|');
		theApp.WriteProfileString(_T("KdDeskTC"), _T("ridlst"), strtmp);
	}*/
	theApp.WriteProfileString(_T("KdDeskTC"), _T("rid"), srid);
}
void CKdDeskTCDlg::show_tab_rmtctr()
{
	TCHAR buftmp[256];
	CString strtmp, strtmp2;
	if (NULL == m_plab_curdvc) {
		m_plab_curdvc = new CStatic();
		m_plab_curdvc->Create(_T("当前设备"), WS_CHILD | WS_VISIBLE, CRect(220,100,320,160), this, IDC_LAB_CURDVC);
		CFont* fntptr26 = CreateFontEx(26, FW_BOLD);
		m_plab_curdvc->SetFont(fntptr26);

		m_plab_lid = new CStatic();
		m_plab_lid->Create(_T("设备编码"), WS_CHILD | WS_VISIBLE, CRect(220,150,320,170), this, IDC_LAB_LID);
		CFont* fntptr20 = CreateFontEx(20, FW_NORMAL);
		m_plab_lid->SetFont(fntptr20);

		m_plab_lpwd = new CStatic();
		m_plab_lpwd->Create(_T("验证码"), WS_CHILD | WS_VISIBLE, CRect(500,150,600,170), this, IDC_LAB_LPWD);
		m_plab_lpwd->SetFont(fntptr20);

		m_pedt_lid = new CEditEx2();
		m_pedt_lid->Create(WS_CHILD | WS_VISIBLE, CRect(220,178,390,215), this, IDC_EDT_LID);
		CFont* fntptr31 = CreateFontEx(31, FW_NORMAL);
		m_pedt_lid->SetFont(fntptr31);
		m_pedt_lid->SetMargin(CRect(2, 3, 3, 2));
		m_pedt_lid->SetShowType(SHOW_TYPE_NORMAL);
		m_pedt_lid->m_hidedge = 1;
		m_pedt_lid->m_hidecrt = 1;
		m_pedt_lid->m_cstbkclr = 1;
		m_pedt_lid->m_CstBkClr = RGB(255, 255, 255);
		m_pedt_lid->m_csttxtclr = 1;
		m_pedt_lid->m_CstTxtClr = RGB(0, 112, 249);
		m_pedt_lid->lpfn_onlbtndblclk_edt = onlbtndblclk_edt;
		//this->get_id_pwd(strtmp, strtmp2);
		//this->SetDlgItemText(IDC_EDT_LID, convert_cldskid(strtmp, TRUE));//_T("405 001 854"));
		m_pedt_lid->SetText(_T("--- --- ---"));
		m_pedt_lpwd = new CEditEx2();
		m_pedt_lpwd->Create(WS_CHILD | WS_VISIBLE, CRect(500,178,630,215), this, IDC_EDT_LPWD);
		m_pedt_lpwd->SetText(_T("------"));
		m_pedt_lpwd->SetFont(fntptr31);
		m_pedt_lpwd->SetMargin(CRect(2, 3, 3, 2));
		m_pedt_lpwd->SetShowType(SHOW_TYPE_NORMAL);
		m_pedt_lpwd->m_mode = 2;
		m_pedt_lpwd->m_clseye = g_clseye;
		m_pedt_lpwd->m_hidedge = 1;
		m_pedt_lpwd->m_hidecrt = 1;
		m_pedt_lpwd->m_crEdgeOuterNormal = RGB(0, 112, 249);
		m_pedt_lpwd->m_crEdgeOuterHighLight = RGB(0, 112, 249);
		m_pedt_lpwd->m_crEdgeInner = RGB(255, 255, 255);
		m_pedt_lpwd->m_cstbkclr = 1;
		m_pedt_lpwd->m_CstBkClr = RGB(255, 255, 255);
		m_pedt_lpwd->m_csttxtclr = 1;
		m_pedt_lpwd->m_CstTxtClr = RGB(0, 0, 0);
		m_pedt_lpwd->lpfn_onlbtndblclk_edt = onlbtndblclk_edt;
		m_pedt_lpwd->SetLimitText(6);
		//this->SetDlgItemText(IDC_EDT_LPWD, _T("123456"));
		//m_pedt_lpwd->SetFocus();
		GetDlgItem(IDC_EDT_LPWD)->GetWindowRect(&m_rct_edtlpwd);
		ScreenToClient(&m_rct_edtlpwd);

		m_pbtn_copy = new CButtonEx2();
		m_pbtn_copy->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(395,188,412,202), this, IDC_BTN_COPY);
		m_pbtn_copy->LoadImages(
			_T("SkinPic/copy_nor.png"),
			_T("SkinPic/copy_hot.png"),
			_T("SkinPic/copy_pre.png"),
			_T("SkinPic/copy_nor.png"),
			_T("SkinPic/copy_nor.png"));

		m_pbtn_vline = new CButtonEx2();
		m_pbtn_vline->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(445,165,450,209), this, IDC_BTN_VLINE);
		m_pbtn_vline->LoadImages(
			_T("SkinPic/vline.png"),
			_T("SkinPic/vline.png"),
			_T("SkinPic/vline.png"),
			_T("SkinPic/vline.png"),
			_T("SkinPic/vline.png"));

		m_pbtn_eye = new CButtonEx2();
		m_pbtn_eye->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(640,189,657,203), this, IDC_BTN_EYE);
		if (1 == g_clseye) {
		m_pbtn_eye->LoadImages(
			_T("SkinPic/clseye_nor.png"),
			_T("SkinPic/clseye_hot.png"),
			_T("SkinPic/clseye_pre.png"),
			_T("SkinPic/clseye_nor.png"),
			_T("SkinPic/clseye_nor.png"));
		} else {
		m_pbtn_eye->LoadImages(
			_T("SkinPic/opneye_nor.png"),
			_T("SkinPic/opneye_hot.png"),
			_T("SkinPic/opneye_pre.png"),
			_T("SkinPic/opneye_nor.png"),
			_T("SkinPic/opneye_nor.png"));
		}
		GetDlgItem(IDC_BTN_EYE)->GetWindowRect(&m_rct_btneye);
		ScreenToClient(&m_rct_btneye);
		m_pbtn_updpwd = new CButtonEx2();
		m_pbtn_updpwd->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(672,189,685,203), this, IDC_BTN_UPDPWD);
		m_pbtn_updpwd->LoadImages(
			_T("SkinPic/updpwd_nor.png"),
			_T("SkinPic/updpwd_hot.png"),
			_T("SkinPic/updpwd_pre.png"),
			_T("SkinPic/updpwd_nor.png"),
			_T("SkinPic/updpwd_nor.png"));

		m_plab_rid = new CStatic();
		m_plab_rid->Create(_T("远程协助设备"), WS_CHILD | WS_VISIBLE, CRect(220,250,360,300), this, IDC_LAB_RID);
		m_plab_rid->SetFont(fntptr26);

		m_pcmb_rid = new CComboBoxDropDown();
		m_pcmb_rid->m_nButtonWidth = 34;
		m_pcmb_rid->m_nEditOffsetTop = 10;
		m_pcmb_rid->m_nEditOffsetRight = 2 + m_pcmb_rid->m_nButtonWidth + m_pcmb_rid->m_nButtonOffsetRight;
		m_pcmb_rid->Create(TZ_COMBOBOXDROPDOWN, _T(""), WS_CHILD | WS_VISIBLE, CRect(220,290,560,334), this, IDC_CMB_RID);
		m_pcmb_rid->SetFontEx(22, FW_NORMAL);
		m_pcmb_rid->m_cListCtrl.m_crFrame = RGB(0, 112, 249);
		m_pcmb_rid->m_cListCtrl.m_crBackColorHovered = RGB(0, 112, 249);
		m_pcmb_rid->m_cListCtrl.m_crTextColorNormal = RGB(63, 69, 73);
		m_pcmb_rid->m_crEdgeInner = RGB(255, 255, 255);
		m_pcmb_rid->m_crEdgeOuter = RGB(225, 225, 225);
		m_pcmb_rid->m_crEdgeOuterHighLight = RGB(0, 112, 249);
		m_pcmb_rid->LoadImages(
			_T("SkinPic/cmb_nml.png"),
			_T("SkinPic/cmb_hvr.png"),
			_T("SkinPic/cmb_prs.png"));
		m_pcmb_rid->m_cEdit.m_cldskmode = TRUE;
		m_pcmb_rid->m_cEdit.m_crText = RGB(63, 69, 73);
		m_pcmb_rid->m_cEdit.lpfn_onlbtndblclk_edt = onlbtndblclk_edt_r;
		m_pcmb_rid->m_cListCtrl.m_mode = 1;
		m_pcmb_rid->m_lpfn_deleteitem = DelItm_Rid;
		m_pcmb_rid->SetTipText(_T("请输入远程设备编码"));

		m_pbtn_conr = new CButtonEx2();
		m_pbtn_conr->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(565,290,685,334), this, IDC_BTN_CONR);
		m_pbtn_conr->LoadImages(
			_T("SkinPic/conr_nor.png"),
			_T("SkinPic/conr_hot.png"),
			_T("SkinPic/conr_pre.png"),
			_T("SkinPic/conr_nor.png"),
			_T("SkinPic/conr_nor.png"));

		m_pbtn_ready = new CButtonEx2();
		m_pbtn_ready->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(220,346,232,360), this, IDC_BTN_READY);
		m_pbtn_ready->LoadImages(
			_T("SkinPic/ready.png"),
			_T("SkinPic/ready.png"),
			_T("SkinPic/ready.png"),
			_T("SkinPic/ready.png"),
			_T("SkinPic/ready.png"));

		m_plab_ready = new CStatic();
		m_plab_ready->Create(_T("已准备好加密连接"), WS_CHILD | WS_VISIBLE, CRect(237,344,537,362), this, IDC_LAB_READY);
		CFont* fntptr18 = CreateFontEx(18, FW_NORMAL);
		m_plab_ready->SetFont(fntptr18);

		CloseHandle(CreateThread(NULL, 0, threadproc_refresh, NULL, 0, NULL));
		CloseHandle(CreateThread(NULL, 0, threadproc_refresh2, NULL, 0, NULL));
	} else {
		m_plab_curdvc->ShowWindow(SW_SHOW);
		m_plab_lid->ShowWindow(SW_SHOW);
		m_plab_lpwd->ShowWindow(SW_SHOW);
		m_pedt_lid->ShowWindow(SW_SHOW);
		m_pedt_lpwd->ShowWindow(SW_SHOW);
		m_pbtn_copy->ShowWindow(SW_SHOW);
		m_pbtn_vline->ShowWindow(SW_SHOW);
		m_pbtn_eye->ShowWindow(SW_SHOW);
		m_pbtn_updpwd->ShowWindow(SW_SHOW);
		m_plab_rid->ShowWindow(SW_SHOW);
		m_pcmb_rid->ShowWindow(SW_SHOW);
		m_pbtn_conr->ShowWindow(SW_SHOW);
		m_pbtn_ready->ShowWindow(SW_SHOW);
		m_plab_ready->ShowWindow(SW_SHOW);
	}
}
void CKdDeskTCDlg::hide_tab_rmtctr()
{
	if (NULL != m_plab_curdvc) {
		m_plab_curdvc->ShowWindow(SW_HIDE);
		m_plab_lid->ShowWindow(SW_HIDE);
		m_plab_lpwd->ShowWindow(SW_HIDE);
		m_pedt_lid->ShowWindow(SW_HIDE);
		m_pedt_lpwd->ShowWindow(SW_HIDE);
		m_pbtn_copy->ShowWindow(SW_HIDE);
		m_pbtn_vline->ShowWindow(SW_HIDE);
		m_pbtn_eye->ShowWindow(SW_HIDE);
		m_pbtn_updpwd->ShowWindow(SW_HIDE);
		m_plab_rid->ShowWindow(SW_HIDE);
		m_pcmb_rid->ShowWindow(SW_HIDE);
		m_pbtn_conr->ShowWindow(SW_HIDE);
		m_pbtn_ready->ShowWindow(SW_HIDE);
		m_plab_ready->ShowWindow(SW_HIDE);
	}
}
void CKdDeskTCDlg::OnBnClickedChkAutsta()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strtmp;
	int autsta = ((CButton*)this->GetDlgItem(IDC_CHK_AUTSTA))->GetCheck();
	/*if (1 == autsta) {
		setv_lm_dword(_T("SYSTEM\\CurrentControlSet\\services\\KdDeskTC"), _T("Start"), 2);
	} else {
		setv_lm_dword(_T("SYSTEM\\CurrentControlSet\\services\\KdDeskTC"), _T("Start"), 3);
	}*/
	if (1 == autsta) {
		CDlgLogin dlglogin;
		dlglogin.m_mode = 1;
		INT_PTR nResponse = dlglogin.DoModal();
		if (nResponse == IDOK) {
		} else if (nResponse == IDCANCEL) {
			((CButton*)this->GetDlgItem(IDC_CHK_AUTSTA))->SetCheck(FALSE);
			return;
		}
	}
	theApp.WriteProfileInt(_T("KdDeskTC"), _T("autsta"), autsta);
	if (1 == autsta) {
		if (m_pwdupd != 0) {
			m_pwdupd = 0;
			strtmp.Format(_T("%d"), m_pwdupd);
			set_pwdupd(strtmp);
			m_pcmb_pwdupd->SetCurSel(m_pwdupd);
			InvalidateRect(NULL);
		}
		m_pcmb_pwdupd->EnableWindow(FALSE);

		TCHAR buf_reg[1024] = { 0 };
		_tcscpy(buf_reg, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"));
		HKEY hkey;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, buf_reg, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS) {
			_tcscpy(buf_reg, buff_workdir);
			_tcscat(buf_reg, _T("\\KdDeskTCtrl.exe"));
			RegSetValueEx(hkey, _T("kingdee.kddesktc"), 0, REG_SZ, (CONST BYTE *)buf_reg, sizeof(TCHAR)*lstrlen(buf_reg) + 1);

			// for sds.
			/*wsprintf(buf_reg, _T("%s\\SAV\\savmonitor.exe"), buff_workdir_parent);
			if (::PathFileExists(buf_reg)) {
				RegSetValueEx(hkey, NAME_SDV_AUTORUN, 0, REG_SZ, (CONST BYTE *)buf_reg, lstrlen(buf_reg) + 1);
			}*/

			RegCloseKey(hkey);
		}
		if (FALSE == ((CButton*)this->GetDlgItem(IDC_CHK_AUTLOG))->GetCheck()) {
			((CButton*)this->GetDlgItem(IDC_CHK_AUTLOG))->SetCheck(TRUE);
			this->OnBnClickedChkAutlog();
		}
	} else {
		m_pcmb_pwdupd->EnableWindow(TRUE);
		//InvalidateRect(NULL);
		::SHDeleteValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), _T("kingdee.kddesktc"));
	}
	char bufini[MAX_PATH];
	wsprintfA(bufini, "%s\\oplog.ini", t2a(buff_workdir).c_str());
	::WritePrivateProfileStringA("oplog", "lclid", t2a(m_lid.GetString()).c_str(), bufini);
	::WritePrivateProfileStringA("oplog", "lcluid", BufOutUsrId, bufini);
	::WritePrivateProfileStringA("oplog", "type", (1 == autsta ? "开启无人值守" : "关闭无人值守"), bufini);
	::WritePrivateProfileStringA("oplog", "rmtid", "1", bufini);
	::WritePrivateProfileStringA("oplog", "rmtuid", "1", bufini);
	::WritePrivateProfileStringA("oplog", "remark", "1", bufini);
	::SetEvent(event_oplog);
}
void CKdDeskTCDlg::OnBnClickedChkAutlog()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strtmp;
	int autlog = ((CButton*)this->GetDlgItem(IDC_CHK_AUTLOG))->GetCheck();
	theApp.WriteProfileInt(_T("KdDeskTC"), _T("AutoLgn"), autlog);
	if (1 == autlog) {
		//this->GetDlgItem(IDC_CHK_AUTSTA)->EnableWindow(TRUE);
		theApp.WriteProfileString(_T("KdDeskTC"), _T("Certificate"), theApp.GetProfileString(_T("KdDeskTC"), _T("Certificate2"), _T("")));
		theApp.WriteProfileString(_T("KdDeskTC"), _T("Pwd"), theApp.GetProfileString(_T("KdDeskTC"), _T("Pwd2"), _T("")));
	} else {
		//this->GetDlgItem(IDC_CHK_AUTSTA)->EnableWindow(FALSE);
		int autsta = ((CButton*)this->GetDlgItem(IDC_CHK_AUTSTA))->GetCheck();
		if (1 == autsta) {
			((CButton*)this->GetDlgItem(IDC_CHK_AUTSTA))->SetCheck(FALSE);
			OnBnClickedChkAutsta();
		}
	}
}
void CKdDeskTCDlg::show_tab_advset()
{
	CString strtmp, strtmp2;
	if (NULL == m_pchk_autsta) {
		CFont* fntptr20 = CreateFontEx(20, FW_NORMAL);
		CFont* fntptr20b = CreateFontEx(20, FW_BOLD);
		int l = 220, t = 32, ltmp, ttmp;
		m_plab_autsta = new CStatic();
		//m_plab_autsta->Create(_T("开机启动"), WS_CHILD | WS_VISIBLE, CRect(220,50,335,70), this, IDC_LAB_AUTSTA);
		m_plab_autsta->Create(_T("开机启动"), WS_CHILD | WS_VISIBLE, CRect(l,t,l+115,t+20), this, IDC_LAB_AUTSTA);
		m_plab_autsta->SetFont(fntptr20b);
		t += 20 + 5;
		m_pchk_autsta = new CCheckBoxEx();
		//m_pchk_autsta->Create(_T("开机自动启动(无人值守)"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(220,80,470,95), this, IDC_CHK_AUTSTA);
		m_pchk_autsta->Create(_T("开机自动启动(无人值守请禁用系统休眠)"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(l,t,l+350,t+15), this, IDC_CHK_AUTSTA);
		HFONT hfont20 = createfontex(20, FW_NORMAL);
		HFONT hfont20b = createfontex(20, FW_BOLD);
		m_pchk_autsta->SetTextFont(hfont20);
		m_pchk_autsta->SetTextColor(RGB(90, 90, 90));
		m_pchk_autsta->LoadImages(
			_T("SkinPic/cb_unchked.png"), 
			_T("SkinPic/cb_chked.png"),
			_T("SkinPic/cb_unchked.png"), 
			_T("SkinPic/cb_chked.png"));
		
		t += 15 + 10;
		m_plab_fletrn = new CStatic();
		//m_plab_fletrn->Create(_T("文件传输管理器"), WS_CHILD | WS_VISIBLE, CRect(220,115,335,135), this, IDC_LAB_FLETRN);
		m_plab_fletrn->Create(_T("文件传输默认主目录"), WS_CHILD | WS_VISIBLE, CRect(l,t,l+180,t+20), this, IDC_LAB_FLETRN);
		m_plab_fletrn->SetFont(fntptr20b);
		//t += 20 + 5;
		//m_plab_defdir = new CStatic();
		//m_plab_defdir->Create(_T("主目录默认打开"), WS_CHILD | WS_VISIBLE, CRect(l,t,l+115,t+20), this, IDC_LAB_DEFDIR);
		//m_plab_defdir->SetFont(fntptr20);
		ltmp = l;
		ttmp = t + 20 + 10;
		m_prad_prv = new CRadioBoxEx();
		//m_prad_prv->Create(_T("上一次退出前的位置 "), WS_CHILD | WS_VISIBLE, CRect(335,149,495,164), this, IDC_RAD_PRV);
		m_prad_prv->Create(_T("上一次退出前的位置 "), WS_CHILD | WS_VISIBLE, CRect(ltmp,ttmp,ltmp+160,ttmp+15), this, IDC_RAD_PRV);
		m_prad_prv->SetTextFont(hfont20);
		m_prad_prv->SetTextColor(RGB(90, 90, 90));
		m_prad_prv->LoadImages(
			_T("SkinPic/rb_unchked.png"), 
			_T("SkinPic/rb_chked.png"),
			_T("SkinPic/disable_unchecked.png"), 
			_T("SkinPic/disable_checked.png"));
		m_prad_prv->SetAutoAdaption();
		ltmp += 160;
		m_prad_cst = new CRadioBoxEx();
		//m_prad_cst->Create(_T("自定义位置 "), WS_CHILD | WS_VISIBLE, CRect(495,149,700,164), this, IDC_RAD_PRV);
		m_prad_cst->Create(_T("自定义位置 "), WS_CHILD | WS_VISIBLE, CRect(ltmp,ttmp,ltmp+205,ttmp+15), this, IDC_RAD_PRV);
		m_prad_cst->SetTextFont(hfont20);
		m_prad_cst->SetTextColor(RGB(90, 90, 90));
		m_prad_cst->LoadImages(
			_T("SkinPic/rb_unchked.png"), 
			_T("SkinPic/rb_chked.png"),
			_T("SkinPic/disable_unchecked.png"), 
			_T("SkinPic/disable_checked.png"));
		m_prad_cst->SetAutoAdaption();
		t = ttmp + 15 + 10;
		m_pedt_defdir = new CEditEx2();
		//m_pedt_defdir->Create(WS_CHILD | WS_VISIBLE, CRect(220,175,520,207), this, IDC_EDT_DEFDIR);
		m_pedt_defdir->Create(WS_CHILD | WS_VISIBLE, CRect(l,t,l+300,t+32), this, IDC_EDT_DEFDIR);
		m_pedt_defdir->SetFont(fntptr20);
		m_pedt_defdir->SetMargin(CRect(6, 6, 3, 3));
		m_pedt_defdir->SetShowType(SHOW_TYPE_NORMAL);
		m_pedt_defdir->m_crEdgeOuterNormal = RGB(225, 225, 225);
		m_pedt_defdir->m_crEdgeOuterHighLight = RGB(225, 225, 225);
		m_pedt_defdir->m_crEdgeInner = RGB(255, 255, 255);
		m_pedt_defdir->m_csttxtclr = 1;
		m_pedt_defdir->m_CstTxtClr = RGB(38, 39, 41);
		m_pedt_defdir->SetReadOnly(TRUE);
		//this->SetDlgItemText(IDC_EDT_DEFDIR, _T("C:\\Users\\Administrator\\Desktop"));
		ltmp = l + 300 + 5;
		m_pbtn_chgdir = new CButtonEx2();
		//m_pbtn_chgdir->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(525,175,599,207), this, IDC_BTN_CHGDIR);
		m_pbtn_chgdir->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(ltmp,t,ltmp+74,t+32), this, IDC_BTN_CHGDIR);
		m_pbtn_chgdir->LoadImages(
			_T("SkinPic/chgdir_nor.png"),
			_T("SkinPic/chgdir_hot.png"),
			_T("SkinPic/chgdir_pre.png"),
			_T("SkinPic/chgdir_nor.png"),
			_T("SkinPic/chgdir_nor.png"));
		ltmp += 74 + 10;
		ttmp = t + 6 - 1;
		m_pbtn_opndir = new CButtonEx2();
		//m_pbtn_opndir->Create(_T("打开"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(609,181,643,201), this, IDC_BTN_OPNDIR);
		m_pbtn_opndir->Create(_T("打开"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(ltmp,ttmp,ltmp+34,ttmp+20), this, IDC_BTN_OPNDIR);
		m_pbtn_opndir->LoadImages(
			_T("SkinPic/lnkbtn_opndir.png"),
			_T("SkinPic/lnkbtn_opndir.png"),
			_T("SkinPic/lnkbtn_opndir.png"),
			_T("SkinPic/lnkbtn_opndir.png"),
			_T("SkinPic/lnkbtn_opndir.png"));
		//HFONT hfont = createfontex(20, FW_NORMAL);
		m_pbtn_opndir->SetTextFont(hfont20);
		m_pbtn_opndir->m_crTextColor = RGB(54, 129, 255);
		m_pbtn_opndir->m_crTextColor_hvr = RGB(102, 171, 255);
		m_pbtn_opndir->m_crTextColor_pre = RGB(54, 129, 255);

		//this->get_cstdir(m_cstdir);
		if (m_cstdir.IsEmpty()) {
			m_prad_prv->SetCheck(TRUE);
			m_pedt_defdir->ShowWindow(SW_HIDE);
			m_pbtn_chgdir->ShowWindow(SW_HIDE);
			m_pbtn_opndir->ShowWindow(SW_HIDE);
			t += 10;
			//t += 32 + 20;
		} else {
			m_prad_cst->SetCheck(TRUE);
			this->SetDlgItemText(IDC_EDT_DEFDIR, m_cstdir);
			t += 32 + 10;
		}
		
		m_plab_pwdupd = new CStatic();
		//m_plab_pwdupd->Create(_T("临时密码更新"), WS_CHILD | WS_VISIBLE, CRect(220,223,335,243), this, IDC_LAB_PWDUPD);
		m_plab_pwdupd->Create(_T("验证码策略"), WS_CHILD | WS_VISIBLE, CRect(l,t,l+145,t+20), this, IDC_LAB_PWDUPD);
		m_plab_pwdupd->SetFont(fntptr20b);
		t += 20 + 5;
		m_pcmb_pwdupd = new CComboBoxDropList();
		m_pcmb_pwdupd->m_nButtonWidth = 18;
		m_pcmb_pwdupd->m_nEditOffsetTop = 1;
		m_pcmb_pwdupd->m_nEditOffsetRight = 2 + m_pcmb_pwdupd->m_nButtonWidth + m_pcmb_pwdupd->m_nButtonOffsetRight;
		//m_pcmb_pwdupd->Create(TZ_COMBOBOXDROPLIST, _T(""), WS_CHILD | WS_VISIBLE, CRect(220,253,398,285), this, IDC_CMB_PWDUPD);
		m_pcmb_pwdupd->Create(TZ_COMBOBOXDROPLIST, _T(""), WS_CHILD | WS_VISIBLE, CRect(l,t,l+178,t+32), this, IDC_CMB_PWDUPD);
		m_pcmb_pwdupd->SetFontEx(20, FW_NORMAL);
		m_pcmb_pwdupd->m_cListCtrl.m_crFrame = RGB(229, 230, 235);
		m_pcmb_pwdupd->m_cListCtrl.m_crBackColorHovered = RGB(0, 112, 249);//RGB(180, 180, 180);//RGB(95, 95, 95);//RGB(245, 247, 255);
		m_pcmb_pwdupd->m_crEdgeInner = RGB(255, 255, 255);
		m_pcmb_pwdupd->m_crEdgeOuter = RGB(225, 225, 225);
		m_pcmb_pwdupd->m_crEdgeOuterHighLight = RGB(225, 225, 225);
		m_pcmb_pwdupd->m_crText = RGB(95, 95, 95);
		m_pcmb_pwdupd->m_cListCtrl.m_crTextColorNormal = RGB(95, 95, 95);
		m_pcmb_pwdupd->LoadImages(
			_T("SkinPic/cmb_lst.png"),
			_T("SkinPic/cmb_lst.png"),
			_T("SkinPic/cmb_lst.png"));
		m_pcmb_pwdupd->DeleteAllString();
		m_pcmb_pwdupd->InsertString(0, _T("手动"));
		m_pcmb_pwdupd->InsertString(1, _T("每次远控后"));
		m_pcmb_pwdupd->InsertString(2, _T("每10分钟"));
		m_pcmb_pwdupd->InsertString(3, _T("每30分钟"));
		m_pcmb_pwdupd->InsertString(4, _T("每1小时"));
		m_pcmb_pwdupd->InsertString(5, _T("每3小时"));
		m_pcmb_pwdupd->InsertString(6, _T("每4小时"));
		m_pcmb_pwdupd->InsertString(7, _T("每天"));
		m_pcmb_pwdupd->SetCurSel(m_pwdupd);
		t += 32 + 10;
		m_plab_idle = new CStatic();
		m_plab_idle->Create(_T("空闲时间策略"), WS_CHILD | WS_VISIBLE, CRect(l,t,l+145,t+20), this, IDC_LAB_IDLE);
		m_plab_idle->SetFont(fntptr20b);
		t += 20 + 5;
		m_pcmb_idle = new CComboBoxDropList();
		m_pcmb_idle->m_nButtonWidth = 18;
		m_pcmb_idle->m_nEditOffsetTop = 1;
		m_pcmb_idle->m_nEditOffsetRight = 2 + m_pcmb_idle->m_nButtonWidth + m_pcmb_idle->m_nButtonOffsetRight;
		m_pcmb_idle->Create(TZ_COMBOBOXDROPLIST, _T(""), WS_CHILD | WS_VISIBLE, CRect(l,t,l+178,t+32), this, IDC_CMB_IDLE);
		m_pcmb_idle->SetFontEx(20, FW_NORMAL);
		m_pcmb_idle->m_cListCtrl.m_crFrame = RGB(229, 230, 235);
		m_pcmb_idle->m_cListCtrl.m_crBackColorHovered = RGB(0, 112, 249);//RGB(180, 180, 180);//RGB(95, 95, 95);//RGB(245, 247, 255);
		m_pcmb_idle->m_crEdgeInner = RGB(255, 255, 255);
		m_pcmb_idle->m_crEdgeOuter = RGB(225, 225, 225);
		m_pcmb_idle->m_crEdgeOuterHighLight = RGB(225, 225, 225);
		m_pcmb_idle->m_crText = RGB(95, 95, 95);
		m_pcmb_idle->m_cListCtrl.m_crTextColorNormal = RGB(95, 95, 95);
		m_pcmb_idle->LoadImages(
			_T("SkinPic/cmb_lst.png"),
			_T("SkinPic/cmb_lst.png"),
			_T("SkinPic/cmb_lst.png"));
		m_pcmb_idle->DeleteAllString();
		m_pcmb_idle->InsertString(0, _T("10分钟"));
		m_pcmb_idle->InsertString(1, _T("30分钟"));
		m_pcmb_idle->InsertString(2, _T("1小时"));
		m_pcmb_idle->InsertString(3, _T("3小时"));
		m_pcmb_idle->SetCurSel(m_idle);
		t += 32 + 10;
		m_plab_clswnd = new CStatic();
		//m_plab_clswnd->Create(_T("关闭主窗口"), WS_CHILD | WS_VISIBLE, CRect(220,295,335,315), this, IDC_LAB_CLSWND);
		m_plab_clswnd->Create(_T("关闭主窗口"), WS_CHILD | WS_VISIBLE, CRect(l,t,l+115,t+20), this, IDC_LAB_CLSWND);
		m_plab_clswnd->SetFont(fntptr20b);
		t += 20 + 5;
		m_pchk_totray = new CCheckBoxEx();
		//m_pchk_totray->Create(_T("最小化到托盘"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(220,325,470,340), this, IDC_CHK_TOTRAY);
		m_pchk_totray->Create(_T("最小化到托盘"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(l,t,l+250,t+15), this, IDC_CHK_TOTRAY);
		m_pchk_totray->SetTextFont(hfont20);
		m_pchk_totray->SetTextColor(RGB(90, 90, 90));
		m_pchk_totray->LoadImages(
			_T("SkinPic/cb_unchked.png"), 
			_T("SkinPic/cb_chked.png"),
			_T("SkinPic/disable_unchecked.png"), 
			_T("SkinPic/disable_checked.png"));

		t += 32;
		m_plab_logset = new CStatic();
		m_plab_logset->Create(_T("登录设置"), WS_CHILD | WS_VISIBLE, CRect(l,t,l+115,t+20), this, IDC_LAB_LOGSET);
		m_plab_logset->SetFont(fntptr20b);
		t += 20 + 5;
		m_pchk_autlog = new CCheckBoxEx();
		m_pchk_autlog->Create(_T("自动登录"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(l,t,l+250,t+15), this, IDC_CHK_AUTLOG);
		m_pchk_autlog->SetTextFont(hfont20);
		m_pchk_autlog->SetTextColor(RGB(90, 90, 90));
		m_pchk_autlog->LoadImages(
			_T("SkinPic/cb_unchked.png"), 
			_T("SkinPic/cb_chked.png"),
			_T("SkinPic/disable_unchecked.png"), 
			_T("SkinPic/disable_checked.png"));

		t += 15 + 20;
		m_plab_about = new CStatic();
		//m_plab_about->Create(_T("关于"), WS_CHILD | WS_VISIBLE, CRect(220,350,335,370), this, IDC_LAB_ABOUT);
		m_plab_about->Create(_T("关于"), WS_CHILD | WS_VISIBLE, CRect(l,t,l+115,t+20), this, IDC_LAB_ABOUT);
		m_plab_about->SetFont(fntptr20b);
		t += 20 + 5;
		m_plab_curver = new CStatic();
		strtmp.Format(_T("当前版本 V%s(2312211633)"), CURVER);
		//m_plab_curver->Create(_T("当前版本 V1.1"), WS_CHILD | WS_VISIBLE, CRect(220,380,335,400), this, IDC_LAB_CURVER);
		m_plab_curver->Create(strtmp, WS_CHILD | WS_VISIBLE, CRect(l,t,l+250,t+20), this, IDC_LAB_CURVER);
		m_plab_curver->SetFont(fntptr20);
		t += 20 + 10;
		m_pbtn_updn = new CButtonEx2();
		m_pbtn_updn->m_h_align = DT_LEFT;
		m_pbtn_updn->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(l,t,l+140,t+24), this, IDC_BTN_UPDN);
		m_pbtn_updn->LoadImages(
			_T("SkinPic/lnk2_nml.png"),
			_T("SkinPic/lnk2_nml.png"),
			_T("SkinPic/lnk2_nml.png"),
			_T("SkinPic/lnk2_nml.png"),
			_T("SkinPic/lnk2_nml.png"));
		m_pbtn_updn->SetTextFont(hfont20);
		m_pbtn_updn->m_crTextColor = RGB(54, 129, 255);
		m_pbtn_updn->m_crTextColor_hvr = RGB(102, 171, 255);
		m_pbtn_updn->m_crTextColor_pre = RGB(54, 129, 255);
		//t += 24 + 10;
		t += 10;
		/*m_pbtn_uninst = new CButtonEx2();
		m_pbtn_uninst->m_h_align = DT_LEFT;
		m_pbtn_uninst->Create(_T("一键卸载"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(l,t,l+140,t+24), this, IDC_BTN_UNINST);
		m_pbtn_uninst->LoadImages(
			_T("SkinPic/lnk2_nml.png"),
			_T("SkinPic/lnk2_nml.png"),
			_T("SkinPic/lnk2_nml.png"),
			_T("SkinPic/lnk2_nml.png"),
			_T("SkinPic/lnk2_nml.png"));
		m_pbtn_uninst->SetTextFont(hfont20b);
		m_pbtn_uninst->m_crTextColor = RGB(255, 90, 90);
		m_pbtn_uninst->m_crTextColor_hvr = RGB(255, 125, 125);
		m_pbtn_uninst->m_crTextColor_pre = RGB(255, 90, 90);*/
	} else {
		m_pchk_autsta->ShowWindow(SW_SHOW);
		//m_plab_defdir->ShowWindow(SW_SHOW);
		m_prad_prv->ShowWindow(SW_SHOW);
		m_prad_cst->ShowWindow(SW_SHOW);
		if (m_cstdir.IsEmpty()) {
		m_pedt_defdir->ShowWindow(SW_HIDE);
		m_pbtn_chgdir->ShowWindow(SW_HIDE);
		m_pbtn_opndir->ShowWindow(SW_HIDE);
		} else {
		m_pedt_defdir->ShowWindow(SW_SHOW);
		m_pbtn_chgdir->ShowWindow(SW_SHOW);
		m_pbtn_opndir->ShowWindow(SW_SHOW);
		}
		m_plab_pwdupd->ShowWindow(SW_SHOW);
		m_pcmb_pwdupd->ShowWindow(SW_SHOW);
		m_plab_idle->ShowWindow(SW_SHOW);
		m_pcmb_idle->ShowWindow(SW_SHOW);
		m_plab_autsta->ShowWindow(SW_SHOW);
		m_plab_fletrn->ShowWindow(SW_SHOW);
		m_plab_clswnd->ShowWindow(SW_SHOW);
		m_pchk_totray->ShowWindow(SW_SHOW);
		m_plab_logset->ShowWindow(SW_SHOW);
		m_pchk_autlog->ShowWindow(SW_SHOW);
		m_plab_about->ShowWindow(SW_SHOW);
		m_plab_curver->ShowWindow(SW_SHOW);
		m_pbtn_updn->ShowWindow(SW_SHOW);
		//m_pbtn_uninst->ShowWindow(SW_SHOW);
	}
	int isautostart = IsAutoStart();
	if (1 == isautostart) {
		if (0 == theApp.GetProfileInt(_T("KdDeskTC"), _T("AutoLgn"), 0)) {
			theApp.WriteProfileInt(_T("KdDeskTC"), _T("autsta"), 0);
			isautostart = 0;
			::SHDeleteValue(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), _T("kingdee.kddesktc"));
		}
	}
	((CButton*)this->GetDlgItem(IDC_CHK_AUTSTA))->SetCheck(isautostart);
	m_pcmb_pwdupd->EnableWindow(!isautostart);
	/*DWORD dwType;
	DWORD cbData = sizeof(DWORD);
	DWORD start = 0;
	SHGetValue(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\services\\KdDeskTC"), _T("Start"), &dwType, &start, &cbData);
	if (2 == start) {
		((CButton*)this->GetDlgItem(IDC_CHK_AUTSTA))->SetCheck(TRUE);
	} else {
		((CButton*)this->GetDlgItem(IDC_CHK_AUTSTA))->SetCheck(FALSE);
	}*/
	((CButton*)this->GetDlgItem(IDC_CHK_TOTRAY))->SetCheck(theApp.GetProfileInt(_T("KdDeskTC"), _T("totray"), 0));
	((CButton*)this->GetDlgItem(IDC_CHK_AUTLOG))->SetCheck(theApp.GetProfileInt(_T("KdDeskTC"), _T("AutoLgn"), 0));
	/*if (!m_updurl.IsEmpty()) {
		strtmp.Format(_T("发现新版本V%s"), m_updurl.Right(9).Left(5));// KdDeskTC-2.0.3.exe
		m_pbtn_updn->SetWindowText(strtmp);
	}
	if (0 == g_in_chkupd && m_updurl.IsEmpty()) {
		CloseHandle(CreateThread(NULL, 0, threadproc_chkupd, NULL, 0, NULL));
	}*/
}
void CKdDeskTCDlg::hide_tab_advset()
{
	if (NULL != m_pchk_autsta) {
		m_pchk_autsta->ShowWindow(SW_HIDE);
		//m_plab_defdir->ShowWindow(SW_HIDE);
		m_prad_prv->ShowWindow(SW_HIDE);
		m_prad_cst->ShowWindow(SW_HIDE);
		m_pedt_defdir->ShowWindow(SW_HIDE);
		m_pbtn_chgdir->ShowWindow(SW_HIDE);
		m_pbtn_opndir->ShowWindow(SW_HIDE);
		m_plab_pwdupd->ShowWindow(SW_HIDE);
		m_pcmb_pwdupd->ShowWindow(SW_HIDE);
		m_plab_idle->ShowWindow(SW_HIDE);
		m_pcmb_idle->ShowWindow(SW_HIDE);
		m_plab_autsta->ShowWindow(SW_HIDE);
		m_plab_fletrn->ShowWindow(SW_HIDE);
		m_plab_clswnd->ShowWindow(SW_HIDE);
		m_pchk_totray->ShowWindow(SW_HIDE);
		m_plab_logset->ShowWindow(SW_HIDE);
		m_pchk_autlog->ShowWindow(SW_HIDE);
		m_plab_about->ShowWindow(SW_HIDE);
		m_plab_curver->ShowWindow(SW_HIDE);
		m_pbtn_updn->ShowWindow(SW_HIDE);
		//m_pbtn_uninst->ShowWindow(SW_HIDE);
	}
}
void CKdDeskTCDlg::show_tab_dvclst()
{
	char buftmp[256];
	if (NULL == m_plst_dvc) {
		CFont* fntptr20 = CreateFontEx(20, FW_NORMAL);
		CFont* fntptr20b = CreateFontEx(20, FW_BOLD);
		UINT utmp = 0;
		if (0 != m_vec_rmtidlst.size()) {
			utmp = WS_VISIBLE;
		}
		m_plab_dvc = new CStatic();
		m_plab_dvc->Create(_T("历史记录"), WS_CHILD | utmp, CRect(200,50,200+200,50+20), this, IDC_LAB_DVCNAME);
		m_plab_dvc->SetFont(fntptr20b);

		m_plst_dvc2 = new CSkinListCtrl();
		m_plst_dvc2->Create(WS_CHILD | utmp, CRect(200,70,200+190,70+180), this, IDC_LST_DVC2);
		m_plst_dvc2->Init();
		m_plst_dvc2->g_MyClrFg = RGB(68, 68, 68);
		m_plst_dvc2->g_MyClrBgHi = RGB(0, 112, 249);//RGB(200, 200, 200);
		LVCOLUMN col;
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;	
		col.fmt = LVCFMT_LEFT;
		TCHAR szTmp[MAX_PATH] = { 0 };
		_tcscpy(szTmp, _T("设备名称"));
		col.pszText = szTmp;
		col.cx = 200;
		m_plst_dvc2->InsertColumn(0, &col);
		m_plst_dvc2->ModifyStyle(LVS_SMALLICON | LVS_ICON | LVS_REPORT, LVS_LIST | LVS_SINGLESEL);
		m_plst_dvc2->SetExtendedStyle(LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
		m_plst_dvc2->SetImageList(&m_imglst_icon, LVSIL_SMALL);//LVSIL_NORMAL);// 设置大图标控制
		for (int i = 0; i < m_vec_rmtidlst.size(); i++) {
			m_plst_dvc2->InsertItem(i, m_vec_rmtidlst[i].strrmk, m_vec_rmtidlst[i].platform);
		}

		m_plab_dvc2 = new CStatic();
		if (0 != m_vec_rmtidlst.size()) {
		m_plab_dvc2->Create(_T("我的设备"), WS_CHILD | WS_VISIBLE, CRect(200,250+10,200+200,250+10+20), this, IDC_LAB_DVCNAME);
		} else {
			m_plab_dvc2->Create(_T("我的设备"), WS_CHILD | WS_VISIBLE, CRect(200,50,200+200,50+20), this, IDC_LAB_DVCNAME);
		}
		m_plab_dvc2->SetFont(fntptr20b);

		m_plst_dvc = new CSkinListCtrl();
		if (0 != m_vec_rmtidlst.size()) {
		m_plst_dvc->Create(WS_CHILD | WS_VISIBLE, CRect(200,280,200+190,280+180), this, IDC_LST_DVC);
		} else {
			m_plst_dvc->Create(WS_CHILD | WS_VISIBLE, CRect(200,70,200+190,70+180), this, IDC_LST_DVC);
		}
		m_plst_dvc->Init();
		m_plst_dvc->g_MyClrFg = RGB(68, 68, 68);
		m_plst_dvc->g_MyClrBgHi = RGB(0, 112, 249);//RGB(200, 200, 200);
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;	
		col.fmt = LVCFMT_LEFT;
		_tcscpy(szTmp, _T(""));
		col.pszText = szTmp;
		col.cx = 200;
		m_plst_dvc->InsertColumn(0, &col);
		//m_plst_dvc->ModifyStyle(LVS_ICON | LVS_SMALLICON | LVS_LIST, LVS_REPORT);
		//m_plst_dvc->ModifyStyle(LVS_SMALLICON | LVS_LIST | LVS_REPORT, LVS_ICON);
		m_plst_dvc->ModifyStyle(LVS_SMALLICON | LVS_ICON | LVS_REPORT, LVS_LIST | LVS_SINGLESEL);
		//m_plst_dvc->ModifyStyle(LVS_LIST | LVS_ICON | LVS_REPORT, LVS_SMALLICON);
		m_plst_dvc->SetExtendedStyle(LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
		m_plst_dvc->SetImageList(&m_imglst_icon, LVSIL_SMALL);//LVSIL_NORMAL);// 设置大图标控制
		CHeaderCtrl* pHeader = m_plst_dvc->GetHeaderCtrl();
		if (NULL != pHeader) {
        pHeader->ModifyStyle(HDS_BUTTONS, 0);
		}
		for (int i = 0; i < g_vec_dvc.size(); i++) {
			m_plst_dvc->InsertItem(i, g_vec_dvc[i].strrmk, g_vec_dvc[i].platform);
		}

		m_pbtn_vline3 = new CButtonEx2();
		m_pbtn_vline3->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(420,30,420+1,30+442), this, IDC_BTN_VLINE3);
		m_pbtn_vline3->LoadImages(
			_T("SkinPic/vline3.png"),
			_T("SkinPic/vline3.png"),
			_T("SkinPic/vline3.png"),
			_T("SkinPic/vline3.png"),
			_T("SkinPic/vline3.png"));
		m_pbtn_vline3->EnableWindow(FALSE);

		m_plab_dvcname = new CStatic();
		m_plab_dvcname->Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(460,50,460+250,50+20), this, IDC_LAB_DVCNAME);
		m_plab_dvcname->SetFont(fntptr20b);

		m_plab_dvccode = new CStatic();
		m_plab_dvccode->Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(460,70+20,460+200,70+20+20), this, IDC_LAB_DVCCODE);
		m_plab_dvccode->SetFont(fntptr20);

		m_plab_dvcname->GetWindowRect(&m_rct_dvcname);
		ScreenToClient(&m_rct_dvcname);
		m_plab_dvccode->GetWindowRect(&m_rct_dvccode);
		ScreenToClient(&m_rct_dvccode);

		m_pbtn_rmtcon = new CButtonEx2();
		m_pbtn_rmtcon->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(460,140,460+48,140+50), this, IDC_BTN_RMTCON);
		m_pbtn_rmtcon->LoadImages(
			_T("SkinPic/rmtcon_nor.png"),
			_T("SkinPic/rmtcon_hot.png"),
			_T("SkinPic/rmtcon_nor.png"),
			_T("SkinPic/rmtcon_nor.png"),
			_T("SkinPic/rmtcon_nor.png"));

		m_pbtn_rmtfle = new CButtonEx2();
		m_pbtn_rmtfle->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(460+48+30,140,460+48+30+48,140+50), this, IDC_BTN_RMTFLE);
		m_pbtn_rmtfle->LoadImages(
			_T("SkinPic/rmtfle_nor.png"),
			_T("SkinPic/rmtfle_hot.png"),
			_T("SkinPic/rmtfle_nor.png"),
			_T("SkinPic/rmtfle_nor.png"),
			_T("SkinPic/rmtfle_nor.png"));
		m_pbtn_rmtcon->ShowWindow(SW_HIDE);
		m_pbtn_rmtfle->ShowWindow(SW_HIDE);
	} else {
		m_plab_dvc2->ShowWindow(SW_SHOW);
		m_plst_dvc->ShowWindow(SW_SHOW);
		m_plst_dvc->m_SkinHorizontalScrollbar.ShowWindow(SW_SHOW);
		m_plst_dvc->m_SkinVerticleScrollbar.ShowWindow(SW_SHOW);
		if (0 != m_vec_rmtidlst.size()) {
			m_plab_dvc->ShowWindow(SW_SHOW);
			m_plst_dvc2->ShowWindow(SW_SHOW);
			m_plst_dvc2->m_SkinHorizontalScrollbar.ShowWindow(SW_SHOW);
			m_plst_dvc2->m_SkinVerticleScrollbar.ShowWindow(SW_SHOW);
		}

		m_pbtn_vline3->ShowWindow(SW_SHOW);
		int selitem = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
		if (-1 != selitem) {
			m_plab_dvcname->ShowWindow(SW_SHOW);
			m_plab_dvccode->ShowWindow(SW_SHOW);
			if (-1 == g_vec_dvc[selitem].strrmk.Find(_T("(本设备)"))) {
			m_pbtn_rmtcon->ShowWindow(SW_SHOW);
			m_pbtn_rmtfle->ShowWindow(SW_SHOW);
			} else {
			m_pbtn_rmtcon->ShowWindow(SW_HIDE);
			m_pbtn_rmtfle->ShowWindow(SW_HIDE);
			}
		} else {
			selitem = m_plst_dvc2->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
			if (-1 != selitem) {
				m_plab_dvcname->ShowWindow(SW_SHOW);
				m_plab_dvccode->ShowWindow(SW_SHOW);
				if (-1 == this->m_vec_rmtidlst[selitem].strrmk.Find(_T("(本设备)"))) {
				m_pbtn_rmtcon->ShowWindow(SW_SHOW);
				m_pbtn_rmtfle->ShowWindow(SW_SHOW);
				} else {
				m_pbtn_rmtcon->ShowWindow(SW_HIDE);
				m_pbtn_rmtfle->ShowWindow(SW_HIDE);
				}
			}
		}
	}
}
void CKdDeskTCDlg::hide_tab_dvclst()
{
	if (NULL != m_plst_dvc) {
		m_plab_dvc->ShowWindow(SW_HIDE);
		m_plab_dvc2->ShowWindow(SW_HIDE);
		m_plst_dvc->ShowWindow(SW_HIDE);
		m_plst_dvc->m_SkinHorizontalScrollbar.ShowWindow(SW_HIDE);
		m_plst_dvc->m_SkinVerticleScrollbar.ShowWindow(SW_HIDE);
		m_plst_dvc2->ShowWindow(SW_HIDE);
		m_plst_dvc2->m_SkinHorizontalScrollbar.ShowWindow(SW_HIDE);
		m_plst_dvc2->m_SkinVerticleScrollbar.ShowWindow(SW_HIDE);
		//delete m_plst_dvc;
		//m_plst_dvc = NULL;
		m_pbtn_vline3->ShowWindow(SW_HIDE);
		m_plab_dvcname->ShowWindow(SW_HIDE);
		m_plab_dvccode->ShowWindow(SW_HIDE);
		m_pbtn_rmtcon->ShowWindow(SW_HIDE);
		m_pbtn_rmtfle->ShowWindow(SW_HIDE);
	}
}
void CKdDeskTCDlg::OnEndtrackSkinList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	//LogMsg(_T("OnEndtrackSkinList."));
	m_plst_dvc->m_SkinHorizontalScrollbar.UpdateThumbPosition();

	*pResult = 0;
}
void CKdDeskTCDlg::OnBnClickedBtnHlp()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, _T("open"), _T("https://kisdoc.kingdee.com/web/#/1452/2832"), _T(""), NULL, SW_SHOWNORMAL);
}
void CKdDeskTCDlg::popup_menu_ex1() {
	CString strtmp;
	POINT pt;
	if (NULL == m_pdlg_menu1) {
	this->m_pdlg_menu1 = new CDlgMenu2();
	this->m_pdlg_menu1->m_flag = 1;
	this->m_pdlg_menu1->Create(IDD_DLG_MENU, this);
	}
	this->m_pdlg_menu1->RefreshMenu();
	RECT rct, rctdsk, rctmp;
	this->m_pdlg_menu1->GetWindowRect(&rct);
	int w = rct.right - rct.left;
	int h = rct.bottom - rct.top;
	::GetWindowRect(::GetDesktopWindow(), &rctdsk);

	GetCursorPos(&pt);
	pt.y -= h;
	set_foreground_wnd(m_pdlg_trayex->m_hWnd);

	if (pt.x + w > m_rcMonitor.right) {
		pt.x = m_rcMonitor.right - w;
	}
	::SetWindowPos(m_pdlg_menu1->m_hWnd, HWND_TOPMOST, pt.x, pt.y, 0, 0, SWP_NOSIZE);
	m_pdlg_menu1->ShowWindow(SW_SHOW);
}
void CKdDeskTCDlg::popup_menu_ex2() {
	CString strtmp;
	POINT pt;
	if (NULL == m_pdlg_menu2) {
	this->m_pdlg_menu2 = new CDlgMenu2();
	this->m_pdlg_menu2->m_flag = 2;
	this->m_pdlg_menu2->Create(IDD_DLG_MENU, this);
	}
	this->m_pdlg_menu2->RefreshMenu();
	RECT rct, rctdsk, rctmp;
	this->m_pdlg_menu2->GetWindowRect(&rct);
	int w = rct.right - rct.left;
	int h = rct.bottom - rct.top;
	::GetWindowRect(::GetDesktopWindow(), &rctdsk);

	GetDlgItem(IDC_BTN_UPDPWD)->GetWindowRect(&rctmp);
	pt.x = rctmp.left;
	pt.y = rctmp.bottom;
	
	/*if (pt.x + w > rctdsk.right) {
		pt.x = rctdsk.right - w;
	}*/
	if (pt.x + w > m_rcMonitor.right) {
		pt.x = m_rcMonitor.right - w;
	}
	::SetWindowPos(m_pdlg_menu2->m_hWnd, HWND_TOPMOST, pt.x, pt.y, 0, 0, SWP_NOSIZE);
	this->m_pdlg_menu2->ShowWindow(SW_SHOW);
}
void CKdDeskTCDlg::popup_menu_ex3() {
	CString strtmp;
	POINT pt;
	if (NULL == m_pdlg_menu3) {
	this->m_pdlg_menu3 = new CDlgMenu2();
	this->m_pdlg_menu3->m_flag = 3;
	this->m_pdlg_menu3->Create(IDD_DLG_MENU, this);
	}
	this->m_pdlg_menu3->RefreshMenu();
	RECT rct, rctdsk, rctmp;
	this->m_pdlg_menu3->GetWindowRect(&rct);
	int w = rct.right - rct.left;
	int h = rct.bottom - rct.top;
	::GetWindowRect(::GetDesktopWindow(), &rctdsk);

	GetCursorPos(&pt);
	//pt.y -= h;

	if (pt.x + w > m_rcMonitor.right) {
		pt.x = m_rcMonitor.right - w;
	}
	::SetWindowPos(m_pdlg_menu3->m_hWnd, HWND_TOPMOST, pt.x, pt.y, 0, 0, SWP_NOSIZE);
	m_pdlg_menu3->ShowWindow(SW_SHOW);
}
BOOL CKdDeskTCDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (pMsg->message) {
		case WM_LBUTTONDOWN: {
			// 经测试发现，单击控件也能触发WM_LBUTTONDOWN消息。
			//LogMsg(_T("CCloudesktopDlg->PreTranslateMessage, WM_LBUTTONDOWN."));
			if (0 == m_curtab) {
			POINT pt = pMsg->pt;
			::ScreenToClient(m_hWnd, &pt);
			if (!PtInRect(&m_rct_edtlpwd, pt)) {
				edtlpwd(0);
				if (::GetFocus() == m_pedt_lpwd->m_hWnd) {
					m_pbtn_conr->SetFocus();
				}
			}
			}
			//LogMsg(_T("WM_LBUTTONDOWN, hwnd:%u, wParam:%u, x:%d, y:%d, time:%u, pt[x:%d, y:%d]."), (UINT)pMsg->hwnd, pMsg->wParam, ((int)(short)LOWORD(pMsg->lParam)), ((int)(short)HIWORD(pMsg->lParam)), pMsg->time, pMsg->pt.x, pMsg->pt.y);
			//LogMsg(_T("WM_LBUTTONDOWN, %d."), ::GetKeyState(VK_LBUTTON));
			//m_tc_dn = ::GetTickCount();
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
int CKdDeskTCDlg::GetCurMntrInfo() {
	int ret__ = 0;
	// If the window is currently minimized, MonitorFromWindow uses the rectangle of the window before it was minimized.
	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO mi = { 0 };
	mi.cbSize = sizeof(MONITORINFO);
	BOOL ret_gmi = GetMonitorInfo(hMonitor, &mi);
	if (ret_gmi) {
		LogMsg(_T("CCloudesktopDlg->GetCurMntrInfo, GetMonitorInfo(%p) success."), hMonitor);
	} else {
		::GetWindowRect(::GetDesktopWindow(), &mi.rcMonitor);
		SystemParametersInfo(SPI_GETWORKAREA, 0, &mi.rcWork, 0);
		LogMsg(_T("CCloudesktopDlg->GetCurMntrInfo, GetMonitorInfo(%p) fail."), hMonitor);
	}
	if (0 != memcmp(&m_rcMonitor, &mi.rcMonitor, sizeof(RECT)) || 0 != memcmp(&m_rcWork, &mi.rcWork, sizeof(RECT))) {
		// rcMonitor:[x:0, y:0, w:2560, h:1440] -> [x:-1920, y:0, w:1920, h:1080], rcWork:[x:0, y:0, w:2560, h:1410] -> [x:-1920, y:0, w:1920, h:1050].
		// rcMonitor:[x:0, y:0, w:1920, h:1080] -> [x:0, y:0, w:2560, h:1440], rcWork:[x:0, y:0, w:1920, h:1080] -> [x:0, y:0, w:2560, h:1440]
		LogMsg(_T("CCloudesktopDlg->GetCurMntrInfo, rcMonitor:[x:%d, y:%d, w:%d, h:%d] -> [x:%d, y:%d, w:%d, h:%d], rcWork:[x:%d, y:%d, w:%d, h:%d] -> [x:%d, y:%d, w:%d, h:%d]."), 
			m_rcMonitor.left, m_rcMonitor.top, m_rcMonitor.right-m_rcMonitor.left, m_rcMonitor.bottom-m_rcMonitor.top, 
			mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right-mi.rcMonitor.left, mi.rcMonitor.bottom-mi.rcMonitor.top, 
			m_rcWork.left, m_rcWork.top, m_rcWork.right-m_rcWork.left, m_rcWork.bottom-m_rcWork.top, 
			mi.rcWork.left, mi.rcWork.top, mi.rcWork.right-mi.rcWork.left, mi.rcWork.bottom-mi.rcWork.top
			);
		this->m_rcMonitor = mi.rcMonitor;
		this->m_rcWork = mi.rcWork;
		ret__ = 1;
	}
	DEVMODE devmode = { 0 };
	devmode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	/*if (g_clrbits != devmode.dmBitsPerPel) {
		LogMsg(_T("CCloudesktopDlg->GetCurMntrInfo, g_clrbits:%u->%u."), g_clrbits, devmode.dmBitsPerPel);
		g_clrbits = devmode.dmBitsPerPel;
	}*/
	return ret__;
}
void CKdDeskTCDlg::OnNMRclickLstDvc(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int selitem = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 == selitem) return;
	int selitem2 = m_plst_dvc2->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 != selitem2) {
	m_plst_dvc2->SetItemState(selitem2, 0,-1);
	}
	popup_menu_ex3();
}
void CKdDeskTCDlg::OnNMRclickLstDvc2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int selitem = m_plst_dvc2->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 == selitem) return;
	int selitem2 = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 != selitem2) {
	m_plst_dvc->SetItemState(selitem2, 0,-1);
	}
	popup_menu_ex3();
}
void CKdDeskTCDlg::OnNMLclickLstDvc(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CString strtmp;
	*pResult = 0;
	/*int selitem = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 == selitem) return;
	int selitem2 = m_plst_dvc2->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 != selitem2) {
	m_plst_dvc2->SetItemState(selitem2, 0,-1);
	}
	m_plab_dvcname->SetWindowText(g_vec_dvc[selitem].strrmk);
	strtmp.Format(_T("设备代码 %s"), g_vec_dvc[selitem].strid);
	m_plab_dvccode->SetWindowText(strtmp);
	m_plab_dvcname->ShowWindow(SW_SHOW);
	m_plab_dvccode->ShowWindow(SW_SHOW);
	if (-1 == g_vec_dvc[selitem].strrmk.Find(_T("(本设备)"))) {
	m_pbtn_rmtcon->ShowWindow(SW_SHOW);
	m_pbtn_rmtfle->ShowWindow(SW_SHOW);
	} else {
	m_pbtn_rmtcon->ShowWindow(SW_HIDE);
	m_pbtn_rmtfle->ShowWindow(SW_HIDE);
	}
	InvalidateRect(&m_rct_dvcname);
	InvalidateRect(&m_rct_dvccode);
	UpdateWindow();*/
}
void CKdDeskTCDlg::OnNMLclickLstDvc2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CString strtmp;
	*pResult = 0;
	/*int selitem = m_plst_dvc2->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 == selitem) return;
	int selitem2 = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
	if (-1 != selitem2) {
	m_plst_dvc->SetItemState(selitem2, 0,-1);
	}
	m_plab_dvcname->SetWindowText(m_vec_rmtidlst[selitem].strrmk);
	strtmp.Format(_T("设备代码 %s"), m_vec_rmtidlst[selitem].strid);
	m_plab_dvccode->SetWindowText(strtmp);
	m_plab_dvcname->ShowWindow(SW_SHOW);
	m_plab_dvccode->ShowWindow(SW_SHOW);
	if (-1 == m_vec_rmtidlst[selitem].strrmk.Find(_T("(本设备)"))) {
	m_pbtn_rmtcon->ShowWindow(SW_SHOW);
	m_pbtn_rmtfle->ShowWindow(SW_SHOW);
	} else {
	m_pbtn_rmtcon->ShowWindow(SW_HIDE);
	m_pbtn_rmtfle->ShowWindow(SW_HIDE);
	}
	InvalidateRect(&m_rct_dvcname);
	InvalidateRect(&m_rct_dvccode);
	UpdateWindow();*/
}
void CKdDeskTCDlg::OnItemchangedLstdvc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString strtmp;
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED)) {
        int selitem = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
		if (-1 == selitem) return;
		int selitem2 = m_plst_dvc2->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
		if (-1 != selitem2) {
		m_plst_dvc2->SetItemState(selitem2, 0,-1);
		}
		m_plab_dvcname->SetWindowText(g_vec_dvc[selitem].strrmk);
		strtmp.Format(_T("设备代码 %s"), g_vec_dvc[selitem].strid);
		m_plab_dvccode->SetWindowText(strtmp);
		m_plab_dvcname->ShowWindow(SW_SHOW);
		m_plab_dvccode->ShowWindow(SW_SHOW);
		if (-1 == g_vec_dvc[selitem].strrmk.Find(_T("(本设备)"))) {
		m_pbtn_rmtcon->ShowWindow(SW_SHOW);
		m_pbtn_rmtfle->ShowWindow(SW_SHOW);
		} else {
		m_pbtn_rmtcon->ShowWindow(SW_HIDE);
		m_pbtn_rmtfle->ShowWindow(SW_HIDE);
		}
		InvalidateRect(&m_rct_dvcname);
		InvalidateRect(&m_rct_dvccode);
		UpdateWindow();
    }
}
void CKdDeskTCDlg::OnItemchangedLstdvc2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString strtmp;
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED)) {
        int selitem = m_plst_dvc2->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
		if (-1 == selitem) return;
		int selitem2 = m_plst_dvc->GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);
		if (-1 != selitem2) {
		m_plst_dvc->SetItemState(selitem2, 0,-1);
		}
		m_plab_dvcname->SetWindowText(m_vec_rmtidlst[selitem].strrmk);
		strtmp.Format(_T("设备代码 %s"), m_vec_rmtidlst[selitem].strid);
		m_plab_dvccode->SetWindowText(strtmp);
		m_plab_dvcname->ShowWindow(SW_SHOW);
		m_plab_dvccode->ShowWindow(SW_SHOW);
		if (-1 == m_vec_rmtidlst[selitem].strrmk.Find(_T("(本设备)"))) {
		m_pbtn_rmtcon->ShowWindow(SW_SHOW);
		m_pbtn_rmtfle->ShowWindow(SW_SHOW);
		} else {
		m_pbtn_rmtcon->ShowWindow(SW_HIDE);
		m_pbtn_rmtfle->ShowWindow(SW_HIDE);
		}
		InvalidateRect(&m_rct_dvcname);
		InvalidateRect(&m_rct_dvccode);
		UpdateWindow();
    }
}
int CKdDeskTCDlg::edtlpwd(int flag)
{
	int ret__ = 0;
	CString strtmp;
	if (0 == flag) { // disable editable.
		if (0 == m_pedt_lpwd->m_hidedge) {
			strtmp = m_pedt_lpwd->GetText();
			if (4 > strtmp.GetLength()) {
				CDlgMsgBox2 dlgmsgbox(0, _T("提示"), _T("请输入4-8位字符！"));
				dlgmsgbox.DoModal();
				ret__ = 1;
			} else {
				m_pedt_lpwd->m_hidedge = 1;
				m_pedt_lpwd->m_hidecrt = 1;
				m_pedt_lpwd->m_cstbkclr = 1;
				m_pedt_lpwd->SetReadOnly(TRUE);
				InvalidateRect(&m_rct_edtlpwd);
				UpdateWindow();
				this->set_pwd(strtmp);
				m_pedt_lpwd->SetText(strtmp);
				m_pedt_lpwd->SetFocus();
				ret__ = 2;
			}
		}
	} else { // enable editable.
		if (1 == m_pedt_lpwd->m_hidedge) {
			m_pedt_lpwd->m_hidedge = 0;
			m_pedt_lpwd->m_hidecrt = 0;
			m_pedt_lpwd->m_cstbkclr = 0;
			m_pedt_lpwd->SetReadOnly(FALSE);
			InvalidateRect(&m_rct_edtlpwd);
			UpdateWindow();
		}
	}
	return ret__;
}
void CKdDeskTCDlg::OnBnClickedChkTotray()
{
	// TODO: 在此添加控件通知处理程序代码
	int totray = ((CButton*)this->GetDlgItem(IDC_CHK_TOTRAY))->GetCheck();
	theApp.WriteProfileInt(_T("KdDeskTC"), _T("totray"), totray);
}

void CKdDeskTCDlg::OnBnClickedRadPrv()
{
	CString strtmp;
	int dirprv = ((CButton*)this->GetDlgItem(IDC_RAD_PRV))->GetCheck(), hoff;
	LogMsg(_T("OnBnClickedRadPrv, dirprv:%d."), dirprv);
	if (1 == dirprv) {
		if (m_cstdir.IsEmpty()) return;
		this->m_cstdir = _T("");
		this->set_cstdir(m_cstdir);
		m_pedt_defdir->ShowWindow(SW_HIDE);
		m_pbtn_chgdir->ShowWindow(SW_HIDE);
		m_pbtn_opndir->ShowWindow(SW_HIDE);
		hoff = -32;
	} else {
		if (!m_cstdir.IsEmpty()) return;
		this->m_cstdir = buff_workdir;
		this->SetDlgItemText(IDC_EDT_DEFDIR, m_cstdir);
		this->set_cstdir(m_cstdir);
		m_pedt_defdir->ShowWindow(SW_SHOW);
		m_pbtn_chgdir->ShowWindow(SW_SHOW);
		m_pbtn_opndir->ShowWindow(SW_SHOW);
		hoff = 32;
	}
	RECT rctmp;
	m_plab_pwdupd->GetClientRect(&rctmp);
	m_plab_pwdupd->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_plab_pwdupd->MoveWindow(&rctmp);

	m_pcmb_pwdupd->GetClientRect(&rctmp);
	m_pcmb_pwdupd->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_pcmb_pwdupd->MoveWindow(&rctmp);

	m_plab_idle->GetClientRect(&rctmp);
	m_plab_idle->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_plab_idle->MoveWindow(&rctmp);

	m_pcmb_idle->GetClientRect(&rctmp);
	m_pcmb_idle->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_pcmb_idle->MoveWindow(&rctmp);

	m_plab_clswnd->GetClientRect(&rctmp);
	m_plab_clswnd->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_plab_clswnd->MoveWindow(&rctmp);

	m_pchk_totray->GetClientRect(&rctmp);
	m_pchk_totray->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_pchk_totray->MoveWindow(&rctmp);

	m_plab_logset->GetClientRect(&rctmp);
	m_plab_logset->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_plab_logset->MoveWindow(&rctmp);

	m_pchk_autlog->GetClientRect(&rctmp);
	m_pchk_autlog->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_pchk_autlog->MoveWindow(&rctmp);

	m_plab_about->GetClientRect(&rctmp);
	m_plab_about->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_plab_about->MoveWindow(&rctmp);

	m_plab_curver->GetClientRect(&rctmp);
	m_plab_curver->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_plab_curver->MoveWindow(&rctmp);

	m_pbtn_updn->GetClientRect(&rctmp);
	m_pbtn_updn->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_pbtn_updn->MoveWindow(&rctmp);

	/*m_pbtn_uninst->GetClientRect(&rctmp);
	m_pbtn_uninst->ClientToScreen(&rctmp);
	this->ScreenToClient(&rctmp);
	rctmp.top += hoff;
	rctmp.bottom += hoff;
	m_pbtn_uninst->MoveWindow(&rctmp);*/

	InvalidateRect(NULL);
}
void CKdDeskTCDlg::OnBnClickedRadCst()
{
	int dircst = ((CButton*)this->GetDlgItem(IDC_RAD_CST))->GetCheck();
	LogMsg(_T("OnBnClickedRadCst, dircst:%d."), dircst);
}
void CKdDeskTCDlg::delitm_rid(int indx)
{
	CString strtmp, sridlst, srid_def, srid_del;
	int indx__ = 0;
	vector<DVCITEM2>::iterator iter__ = m_vec_rmtidlst.begin();
	while (iter__ != m_vec_rmtidlst.end()) {
		if (indx__ == indx) {
			srid_del = iter__->strid;
			m_vec_rmtidlst.erase(iter__);
			break;
		}
		++indx__;
		++iter__;
	}
	if (srid_del.IsEmpty()) {
		LogErr(_T("CKdDeskTCDlg->delitm_rid, fail, %d."), indx);
		return;
	}
	/*iter__ = m_vec_rmtidlst.begin();
	while (iter__ != m_vec_rmtidlst.end()) {
		sridlst += iter__->c_str();
		sridlst += _T("|");
		++iter__;
	}
	sridlst.TrimRight('|');
	theApp.WriteProfileString(_T("KdDeskTC"), _T("ridlst"), sridlst);*/
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --del-rid \"%s\""), buff_workdir, srid_del);
	exec_nowait(strtmp.GetBuffer(), buff_workdir);

	srid_def = theApp.GetProfileString(_T("KdDeskTC"), _T("rid"), _T(""));
	if (srid_def == srid_del) {
		theApp.WriteProfileString(_T("KdDeskTC"), _T("rid"), NULL);
	}
	//this->r_rmtidlst();
	m_pcmb_rid->DeleteString(indx);
	if (NULL != m_plst_dvc2) {
		m_plst_dvc2->DeleteItem(indx);
	}
}
void CKdDeskTCDlg::OnCbnSelchangeCmbrid()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strid, strtmp, strtmp2;
	int ntmp;
	//LogMsg(_T("CCloudesktopDlg::OnCbnSelchangeCmbrid, %d, %d, %d."), m_cmb_rid.GetCurSel(), m_cmb_rid.m_nCurSel, m_vec_rmtidlst.size());
	//if (m_cmb_rid.GetCurSel() == m_vec_rmtidlst.size()) {
	if (m_pcmb_rid->m_nCurSel == m_vec_rmtidlst.size()) {
		theApp.WriteProfileString(_T("KdDeskTC"), _T("ridlst"), NULL);
		theApp.WriteProfileString(_T("KdDeskTC"), _T("rid"), NULL);
		this->m_pcmb_rid->DeleteAllString();
		this->m_pcmb_rid->SetCurText(_T(""));
		return;
	}
}
void CKdDeskTCDlg::OnBnClickedBtnConr()
{
	// TODO: 在此添加控件通知处理程序代码
	CString srid, srpwd, strtmp, strtmp2;
	int ntmp;
	INT_PTR nResponse;
	if (1 == m_in_conr) return;
	this->m_pcmb_rid->GetCurText(srid);
	srid = this->convert_cldskid(srid, FALSE);
	
	if (srid.IsEmpty()) {
		CDlgMsgBox2 dlgmsgbox(0, _TXTLNG(_T("提示"), _T("Prompt")), _T("请输入远程连接ID！"));
		dlgmsgbox.DoModal();
		return;
	}
	if (srid == m_lid) {
		CDlgMsgBox2 dlgmsgbox(0, _TXTLNG(_T("提示"), _T("Prompt")), _T("设备不能连接自己！"));
		dlgmsgbox.DoModal();
		return;
	}
	
	STARTUPINFO si = { 0 }; 
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;

	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --connect \"%s\""), buff_workdir, srid);

	CString workdir;
	workdir.Format(_T("%s"), buff_workdir);
	if (CreateProcess(NULL, (LPTSTR)strtmp.GetBuffer(), NULL, NULL, FALSE, 0, NULL, workdir.GetBuffer(), &si, &pi)) {
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		m_in_conr = 1;
		//this->SetTimer(TIMER_CONRTO_EVENT, TIMER_CONRTO_ELAPSE, 0);
		::PostMessage(this->m_hWnd, WM_CLT_COMMAND, 20, 1);
	} else {
		LogErr(_T("CreateProcess fail, %s."), strtmp);
	}
}
int CKdDeskTCDlg::set_pwd(CString strpwd)
{
	CString strtmp;
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --password \"%s\""), buff_workdir, strpwd);
	exec_nowait(strtmp.GetBuffer(), buff_workdir);
	//strtmp.Format(_T("%s\\result.ini"), buff_workdir);
	//GetPrivateProfileString(_T("result"), _T("pwd"), _T(""), buftmp, 128, strtmp);
	//strpwd = buftmp;
	return 0;
}
int CKdDeskTCDlg::upd_pwd(CString& strpwd)
{
	CString strtmp;
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --upd-pwd"), buff_workdir);
	exec_wait_ex(strtmp.GetBuffer(), buff_workdir);
	strtmp.Format(_T("%s\\result2.ini"), buff_workdir);
	GetPrivateProfileString(_T("result"), _T("pwd"), _T(""), buftmp, 128, strtmp);
	strpwd = buftmp;
	//::SetEvent(event_refresh);
	return 0;
}
int CKdDeskTCDlg::set_cstdir(CString strcstdir)
{
	CString strtmp;
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --set-cstdir \"%s\""), buff_workdir, strcstdir);
	exec_nowait(strtmp.GetBuffer(), buff_workdir);
	return 0;
}
int CKdDeskTCDlg::set_uid(CString struid)
{
	CString strtmp;
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --set-uid \"%s\""), buff_workdir, struid);
	exec_nowait(strtmp.GetBuffer(), buff_workdir);
	return 0;
}
int CKdDeskTCDlg::get_updurl()
{
	CString strtmp;
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --get-updurl"), buff_workdir);
	exec_wait_ex(strtmp.GetBuffer(), buff_workdir);
	::PostMessage(this->m_hWnd, WM_CLT_COMMAND, 11, 0);
	return 0;
}
int CKdDeskTCDlg::get_idlst()
{
	CString strtmp;
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --get-idlst"), buff_workdir);
	exec_wait_ex(strtmp.GetBuffer(), buff_workdir);
	return 0;
}
int CKdDeskTCDlg::rmt_cnt(CString strid)
{
	CString strtmp;
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --connect \"%s\""), buff_workdir, strid);
	exec_nowait(strtmp.GetBuffer(), buff_workdir, FALSE);
	return 0;
}
int CKdDeskTCDlg::rmt_fle(CString strid)
{
	CString strtmp;
	TCHAR buftmp[128];
	strtmp.Format(_T("\"%s\\KdDeskTC.exe\" --file-transfer \"%s\""), buff_workdir, strid);
	exec_nowait(strtmp.GetBuffer(), buff_workdir, FALSE);
	return 0;
}
int CKdDeskTCDlg::IsAutoStart()
{
	return GetPrivateProfileInt(_T("KdDeskTC"), _T("autsta"), 0, buff_inipath);
}
void CKdDeskTCDlg::OnCbnSelchangeCmbpwdupd()
{
	CString strtmp;
	int pwdupd = this->m_pcmb_pwdupd->GetCurSel();
	if (pwdupd == m_pwdupd) {
		return;
	}
	m_pwdupd = pwdupd;
	strtmp.Format(_T("%d"), pwdupd);
	set_pwdupd(strtmp);
}
void CKdDeskTCDlg::OnCbnSelchangeCmbidle()
{
	CString strtmp;
	int idle = this->m_pcmb_idle->GetCurSel();
	if (idle == m_idle) {
		return;
	}
	m_idle = idle;
	strtmp.Format(_T("%d"), idle);
	set_idle(strtmp);
}