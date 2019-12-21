
// SimpleNotePadDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "SimpleNotePadDlg.h"
#include "afxdialogex.h"
#include "HexViewDlg.h"
#include "FormatConvertDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSimpleNotePadDlg �Ի���



CSimpleNotePadDlg::CSimpleNotePadDlg(CString file_path, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SIMPLENOTEPAD_DIALOG, pParent), m_file_path(file_path)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleNotePadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
}

void CSimpleNotePadDlg::OpenFile(LPCTSTR file_path)
{
	////�����ļ�ǰѯ���û��Ƿ񱣴�
	//if (!SaveInquiry())
	//	return;

	CWaitCursor wait_cursor;

	m_edit_str.clear();
	m_modified = false;
	ifstream file{ file_path, std::ios::binary };
	if (file.fail())
	{
		CString info;
		info.Format(_T("�޷����ļ���%s����"), file_path);
		MessageBox(info, NULL, MB_OK | MB_ICONSTOP);
		m_file_path.Empty();
		return;
	}
	while (!file.eof())
	{
		m_edit_str.push_back(file.get());
		if (m_edit_str.size() > MAX_FILE_SIZE)	//���ļ���С����MAX_FILE_SIZEʱ��ֹ��
		{
			CString info;
			info.Format(_T("��%s���ļ�̫�󣬽�ֻ�����ļ�ǰ��%dMB�����ݣ�Ҫ������"), file_path, MAX_FILE_SIZE / 1024 / 1024);
			if (MessageBox(info, NULL, MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				break;
			}
			else
			{
				m_file_path.Empty();
				m_edit_str.clear();
				return;
			}
		}
	}
	m_edit_str.pop_back();

	JudgeCode();											//�жϱ�������
	m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);	//ת����Unicode
	if (m_edit_wcs.size() < m_edit_str.size() / 4)		//������Զ�ʶ��ĸ�ʽת����Unicode��Unicode�ַ����ĳ���С�ڶ��ֽ��ַ������ȵ�1/4�����ı��ı����ʽ������UTF16
	{
		m_code = CodeType::UTF16;
		m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);	//����ת����Unicode
	}
	m_edit.SetWindowText(m_edit_wcs.c_str());				//���ļ��е�������ʾ���༩������
	//m_flag = true;
	ShowStatusBar();										//����״̬��
}

bool CSimpleNotePadDlg::SaveFile(LPCTSTR file_path, CodeType code)
{
	bool char_connot_convert;
	m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, char_connot_convert, code);
	if (char_connot_convert)	//���ļ��а���Unicode�ַ�ʱ��ѯ���û��Ƿ�Ҫѡ��һ��Unicode�����ʽ�ٱ���
	{
		CString info;
		info.LoadString(IDS_STRING102);		//��string table�����ַ���
		if (MessageBox(info, NULL, MB_OKCANCEL | MB_ICONWARNING) != IDOK) return false;		//����û������ȡ����ť���򷵻�false
	}
	ofstream file{ file_path, std::ios::binary };
	if (!file.good())		//����޷������ļ����򷵻�false
		return false;
	file << m_edit_str;
	m_modified = false;
	ShowStatusBar();		//�����ˢ��״̬��
	return true;
}

void CSimpleNotePadDlg::JudgeCode()
{
	if (m_edit_str.size() >= 3 && m_edit_str[0] == -17 && m_edit_str[1] == -69 && m_edit_str[2] == -65)
		m_code = CodeType::UTF8;
	else if (m_edit_str.size() >= 2 && m_edit_str[0] == -1 && m_edit_str[1] == -2)
		m_code = CodeType::UTF16;
	else if (CCommon::IsUTF8Bytes(m_edit_str.c_str()))
		m_code = CodeType::UTF8_NO_BOM;
	else m_code = CodeType::ANSI;
}

void CSimpleNotePadDlg::ShowStatusBar()
{
	//��ʾ�����ʽ
	CString str{_T("�����ʽ��")};
	//if (!m_edit_wcs.empty())
	//{
		switch (m_code)
		{
		case CodeType::ANSI: str += _T("ANSI"); break;
		case CodeType::UTF8: str += _T("UTF8"); break;
		case CodeType::UTF8_NO_BOM: str += _T("UTF8��BOM"); break;
		case CodeType::UTF16: str += _T("UTF16"); break;
		}
	//}
	m_status_bar.SetText(str, 2, 0);

	//��ʾ�ַ���
	//if (m_edit_wcs.empty())
	//	str.Empty();
	if (m_edit_str.size() < 1024)
		str.Format(_T("��%d���ֽڣ�%d���ַ�"), m_edit_str.size(), m_edit_wcs.size());
	else
		str.Format(_T("��%d���ֽ�(%dKB)��%d���ַ�"), m_edit_str.size(), m_edit_str.size() / 1024, m_edit_wcs.size());
	m_status_bar.SetText(str, 0, 0);

	//��ʾ�Ƿ��޸�
	m_status_bar.SetText(m_modified?_T("���޸�"):_T("δ�޸�"), 1, 0);
}

void CSimpleNotePadDlg::ChangeCode()
{
	m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);
	m_edit.SetWindowText(m_edit_wcs.c_str());
	//m_flag = true;
	ShowStatusBar();
}

bool CSimpleNotePadDlg::BeforeChangeCode()
{
	return SaveInquiry(_T("ע�⣬������ı����ʽ��δ��������и��Ķ�����ʧ���Ƿ�Ҫ���棿"));
}

