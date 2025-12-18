#include "TcpClient.h"
#include <QDebug>
#include <QThread>
#include <QNetworkProxy>

TcpClient::TcpClient(QObject* parent)
    : QObject(parent),
    m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::connected,
        this, &TcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected,
        this, &TcpClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead,
        this, &TcpClient::onReadyRead);
    connect(m_socket,
        QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
        this, &TcpClient::onError);
}

TcpClient::~TcpClient()
{
    if (m_socket->isOpen())
        m_socket->close();
}

void TcpClient::connectToServer(const QString& host, quint16 port)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        return;

    m_socket->setProxy(QNetworkProxy::NoProxy);

    m_socket->connectToHost(host, port);
}

void TcpClient::disconnectFromServer()
{
    m_socket->disconnectFromHost();
}

void TcpClient::SendRequest(const QByteArray& data)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->write(data);
}

void TcpClient::onConnected()
{
    qDebug() << "[Thread]" << QThread::currentThreadId() << "connected";
    emit connected();
}

void TcpClient::onDisconnected()
{
    emit disconnected();
}

void TcpClient::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    emit dataReceived(data);
}

void TcpClient::onError(QAbstractSocket::SocketError)
{
    qDebug() << "[Thread]" << m_socket->errorString() << "connected err";
    emit errorOccurred(m_socket->errorString());
}
