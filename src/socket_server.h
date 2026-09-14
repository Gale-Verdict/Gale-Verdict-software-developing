#pragma once

#include "charging_service.h"

#include <QTcpServer>

class SocketServer : public QTcpServer {
    Q_OBJECT

public:
    explicit SocketServer(ChargingService *service, QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    ChargingService *service_;
};