void CSimpleNotePadDlg::SaveConfig()
{
	//������������
	WritePrivateProfileString(_T("config"), _T("font_name"), m_font_name, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileInt(L"config", L"font_size", m_font_size, theApp.m_config_path.c_str());
	//���洰�ڴ�С
	CCommon::WritePrivateProfileInt(L"config", L"window_width", m_window_width, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileInt(L"config", L"window_hight", m_window_hight, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileInt(L"config", L"word_wrap", m_word_wrap, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileInt(L"config", L"always_on_top", m_always_on_top, theApp.m_config_path.c_str());

	CCommon::WritePrivateProfileInt(L"config", L"find_no_case", m_find_no_case, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileInt(L"config", L"find_whole_word", m_find_whole_word, theApp.m_config_path.c_str());
}

void CSimpleNotePadDlg::LoadConfig()
{
	//������������
	GetPrivateProfileString(_T("config"), _T("font_name"), _T("΢���ź�"), m_font_name.GetBuffer(32), 32, theApp.m_config_path.c_str());
	m_font_size = GetPrivateProfileInt(_T("config"), _T("font_size"), 10, theApp.m_config_path.c_str());
	//���봰�ڴ�С
	m_window_width = GetPrivateProfileInt(_T("config"), _T("window_width"), 560, theApp.m_config_path.c_str());
	m_window_hight = GetPrivateProfileInt(_T("config"), _T("window_hight"), 350, theApp.m_config_path.c_str());
	m_word_wrap = (GetPrivateProfileInt(_T("config"), _T("word_wrap"), 1, theApp.m_config_path.c_str()) != 0);
	m_always_on_top = (GetPrivateProfileInt(_T("config"), _T("always_on_top"), 0, theApp.m_config_path.c_str()) != 0);

	m_find_no_case = (GetPrivateProfileInt(_T("config"), _T("find_no_case"), 0, theApp.m_config_path.c_str()) != 0);
	m_find_whole_word = (GetPrivateProfileInt(_T("config"), _T("find_whole_word"), 0, theApp.m_config_path.c_str()) != 0);
}

bool CSimpleNotePadDlg::SaveInquiry(LPCTSTR info)
{
	if (m_modified)
	{
		CString text;
		if (info == NULL)
		{
			if (m_file_path.IsEmpty())
				text = _T("�ޱ��� �е������Ѹ��ģ��Ƿ񱣴棿");
			else
				text.Format(_T("��%s���ļ��е������Ѹ��ģ��Ƿ񱣴棿"), m_file_path);
		}
		else
		{
			text = info;
		}

		int rtn = MessageBox(text, NULL, MB_YESNOCANCEL | MB_ICONWARNING);
		switch (rtn)
		{
		case IDYES: return _OnFileSave();
		case IDNO: m_modified = false; break;
		default: return false;
		}
	}
	return true;
}

void CSimpleNotePadDlg::SetTitle()
{
	if(!m_file_path.IsEmpty())
		SetWindowText(m_file_path + _T(" - SimpleNotePad"));
	else
		SetWindowText(_T("�ޱ��� - SimpleNotePad"));
}

bool CSimpleNotePadDlg::_OnFileSave()
{
	if (m_modified)		//ֻ�����Ѹ��Ĺ�֮��ű���
	{
		//�Ѿ��򿪹�һ���ļ�ʱ��ֱ�ӱ��棬��û�д�һ���ļ��͵��������Ϊ���Ի���
		if (!m_file_path.IsEmpty())
		{
			if (SaveFile(m_file_path, m_code))
				return true;
			else
				return _OnFileSaveAs();		//�ļ��޷�����ʱ���������Ϊ���Ի���
		}
		else
		{
			return _OnFileSaveAs();
		}
	}
	return false;
}

bool CSimpleNotePadDlg::_OnFileSaveAs()
{
	//���ù�����
	const wchar_t* szFilter = _T("�ı��ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");
	//�������Ϊʱ��Ĭ���ļ���
	wstring file_name;
	if (!m_file_path.IsEmpty())
	{
		wstring file_path(m_file_path);
		size_t index = file_path.find_last_of(L'\\');
		file_name = file_path.substr(index + 1);
	}
	else
	{
		file_name = L"�ޱ���";
	}
	//���챣���ļ��Ի���
	CFileDialog fileDlg(FALSE, _T("txt"), file_name.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	//Ϊ�����Ϊ���Ի������һ�����ѡ���
	fileDlg.AddComboBox(IDC_SAVE_COMBO_BOX);
	//Ϊ���ѡ��������Ŀ
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 0, _T("��ANSI��ʽ����"));
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 1, _T("��UTF-8��ʽ����"));
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 2, _T("��UTF-8��BOM��ʽ����"));
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 3, _T("��UTF-16��ʽ����"));
	//fileDlg.SetControlLabel(IDC_SAVE_COMBO_BOX, _T("�������ͣ�"));
	//���ݵ�ǰ���õ����Ϊ��ʽΪ���ѡ�������Ĭ��ѡ�е���Ŀ
	fileDlg.SetSelectedControlItem(IDC_SAVE_COMBO_BOX, static_cast<DWORD>(m_save_code));

	////���á����Ϊ���Ի������
	//CString str{ _T("���Ϊ") };
	//switch (m_save_code)
	//{
	//case CodeType::ANSI: str += _T("ANSI"); break;
	//case CodeType::UTF8: str += _T("UTF8"); break;
	//case CodeType::UTF8_NO_BOM: str += _T("UTF8��BOM"); break;
	//case CodeType::UTF16: str += _T("UTF16"); break;
	//}
	//fileDlg.m_ofn.lpstrTitle = str.GetString();
	//��ʾ�����ļ��Ի���
	if (IDOK == fileDlg.DoModal())
	{
		DWORD selected_item;
		fileDlg.GetSelectedControlItem(IDC_SAVE_COMBO_BOX, selected_item);	//��ȡ�������ʽ����ѡ�е���Ŀ
		m_save_code = static_cast<CodeType>(selected_item);
		if (SaveFile(fileDlg.GetPathName().GetString(), m_save_code))
		{
			m_file_path = fileDlg.GetPathName();	//���Ϊ�󣬵�ǰ�ļ���Ϊ������ļ���
			SetTitle();					//���µ��ļ������ñ���
			m_code = m_save_code;		//���Ϊ��ǰ������������Ϊ���Ϊ�ı�������
			ShowStatusBar();			//ˢ��״̬��
			return true;
		}
	}
	return false;
}

void CSimpleNotePadDlg::SaveHex()
{
	ofstream file{ m_file_path, std::ios::binary };
	if (file.fail())
	{
		MessageBox(_T("����ʧ�ܣ�"), NULL, MB_ICONWARNING);
		return;
	}
	file << m_edit_str;
	m_modified = false;
	ShowStatusBar();
	MessageBox(_T("ʮ�����Ʊ༭�ĸ����ѱ���"), NULL, MB_ICONINFORMATION);
}

void CSimpleNotePadDlg::SetAlwaysOnTop()
{
    if (m_always_on_top)
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//�����ö�
    else
        SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//ȡ���ö�
}

//void CSimpleNotePadDlg::SaveAsHex()
//{
//	//���ù�����
//	const wchar_t* szFilter = _T("�ı��ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");
//	//���챣���ļ��Ի���
//	CFileDialog fileDlg(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
//	//���á����Ϊ���Ի������
//	fileDlg.m_ofn.lpstrTitle = _T("ʮ�����Ʊ���");
//	//��ʾ�����ļ��Ի���
//	if (IDOK == fileDlg.DoModal())
//	{
//		m_file_path = fileDlg.GetPathName();	//���Ϊ�󣬵�ǰ�ļ���Ϊ������ļ���
//		SaveHex();
//		SetTitle();					//���µ��ļ������ñ���
//		ShowStatusBar();			//ˢ��״̬��
//	}
//}


BEGIN_MESSAGE_MAP(CSimpleNotePadDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_APP_ABOUT, &CSimpleNotePadDlg::OnAppAbout)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, &CSimpleNotePadDlg::OnFileOpen)
	ON_COMMAND(ID_CODE_ANSI, &CSimpleNotePadDlg::OnCodeAnsi)
	ON_COMMAND(ID_CODE_UTF8, &CSimpleNotePadDlg::OnCodeUtf8)
	ON_COMMAND(ID_CODE_UTF16, &CSimpleNotePadDlg::OnCodeUtf16)
//	ON_UPDATE_COMMAND_UI(ID_CODE_ANSI, &CSimpleNotePadDlg::OnUpdateCodeAnsi)
//	ON_UPDATE_COMMAND_UI(ID_CODE_UTF8, &CSimpleNotePadDlg::OnUpdateCodeUtf8)
//	ON_UPDATE_COMMAND_UI(ID_CODE_UTF16, &CSimpleNotePadDlg::OnUpdateCodeUtf16)
//	ON_WM_INITMENUPOPUP()
	ON_EN_CHANGE(IDC_EDIT1, &CSimpleNotePadDlg::OnEnChangeEdit1)
//	ON_COMMAND(ID_SAVE_ANSI, &CSimpleNotePadDlg::OnSaveAnsi)
//	ON_COMMAND(ID_SAVE_UTF8, &CSimpleNotePadDlg::OnSaveUtf8)
//	ON_COMMAND(ID_SAVE_UTF8_NOBOM, &CSimpleNotePadDlg::OnSaveUtf8Nobom)
//	ON_COMMAND(ID_SAVE_UTF16, &CSimpleNotePadDlg::OnSaveUtf16)
//	ON_UPDATE_COMMAND_UI(ID_SAVE_ANSI, &CSimpleNotePadDlg::OnUpdateSaveAnsi)
//	ON_UPDATE_COMMAND_UI(ID_SAVE_UTF8, &CSimpleNotePadDlg::OnUpdateSaveUtf8)
//	ON_UPDATE_COMMAND_UI(ID_SAVE_UTF8_NOBOM, &CSimpleNotePadDlg::OnUpdateSaveUtf8Nobom)
//	ON_UPDATE_COMMAND_UI(ID_SAVE_UTF16, &CSimpleNotePadDlg::OnUpdateSaveUtf16)
	ON_COMMAND(ID_FILE_SAVE, &CSimpleNotePadDlg::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CSimpleNotePadDlg::OnFileSaveAs)
	ON_COMMAND(ID_FORMAT_FONT, &CSimpleNotePadDlg::OnFormatFont)
	ON_COMMAND(ID_EDIT_UNDO, &CSimpleNotePadDlg::OnEditUndo)
	ON_COMMAND(ID_EDIT_CUT, &CSimpleNotePadDlg::OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, &CSimpleNotePadDlg::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CSimpleNotePadDlg::OnEditPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CSimpleNotePadDlg::OnEditSelectAll)
	ON_COMMAND(ID_HEX_VIEW, &CSimpleNotePadDlg::OnHexView)
//	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_FILE_NEW, &CSimpleNotePadDlg::OnFileNew)
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_COMPARE, &CSimpleNotePadDlg::OnFileCompare)
	ON_COMMAND(ID_WORD_WRAP, &CSimpleNotePadDlg::OnWordWrap)
