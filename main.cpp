#include <QApplication>
#include "loginwindow.h"
#include "mainwindow.h"
#include "mytcpserver.h"
#include "databasesingleton.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Инициализация базы данных (из первого проекта)
    DatabaseSingleton* db = DatabaseSingleton::getInstance();
    if (!db->getDatabase().isOpen()) {
        qDebug() << "FATAL: Database connection failed!";
        return 1;
    }

    // Запуск сервера в отдельном потоке (из первого проекта)
    QThread serverThread;
    MyTcpServer server;
    server.moveToThread(&serverThread);
    QObject::connect(&serverThread, &QThread::started, &server, &MyTcpServer::startServer);
    serverThread.start();

    // Создание окон (логика из обоих проектов)
    LoginWindow loginWindow;
    MainWindow mainWindow;

    // Соединяем сигнал успешного логина с отображением главного окна
    QObject::connect(&loginWindow, &LoginWindow::loginSucceeded, [&]() {
        mainWindow.show();
        loginWindow.close();
    });

    loginWindow.show();
    return a.exec();
}
