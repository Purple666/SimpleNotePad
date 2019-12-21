// HexView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "HexViewDlg.h"
#include "afxdialogex.h"
#include "InsertDataDlg.h"
#include "DeleteDataDlg.h"

// CHexViewDlg �Ի���

IMPLEMENT_DYNAMIC(CHexViewDlg, CDialog)

CHexViewDlg::CHexViewDlg(string& data, CodeType code, const CString& file_path, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HEX_VIEW_DIALOG, pParent), m_data(data), m_code(code), m_file_path(file_path)
{
}

CHexViewDlg::~CHexViewDlg()
{
}

void CHexViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HEX_EDIT, m_edit);
	DDX_Control(pDX, IDC_MODIFIED_LIST, m_modified_list);
	DDX_Control(pDX, IDC_STATIC_HEAD, m_static_head);
}


BEGIN_MESSAGE_MAP(CHexViewDlg, CDialog)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_ADDRESS, &CHexViewDlg::OnEnChangeEditAddress)
	ON_BN_CLICKED(IDC_SEARCH, &CHexViewDlg::OnBnClickedSearch)
	//ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, &CHexViewDlg::OnBnClickedButtonPrevious)
	//ON_BN_CLICKED(IDC_BUTTON_NEXT, &CHexViewDlg::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_MODIFY, &CHexViewDlg::OnBnClickedModify)
	ON_BN_CLICKED(IDC_REFRESH_BUTTON, &CHexViewDlg::OnBnClickedRefreshButton)
	ON_BN_CLICKED(IDC_RADIO_ANSI, &CHexViewDlg::OnBnClickedRadioAnsi)
	ON_BN_CLICKED(IDC_RADIO_UTF8, &CHexViewDlg::OnBnClickedRadioUtf8)
	ON_BN_CLICKED(IDC_RADIO_UTF16, &CHexViewDlg::OnBnClickedRadioUtf16)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CHexViewDlg::OnDeltaposSpin1)
	ON_BN_CLICKED(IDC_RADIO_BYTE, &CHexViewDlg::OnBnClickedRadioByte)
	ON_BN_CLICKED(IDC_RADIO_WORD, &CHexViewDlg::OnBnClickedRadioWord)
	ON_BN_CLICKED(IDC_RADIO_DWORD, &CHexViewDlg::OnBnClickedRadioDword)
	ON_CBN_SELCHANGE(IDC_SIZE_UNIT_COMBO, &CHexViewDlg::OnCbnSelchangeSizeUnitCombo)
	ON_BN_CLICKED(IDC_MODIFY_SIZE, &CHexViewDlg::OnBnClickedModifySize)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_INSERT_DATA_BUTTON, &CHexViewDlg::OnBnClickedInsertDataButton)
	ON_BN_CLICKED(IDC_DELETE_DATA_BUTTON, &CHexViewDlg::OnBnClickedDeleteDataButton)
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CHexViewDlg ��Ϣ�������


void CHexViewDlg::ShowHexData(bool ini)
{
	CWaitCursor wait_cursor;
	string a_line_str;
	wstring a_line;
	CString temp;
	size_t max{ ((m_data.size() - 1) / 16 + 1) * 16 };

	for (size_t i{ 0 }; i <= max; i++)
	{
		if (i % 16 == 0)
		{
			//��ʾDump������ַ���
			if (i > 0)
			{
				a_line_str = m_data.substr(i - 16, 16);
				if (m_code == CodeType::UTF16)	//���������UTF16
				{
					for (int i{}; i < a_line_str.size() - 1; i++)
					{
						if (i % 2 == 0 && a_line_str[i + 1] == 0)	//���������ֽ���0��������һ��ASCII�ַ�
						{
							if (a_line_str[i] >= 0 && a_line_str[i]<=32)	//������ַ��ǿ����ַ���0����ת���ɿո�
							//a_line_str[i + 1] = 32;
							a_line_str[i] = 32;
						}
					}
				}
				else
				{
					for (auto& ch : a_line_str)		//�������ַ�ȫ��ת��Ϊ�ո�
						if (ch >= 0 && ch < 32) ch = 32;
				}
				a_line = CCommon::StrToUnicode(a_line_str, m_code);
				m_str += _T("   ");
				m_str += a_line.c_str();
				m_str += _T("\r\n");
			}
			//��ʾ��ַ
			if (i < max)
			{
				temp.Format(_T("%.8x  "), i);
				temp.MakeUpper();
				m_str += temp;
			}
		}
		//��ʾ����
		if (i < max)
		{
			if (i < m_data.size())
			{
				temp.Format(_T("%.2x "), static_cast<unsigned char>(m_data[i]));
				temp.MakeUpper();
			}
			else
			{
				temp = _T("   ");
			}
			m_str += temp;
		}
	}
	//m_edit.SetWindowText(m_str);
	if (ini)
		SetTimer(1235, 50, NULL);		//�ڳ�ʼ��ʱ���ô˺�������Ҫ�ӳ�50��������ʾ����
	else
		m_edit.SetWindowText(m_str);
}

