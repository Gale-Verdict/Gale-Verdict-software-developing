#include "charging_repository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

ChargingRepository::ChargingRepository(const QString &dbPath)
    : database_(QSqlDatabase::addDatabase("QSQLITE")) {
    database_.setDatabaseName(dbPath);
}

bool ChargingRepository::initialize() {
    if (!database_.open()) {
        return false;
    }

    QSqlQuery query(database_);
    if (!query.exec("CREATE TABLE IF NOT EXISTS stations ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL,"
                    "city TEXT NOT NULL,"
                    "connectors INTEGER NOT NULL,"
                    "available INTEGER NOT NULL)")) {
        return false;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS sessions ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "user_id TEXT NOT NULL,"
                    "station_id INTEGER NOT NULL,"
                    "status TEXT NOT NULL," 
                    "FOREIGN KEY(station_id) REFERENCES stations(id))")) {
        return false;
    }

    if (!query.exec("SELECT COUNT(*) FROM stations")) {
        return false;
    }

    if (query.next() && query.value(0).toInt() == 0) {
        query.prepare("INSERT INTO stations(name, city, connectors, available) VALUES(?, ?, ?, ?)");
        const QList<QList<QVariant>> seedRows = {
            {"浑南站", "沈阳", 12, 8},
            {"中街站", "沈阳", 8, 3},
            {"高新区站", "大连", 10, 6}
        };
        for (const auto &row : seedRows) {
            query.addBindValue(row[0]);
            query.addBindValue(row[1]);
            query.addBindValue(row[2]);
            query.addBindValue(row[3]);
            if (!query.exec()) {
                return false;
            }
        }
    }

    return true;
}

QJsonArray ChargingRepository::findStationsByCity(const QString &city) {
    QSqlQuery query(database_);
    query.prepare("SELECT id, name, connectors, available FROM stations WHERE city = ? ORDER BY available DESC");
    query.addBindValue(city);

    QJsonArray stations;
    if (!query.exec()) {
        return stations;
    }

    while (query.next()) {
        stations.append(QJsonObject{{"id", query.value(0).toInt()},
                                    {"name", query.value(1).toString()},
                                    {"connectors", query.value(2).toInt()},
                                    {"available", query.value(3).toInt()}});
    }
    return stations;
}

int ChargingRepository::startSession(const QString &userId, int stationId) {
    QSqlQuery updateStation(database_);
    updateStation.prepare("UPDATE stations SET available = available - 1 WHERE id = ? AND available > 0");
    updateStation.addBindValue(stationId);
    if (!updateStation.exec() || updateStation.numRowsAffected() == 0) {
        return -1;
    }

    QSqlQuery insertSession(database_);
    insertSession.prepare("INSERT INTO sessions(user_id, station_id, status) VALUES(?, ?, 'active')");
    insertSession.addBindValue(userId);
    insertSession.addBindValue(stationId);
    if (!insertSession.exec()) {
        return -1;
    }

    return insertSession.lastInsertId().toInt();
}

bool ChargingRepository::endSession(int sessionId) {
    QSqlQuery stationQuery(database_);
    stationQuery.prepare("SELECT station_id FROM sessions WHERE id = ? AND status = 'active'");
    stationQuery.addBindValue(sessionId);
    if (!stationQuery.exec() || !stationQuery.next()) {
        return false;
    }

    const int stationId = stationQuery.value(0).toInt();

    QSqlQuery endQuery(database_);
    endQuery.prepare("UPDATE sessions SET status = 'finished' WHERE id = ?");
    endQuery.addBindValue(sessionId);
    if (!endQuery.exec() || endQuery.numRowsAffected() == 0) {
        return false;
    }

    QSqlQuery releaseConnector(database_);
    releaseConnector.prepare("UPDATE stations SET available = available + 1 WHERE id = ?");
    releaseConnector.addBindValue(stationId);
    return releaseConnector.exec() && releaseConnector.numRowsAffected() > 0;
}

QJsonArray ChargingRepository::utilizationByStation() {
    QSqlQuery query(database_);
    query.prepare("SELECT id, name, connectors, available, (connectors - available) AS in_use FROM stations ORDER BY id");

    QJsonArray metrics;
    if (!query.exec()) {
        return metrics;
    }

    while (query.next()) {
        metrics.append(QJsonObject{{"stationId", query.value(0).toInt()},
                                   {"stationName", query.value(1).toString()},
                                   {"connectors", query.value(2).toInt()},
                                   {"available", query.value(3).toInt()},
                                   {"inUse", query.value(4).toInt()}});
    }
    return metrics;
}
