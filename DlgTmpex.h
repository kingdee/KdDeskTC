#pragma once


// CDlgTmpex �Ի���

class CDlgTmpex : public CDialog
{
	DECLARE_DYNAMIC(CDlgTmpex)

public:
	CDlgTmpex(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTmpex();

// �Ի�������
	enum { IDD = IDD_DLG_TMPEX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CSkinListCtrl m_lst_tmpex;
	virtual BOOL OnInitDialog();
};