unsigned int CHexViewDlg::GetValueAndStr(unsigned int address, EditUnit edit_unit, CString& value_str)
{
	unsigned int value;
	switch (edit_unit)
	{
	case EditUnit::BYTE:
		value = (m_address < m_data.size() ? m_data[address] : 0);
		value_str.Format(_T("%.2x"), static_cast<unsigned char>(value));
		break;
	case EditUnit::WORD:
		value = (m_address < m_data.size() ? static_cast<unsigned char>(m_data[address]) : 0)
			+ (m_address + 1 < m_data.size() ? 0x100 * static_cast<unsigned char>(m_data[address + 1]) : 0);
		value_str.Format(_T("%.4x"), static_cast<unsigned short>(value));
		break;
	case EditUnit::DWORD:
		value = (m_address < m_data.size() ? static_cast<unsigned char>(m_data[address]) : 0)
			+ (m_address + 1 < m_data.size() ? 0x100 * static_cast<unsigned char>(m_data[address + 1]) : 0)
			+ (m_address + 2 < m_data.size() ? 0x10000 * static_cast<unsigned char>(m_data[address + 2]) : 0)
			+ (m_address + 3 < m_data.size() ? 0x1000000 * static_cast<unsigned char>(m_data[address + 3]) : 0);
		value_str.Format(_T("%.8x"), static_cast<unsigned long>(value));
		break;
	default:
		break;
	}
	return value;
}

void CHexViewDlg::ShowSizeInfo()
{
	CString tmp;
	switch (m_size_unit)
	{
	case SizeUnit::B:
		tmp.Format(_T("�޸��ļ���С������ǰ��%d B��"), m_data.size());
		break;
	case SizeUnit::KB:
		tmp.Format(_T("�޸��ļ���С������ǰ��%.2f KB��"), static_cast<double>(m_data.size()) / 1024.0);
		break;
	case SizeUnit::MB:
		tmp.Format(_T("�޸��ļ���С������ǰ��%.2f MB��"), static_cast<double>(m_data.size()) / 1024.0 / 1024.0);
		break;
	}
	SetDlgItemText(IDC_STATIC_FILESIZE, tmp);
}

