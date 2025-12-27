#include "TlsClientManager.h"
#include "Logger.h"

#include <QJsonObject>
#include <QJsonDocument>

TlsClientManager::TlsClientManager(QObject* parent)
    : QObject(parent)
    , m_client(nullptr)
    , m_iReqID(0)
{
    m_client = new TlsClient();
    m_client->moveToThread(&m_thread);

    // 线程结束 → 自动释放 TlsClient
    connect(&m_thread, &QThread::finished,
        m_client, &QObject::deleteLater);

    // 转发子线程信号到主线程
    connect(m_client, &TlsClient::connected,
        this, &TlsClientManager::connected);
    connect(m_client, &TlsClient::disconnected,
        this, &TlsClientManager::disconnected);
    connect(m_client, &TlsClient::dataReceived,
        this, &TlsClientManager::OnDataReceived);
    connect(m_client, &TlsClient::errorOccurred,
        this, &TlsClientManager::errorOccurred);

    m_thread.start();
}

TlsClientManager::~TlsClientManager()
{
    m_thread.quit();
    m_thread.wait();
}

void TlsClientManager::connectToServer(const QString& host, quint16 port)
{
    QMetaObject::invokeMethod(m_client, "connectToServer",
        Qt::QueuedConnection,
        Q_ARG(QString, host),
        Q_ARG(quint16, port));
}

void TlsClientManager::disconnectFromServer()
{
    QMetaObject::invokeMethod(m_client, "disconnectFromServer",
        Qt::QueuedConnection);
}

TlsSingleRequestPtr TlsClientManager::SendRequest(QJsonObject jsData)
{
    TlsSingleRequestPtr pReq = std::make_shared<TlsSingleRequest>(++m_iReqID);
    m_mapReqs.insert({ pReq->GetRequestId(), pReq });
    jsData["requestId"] = pReq->GetRequestId();
    
    QMetaObject::invokeMethod(m_client, "SendRequest",
        Qt::QueuedConnection,
        Q_ARG(QJsonObject, jsData));

    return pReq;
}

void TlsClientManager::OnDataReceived( QJsonObject jsResponse)
{
    int iRequestid = jsResponse["requestId"].toInt();
    int iMsgId = jsResponse["msgid"].toInt();

    QJsonDocument doc(jsResponse);

    LOG_DEBUG(doc.toJson().toStdString());

    if (iMsgId == 5)
    {
        // 聊天消息
        emit SignalMsgReceived(jsResponse);
        return;
    }
    TlsSingleRequestPtr pReq = m_mapReqs[iRequestid];
    if (pReq && pReq->GetRequestId() == iRequestid)
    {
        emit pReq.get()->dataReceived(jsResponse);
    }
}