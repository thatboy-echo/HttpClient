// HttpClientDlg.h: 头文件
//

#pragma once
#include <httplib.h>

// CHttpClientDlg 对话框
class CHttpClientDlg : public CDialogEx
{
	// 构造
public:
	CHttpClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HTTPCLIENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPost();
	afx_msg void OnBnClickedGet();
	CString serverPort;
	CString postPath;
	CString postBody;
	CString postContType;
	CString rpResult;
	CString serverIP;
	CString getPath;
};
