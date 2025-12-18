#include "JQChatMainFrame.h"
#include "Logger.h"
#include <QJsonDocument>
#include <list>

JQChatMainFrame::JQChatMainFrame(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.friendView, &FriendListView::SignalOpenChat, this, &JQChatMainFrame::OnOpenChat);

	qRegisterMetaType<std::list<MessageInfoPtr>>("std::list<MessageInfoPtr>");
}

void JQChatMainFrame::OnGetFriendsInfo(const std::vector<UserInfo_t>& friendInfos)
{
	ui.friendView->AddFriends(friendInfos);
}

void JQChatMainFrame::OnOpenChat(UserInfo_t info, std::list<MessageInfoPtr> msgs)
{
	LOG_DEBUG(QString("open chat,user name is %1").arg(info.strUserName.c_str()).toStdString());

	ui.wgtChat->AddFriendInfo(info);
	ui.wgtChat->AddMsgs(msgs);
	ui.wgtChat->show();
}

void JQChatMainFrame::OnReceiveOfflineMsgs(const QVector<QJsonObject>& vecMsgs)
{
	LOG_DEBUG(QString("offline msg size is %1").arg(vecMsgs.size()).toStdString());

	for (int i = 0; i < vecMsgs.size(); ++i)
	{
		QString jsonStr = QString::fromUtf8(QJsonDocument(vecMsgs[i]).toJson(QJsonDocument::Compact));
		LOG_DEBUG(QString("offline msg  is %1").arg(jsonStr).toStdString());
		ui.friendView->ReceiveMsg(vecMsgs[i]);
	}
}

bool JQChatMainFrame::AddUnReadMsg(MessageInfoPtr ptr)
{
	return ui.wgtChat->AddMsg(ptr);
}

