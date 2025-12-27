#pragma once

#include <QObject>
#include <QThread>
#include "TlsClient.h"
#include "TlsSingleRequest.h"
#include <map>

class TlsClientManager : public QObject
{
    Q_OBJECT
public:
    explicit TlsClientManager(QObject* parent = nullptr);
    ~TlsClientManager();

    void connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();
    
    TlsSingleRequestPtr SendRequest(QJsonObject jsData);
signals:
    // ×ª·¢¸ø UI
    void connected();
    void disconnected();
    void errorOccurred(const QString& error);
    void SignalMsgReceived(const QJsonObject& json);

private slots:
    void OnDataReceived(QJsonObject jsResponse);


private:
    QThread m_thread;
    TlsClient* m_client;
    std::map<int,std::shared_ptr<TlsSingleRequest>> m_mapReqs;
    std::atomic_int m_iReqID;
};


