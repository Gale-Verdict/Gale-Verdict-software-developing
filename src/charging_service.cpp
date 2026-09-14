#include "charging_service.h"

#include <QStringList>

ChargingService::ChargingService(ChargingRepository *repository, QObject *parent)
    : QObject(parent), repository_(repository) {}

QJsonObject ChargingService::handleRequest(const QString &commandLine) {
    const QStringList parts = commandLine.trimmed().split(' ', Qt::SkipEmptyParts);
    if (parts.isEmpty()) {
        return { {"ok", false}, {"error", "empty command"} };
    }

    const QString command = parts[0].toUpper();
    if (command == "FIND_STATIONS" && parts.size() == 2) {
        return {{"ok", true}, {"stations", repository_->findStationsByCity(parts[1])}};
    }

    if (command == "START_SESSION" && parts.size() == 3) {
        const int sessionId = repository_->startSession(parts[1], parts[2].toInt());
        if (sessionId < 0) {
            return {{"ok", false}, {"error", "station unavailable"}};
        }
        return {{"ok", true}, {"sessionId", sessionId}};
    }

    if (command == "END_SESSION" && parts.size() == 2) {
        const bool ended = repository_->endSession(parts[1].toInt());
        return {{"ok", ended}, {"sessionEnded", ended}};
    }

    if (command == "UTILIZATION") {
        return {{"ok", true}, {"metrics", repository_->utilizationByStation()}};
    }

    return {{"ok", false}, {"error", "unsupported command"}};
}
