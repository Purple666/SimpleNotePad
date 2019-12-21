#pragma once
#include "Common.h"
#include "afxwin.h"
#include "afxcmn.h"

using std::string;

// CHexViewDlg �Ի���

class CHexViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CHexViewDlg)

public:
	CHexViewDlg(string& data, CodeType code, const CString& file_path, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHexViewDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HEX_VIEW_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	//ʮ�����Ʊ༭�ĵ�λ
	enum class EditUnit
	{
		BYTE,
		WORD,
		DWORD
	};

	//���ݴ�С��λ
	enum class SizeUnit
	{
		B,
		KB,
		MB
	};

	//�޸Ĺ�����Ŀ
	struct Item
	{
		unsigned int address;
		unsigned int old_value;
		unsigned int value;
		//EditUnit edit_unit;
	};

	const CString& m_file_path;
	string& m_data;
	CodeType m_code;
	CFont m_font;

	CString m_str;		//�ı��༭������ʾ���ַ���
	CEdit m_edit;

	unsigned int m_address;
	unsigned int m_value;

	vector<Item> m_modified_data;		//����༭��������
	bool m_modified;		//���ڱ����޸Ĺ���״̬��������޸Ĺ�����Ϊtrue

	EditUnit m_edit_unit{};
	SizeUnit m_size_unit{};

    CSize m_min_size;

protected:
	void ShowHexData(bool ini = false);	//���ı��༭������ʾ��ʮ����������(�ڳ�ʼ��ʱ����Ҫ�Ѳ�����Ϊtrue)
	unsigned int GetValueAndStr(unsigned int address, EditUnit edit_unit, CString& value_str);		//���ݵ�ַ�ͱ༭��λ����ֵ����ת�����ַ������浽value_str��
	void ShowSizeInfo();		//�ڡ��޸��ļ���С������Static�ؼ�����ʾ�ļ���С

	void SaveConfig() const;
	void LoadConfig();

public:
	bool IsModified() { return m_modified; }

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEditAddress();
	afx_msg void OnBnClickedSearch();
//	afx_msg void OnBnClickedButtonPrevious();
//	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedModify();
	CListCtrl m_modified_list;
	CStatic m_static_head;
	afx_msg void OnBnClickedRefreshButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedRadioAnsi();
	afx_msg void OnBnClickedRadioUtf8();
	afx_msg void OnBnClickedRadioUtf16();
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioByte();
	afx_msg void OnBnClickedRadioWord();
	afx_msg void OnBnClickedRadioDword();
	afx_msg void OnCbnSelchangeSizeUnitCombo();
	afx_msg void OnBnClickedModifySize();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedInsertDataButton();
	afx_msg void OnBnClickedDeleteDataButton();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
