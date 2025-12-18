#pragma once

#include <QObject>
#include <functional>

class TcpSingleRequest : public QObject
{
    Q_OBJECT
public:
    TcpSingleRequest(int iReqId);
    ~TcpSingleRequest();

    int GetRequestId() { return m_iRequestId; }
signals:
    void dataReceived(const QJsonObject& jsResponse);

private:
    int             m_iRequestId;       // «Î«Ûid
};

using TcpSingleRequestPtr = std::shared_ptr<TcpSingleRequest>;
