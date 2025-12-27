#include "Login.h"
#include "JQAppManager.h"
#include "TcpClientManager.h"
#include <QJsonDocument>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QJsonArray>
#include <QGraphicsDropShadowEffect>

#include "Logger.h"
#include "TcpSingleRequest.h"

LoginDlg::LoginDlg(QWidget* parent)
	:QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	auto* frameless = new FramelessHelper(this);
	frameless->setTitleBarHeight(60);


	Init();

	qRegisterMetaType<std::vector<UserInfo_t>>("std::vector<UserInfo_t>");
	qRegisterMetaType<QVector<QJsonObject>>("QVector<QJsonObject>");

	// 加阴影
	// 创建一个QGraphicsDropShadowEffect对象，并设置模糊半径和阴影颜色
	QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);

	shadowEffect->setBlurRadius(10);
	shadowEffect->setColor(Qt::black);
	shadowEffect->setOffset(0, 0);

	// 将不规则图形应用阴影效果
	setGraphicsEffect(shadowEffect);
}

LoginDlg::~LoginDlg()
{

}

void LoginDlg::Init()
{
	connect(ui.btnLogin, &QPushButton::clicked, this, &LoginDlg::OnDealLogin);
	connect(ui.btnRegister, &QPushButton::clicked, this, &LoginDlg::OnDealRegiste);
	connect(ui.btnGotoLogin, &QPushButton::clicked, this, [=] {
		ui.stackedWidget->setCurrentIndex(0);
	});
	connect(ui.btnGotoRegister, &QPushButton::clicked, this, [=] {
		ui.stackedWidget->setCurrentIndex(1);
	});
	ui.stackedWidget->setCurrentIndex(0);
}

void LoginDlg::DealLoginSuccess(const QJsonObject& jsRes)
{
	UserInfo_t userInfo;
	userInfo.iUserid = jsRes["id"].toInt();
	userInfo.strUserName = jsRes["name"].toString().toStdString();
	userInfo.bOnline = true;

	JQApp.SetUserInfo(std::move(userInfo));

	// 朋友信息
	QJsonArray jsFriends = jsRes["friend"].toArray();
	std::vector<UserInfo_t> vecFriends;
	for (int i = 0; i < jsFriends.size(); ++i)
	{
		QJsonObject jsFriend = jsFriends[i].toObject();
		UserInfo_t friendInfo;
		int iyser = jsFriend["userid"].toInt();
		friendInfo.iUserid = jsFriend["userid"].toInt();
		friendInfo.strUserName = jsFriend["name"].toString().toStdString();
		friendInfo.bOnline = jsFriend["state"].toString() == "online";
		vecFriends.push_back(friendInfo);
		
	}
	if(jsFriends.size() > 0)
		emit SignalGetFriendInfo(vecFriends);

	// 未读消息
	QVector<QJsonObject> vecMsg;
	if (jsRes.contains("offlinemsg") && jsRes["offlinemsg"].isArray())
	{
		QJsonArray jsonMsgArray = jsRes["offlinemsg"].toArray();
		
		for (int i = 0; i < jsonMsgArray.size(); ++i)
		{
			QString jsonStr = jsonMsgArray[i].toString();
			QJsonDocument doc =	QJsonDocument::fromJson(jsonStr.toUtf8());
			QJsonObject jsMsg = doc.object();

			LOG_DEBUG(QString("offline msg  is %1").arg(jsonStr).toStdString());
			vecMsg.push_back(jsMsg);
		}
	}

	if (vecMsg.size() > 0)
	{
		emit SignalReceiveOfflineMsgs(vecMsg);
	}
}

void LoginDlg::OnDealLogin()
{
	int iuserid = ui.lineEditUserName->text().toInt();
	QString strPassword = ui.lineEditPwd->text();

	QJsonObject jsonData;
	jsonData["msgid"] = 2;
	jsonData["id"] = iuserid;
	jsonData["password"] = strPassword;

	TcpSingleRequestPtr pSingleReq = JQApp.m_tcpclientManager->SendRequest(jsonData);
	connect(pSingleReq.get(), &TcpSingleRequest::dataReceived, this, [=](const QJsonObject& jsRes) {

		int errcode = jsRes["errno"].toInt();
		if (10000 == errcode)
		{
			LOG_DEBUG("login success.");
			DealLoginSuccess(jsRes);
			accept();
		}
		else
		{
			
			std::string strLog = "login failed." + jsRes["errmsg"].toString().toStdString();
			LOG_ERROR(strLog);
		}
	});

	
}

void LoginDlg::OnDealRegiste()
{

}

void LoginDlg::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	QPainterPath path;
	path.addRoundedRect(rect(), 10, 10);

	painter.fillPath(path, QColor(255, 255, 255));
}
