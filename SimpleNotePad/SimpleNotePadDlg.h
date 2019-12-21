
// SimpleNotePadDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "Common.h"
#include "FileCompareDlg.h"
#include "EditEx.h"

const int WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);	//��FINDMSGSTRINGע��ΪWM_FINDREPLACE��Ϣ
// CSimpleNotePadDlg �Ի���
class CSimpleNotePadDlg : public CDialog
{
// ����
public:
	CSimpleNotePadDlg(CString file_path = _T(""), CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLENOTEPAD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	//CMenu m_menu;	//�˵�
	CEditEx m_edit;					//�༭��
	CStatusBarCtrl m_status_bar;	//״̬��
	CFont m_font;			//����
	CString m_font_name;	//��������
	int m_font_size;		//�����С

	CString m_file_path;	//��ǰ�򿪵��ļ�·��
	string m_edit_str;		//������ļ����������
	wstring m_edit_wcs;		//������ʾ�����ڵ�����

	CodeType m_code{ CodeType::ANSI };		//�����ʽ
	CodeType m_save_code{ CodeType::ANSI };	//���Ϊʱ�ı����ʽ
	bool m_modified{ false };				//����ļ����༭������m_modifiedΪtrue
	//bool m_flag{ false };			//һ��flag��������m_edit.SetWindowText()�������ı���ʾ���ı�����ʱ��������Ϊtrue
	int m_dpi;		//��ǰ��ʾ����DPI����100%ʱΪ96

	int m_window_width;			//���ڿ��
	int m_window_hight;			//���ڸ߶�

	int m_status_bar_hight;		//״̬���߶�
	int m_edit_bottom_space;	//�༭���ڵײ��ര�ڵײ��ľ���
	int m_status_bar_mid_width;	//״̬������߲��ֵĿ��
	int m_status_bar_right_width;	//״̬�����ұ߲��ֵĿ��

	bool m_word_wrap;		//�Ƿ��Զ�����
    bool m_always_on_top{ false };

	//const CString CONFIG_PATH{ _T("./config.ini") };
	//wstring m_config_path;		//�����ļ����ڵ�·��

	CFindReplaceDialog* m_pFindDlg{};	//���ҶԻ���
	CFindReplaceDialog* m_pReplaceDlg{};	//�滻�Ի���
	wstring m_find_str;		//���ҵ��ַ���
	wstring m_replace_str;	//�滻���ַ���
	int m_find_index{ -1 };	//���ҵ��ַ���������
	bool m_find_down{ true };		//�Ƿ�������
	bool m_find_no_case;			//����ʱ�Ƿ����ִ�Сд
	bool m_find_whole_word;			//����ʱ�Ƿ�ȫ��ƥ��
	bool m_find_flag{ false };
	
	void OpenFile(LPCTSTR file_path);					//���ļ��Ĵ���
	bool SaveFile(LPCTSTR file_path, CodeType code);	//�����ļ��Ĵ���
	void JudgeCode();				//�жϱ����ʽ
	void ShowStatusBar();			//ˢ��״̬��
	void ChangeCode();				//���ı����ʽʱ�Ĵ���
	bool BeforeChangeCode();		//�ڸ��ı����ʽǰ�Ĵ���
	void SaveConfig();				//��������
	void LoadConfig();				//��������
	bool SaveInquiry(LPCTSTR info = NULL);	//ѯ���û��Ƿ񱣴棬����Ϊ��ʾ��Ϣ(�û������ȡ���󷵻�false�����򷵻�true)
	void SetTitle();				//���ô��ڱ���

	bool _OnFileSave();		//����Ѿ�ִ���˱���������򷵻�true�����򷵻�false
	bool _OnFileSaveAs();	//

	void SaveHex();
	//void SaveAsHex();

    void SetAlwaysOnTop();

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAppAbout();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileOpen();
	afx_msg void OnCodeAnsi();
	afx_msg void OnCodeUtf8();
	afx_msg void OnCodeUtf16();
//	afx_msg void OnUpdateCodeAnsi(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateCodeUtf8(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateCodeUtf16(CCmdUI *pCmdUI);
//	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnEnChangeEdit1();
//	afx_msg void OnSaveAnsi();
//	afx_msg void OnSaveUtf8();
//	afx_msg void OnSaveUtf8Nobom();
//	afx_msg void OnSaveUtf16();
//	afx_msg void OnUpdateSaveAnsi(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateSaveUtf8(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateSaveUtf8Nobom(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateSaveUtf16(CCmdUI *pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFormatFont();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEditUndo();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditSelectAll();
	afx_msg void OnHexView();
//	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFileNew();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnFileCompare();
	afx_msg void OnWordWrap();
//	afx_msg void OnUpdateWordWrap(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnFind();
	afx_msg LONG OnFindReplace(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFindNext();
	afx_msg void OnMarkAll();
	afx_msg void OnReplace();
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnFormatConvert();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnAlwaysOnTop();
};
