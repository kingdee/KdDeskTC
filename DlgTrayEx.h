#pragma once


// CDlgTrayEx �Ի���

class CDlgTrayEx : public CDialog
{
	DECLARE_DYNAMIC(CDlgTrayEx)

public:
	CDlgTrayEx(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTrayEx();

// �Ի�������
	enum { IDD = IDD_DLG_TRAYEX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
