// DlgTmpex.cpp : 实现文件
//

#include "stdafx.h"
#include "KdDeskTC.h"
#include ".\dlgtmpex.h"


// CDlgTmpex 对话框

IMPLEMENT_DYNAMIC(CDlgTmpex, CDialog)
CDlgTmpex::CDlgTmpex(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTmpex::IDD, pParent)
{
}

CDlgTmpex::~CDlgTmpex()
{
}

void CDlgTmpex::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_TMPEX, m_lst_tmpex);
}


BEGIN_MESSAGE_MAP(CDlgTmpex, CDialog)
END_MESSAGE_MAP()


// CDlgTmpex 消息处理程序

BOOL CDlgTmpex::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_lst_tmpex.Init();
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;	
	col.fmt = LVCFMT_LEFT;

	TCHAR szTmp[MAX_PATH] = { 0 };
	_tcscpy(szTmp, _T(""));
	col.pszText = szTmp;
	col.cx = 250;
	this->m_lst_tmpex.InsertColumn(0, &col);
	_tcscpy(szTmp, _T(""));
	col.pszText = szTmp;
	col.cx = 250;
	this->m_lst_tmpex.InsertColumn(1, &col);
	//this->m_lst_tmpex.ModifyStyle(LVS_ICON | LVS_SMALLICON | LVS_LIST, LVS_REPORT);
	//this->m_lst_tmpex.SetExtendedStyle(LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
	for (int i = 0; i < 20; i++) {
	m_lst_tmpex.InsertItem(i, _T("xxx"), 0);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