//	ON_UPDATE_COMMAND_UI(ID_WORD_WRAP, &CSimpleNotePadDlg::OnUpdateWordWrap)
	ON_WM_CLOSE()
	ON_COMMAND(ID_EDIT_FIND, &CSimpleNotePadDlg::OnFind)
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace)
	ON_COMMAND(ID_FIND_NEXT, &CSimpleNotePadDlg::OnFindNext)
	ON_COMMAND(ID_MARK_ALL, &CSimpleNotePadDlg::OnMarkAll)
	ON_COMMAND(ID_EDIT_REPLACE, &CSimpleNotePadDlg::OnReplace)
	ON_WM_MENUSELECT()
	ON_WM_INITMENU()
	ON_COMMAND(ID_FORMAT_CONVERT, &CSimpleNotePadDlg::OnFormatConvert)
	ON_WM_GETMINMAXINFO()
    ON_COMMAND(ID_ALWAYS_ON_TOP, &CSimpleNotePadDlg::OnAlwaysOnTop)
END_MESSAGE_MAP()

// CSimpleNotePadDlg ��Ϣ�������

BOOL CSimpleNotePadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//��ȡ�����ļ���·������ǰexe�ļ�����·��\config.ini��
	//m_config_path = CCommon::GetCurrentPath() + L"config.ini";
	//��������
	LoadConfig();
	//��ʼ�����ڴ�С
	CRect rect;
	rect.right = m_window_width;
	rect.bottom = m_window_hight;
	MoveWindow(rect);

	//���ݵ�ǰϵͳDPI��������״̬����С
	CWindowDC dc(this);
	HDC hDC = dc.GetSafeHdc();
	m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
	m_status_bar_hight = m_dpi * 20 / 96;
	m_edit_bottom_space = m_dpi * 22 / 96;
	m_status_bar_mid_width = m_dpi * 60 / 96;
	m_status_bar_right_width = m_dpi * 160 / 96;

	//��ʼ���༭���С
	GetClientRect(&rect);
	//rect.bottom = rect.bottom - 22;
	rect.bottom = rect.bottom - m_edit_bottom_space;
	m_edit.MoveWindow(rect);
	
	//��ʼ��״̬��
	GetClientRect(&rect);
	//rect.top = rect.bottom - 20;
	rect.top = rect.bottom - m_status_bar_hight;
	m_status_bar.Create(WS_VISIBLE | CBRS_BOTTOM, rect, this, 3);

	int nParts[3] = { rect.Width() - m_status_bar_right_width - m_status_bar_mid_width, rect.Width() - m_status_bar_right_width, -1 }; //�ָ�ߴ�
	m_status_bar.SetParts(3, nParts); //�ָ�״̬��
	ShowStatusBar();

	//��ʼ������
	m_font.CreatePointFont(m_font_size * 10, m_font_name);
	m_edit.SetFont(&m_font);

	//���m_file_path�����ͨ�����캯���������ݵġ����������е��ļ�·��������ļ�
	if (!m_file_path.IsEmpty())
	{
		//�������л�ȡ���ļ�·����������ո�����ǰ��������ţ�����оͰ�����ɾ��
		wstring file_path{ m_file_path.GetString() };
		if (file_path.front() == L'\"')
			file_path = file_path.substr(1);
		if (file_path.back() == L'\"')
			file_path.pop_back();
		m_file_path = file_path.c_str();
		//OpenFile(m_file_path);
		SetTimer(1234, 100, NULL);
	}
	SetTitle();

	//��������ı�����
	m_edit.SetLimitText(static_cast<UINT>(-1));

    SetAlwaysOnTop();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSimpleNotePadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSimpleNotePadDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSimpleNotePadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSimpleNotePadDlg::OnAppAbout()
{
	// TODO: �ڴ���������������
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CSimpleNotePadDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	CRect size;		//�༭���������
	size.right = cx;
	//size.bottom = cy - 22;		//�����·�״̬��ռ20�����ظ߶�
	size.bottom = cy - m_edit_bottom_space;		//�����·�״̬��ռ20�����ظ߶�
	if (nType != SIZE_MINIMIZED && m_edit.m_hWnd != NULL)
	{
		m_edit.MoveWindow(size);		//���ڴ�С�ı�ʱ�ı�༭���С
	}

	CRect status_bar_size;
	status_bar_size.right = cx;
	status_bar_size.bottom = cy;
	//status_bar_size.top = cy - 20;
	status_bar_size.top = cy - m_status_bar_hight;
	if (nType != SIZE_MINIMIZED && m_status_bar.m_hWnd != NULL)
	{
		m_status_bar.MoveWindow(status_bar_size);
		int nParts[3] = { cx - m_status_bar_right_width - m_status_bar_mid_width, cx - m_status_bar_right_width, -1 }; //�ָ�ߴ�
		m_status_bar.SetParts(3, nParts); //�ָ�״̬��
	}
	if (nType != SIZE_MAXIMIZED && nType != SIZE_MINIMIZED)
	{
		//m_window_width = cx;
		//m_window_hight = cy;
		CRect rect;
		GetWindowRect(&rect);
		m_window_width = rect.Width();
		m_window_hight = rect.Height();
	}
}


