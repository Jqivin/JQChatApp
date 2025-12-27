#pragma once

#include <QObject>
#include <QSslSocket>
#include <QJsonObject>
#include <QJsonDocument>

class TlsClient : public QObject
{
    Q_OBJECT
public:
    explicit TlsClient(QObject* parent = nullptr);
    ~TlsClient();

    
    

signals:
    void connected();
    void disconnected();
    void dataReceived(const QJsonObject& json);
    void errorOccurred(const QString& err);

public slots:
    void connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();
    void SendRequest(const QJsonObject& json);

private slots:
    void onConnected();
    void onEncrypted();
    void onReadyRead();
    void onSslErrors(const QList<QSslError>& errors);
    void onError(QAbstractSocket::SocketError error);
    void onDisconnected();

private:
    QSslSocket* m_socket;
};
