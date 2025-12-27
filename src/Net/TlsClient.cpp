#include "TlsClient.h"
#include "Logger.h"

#include <QFile>
#include <QSslConfiguration>
#include <QAbstractSocket>
#include <QNetworkProxy>

TlsClient::TlsClient(QObject* parent)
    : QObject(parent)
{
    m_socket = new QSslSocket(this);

    connect(m_socket, &QSslSocket::connected,
        this, &TlsClient::onConnected);

    connect(m_socket, &QSslSocket::readyRead,
        this, &TlsClient::onReadyRead);

    connect(m_socket, &QSslSocket::disconnected,
        this, &TlsClient::onDisconnected);

    connect(m_socket,
        QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors),
        this,
        &TlsClient::onSslErrors);

    connect(m_socket, &QSslSocket::errorOccurred,
        this, &TlsClient::onError);

    connect(m_socket, &QSslSocket::encrypted,
        this, &TlsClient::onEncrypted);
}

TlsClient::~TlsClient()
{
    m_socket->disconnectFromHost();
}

void TlsClient::connectToServer(const QString& host, quint16 port)
{
    LOG_DEBUG(("Connecting TLS to " + host + ":" + QString::number(port)).toStdString());

    // 如果socket还在使用中，先断开
    if (m_socket->state() != QAbstractSocket::UnconnectedState)
    {
        LOG_DEBUG("Socket is not disconnected, disconnecting first...");
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState)
        {
            m_socket->waitForDisconnected(3000);
        }
    }

    // 禁用代理，直接连接（避免代理协议错误）
    m_socket->setProxy(QNetworkProxy::NoProxy);
    
    // 配置 SSL/TLS
    QSslConfiguration sslConf = m_socket->sslConfiguration();
    
    // 使用 VerifyNone 模式（开发/测试环境，因为服务器证书可能无法验证）
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    
    // 尝试只使用 TLS 1.2（更兼容）
    sslConf.setProtocol(QSsl::TlsV1_2);
    LOG_DEBUG("Using TLS 1.2 protocol");
    
    // 加载自签 CA（如果存在）
    QFile caFile(":/JQChat/ca/ca.crt");
    if (caFile.open(QIODevice::ReadOnly))
    {
        QSslCertificate caCert(caFile.readAll(), QSsl::Pem);
        caFile.close();
        
        if (caCert.isNull())
        {
            LOG_ERROR("Failed to parse CA certificate from resource");
        }
        else
        {
            LOG_DEBUG("CA certificate loaded successfully");
            QList<QSslCertificate> caList = sslConf.caCertificates();
            caList.append(caCert);
            sslConf.setCaCertificates(caList);
        }
    }
    else
    {
        LOG_DEBUG("CA certificate file not found in resources, using VerifyNone mode");
    }
    
    // 应用 SSL 配置
    m_socket->setSslConfiguration(sslConf);
    
    // 预先忽略所有 SSL 错误（开发/测试环境）
    // 这样可以避免在握手过程中因为证书问题而中断
    m_socket->ignoreSslErrors();
    
    LOG_DEBUG("Starting encrypted connection...");
    // 使用 connectToHostEncrypted 会自动设置 SNI
    m_socket->connectToHostEncrypted(host, port);
}

void TlsClient::disconnectFromServer()
{
    LOG_DEBUG("Disconnecting from server...");
    if (m_socket->state() != QAbstractSocket::UnconnectedState)
    {
        m_socket->disconnectFromHost();
    }
}

void TlsClient::SendRequest(const QJsonObject& json)
{
    if (!m_socket->isEncrypted())
        return;

    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);
    data.append('\n');   // 简单分包

    m_socket->write(data);


    QJsonDocument doc(json);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    QString jsonPretty = doc.toJson(QJsonDocument::Indented);
    LOG_DEBUG(jsonPretty.toLatin1().toStdString());
}

void TlsClient::onConnected()
{
    LOG_DEBUG("TCP connected, waiting for SSL handshake...");
    QString protocolStr = "Unknown";
    if (m_socket->protocol() == QSsl::TlsV1_2)
        protocolStr = "TLS 1.2";
    else if (m_socket->protocol() == QSsl::TlsV1_3)
        protocolStr = "TLS 1.3";
    else if (m_socket->protocol() == QSsl::TlsV1_0)
        protocolStr = "TLS 1.0";
    else if (m_socket->protocol() == QSsl::TlsV1_1)
        protocolStr = "TLS 1.1";
    LOG_DEBUG(QString("Socket state: %1, Encrypted: %2, Protocol: %3")
        .arg(m_socket->state())
        .arg(m_socket->isEncrypted())
        .arg(protocolStr).toStdString());
}

void TlsClient::onEncrypted()
{
    LOG_DEBUG("TLS encrypted, handshake completed.");
    emit connected();
}

void TlsClient::onReadyRead()
{
    QByteArray data = m_socket->readAll();

    LOG_DEBUG(data.data());

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error == QJsonParseError::NoError)
        emit dataReceived(doc.object());
}


void TlsClient::onSslErrors(const QList<QSslError>& errors)
{
    LOG_DEBUG("SSL errors detected, ignoring and continuing...");
    QString errorMsg = "SSL Errors: ";
    for (const auto& err : errors)
    {
        QString errStr = err.errorString();
        LOG_ERROR(("SSL Error: " + errStr).toStdString());
        errorMsg += errStr + "; ";
    }
    
    // 忽略 SSL 错误继续连接
    m_socket->ignoreSslErrors();
    
    // 不发出错误信号，因为我们决定忽略这些错误继续连接
    // emit errorOccurred(errorMsg);
}

void TlsClient::onError(QAbstractSocket::SocketError error)
{
    QString errorString = m_socket->errorString();
    LOG_ERROR(("Socket Error: " + QString::number(error) + " - " + errorString).toStdString());
    
    // 添加详细的诊断信息
    LOG_ERROR(QString("Socket state: %1, Encrypted: %2, Peer verify mode: %3")
        .arg(m_socket->state())
        .arg(m_socket->isEncrypted())
        .arg(m_socket->peerVerifyMode()).toStdString());
    
    QString errorMsg = QString("Connection error (%1): %2")
        .arg(error)
        .arg(errorString);
    emit errorOccurred(errorMsg);
}

void TlsClient::onDisconnected()
{
    LOG_DEBUG("Socket disconnected");
    emit disconnected();
}