void CSimpleNotePadDlg::OnFileOpen()
{
	// TODO: �ڴ���������������
	//�����ļ�ǰѯ���û��Ƿ񱣴�
	if (!SaveInquiry())
		return;
	//���ù�����
	LPCTSTR szFilter = _T("�ı��ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");
	//������ļ��Ի���
	CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);
	//��ʾ���ļ��Ի���
	if (IDOK == fileDlg.DoModal())
	{
		m_file_path = fileDlg.GetPathName();	//��ȡ�򿪵��ļ�·��
		OpenFile(m_file_path);					//���ļ�
		SetTitle();								//���ô��ڱ���
	}
}


void CSimpleNotePadDlg::OnCodeAnsi()
{
	// TODO: �ڴ���������������
	//m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, m_code);
	if(!BeforeChangeCode()) return;
	m_code = CodeType::ANSI;
	ChangeCode();
}


void CSimpleNotePadDlg::OnCodeUtf8()
{
	// TODO: �ڴ���������������
	//m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, m_code);
	if (!BeforeChangeCode()) return;
	if(m_code != CodeType::UTF8_NO_BOM)
		m_code = CodeType::UTF8;
	ChangeCode();
}


void CSimpleNotePadDlg::OnCodeUtf16()
{
	// TODO: �ڴ���������������
	//m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, m_code);
	if (!BeforeChangeCode()) return;
	m_code = CodeType::UTF16;
	ChangeCode();
}


