#pragma once

#include <QObject>
#include <QThread>
#include "TcpClient.h"
#include "TcpSingleRequest.h"
#include <map>

class TcpClientManager : public QObject
{
    Q_OBJECT
public:
    explicit TcpClientManager(QObject* parent = nullptr);
    ~TcpClientManager();

    void connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();
    TcpSingleRequestPtr SendRequest(QJsonObject jsData);

signals:
    // ×ª·¢¸ø UI
    void connected();
    void disconnected();
    void errorOccurred(const QString& error);
    void SignalMsgReceived(const QJsonObject& json);

private slots:
    void OnDataReceived(QByteArray data);

private:
    QThread m_thread;
    TcpClient* m_client;
    std::map<int,std::shared_ptr<TcpSingleRequest>> m_mapReqs;
    std::atomic_int m_iReqID;
};


