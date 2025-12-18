#pragma once
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <vector>
#include <list>
#include "CommonDefines.h"
#include <QGraphicsSceneMouseEvent>
#include <QJsonObject>

class FriendWidget : public QGraphicsWidget
{
	Q_OBJECT
public:
	FriendWidget(const UserInfo_t& info);
	~FriendWidget();

	UserInfo_t GetUserInfo() { return m_userInfo; }

	void AddUnReadMsg(const MessageInfoPtr msg);
	std::list<MessageInfoPtr> GetUnReadMsgs() { return m_listUnReadMsgs; }

	void ClearUnReadMsg();
protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
	virtual void resizeEvent(QGraphicsSceneResizeEvent *);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent*);

signals:
	void SignalOpenChat(FriendWidget*);
private:
	QGraphicsTextItem*		m_pNickname;
	QGraphicsPixmapItem*	m_pAvatar;
	UserInfo_t				m_userInfo;

	std::list<MessageInfoPtr>	m_listUnReadMsgs;
};

class FriendListView : public QGraphicsView
{
	Q_OBJECT
public:
	FriendListView(QWidget *parent = nullptr);
	~FriendListView();

	void AddFriends(const std::vector<UserInfo_t>& friendInfos);

	void ReceiveMsg(const QJsonObject& msg);

public slots:
	void OnOpenChat(FriendWidget* self);

	void OnDealMsgReceived(const QJsonObject& msg);

signals:
	void SignalOpenChat(UserInfo_t, std::list<MessageInfoPtr>);


private:
	std::list<FriendWidget*>	m_lstFriendwgt;
};

