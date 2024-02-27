#pragma once


// CDlgTmpex 对话框

class CDlgTmpex : public CDialog
{
	DECLARE_DYNAMIC(CDlgTmpex)

public:
	CDlgTmpex(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTmpex();

// 对话框数据
	enum { IDD = IDD_DLG_TMPEX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CSkinListCtrl m_lst_tmpex;
	virtual BOOL OnInitDialog();
};
