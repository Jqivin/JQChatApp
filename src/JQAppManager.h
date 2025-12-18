#pragma once
#include <QObject>
#include "CommonDefines.h"

class JQChatMainFrame;
class LoginDlg;
class TcpClientManager;

class JQAppManager : public QObject
{
	Q_OBJECT
public:
	JQAppManager();
	~JQAppManager();

	// 初始化
	bool InitInstance();
	// 清理资源
	void ExitInstance();
	// 设置当前用户
	void SetUserInfo(const UserInfo_t& info);

private:
	void InitUi();

public:

	JQChatMainFrame*		m_pChatMainFrame;	// 主框架
	LoginDlg*				m_pLoginDlg;		// 登录界面
	TcpClientManager*		m_tcpclientManager; // 网络管理模块


	UserInfo_t				m_userInfo;			// 已登录用户信息	

};

// 声明
extern JQAppManager JQApp;