void CHexViewDlg::SaveConfig() const
{
	CCommon::WritePrivateProfileInt(L"config", L"edit_unit", static_cast<int>(m_edit_unit), theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileInt(L"config", L"size_unit", static_cast<int>(m_size_unit), theApp.m_config_path.c_str());
}

void CHexViewDlg::LoadConfig()
{
	m_edit_unit = static_cast<EditUnit>(GetPrivateProfileInt(_T("config"), _T("edit_unit"), 0, theApp.m_config_path.c_str()));
	m_size_unit = static_cast<SizeUnit>(GetPrivateProfileInt(_T("config"), _T("size_unit"), 0, theApp.m_config_path.c_str()));
}

BOOL CHexViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

    //��ȡ��ʼʱ���ڵĴ�С
    CRect rect;
    GetWindowRect(rect);
    m_min_size.cx = rect.Width();
    m_min_size.cy = rect.Height();

	LoadConfig();
	if(!m_file_path.IsEmpty())
		SetWindowText(m_file_path + _T(" - ʮ�����Ʋ鿴"));

	ShowHexData(true);

	m_modified_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_modified_list.GetClientRect(rect);
	size_t width1 = rect.Width() * 3 / 10;		//�еĿ�ȣ��б��ȵ�3/10
	//size_t width2 = rect.Width() / 4;		//��2�еĿ�ȣ��б��ȵ�1/4
	m_modified_list.InsertColumn(0, _T("��ַ"), LVCFMT_LEFT, width1);		//�����1��
	m_modified_list.InsertColumn(1, _T("�޸�ǰ"), LVCFMT_LEFT, width1);		//�����2��
	m_modified_list.InsertColumn(2, _T("�޸ĺ�"), LVCFMT_LEFT, width1);		//�����3��

	//��������
	m_font.CreatePointFont(100, _T("������"));
	m_edit.SetFont(&m_font);
	m_static_head.SetFont(&m_font);

	//���õ�ѡ�ؼ��ĳ�ʼ״̬
	switch (m_code)
	{
	case CodeType::ANSI: ((CButton*)GetDlgItem(IDC_RADIO_ANSI))->SetCheck(TRUE); break;
	case CodeType::UTF8: case CodeType::UTF8_NO_BOM: ((CButton*)GetDlgItem(IDC_RADIO_UTF8))->SetCheck(TRUE); break;
	case CodeType::UTF16: ((CButton*)GetDlgItem(IDC_RADIO_UTF16))->SetCheck(TRUE); break;
	}

	switch (m_edit_unit)
	{
	case EditUnit::BYTE: ((CButton*)GetDlgItem(IDC_RADIO_BYTE))->SetCheck(TRUE); break;
	case EditUnit::WORD: ((CButton*)GetDlgItem(IDC_RADIO_WORD))->SetCheck(TRUE); break;
	case EditUnit::DWORD: ((CButton*)GetDlgItem(IDC_RADIO_DWORD))->SetCheck(TRUE); break;
	default: break;
	}

	//Ϊ��Ͽ������Ŀ
	((CComboBox*)GetDlgItem(IDC_SIZE_UNIT_COMBO))->AddString(_T("B"));
	((CComboBox*)GetDlgItem(IDC_SIZE_UNIT_COMBO))->AddString(_T("KB"));
	((CComboBox*)GetDlgItem(IDC_SIZE_UNIT_COMBO))->AddString(_T("MB"));
	((CComboBox*)GetDlgItem(IDC_SIZE_UNIT_COMBO))->SetCurSel(static_cast<int>(m_size_unit));

	ShowSizeInfo();

	//���øöԻ�������������ʾ
	ModifyStyleEx(0, WS_EX_APPWINDOW);

	m_modified = false;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CHexViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == 1235)
	{
		KillTimer(1235);		//��ʱ����Ӧһ�κ�ͽ�������
		m_edit.SetWindowText(m_str);	//�ӳ�һ��ʱ����ʾ
	}

	CDialog::OnTimer(nIDEvent);
}


void CHexViewDlg::OnEnChangeEditAddress()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString address;
	GetDlgItemText(IDC_EDIT_ADDRESS, address);
	m_address = wcstoul(address, NULL, 16);
	if (m_edit_unit == EditUnit::WORD)		//����༭�ĵ�λ��WORD�����ַֻ����2�ı���
	{
		m_address /= 2;
		m_address *= 2;
	}
	else if (m_edit_unit == EditUnit::DWORD)		//����༭�ĵ�λ��DWORD�����ַֻ����4�ı���
	{
		m_address /= 4;
		m_address *= 4;
	}
}


void CHexViewDlg::OnBnClickedSearch()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//������Һ�������ʾ��ַ���е�ֵ
	CString address_str;
	address_str.Format(_T("%.8x"), m_address);
	address_str.MakeUpper();
	SetDlgItemText(IDC_EDIT_ADDRESS, address_str);

	CString value_str;
	if (m_address < m_data.size())
	{
		m_value = GetValueAndStr(m_address, m_edit_unit, value_str);
		value_str.MakeUpper();

		//���Һ������������
		int first_line = m_edit.GetFirstVisibleLine();	//��ȡ�༭����������ɼ��е��к�
		int scroll_line = m_address / 16 - first_line;	//����Ҫ������������
		m_edit.LineScroll(scroll_line);
	}
	else
	{
		value_str = _T("������");
	}
	SetDlgItemText(IDC_EDIT_VALUE, value_str);
}


