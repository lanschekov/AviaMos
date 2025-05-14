#include <QCoreApplication>
#include "mytcpserver.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    DatabaseSingleton* db = DatabaseSingleton::getInstance();
    if (!db->getDatabase().isOpen()) {
        qDebug() << "FATAL: Database failed to open!";
        return 1;
    }

    MyTcpServer server;
    qDebug() << "Aviation MOS Server running...";
    qDebug() << "Database location:" << db->getDatabase().databaseName();

    return a.exec();
}
