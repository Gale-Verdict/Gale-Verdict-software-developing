#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QSqlDatabase>

class ChargingRepository {
public:
    explicit ChargingRepository(const QString &dbPath);

    bool initialize();
    QJsonArray findStationsByCity(const QString &city);
    int startSession(const QString &userId, int stationId);
    bool endSession(int sessionId);
    QJsonArray utilizationByStation();

private:
    QSqlDatabase database_;
};