//void CHexViewDlg::OnBnClickedButtonPrevious()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	CString address_str;
//	GetDlgItemText(IDC_EDIT_ADDRESS, address_str);	//��ȡ���ҿ��е��ַ���
//	m_address = wcstoul(address_str, NULL, 16);
//	m_address--;		//���ҿ�ԭ���ĵ�ַ��1
//	address_str.Format(_T("%.8x"), m_address);
//	address_str.MakeUpper();
//	SetDlgItemText(IDC_EDIT_ADDRESS, address_str);
//	OnBnClickedSearch();
//}


//void CHexViewDlg::OnBnClickedButtonNext()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	CString address_str;
//	GetDlgItemText(IDC_EDIT_ADDRESS, address_str);	//��ȡ���ҿ��е��ַ���
//	m_address = wcstoul(address_str, NULL, 16);
//	m_address++;		//���ҿ�ԭ���ĵ�ַ��1
//	address_str.Format(_T("%.8x"), m_address);
//	address_str.MakeUpper();
//	SetDlgItemText(IDC_EDIT_ADDRESS, address_str);
//	OnBnClickedSearch();
//}


void CHexViewDlg::OnBnClickedModify()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString out_info;
	if (m_address < m_data.size())
	{
		Item item;
		CString value_str;
		CString old_value_str;
		item.address = m_address;
		//���ݵ�ַ��ȡԭ����ֵ
		item.old_value = GetValueAndStr(m_address, m_edit_unit, old_value_str);
		old_value_str.MakeUpper();
		//��ȡ�༭���е��޸ĵ�ֵ
		GetDlgItemText(IDC_EDIT_VALUE, value_str);
		value_str.MakeUpper();
		switch (m_edit_unit)
		{
		case EditUnit::BYTE: CCommon::ChangeStringLength(value_str, 2, _T('0')); break;
		case EditUnit::WORD: CCommon::ChangeStringLength(value_str, 4, _T('0')); break;
		case EditUnit::DWORD: CCommon::ChangeStringLength(value_str, 8, _T('0')); break;
		default: break;
		}
		item.value = wcstoul(value_str, NULL, 16);

		if (item.old_value != item.value)
		{
			//���µ�ֵд������
			//m_data[m_address] = item.value;
			switch (m_edit_unit)
			{
			case EditUnit::BYTE:
				m_data[m_address] = static_cast<unsigned int>(item.value);
				break;
			case EditUnit::WORD:
				m_data[m_address] = static_cast<unsigned int>(item.value % 0x100);
				m_data[m_address + 1] = static_cast<unsigned int>(item.value / 0x100 % 0x100);
				break;
			case EditUnit::DWORD:
				m_data[m_address] = static_cast<unsigned int>(item.value % 0x100);
				m_data[m_address + 1] = static_cast<unsigned int>(item.value / 0x100 % 0x100);
				m_data[m_address + 2] = static_cast<unsigned int>(item.value / 0x10000 % 0x100);
				m_data[m_address + 3] = static_cast<unsigned int>(item.value / 0x1000000 % 0x100);
				break;
			default: break;
			}

			m_modified_data.push_back(item);
			m_modified = true;

			//���޸������б������Ŀ
			//��ӵ�ַ
			CString str;
			str.Format(_T("%.8x"), item.address);
			str.MakeUpper();
			m_modified_list.InsertItem(m_modified_data.size() - 1, str);
			//����޸�ǰ��ֵ
			m_modified_list.SetItemText(m_modified_data.size() - 1, 1, old_value_str);
			//����޸ĺ��ֵ
			m_modified_list.SetItemText(m_modified_data.size() - 1, 2, value_str);
		}
		out_info.Format(_T("%.8x:%s->%s"), item.address, old_value_str, value_str);
		out_info.MakeUpper();
	}
	else
	{
		out_info = _T("��ַ������Χ");
	}
	SetDlgItemText(IDC_STATIC_OUT, out_info);
}


