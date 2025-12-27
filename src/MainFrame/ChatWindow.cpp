
#include "ChatWindow.h"
#include "MessageBubble.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QJsonObject>
#include <QJsonDocument>
#include "JQAppManager.h"
#include "TlsClientManager.h"


ChatWindow::ChatWindow(QWidget* parent)
	: QWidget(parent)
{
	setWindowTitle("Chat");
	resize(480, 640);


	QVBoxLayout* main = new QVBoxLayout(this);


	m_container = new QWidget;
	m_msgLayout = new QVBoxLayout(m_container);
	m_msgLayout->setAlignment(Qt::AlignTop);


	m_scroll = new QScrollArea;
	m_scroll->setWidget(m_container);
	m_scroll->setWidgetResizable(true);


	main->addWidget(m_scroll);


	QHBoxLayout* inputLay = new QHBoxLayout;
	m_input = new QTextEdit;
	m_input->setFixedHeight(60);


	m_sendBtn = new QPushButton("发送");
	inputLay->addWidget(m_input);
	inputLay->addWidget(m_sendBtn);


	main->addLayout(inputLay);


	connect(m_sendBtn, &QPushButton::clicked,
		this, &ChatWindow::onSendClicked);


	// 示例：好友发来一条
	addFriendMessage("你好 👋");
}

void ChatWindow::AddFriendInfo(UserInfo_t info)
{
	m_friendInfo = info;
}



void ChatWindow::AddMsgs(std::list<MessageInfoPtr> msgs)
{
	for (auto& msg : msgs)
	{
		addFriendMessage(msg->strMsgContent.c_str());
	}
}

bool ChatWindow::AddMsg(MessageInfoPtr ptr)
{
	if (ptr->iSenderId == m_friendInfo.iUserid)
	{
		addFriendMessage(ptr->strMsgContent.c_str());
		return true;
	}
	return false;
}

void ChatWindow::onSendClicked()
{
	QString text = m_input->toPlainText().trimmed();
	if (text.isEmpty()) return;


	addMyMessage(text);

	// 发送给服务器
	SendMessageToFriend(text);

	m_input->clear();
}


void ChatWindow::addMyMessage(const QString& text)
{
	auto* bubble = new MessageBubble(text, MessageBubble::Right);
	m_msgLayout->addWidget(bubble);
	m_scroll->verticalScrollBar()->setValue(m_scroll->verticalScrollBar()->maximum());
}


void ChatWindow::addFriendMessage(const QString& text)
{
	auto* bubble = new MessageBubble(text, MessageBubble::Left);
	m_msgLayout->addWidget(bubble);
	m_scroll->verticalScrollBar()->setValue(m_scroll->verticalScrollBar()->maximum());
}

void ChatWindow::SendMessageToFriend(const QString& strText)
{
	QJsonObject jsData;
	jsData["msgid"] = 5;
	jsData["id"] = JQApp.m_userInfo.iUserid;
	jsData["toid"] = m_friendInfo.iUserid;
	jsData["msg"] = strText;
	jsData["name"] = JQApp.m_userInfo.strUserName.c_str();
	jsData["time"] = getCurrentTime().c_str();

	JQApp.m_tlsclientManager->SendRequest(jsData);
}

std::string ChatWindow::getCurrentTime()
{
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	char date[60] = { 0 };
	sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
		(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
		(int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
	return std::string(date);
}