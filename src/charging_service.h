#pragma once

#include "charging_repository.h"

#include <QJsonObject>
#include <QObject>

class ChargingService : public QObject {
    Q_OBJECT

public:
    explicit ChargingService(ChargingRepository *repository, QObject *parent = nullptr);

public slots:
    QJsonObject handleRequest(const QString &commandLine);

private:
    ChargingRepository *repository_;
};
