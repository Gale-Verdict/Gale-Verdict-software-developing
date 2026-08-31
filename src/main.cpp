#include "charging_repository.h"
#include "charging_service.h"
#include "socket_server.h"

#include <QCoreApplication>
#include <QDebug>
#include <QHostAddress>
#include <QTcpSocket>
#include <QTimer>

namespace {
void runDemoClientRequest(const QString &request) {
    QTcpSocket socket;
    socket.connectToHost("127.0.0.1", 6789);
    if (!socket.waitForConnected(2000)) {
        qWarning() << "用户端连接失败";
        return;
    }

    socket.write(request.toUtf8());
    socket.waitForBytesWritten(1000);
    if (socket.waitForReadyRead(2000)) {
        qInfo().noquote() << QString::fromUtf8(socket.readAll());
    }
}
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    ChargingRepository repository("charging_platform.db");
    if (!repository.initialize()) {
        qCritical() << "数据库初始化失败";
        return 1;
    }

    ChargingService service(&repository);

    SocketServer server(&service);
    if (!server.listen(QHostAddress::AnyIPv4, 6789)) {
        qCritical() << "服务端启动失败";
        return 1;
    }

    QTimer::singleShot(200, &app, []() { runDemoClientRequest("FIND_STATIONS 沈阳"); });
    QTimer::singleShot(400, &app, []() { runDemoClientRequest("UTILIZATION"); });
    QTimer::singleShot(800, &app, &QCoreApplication::quit);

    return app.exec();
}