//void CSimpleNotePadDlg::OnUpdateCodeAnsi(CCmdUI *pCmdUI)
//{
//	// TODO: �ڴ������������û����洦��������
//	pCmdUI->SetCheck(m_code == CodeType::ANSI);
//}


//void CSimpleNotePadDlg::OnUpdateCodeUtf8(CCmdUI *pCmdUI)
//{
//	// TODO: �ڴ������������û����洦��������
//	pCmdUI->SetCheck(m_code == CodeType::UTF8 || m_code == CodeType::UTF8_NO_BOM);
//}


//void CSimpleNotePadDlg::OnUpdateCodeUtf16(CCmdUI *pCmdUI)
//{
//	// TODO: �ڴ������������û����洦��������
//	pCmdUI->SetCheck(m_code == CodeType::UTF16);
//}


//void CSimpleNotePadDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
//{
//	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
//
//	// TODO: �ڴ˴������Ϣ����������
//	ASSERT(pPopupMenu != NULL);
//	// Check the enabled state of various menu items.
//
//	CCmdUI state;
//	state.m_pMenu = pPopupMenu;
//	ASSERT(state.m_pOther == NULL);
//	ASSERT(state.m_pParentMenu == NULL);
//
//	// Determine if menu is popup in top-level menu and set m_pOther to
//	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
//	HMENU hParentMenu;
//	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
//		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
//	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
//	{
//		CWnd* pParent = this;
//		// Child Windows dont have menus--need to go to the top!
//		if (pParent != NULL &&
//			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
//		{
//			int nIndexMax = ::GetMenuItemCount(hParentMenu);
//			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
//			{
//				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
//				{
//					// When popup is found, m_pParentMenu is containing menu.
//					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
//					break;
//				}
//			}
//		}
//	}
//
//	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
//	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
//		state.m_nIndex++)
//	{
//		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
//		if (state.m_nID == 0)
//			continue; // Menu separator or invalid cmd - ignore it.
//
//		ASSERT(state.m_pOther == NULL);
//		ASSERT(state.m_pMenu != NULL);
//		if (state.m_nID == (UINT)-1)
//		{
//			// Possibly a popup menu, route to first item of that popup.
//			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
//			if (state.m_pSubMenu == NULL ||
//				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
//				state.m_nID == (UINT)-1)
//			{
//				continue;       // First item of popup cant be routed to.
//			}
//			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
//		}
//		else
//		{
//			// Normal menu item.
//			// Auto enable/disable if frame window has m_bAutoMenuEnable
//			// set and command is _not_ a system command.
//			state.m_pSubMenu = NULL;
//			state.DoUpdate(this, FALSE);
//		}
//
//		// Adjust for menu deletions and additions.
//		UINT nCount = pPopupMenu->GetMenuItemCount();
//		if (nCount < state.m_nIndexMax)
//		{
//			state.m_nIndex -= (state.m_nIndexMax - nCount);
//			while (state.m_nIndex < nCount &&
//				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
//			{
//				state.m_nIndex++;
//			}
//		}
//		state.m_nIndexMax = nCount;
//	}
//}



void CSimpleNotePadDlg::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString edit_text;
	m_edit.GetWindowText(edit_text);
	m_edit_wcs.assign(edit_text);
	//m_flag = false;
	//if (!m_flag)
	//{
		m_modified = true;
	//}
	ShowStatusBar();
}


//void CSimpleNotePadDlg::OnSaveAnsi()
//{
//	// TODO: �ڴ���������������
//	m_save_code = CodeType::ANSI;
//}


//void CSimpleNotePadDlg::OnSaveUtf8()
//{
//	// TODO: �ڴ���������������
//	m_save_code = CodeType::UTF8;
//}


//void CSimpleNotePadDlg::OnSaveUtf8Nobom()
//{
//	// TODO: �ڴ���������������
//	m_save_code = CodeType::UTF8_NO_BOM;
//}


//void CSimpleNotePadDlg::OnSaveUtf16()
//{
//	// TODO: �ڴ���������������
//	m_save_code = CodeType::UTF16;
//}


//void CSimpleNotePadDlg::OnUpdateSaveAnsi(CCmdUI *pCmdUI)
//{
//	// TODO: �ڴ������������û����洦��������
//	pCmdUI->SetCheck(m_save_code == CodeType::ANSI);
//}


//void CSimpleNotePadDlg::OnUpdateSaveUtf8(CCmdUI *pCmdUI)
//{
//	// TODO: �ڴ������������û����洦��������
//	pCmdUI->SetCheck(m_save_code == CodeType::UTF8);
//}


//void CSimpleNotePadDlg::OnUpdateSaveUtf8Nobom(CCmdUI *pCmdUI)
//{
//	// TODO: �ڴ������������û����洦��������
//	pCmdUI->SetCheck(m_save_code == CodeType::UTF8_NO_BOM);
//}


//void CSimpleNotePadDlg::OnUpdateSaveUtf16(CCmdUI *pCmdUI)
//{
//	// TODO: �ڴ������������û����洦��������
//	pCmdUI->SetCheck(m_save_code == CodeType::UTF16);
//}


void CSimpleNotePadDlg::OnFileSave()
{
	// TODO: �ڴ���������������
	_OnFileSave();
}


