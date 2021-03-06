﻿// InsertDataDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "InsertDataDlg.h"
#include "afxdialogex.h"


// CInsertDataDlg 对话框

IMPLEMENT_DYNAMIC(CInsertDataDlg, CDialog)

CInsertDataDlg::CInsertDataDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_INSERT_DATA_DIALOG, pParent)
{

}

CInsertDataDlg::~CInsertDataDlg()
{
}

void CInsertDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInsertDataDlg, CDialog)
END_MESSAGE_MAP()


// CInsertDataDlg 消息处理程序


void CInsertDataDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CString strTmp;
	GetDlgItemText(IDC_INSERT_ADDR_EDIT, strTmp);
	m_address = wcstoul(strTmp, nullptr, 16);

	GetDlgItemText(IDC_INSERT_SIZE_EDIT, strTmp);
	m_size = _ttoi(strTmp);

	CDialog::OnOK();
}
