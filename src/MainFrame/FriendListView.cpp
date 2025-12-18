#include "FriendListView.h"
#include <QTimer>
#include <QScrollBar>
#include <QBrush>
#include <QPainter>
#include <QDebug>
#include <algorithm>
#include "JQAppManager.h"
#include "TcpClientManager.h"
#include "JQChatMainFrame.h"

FriendWidget::FriendWidget(const UserInfo_t& info)
	: m_pNickname(nullptr)
	, m_pAvatar(nullptr)
{
	setAcceptedMouseButtons(Qt::LeftButton);
	setAcceptHoverEvents(true);

	m_userInfo = info;

	m_pNickname = new QGraphicsTextItem(this);
	m_pNickname->setPlainText(m_userInfo.strUserName.c_str());

	m_pAvatar = new QGraphicsPixmapItem(this);
	QString strPath = ":/JQChat/images/icon.png";
	m_pAvatar->setPixmap(QPixmap(/*m_userInfo.strAvatarpath.c_str()*/strPath).scaled(QSize(30,30)));
	m_pAvatar->setVisible(true);
}
FriendWidget::~FriendWidget()
{

}

void FriendWidget::AddUnReadMsg(const MessageInfoPtr msg)
{
	m_listUnReadMsgs.push_back(msg);
	update();
}

void FriendWidget::ClearUnReadMsg()
{
	m_listUnReadMsgs.clear();
}

void FriendWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setPen(Qt::NoPen);
	painter->setBrush(QBrush(Qt::red));
	painter->drawEllipse(80, 10, 20, 20);
	painter->setPen(Qt::black);
	std::string strNum = std::to_string(m_listUnReadMsgs.size());
	painter->drawText(90, 15, strNum.c_str());
}

void FriendWidget::resizeEvent(QGraphicsSceneResizeEvent * e)
{
	m_pNickname->setPos(50, 10);
	m_pAvatar->setPos(10, 10);

	return QGraphicsWidget::resizeEvent(e);
}

void FriendWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	
	QGraphicsWidget::mouseReleaseEvent(event);
}

void FriendWidget::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		// 打开详情
		emit SignalOpenChat(this);
		e->accept();
		return;
	}
	QGraphicsWidget::mousePressEvent(e);
}

FriendListView::FriendListView(QWidget *parent)
	: QGraphicsView(parent)
{
	// 设置视图
	QGraphicsScene* pScene = new QGraphicsScene(this);
	setScene(pScene);
	// 设置是否可以滑动
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setAlignment(Qt::AlignTop | Qt::AlignLeft);

	qRegisterMetaType<FriendWidget>("FriendWidget");
	setDragMode(QGraphicsView::NoDrag);

	connect(JQApp.m_tcpclientManager, &TcpClientManager::SignalMsgReceived, this, &FriendListView::OnDealMsgReceived);
	
}

FriendListView::~FriendListView()
{
}

void FriendListView::AddFriends(const std::vector<UserInfo_t>& friendInfos)
{
	QGraphicsScene* pScene = scene();

	for (const auto& info : friendInfos)
	{
		FriendWidget* w = new FriendWidget(info);
		pScene->addItem(w);
		m_lstFriendwgt.push_back(w);

		connect(w, &FriendWidget::SignalOpenChat, this, &FriendListView::OnOpenChat);
	}

	QRect rc = viewport()->contentsRect();
	int y = 0;

	for (auto* item : m_lstFriendwgt)
	{
		item->resize(rc.width() - 1, 60);
		item->setPos(0, y);
		y += 60;
	}

	pScene->setSceneRect(0, 0, rc.width(), y);
	verticalScrollBar()->setValue(0);
}

void FriendListView::OnOpenChat(FriendWidget* w)
{
	if (w)
	{
		UserInfo_t info = w->GetUserInfo();
		emit SignalOpenChat(info,w->GetUnReadMsgs());

		w->ClearUnReadMsg();
	}
}

void FriendListView::ReceiveMsg(const QJsonObject& msg)
{
	int sendid = msg["id"].toInt();
	QString strMsg = msg["msg"].toString();
	QString strTime = msg["time"].toString();

	auto iter = std::find_if(m_lstFriendwgt.begin(), m_lstFriendwgt.end(), [=] (FriendWidget* wgt){
		if (wgt->GetUserInfo().iUserid == sendid)
			return true;
		return false;
	});

	if (iter != m_lstFriendwgt.end())
	{
		MessageInfoPtr ptr = std::make_shared<MessageInfo_t>(strTime.toStdString(), sendid, strMsg.toStdString());
		
		if (!JQApp.m_pChatMainFrame->AddUnReadMsg(ptr))
		{
			(*iter)->AddUnReadMsg(ptr);
		}
	}
}

void FriendListView::OnDealMsgReceived(const QJsonObject& msg)
{
	ReceiveMsg(msg);
}