void CHexViewDlg::OnBnClickedRefreshButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int first_line = m_edit.GetFirstVisibleLine();	//ˢ��ǰ��ȡ���Ϸ��ɼ��е��к�
	m_str.Empty();
	ShowHexData();
	m_edit.LineScroll(first_line);		//ˢ�º������ˢ��ǰ��λ��
}


BOOL CHexViewDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedSearch();
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CHexViewDlg::OnBnClickedRadioAnsi()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_code != CodeType::ANSI)
	{
		m_code = CodeType::ANSI;
		OnBnClickedRefreshButton();
	}
}


void CHexViewDlg::OnBnClickedRadioUtf8()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_code != CodeType::UTF8 && m_code != CodeType::UTF8_NO_BOM)
	{
		m_code = CodeType::UTF8;
		OnBnClickedRefreshButton();
	}
}


void CHexViewDlg::OnBnClickedRadioUtf16()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_code != CodeType::UTF16)
	{
		m_code = CodeType::UTF16;
		OnBnClickedRefreshButton();
	}
}


void CHexViewDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (pNMUpDown->iDelta > 0)		//�����Spin�����¼�ͷ
	{
		//CString address_str;
		//GetDlgItemText(IDC_EDIT_ADDRESS, address_str);	//��ȡ���ҿ��е��ַ���
		//m_address = wcstoul(address_str, NULL, 16);
		switch (m_edit_unit)
		{
		case CHexViewDlg::EditUnit::BYTE: m_address--;		//���ҿ�ԭ���ĵ�ַ��1
			break;
		case CHexViewDlg::EditUnit::WORD: m_address -= 2;		//���ҿ�ԭ���ĵ�ַ��2
			break;
		case CHexViewDlg::EditUnit::DWORD: m_address -= 4;		//���ҿ�ԭ���ĵ�ַ��4
			break;
		default:
			break;
		}
		
		//address_str.Format(_T("%.8x"), m_address);
		//address_str.MakeUpper();
		//SetDlgItemText(IDC_EDIT_ADDRESS, address_str);
		OnBnClickedSearch();
	}
	else		//�����Spin�����ϼ�ͷ
	{
		//CString address_str;
		//GetDlgItemText(IDC_EDIT_ADDRESS, address_str);	//��ȡ���ҿ��е��ַ���
		//m_address = wcstoul(address_str, NULL, 16);
		switch (m_edit_unit)
		{
		case CHexViewDlg::EditUnit::BYTE: m_address++;		//���ҿ�ԭ���ĵ�ַ��1
			break;
		case CHexViewDlg::EditUnit::WORD: m_address += 2;		//���ҿ�ԭ���ĵ�ַ��2
			break;
		case CHexViewDlg::EditUnit::DWORD: m_address += 4;		//���ҿ�ԭ���ĵ�ַ��4
			break;
		default:
			break;
		}
		//address_str.Format(_T("%.8x"), m_address);
		//address_str.MakeUpper();
		//SetDlgItemText(IDC_EDIT_ADDRESS, address_str);
		OnBnClickedSearch();
	}
	*pResult = 0;
}


void CHexViewDlg::OnBnClickedRadioByte()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_edit_unit = EditUnit::BYTE;
	OnEnChangeEditAddress();
}


void CHexViewDlg::OnBnClickedRadioWord()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_edit_unit = EditUnit::WORD;
	OnEnChangeEditAddress();
}


void CHexViewDlg::OnBnClickedRadioDword()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_edit_unit = EditUnit::DWORD;
	OnEnChangeEditAddress();
}


void CHexViewDlg::OnCbnSelchangeSizeUnitCombo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_size_unit = static_cast<SizeUnit>(((CComboBox*)GetDlgItem(IDC_SIZE_UNIT_COMBO))->GetCurSel());
	ShowSizeInfo();
}


