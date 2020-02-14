// HttpClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "HttpClient.h"
#include "HttpClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CHttpClientDlg 对话框

CHttpClientDlg::CHttpClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HTTPCLIENT_DIALOG, pParent)
	, postPath(_T(""))
	, postBody(_T(""))
	, postContType(_T(""))
	, rpResult(_T(""))
	, serverIP(_T(""))
	, getPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHttpClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PORT, serverPort);
	DDX_Text(pDX, IDC_POST_PATH, postPath);
	DDX_Text(pDX, IDC_POST_BODY, postBody);
	DDX_Text(pDX, IDC_POST_CONTTYPE, postContType);
	DDX_Text(pDX, IDC_POST_RESULT, rpResult);
	DDX_Text(pDX, IDC_IP, serverIP);
	DDX_Text(pDX, IDC_GET_PATH, getPath);
}

BEGIN_MESSAGE_MAP(CHttpClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_POST, &CHttpClientDlg::OnBnClickedPost)
	ON_BN_CLICKED(IDC_GET, &CHttpClientDlg::OnBnClickedGet)
END_MESSAGE_MAP()

// CHttpClientDlg 消息处理程序

BOOL CHttpClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHttpClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHttpClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// UTF-8 UNICODE转换
// CP_ACP        default to ANSI code page
// CP_OEMCP      default to OEM  code page
// CP_MACCP      default to MAC  code page
// CP_THREAD_ACP current thread's ANSI code page
// CP_SYMBOL     SYMBOL translations
// CP_UTF7       UTF-7 translation
// CP_UTF8       UTF-8 translation
CString UTF8UnicodeConvert(const CString& strSource, UINT nSourceCodePage, UINT nTargetCodePage)
{
	CString strTarget;

	wchar_t* pWideBuf;
	int nWideBufLen;

	char* pMultiBuf;
	int nMiltiBufLen;

	nWideBufLen = MultiByteToWideChar(nSourceCodePage, 0, strSource, -1, NULL, NULL);
	pWideBuf = new wchar_t[nWideBufLen + 1]();
	MultiByteToWideChar(nSourceCodePage, 0, strSource, -1, pWideBuf, nWideBufLen);

	nMiltiBufLen = WideCharToMultiByte(nTargetCodePage, 0, pWideBuf, -1, 0, 0, NULL, FALSE);
	pMultiBuf = new char[nMiltiBufLen + 1]();
	WideCharToMultiByte(nTargetCodePage, 0, pWideBuf, -1, pMultiBuf, nMiltiBufLen, NULL, FALSE);

	strTarget.Format(_T("%s"), pMultiBuf);

	delete[]pWideBuf;
	delete[]pMultiBuf;

	return strTarget;
}

/*
	Post			向指定服务器发送Post请求
	szIP:			域名或者IP
	szPath:			路径
	szBody:			内容
	szContentType:	类型，默认为text/plain
	返回字符串，失败返回空字符串。
*/
CString Post(LPCTSTR szIP, INT nPort, LPCTSTR szPath, LPCTSTR szBody, LPCTSTR szContentType = "text/plain")
{
	httplib::Client client(szIP, nPort);
	auto result = client.Post(szPath, szBody, szContentType);
	return result ? result->body.c_str() : "";
}
/*
	Get				向指定服务器发送Get请求
	szIP:			域名或者IP
	szPath:			路径
	返回字符串，失败返回空字符串。
*/
CString Get(LPCTSTR szIP, INT nPort, LPCTSTR szPath)
{
	httplib::Client client(szIP, nPort);
	auto result = client.Get(szPath);
	return result ? result->body.c_str() : "";
}

void CHttpClientDlg::OnBnClickedPost()
{
	INT nPort;

	UpdateData();
	nPort = ::strtod(serverPort.GetString(), nullptr);
	if (nPort == 0)
		nPort = 80;

	// 创建客户端
	try
	{
		rpResult = UTF8UnicodeConvert(Post(serverIP.GetString(), nPort, postPath.GetString(), postBody.GetString(), postContType.GetString()), CP_UTF8, CP_ACP);
		UpdateData(FALSE);
	}
	catch (...)
	{
		MessageBox("连接到服务器出错");
	}
}

void CHttpClientDlg::OnBnClickedGet()
{
	INT nPort;

	UpdateData();
	nPort = ::strtod(serverPort.GetString(), nullptr);
	if (nPort == 0)
		nPort = 80;
	// 创建客户端
	try
	{
		rpResult = UTF8UnicodeConvert(Get(serverIP.GetString(), nPort, getPath), CP_UTF8, CP_ACP);
		UpdateData(FALSE);
	}
	catch (...)
	{
		MessageBox("连接到服务器出错");
	}
}