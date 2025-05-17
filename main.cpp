#include <QApplication>
#include "loginwindow.h"
#include "mytcpserver.h"
#include "databasesingleton.h"
#include <ticket.h>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    DatabaseSingleton* db = DatabaseSingleton::getInstance();
    if (!db->getDatabase().isOpen()) {
        qDebug() << "FATAL: Database connection failed!";
        return 1;
    }

    // Запуск сервера в отдельном потоке
    QThread serverThread;
    MyTcpServer server;
    server.moveToThread(&serverThread);
    QObject::connect(&serverThread, &QThread::started, &server, &MyTcpServer::startServer);
    serverThread.start();

    // Создаем первое окно входа
    LoginWindow *loginWindow = new LoginWindow();
    loginWindow->show();

    return a.exec();
}