void CSimpleNotePadDlg::OnFileSaveAs()
{
	// TODO: �ڴ���������������
	_OnFileSaveAs();
}


void CSimpleNotePadDlg::OnFormatFont()
{
	// TODO: �ڴ���������������
	LOGFONT lf{ 0 };             //LOGFONT����
	m_font.GetLogFont(&lf);
	//_tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("΢���ź�"));	//��lf�е�Ԫ����������Ϊ��΢���źڡ�
	CFontDialog fontDlg(&lf);	//��������Ի��򣬳�ʼѡ������Ϊ֮ǰ����
	if (IDOK == fontDlg.DoModal())     // ��ʾ����Ի���
	{
		//���m_font�Ѿ�������һ��������Դ�������ͷ���
		if (m_font.m_hObject)
		{
			m_font.DeleteObject();
		}
		//ʹ��ѡ�������LOGFONT�����µ�����
		m_font.CreateFontIndirect(fontDlg.m_cf.lpLogFont);
		//��������
		m_edit.SetFont(&m_font);
		//��ȡ������Ϣ
		m_font_name = fontDlg.m_cf.lpLogFont->lfFaceName;
		m_font_size = fontDlg.m_cf.iPointSize / 10;
		//����������д�뵽ini�ļ�
		SaveConfig();
	}
}


BOOL CSimpleNotePadDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	//����ESC���˳�
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (GetKeyState(VK_CONTROL) & 0x80)
	{
		////���ð�Ctr+Aȫѡ
		//if (pMsg->wParam == 'a' || pMsg->wParam == 'A')
		//{
		//	OnEditSelectAll();
		//	return TRUE;
		//}
		//����Ctr+S����
		if (pMsg->wParam == 's' || pMsg->wParam == 'S')
		{
			_OnFileSave();
			return TRUE;
		}
		//����Ctr+O��
		else if (pMsg->wParam == 'o' || pMsg->wParam == 'O')
		{
			OnFileOpen();
			return TRUE;
		}
		//����Ctr+N�½�
		else if (pMsg->wParam == 'n' || pMsg->wParam == 'N')
		{
			OnFileNew();
			return TRUE;
		}
		//����Ctr+H��ʮ�����Ʋ鿴
		else if (pMsg->wParam == 'e' || pMsg->wParam == 'E')
		{
			OnHexView();
			return TRUE;
		}
		//����Ctr+F����
		else if (pMsg->wParam == 'f' || pMsg->wParam == 'F')
		{
			OnFind();
			return TRUE;
		}
		//����Ctr+H�滻
		else if (pMsg->wParam == 'h' || pMsg->wParam == 'H')
		{
			OnReplace();
			return TRUE;
		}
	}
	//���ð�F3������һ��
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F3)
	{
		OnFindNext();
		return TRUE;
	}
	//����Edit�е�TAB��
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB && pMsg->hwnd == m_edit.GetSafeHwnd())
	{
		CString str;
		m_edit.GetWindowText(str);
		int nStart, nEnd;
		m_edit.GetSel(nStart, nEnd);
		if (nStart != nEnd)
		{
			str = str.Left(nStart) + str.Mid(nEnd);
		}
		str.Insert(nStart, _T("\t"));
		m_edit.SetWindowText(str);
		m_edit.SetSel(nStart + 1, nStart + 1);
		m_edit_wcs = str.GetString();
		ShowStatusBar();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CSimpleNotePadDlg::OnEditUndo()
{
	// TODO: �ڴ���������������
	m_edit.Undo();
}


void CSimpleNotePadDlg::OnEditCut()
{
	// TODO: �ڴ���������������
	m_edit.Cut();
}


void CSimpleNotePadDlg::OnEditCopy()
{
	// TODO: �ڴ���������������
	m_edit.Copy();
}


void CSimpleNotePadDlg::OnEditPaste()
{
	// TODO: �ڴ���������������
	m_edit.Paste();
}


void CSimpleNotePadDlg::OnEditSelectAll()
{
	// TODO: �ڴ���������������
	m_edit.SetSel(0, -1);
}


void CSimpleNotePadDlg::OnHexView()
{
	// TODO: �ڴ���������������
	string edit_str{ m_edit_str };		//����ʮ�����Ʊ༭ǰ�ȱ���༭ǰ������
	CHexViewDlg hex_view_dlg(m_edit_str, m_code, m_file_path);
	ShowWindow(SW_HIDE);
	hex_view_dlg.DoModal();
	ShowWindow(SW_SHOW);
	//if (!m_file_path.IsEmpty() && !hex_view_dlg.m_modified_data.empty())
	if (!m_file_path.IsEmpty() && hex_view_dlg.IsModified())
	{
		if (m_modified)
		{
			CString info;
			info.LoadString(IDS_STRING103);
			if (MessageBox(info, NULL, MB_ICONQUESTION | MB_YESNO) == IDNO)
			{
				return;
			}
		}
		if (MessageBox(_T("�Ƿ�Ҫ����ʮ�����Ʊ༭�ĸ��ģ�"), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);
			m_edit.SetWindowText(m_edit_wcs.c_str());
			SaveHex();
		}
		else
		{
			//���û�б���ʮ�����Ʊ༭�ĸ��ģ���ָ�ԭ��������
			m_edit_str = edit_str;
		}
	}
}


//void CSimpleNotePadDlg::OnDestroy()
//{
//	CDialog::OnDestroy();
//
//	// TODO: �ڴ˴������Ϣ����������
//}


void CSimpleNotePadDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//�����ļ�ǰѯ���û��Ƿ񱣴�
	if (!SaveInquiry())
		return;
	wchar_t file_path[MAX_PATH];
	DragQueryFile(hDropInfo, 0, file_path, MAX_PATH);
	m_file_path = file_path;
	OpenFile(m_file_path);	//���ļ�
	SetTitle();				//���ô��ڱ���
	DragFinish(hDropInfo);  //�ϷŽ�����,�ͷ��ڴ�

	CDialog::OnDropFiles(hDropInfo);
}


