#include "JQAppManager.h"
#include "JQChatMainFrame.h"
#include "Login.h"
#include "TcpClientManager.h"
#include "Logger.h"


#include <QTranslator>
#include <QDebug>
#include <QCoreApplication>

// ¶¨Òå
JQAppManager JQApp;

JQAppManager::JQAppManager()
	:m_pChatMainFrame(nullptr)
	, m_pLoginDlg(nullptr)
	, m_tcpclientManager(nullptr)
{
}

JQAppManager::~JQAppManager()
{
}

bool JQAppManager::InitInstance()
{
	QString strExePath = QCoreApplication::applicationDirPath();
	Logger::Instance().StartLogger(strExePath.toStdString());
	m_tcpclientManager = new TcpClientManager();
	if (m_tcpclientManager)
	{
		m_tcpclientManager->connectToServer("192.168.32.128", 8000);
		connect(m_tcpclientManager, &TcpClientManager::connected,this, [=] {
			LOG_INFO("connect success.");
			return true;
		});
	}
	InitUi();
	return true;
}
void JQAppManager::InitUi()
{
	m_pChatMainFrame = new JQChatMainFrame();
	m_pChatMainFrame->hide();

	m_pLoginDlg = new LoginDlg();

	connect(m_pLoginDlg, &LoginDlg::SignalGetFriendInfo, m_pChatMainFrame, &JQChatMainFrame::OnGetFriendsInfo);
	connect(m_pLoginDlg, &LoginDlg::SignalReceiveOfflineMsgs, m_pChatMainFrame, &JQChatMainFrame::OnReceiveOfflineMsgs);
	if (QDialog::Accepted == m_pLoginDlg->exec())
	{
		m_pChatMainFrame->show();
	}
	else
	{
		delete m_pChatMainFrame;
		m_pChatMainFrame = nullptr;

		delete m_pLoginDlg;
		m_pLoginDlg = nullptr;
	}
}

void JQAppManager::ExitInstance()
{
	delete m_pChatMainFrame;
	m_pChatMainFrame = nullptr;

	delete m_pLoginDlg;
	m_pLoginDlg = nullptr;

	delete m_tcpclientManager;
	m_tcpclientManager = nullptr;
}

void JQAppManager::SetUserInfo(const UserInfo_t& info)
{
	m_userInfo = info;
}


