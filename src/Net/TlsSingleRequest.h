#pragma once

#include <QObject>
#include <functional>

class TlsSingleRequest : public QObject
{
    Q_OBJECT
public:
    TlsSingleRequest(int iReqId);
    ~TlsSingleRequest();

    int GetRequestId() { return m_iRequestId; }
signals:
    void dataReceived(const QJsonObject& jsResponse);

private:
    int             m_iRequestId;       // «Î«Ûid
};

using TlsSingleRequestPtr = std::shared_ptr<TlsSingleRequest>;