void CSimpleNotePadDlg::OnFileNew()
{
	// TODO: �ڴ���������������
	//ѯ���Ƿ񱣴�
	if (!SaveInquiry())
		return;

	m_edit_str.clear();
	m_edit_wcs.clear();
	m_file_path.Empty();
	m_edit.SetWindowText(_T(""));
	m_code = CodeType::ANSI;
	m_modified = false;
	ShowStatusBar();
	SetWindowText(_T("�ޱ��� - SimpleNotePad"));
}


void CSimpleNotePadDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == 1234)
	{
		KillTimer(1234);		//��ʱ����Ӧһ�κ�ͽ�������
		OpenFile(m_file_path);		//����ļ���ͨ�������д򿪵ģ�����ʱ100�����ٴ�
	}

	CDialog::OnTimer(nIDEvent);
}


void CSimpleNotePadDlg::OnFileCompare()
{
	// TODO: �ڴ���������������
	CFileCompareDlg aDlg;
	ShowWindow(SW_HIDE);
	aDlg.DoModal();
	ShowWindow(SW_SHOW);
}


void CSimpleNotePadDlg::OnWordWrap()
{
	// TODO: �ڴ���������������
	if (m_word_wrap)
	{
		//m_edit.ModifyStyle(WS_HSCROLL, ES_AUTOHSCROLL);
		m_word_wrap = false;
	}
	else
	{
		//m_edit.ModifyStyle(ES_AUTOHSCROLL, WS_HSCROLL);
		m_word_wrap = true;
	}
}


//void CSimpleNotePadDlg::OnUpdateWordWrap(CCmdUI *pCmdUI)
//{
//	// TODO: �ڴ������������û����洦��������
//	pCmdUI->SetCheck(m_word_wrap);
//}


void CSimpleNotePadDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//ѯ���Ƿ񱣴�
	if(!SaveInquiry()) return;
	//���洰�ڴ�С
	//if (!IsZoomed())
	//{
	//	CRect rect;
	//	GetWindowRect(&rect);
	//	m_window_width = rect.Width();
	//	m_window_hight = rect.Height();
	//}
	SaveConfig();

	CDialog::OnClose();
}


void CSimpleNotePadDlg::OnFind()
{
	// TODO: �ڴ���������������
	if (m_pFindDlg == nullptr)
	{
		m_pFindDlg = new CFindReplaceDialog;
		//��ʼ�������ҡ��Ի����״̬
		if (m_find_no_case)
			m_pFindDlg->m_fr.Flags &= (~FR_MATCHCASE);
		else
			m_pFindDlg->m_fr.Flags |= FR_MATCHCASE;
		if (m_find_whole_word)
			m_pFindDlg->m_fr.Flags |= FR_WHOLEWORD;
		else
			m_pFindDlg->m_fr.Flags &= (~FR_WHOLEWORD);
		m_pFindDlg->Create(TRUE, NULL, NULL, FR_DOWN/* | FR_HIDEWHOLEWORD | FR_HIDEMATCHCASE*/, this);
	}
	m_pFindDlg->ShowWindow(SW_SHOW);
	m_pFindDlg->SetActiveWindow();
}

LONG CSimpleNotePadDlg::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	//m_pFindDlg = CFindReplaceDialog::GetNotifier(lParam);
	if (m_pFindDlg != nullptr)
	{
		m_find_str = m_pFindDlg->GetFindString();
		m_find_down = (m_pFindDlg->SearchDown() != 0);
		m_find_no_case = (m_pFindDlg->MatchCase() == 0);
		m_find_whole_word = (m_pFindDlg->MatchWholeWord() != 0);
		if (m_pFindDlg->FindNext())		//������һ��ʱ
		{
			OnFindNext();
		}
		if (m_pFindDlg->IsTerminating())	//�رմ���ʱ
		{
			//m_pFindDlg->DestroyWindow();
			m_pFindDlg = nullptr;
		}
	}
	//delete m_pFindDlg;

	if (m_pReplaceDlg != nullptr)
	{
		m_find_str = m_pReplaceDlg->GetFindString();
		m_replace_str = m_pReplaceDlg->GetReplaceString();
		if (m_pReplaceDlg->FindNext())		//������һ��ʱ
		{
			OnFindNext();
		}
		if (m_pReplaceDlg->ReplaceCurrent())	//�滻��ǰʱ
		{
			if (m_find_flag)
			{
				m_edit_wcs.replace(m_find_index, m_find_str.size(), m_replace_str.c_str(), m_replace_str.size());	//�滻�ҵ����ַ���
				m_edit.SetWindowText(m_edit_wcs.c_str());
				m_modified = true;
				ShowStatusBar();
				OnFindNext();
				m_edit.SetSel(m_find_index, m_find_index + m_find_str.size());	//ѡ���滻���ַ���
				SetActiveWindow();		//���༭���������û����
			}
			else
			{
				OnFindNext();
			}
		}
		if (m_pReplaceDlg->ReplaceAll())		//�滻ȫ��ʱ
		{
			int replace_count{};	//ͳ���滻���ַ����ĸ���
			while (true)
			{
				m_find_index = m_edit_wcs.find(m_find_str, m_find_index + 1);	//�����ַ���
				if (m_find_index == string::npos)
					break;
				m_edit_wcs.replace(m_find_index, m_find_str.size(), m_replace_str.c_str(), m_replace_str.size());	//�滻�ҵ����ַ���
				replace_count++;
			}
			m_edit.SetWindowText(m_edit_wcs.c_str());
			m_modified = true;
			ShowStatusBar();
			if (replace_count != 0)
			{
				CString info;
				info.Format(_T("�滻��ɣ����滻%d���ַ�����"),replace_count);
				MessageBox(info, NULL, MB_ICONINFORMATION);
			}
		}
		if (m_pReplaceDlg->IsTerminating())
		{
			m_pReplaceDlg = nullptr;
		}
	}
	return 0;
}


