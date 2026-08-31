#include "socket_server.h"

#include <QJsonDocument>
#include <QMetaObject>
#include <QTcpSocket>
#include <QThread>

SocketServer::SocketServer(ChargingService *service, QObject *parent)
    : QTcpServer(parent), service_(service) {}

void SocketServer::incomingConnection(qintptr socketDescriptor) {
    auto *workerThread = new QThread(this);
    auto *socket = new QTcpSocket();

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        socket->deleteLater();
        workerThread->deleteLater();
        return;
    }

    socket->moveToThread(workerThread);

    connect(workerThread, &QThread::started, socket, [socket, this]() {
        connect(socket, &QTcpSocket::readyRead, socket, [socket, this]() {
            const QString request = QString::fromUtf8(socket->readAll());
            QJsonObject response;
            QMetaObject::invokeMethod(
                service_,
                [&response, this, request]() { response = service_->handleRequest(request); },
                Qt::BlockingQueuedConnection);

            socket->write(QJsonDocument(response).toJson(QJsonDocument::Compact));
            socket->disconnectFromHost();
        });
    });

    connect(socket, &QTcpSocket::disconnected, workerThread, &QThread::quit);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);

    workerThread->start();
}
