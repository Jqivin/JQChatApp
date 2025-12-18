#pragma once

#include <QtWidgets/QWidget>
#include "ui_JQChatMainFrame.h"
#include <vector>
#include "CommonDefines.h"

class JQChatMainFrame : public QWidget
{
	Q_OBJECT

public:
	JQChatMainFrame(QWidget *parent = Q_NULLPTR);

	bool AddUnReadMsg(MessageInfoPtr ptr);

public slots:
	void OnGetFriendsInfo(const std::vector<UserInfo_t>& friendInfos);

	void OnOpenChat(UserInfo_t info, std::list<MessageInfoPtr> msgs);

	void OnReceiveOfflineMsgs(const QVector<QJsonObject>& vecMsgs);

private:
	Ui::JQChatMainFrame ui;
};