void CSimpleNotePadDlg::OnFindNext()
{
	// TODO: �ڴ���������������
	//if (m_find_down)
	//	m_find_index = m_edit_wcs.find(m_find_str, m_find_index + 1);	//������
	//else
	//	m_find_index = m_edit_wcs.rfind(m_find_str, m_find_index - 1);	//��ǰ����
	m_find_index = CCommon::StringFind(m_edit_wcs, m_find_str, m_find_no_case, m_find_whole_word, m_find_down, (m_find_down ? (m_find_index + 1) : (m_find_index - 1)));
	if (m_find_index == string::npos)
	{
		CString info;
		info.Format(_T("�Ҳ�����%s��"), m_find_str.c_str());
		MessageBox(info, NULL, MB_OK | MB_ICONINFORMATION);
		m_find_flag = false;
	}
	else
	{
		m_edit.SetSel(m_find_index, m_find_index + m_find_str.size());		//ѡ���ҵ����ַ���
		SetActiveWindow();		//���༭��������Ϊ�����
		m_find_flag = true;
	}
}

//�ú�����Ч
void CSimpleNotePadDlg::OnMarkAll()
{
	// TODO: �ڴ���������������
	if (!m_find_str.empty())
	{
		while (true)
		{
			m_find_index = m_edit_wcs.find(m_find_str, m_find_index + 1);
			if (m_find_index == string::npos) return;
			m_edit.SetHighlight(m_find_index, m_find_index + m_find_str.size());
		}
	}
}


void CSimpleNotePadDlg::OnReplace()
{
	// TODO: �ڴ���������������
	if (m_pReplaceDlg == nullptr)
	{
		m_pReplaceDlg = new CFindReplaceDialog;
		m_pReplaceDlg->Create(FALSE, NULL, NULL, FR_DOWN | FR_HIDEWHOLEWORD | FR_HIDEMATCHCASE, this);
	}
	m_pReplaceDlg->ShowWindow(SW_SHOW);
	m_pReplaceDlg->SetActiveWindow();
}


void CSimpleNotePadDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CDialog::OnMenuSelect(nItemID, nFlags, hSysMenu);

	// TODO: �ڴ˴������Ϣ����������
	CString menu_tip;
	menu_tip.LoadString(nItemID);
	//int index = menu_tip.Find(_T('\n'));
	//if (index != -1) menu_tip = menu_tip.Left(index);
	if (!menu_tip.IsEmpty())
		m_status_bar.SetText(menu_tip, 0, 0);
	else
		ShowStatusBar();
}


void CSimpleNotePadDlg::OnInitMenu(CMenu* pMenu)
{
	CDialog::OnInitMenu(pMenu);

	// TODO: �ڴ˴������Ϣ����������
	switch (m_code)
	{
	case CodeType::ANSI: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16, ID_CODE_ANSI, MF_BYCOMMAND | MF_CHECKED); break;
	case CodeType::UTF8: case CodeType::UTF8_NO_BOM: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16, ID_CODE_UTF8, MF_BYCOMMAND | MF_CHECKED); break;
	case CodeType::UTF16: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16, ID_CODE_UTF16, MF_BYCOMMAND | MF_CHECKED); break;
	}
	//switch (m_save_code)
	//{
	//case CodeType::ANSI: pMenu->CheckMenuRadioItem(ID_SAVE_ANSI, ID_SAVE_UTF16, ID_SAVE_ANSI, MF_BYCOMMAND | MF_CHECKED); break;
	//case CodeType::UTF8: pMenu->CheckMenuRadioItem(ID_SAVE_ANSI, ID_SAVE_UTF16, ID_SAVE_UTF8, MF_BYCOMMAND | MF_CHECKED); break;
	//case CodeType::UTF8_NO_BOM: pMenu->CheckMenuRadioItem(ID_SAVE_ANSI, ID_SAVE_UTF16, ID_SAVE_UTF8_NOBOM, MF_BYCOMMAND | MF_CHECKED); break;
	//case CodeType::UTF16: pMenu->CheckMenuRadioItem(ID_SAVE_ANSI, ID_SAVE_UTF16, ID_SAVE_UTF16, MF_BYCOMMAND | MF_CHECKED); break;
	//default: break;
	//}
	pMenu->CheckMenuItem(ID_WORD_WRAP, MF_BYCOMMAND | (m_word_wrap ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_ALWAYS_ON_TOP, MF_BYCOMMAND | (m_always_on_top ? MF_CHECKED : MF_UNCHECKED));
}



void CSimpleNotePadDlg::OnFormatConvert()
{
	// TODO: �ڴ���������������
	CFormatConvertDlg formatConvertDlg;
	ShowWindow(SW_HIDE);
	formatConvertDlg.DoModal();
	ShowWindow(SW_SHOW);
}


void CSimpleNotePadDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	lpMMI->ptMinTrackSize.x = 200 * m_dpi / 96;		//������С���
	lpMMI->ptMinTrackSize.y = 150 * m_dpi / 96;		//������С�߶�

	CDialog::OnGetMinMaxInfo(lpMMI);
}


void CSimpleNotePadDlg::OnAlwaysOnTop()
{
    // TODO: �ڴ���������������
    m_always_on_top = !m_always_on_top;
    SetAlwaysOnTop();
}
