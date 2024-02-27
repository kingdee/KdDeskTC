#pragma once


// CDlgTrayEx 对话框

class CDlgTrayEx : public CDialog
{
	DECLARE_DYNAMIC(CDlgTrayEx)

public:
	CDlgTrayEx(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTrayEx();

// 对话框数据
	enum { IDD = IDD_DLG_TRAYEX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
