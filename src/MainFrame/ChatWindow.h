#pragma once
#include <QWidget>
#include "CommonDefines.h"
#include <string>


class QVBoxLayout;
class QScrollArea;
class QTextEdit;
class QPushButton;


class ChatWindow : public QWidget
{
	Q_OBJECT
public:
	explicit ChatWindow(QWidget* parent = nullptr);

	void AddFriendInfo(UserInfo_t info);

	// 添加消息
	void AddMsgs(std::list<MessageInfoPtr> msgs);

	// 添加消息
	bool AddMsg(MessageInfoPtr msg);

signals:
	void sendMessage(const QString& text); // 可接 TCP


private slots:
	void onSendClicked();


private:
	QWidget* m_container;
	QVBoxLayout* m_msgLayout;
	QScrollArea* m_scroll;
	QTextEdit* m_input;
	QPushButton* m_sendBtn;
	UserInfo_t	m_friendInfo;

	void addMyMessage(const QString& text);
	void addFriendMessage(const QString& text);
	void SendMessageToFriend(const QString& strText);
	std::string getCurrentTime();
};