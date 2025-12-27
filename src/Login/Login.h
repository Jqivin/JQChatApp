#pragma once
#include <QtWidgets/QDialog>
#include "ui_Login.h"
#include "CommonDefines.h"
#include <QJsonObject>
#include <QDialog>
#include "FramelessHelper.h"

class LoginDlg : public QDialog
{
	Q_OBJECT
public:
	LoginDlg(QWidget* parent = nullptr);
	~LoginDlg();

private:
	void Init();
	// 处理登录成功数据
	void DealLoginSuccess(const QJsonObject& jsRes);


protected:
	void paintEvent(QPaintEvent*);

private slots:
	void OnDealLogin();
	void OnDealRegiste();

signals:
	void SignalGetFriendInfo(const std::vector<UserInfo_t>& friendInfos);
	void SignalReceiveOfflineMsgs(const QVector<QJsonObject>& vecMsgs);
private:
	Ui::Login ui;
};
