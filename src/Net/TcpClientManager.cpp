#include "TcpClientManager.h"
#include "Logger.h"

#include <QJsonObject>
#include <QJsonDocument>

TcpClientManager::TcpClientManager(QObject* parent)
    : QObject(parent)
    , m_client(nullptr)
    , m_iReqID(0)
{
    m_client = new TcpClient();
    m_client->moveToThread(&m_thread);

    // 线程结束 → 自动释放 TcpClient
    connect(&m_thread, &QThread::finished,
        m_client, &QObject::deleteLater);

    // 转发子线程信号到主线程
    connect(m_client, &TcpClient::connected,
        this, &TcpClientManager::connected);
    connect(m_client, &TcpClient::disconnected,
        this, &TcpClientManager::disconnected);
    connect(m_client, &TcpClient::dataReceived,
        this, &TcpClientManager::OnDataReceived);
    connect(m_client, &TcpClient::errorOccurred,
        this, &TcpClientManager::errorOccurred);

    m_thread.start();
}

TcpClientManager::~TcpClientManager()
{
    m_thread.quit();
    m_thread.wait();
}

void TcpClientManager::connectToServer(const QString& host, quint16 port)
{
    QMetaObject::invokeMethod(m_client, "connectToServer",
        Qt::QueuedConnection,
        Q_ARG(QString, host),
        Q_ARG(quint16, port));
}

void TcpClientManager::disconnectFromServer()
{
    QMetaObject::invokeMethod(m_client, "disconnectFromServer",
        Qt::QueuedConnection);
}

TcpSingleRequestPtr TcpClientManager::SendRequest(QJsonObject jsData)
{
    TcpSingleRequestPtr pReq = std::make_shared<TcpSingleRequest>(++m_iReqID);
    m_mapReqs.insert({ pReq->GetRequestId(), pReq });
    jsData["requestId"] = pReq->GetRequestId();

    QJsonDocument doc(jsData);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    QString jsonPretty = doc.toJson(QJsonDocument::Indented);
    QByteArray data(jsonPretty.toLatin1());

    LOG_DEBUG(jsonPretty.toStdString());
    
    QMetaObject::invokeMethod(m_client, "SendRequest",
        Qt::QueuedConnection,
        Q_ARG(QByteArray, data));

    return pReq;
}

void TcpClientManager::OnDataReceived( QByteArray data)
{
    QJsonObject jsResponse = QJsonDocument::fromJson(data).object();
    int iRequestid = jsResponse["requestId"].toInt();
    int iMsgId = jsResponse["msgid"].toInt();

    LOG_DEBUG(QString(data).toStdString());

    if (iMsgId == 5)
    {
        // 聊天消息
        emit SignalMsgReceived(jsResponse);
        return;
    }
    TcpSingleRequestPtr pReq = m_mapReqs[iRequestid];
    if (pReq && pReq->GetRequestId() == iRequestid)
    {
        emit pReq.get()->dataReceived(jsResponse);
    }
}