void CHexViewDlg::OnBnClickedModifySize()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�ӱ༭���ȡҪ���ĵ��ļ���С
	CString size_str;
	GetDlgItemText(IDC_EDIT_SIZE, size_str);
	//����ѡ��ĵ�λ�����ļ���С���ֽ���
	int file_size_byte;
	double file_size;
	switch (m_size_unit)
	{
	case SizeUnit::B:
		file_size_byte = _wtoi(size_str);
		file_size = file_size_byte;
		break;
	case SizeUnit::KB:
		file_size = _wtof(size_str);
		file_size_byte = static_cast<int>(file_size * 1024);
		break;
	case SizeUnit::MB:
		file_size = _wtof(size_str);
		file_size_byte = static_cast<int>(file_size * 1024 * 1024);
		break;
	default:
		break;
	}

	if (file_size_byte < 0)
	{
		MessageBox(_T("��������ȷ���ļ���С��"), NULL, MB_OK | MB_ICONWARNING);
		return;
	}
	else if (file_size_byte > MAX_FILE_SIZE)
	{
		MessageBox(_T("��С���ܳ���50MB��"), NULL, MB_OK | MB_ICONWARNING);
		return;
	}

	CString info;
	if (file_size_byte < m_data.size())
	{
		info.Format(_T("��Ҫ���ĵĴ�СС��ԭ���Ĵ�С������Ĳ��ֽ�����������ȷ��Ҫ���ļ��Ĵ�С��%u�ֽڸ���Ϊ%u�ֽ���"), m_data.size(), file_size_byte);
	}
	else
	{
		info.Format(_T("��ȷ��Ҫ���ļ��Ĵ�С��%u�ֽڸ���Ϊ%u�ֽ���"), m_data.size(), file_size_byte);
	}
	if (MessageBox(info, NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		//������ʾ�����Ϣ
		info.Format(_T("�ļ���С: %u Byte -> %u Byte"), m_data.size(), file_size_byte);
		SetDlgItemText(IDC_STATIC_OUT, info);
		//���Ĵ�С
		m_data.resize(file_size_byte);
		ShowSizeInfo();
		m_modified = true;
	}
}


void CHexViewDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	SaveConfig();
}


void CHexViewDlg::OnBnClickedInsertDataButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CInsertDataDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		unsigned int address = dlg.GetInsertAddress();
		unsigned int size = dlg.GetInsertSize();

		if (size <= 0)
		{
			MessageBox(_T("���ݵĳ��ȱ���Ϊ������"), NULL, MB_ICONWARNING | MB_OK);
			return;
		}

		CString info;
		if (address >= m_data.size())
			info.Format(_T("��ַ %08x ������Χ��Ҫ���ļ�ĩβ���� %d ���ֽڵ�������"), address, size);
		else
			info.Format(_T("ȷʵҪ�� %08x ��ַǰ���� %d ���ֽڵ�������"), address, size);
		if (MessageBox(info, NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
		{
			//������ʾ�����Ϣ
			info.Format(_T("��ַ %08x ������ %d �ֽ�"), address, size);
			SetDlgItemText(IDC_STATIC_OUT, info);
			//��������
			if (address >= m_data.size())
				m_data.resize(m_data.size() + size);
			else
				m_data.insert(address, size, 0);
			ShowSizeInfo();
			m_modified = true;
		}
	}
}


void CHexViewDlg::OnBnClickedDeleteDataButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDeleteDataDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		unsigned int address = dlg.GetDeleteAddress();
		unsigned int size = dlg.GetDeleteSize();

		if (size <= 0)
		{
			MessageBox(_T("���ݵĳ��ȱ���Ϊ������"), NULL, MB_ICONWARNING | MB_OK);
			return;
		}

		CString info;
		if (address >= m_data.size())
		{
			info.Format(_T("��ַ %08x ������Χ!"), address);
			MessageBox(info, NULL, MB_ICONWARNING | MB_OK);
			return;
		}

		info.Format(_T("ȷʵҪɾ����ַΪ %08x ��ʼ�� %d ���ֽڵ�������"), address, size);
		if (MessageBox(info, NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
		{
			//������ʾ�����Ϣ
			info.Format(_T("��ַ %08x ��ɾ�� %d �ֽ�"), address, size);
			SetDlgItemText(IDC_STATIC_OUT, info);
			//ɾ������
			m_data.erase(address, size);
			ShowSizeInfo();
			m_modified = true;
		}
	}
}


void CHexViewDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    //���ƴ�����С��С
    lpMMI->ptMinTrackSize.x = m_min_size.cx;		//������С���
    lpMMI->ptMinTrackSize.y = m_min_size.cy;		//������С�߶�

    CDialog::OnGetMinMaxInfo(lpMMI);
}
