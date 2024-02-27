// KdDeskTC.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号
#include "wm_private.h"
#include "logger.h"
#include "cryptoex.h"
#include <winsvc.h>
#include <tlhelp32.h>
#include <Winternl.h>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <WtsApi32.h>
#pragma comment(lib, "WtsApi32.lib")
#define TIMER_ACTWND_EVENT  10
#define TIMER_ACTWND_ELAPSE 100
#define TIMER_REDRAW_EVENT  20
#define TIMER_REDRAW_ELAPSE 100
#define TIMER_CHKUPD_EVENT  30
#define TIMER_CHKUPD_ELAPSE (2*60*60*1000)
#define CURVER _T("1.2")
CString _TXTLNG(CString strch, CString stren);
typedef BOOL (WINAPI *LPFN_WOW64DISABLEWOW64FSREDIRECTION)(PVOID *OldValue);
typedef BOOL (WINAPI *LPFN_WOW64REVERTWOW64FSREDIRECTION)(PVOID OldValue);
typedef int (*ADDDEVICE)(char* pszId, char* pszPwd, char* pszRmk, char* pszKey, char* pszOs);
typedef int (*CLTUSRLGN)(const char* pszTime, const char* pszUsr, const char* pszPwd, const char* pszDvc, int nRmbPwd, char* pszCert, char *pOutUsrId, ADDDEVICE lpfn_AddDevice, char *pOutError);
typedef int (*GETDVCLST)(const char* pszTime, const char* pszUsrId, ADDDEVICE lpfn_AddDevice, char *pOutError);
typedef int (*ADDADVERT)(int nId, char* pszImg, char* pszUrl, char* pszTxt);
typedef int (*GETADVERTLST)(const char* pszTime, const char* pszUsrId, ADDADVERT lpfn_AddAdvert, int* pOutIntvl, char *pOutError);
typedef int (*DELCLDSKID)(const char* pszTime, const char* puid, const char* pcldskid, char *pOutError);
typedef int (*SUBCLDSKID)(const char* pszTime, const char* puid, const char* pcldskid, const char* premark, const char* ppwd, int isrmkupd, const char* pszdvc, char *pOutExpi, int* pOutUsrNum, char *pOutRemark, char* pOutKey, char *pOutError);
typedef int (*SUBLOG)(const char* pszTime, char* pszDvcId, char* pszUid, char* pszType, char* pszOpTime, char* pszRmtDvcId, char* pszRmtUid, char* pszRemark, char *pOutError);
// CKdDeskTCApp:
// 有关此类的实现，请参阅 KdDeskTC.cpp
//

class CKdDeskTCApp : public CWinApp
{
public:
	CKdDeskTCApp();
	void center_dlgitm(CWnd* pWnd, int idl, int idr = 0);
	CString __encrypt(CString S);
	CString __decrypt(CString S);
	BOOL WriteRegistryString(LPCSTR lpszEntry, CStringA strValue);
	CStringA GetRegistryString(LPCSTR lpszEntry, LPCSTR lpszDefault);
	HRESULT CreateShortcut(LPCTSTR lpszFileName, LPCTSTR lpszShortcutPath, LPCTSTR lpszDesc, LPCTSTR lpszArguments, LPCTSTR lpszIconLocation, int nIconIndex);
// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
public:
	int m_runasadmin;
	int m_command;
};

extern CKdDeskTCApp theApp;
