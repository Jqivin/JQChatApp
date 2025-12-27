#pragma once
#include <QObject>
#include "CommonDefines.h"

class JQChatMainFrame;
class LoginDlg;
class TlsClientManager;

class JQAppManager : public QObject
{
	Q_OBJECT
public:
	JQAppManager();
	~JQAppManager();

	// ��ʼ��
	bool InitInstance();
	// ������Դ
	void ExitInstance();
	// ���õ�ǰ�û�
	void SetUserInfo(const UserInfo_t& info);

private:
	bool InitUi();

public:

	JQChatMainFrame*		m_pChatMainFrame;	// �����
	LoginDlg*				m_pLoginDlg;		// ��¼����
	TlsClientManager*		m_tlsclientManager; // �������ģ��


	UserInfo_t				m_userInfo;			// �ѵ�¼�û���Ϣ	

};

// ����
extern JQAppManager JQApp;

