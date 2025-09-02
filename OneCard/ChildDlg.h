#pragma once
#include "afxdialogex.h"


// ChildDlg 대화 상자

class ChildDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ChildDlg)

public:
	ChildDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~ChildDlg();
	CImage m_image;
	std::vector<BYTE> bmpData;
	CRect m_destRect;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHILD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual INT_PTR DoModal();
};
