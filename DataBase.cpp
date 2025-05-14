#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include <QSqlQuery>
#include <QDir>
#include "databasesingleton.h"

void printUsersTable(DatabaseSingleton* db) {
    QSqlQuery query(db->getDatabase());

    if (!query.exec("SELECT * FROM User")) {
        qDebug() << "Error reading table:" << query.lastError().text();
        return;
    }

    QTextStream output(stdout);
    output.setCodec("UTF-8");

    // Шапка таблицы
    output << "\nUsers Table:\n";
    output << QString("| %1 | %2 | %3 |").arg("Login", -20).arg("Password", -20).arg("Status", -20) << "\n";
    output << "--------------------------------------------------------------------------------\n";

    // Данные
    while (query.next()) {
        QString login = query.value("login").toString();
        QString password = query.value("password").toString();
        QString status = query.value("status").toString();
        output << QString("| %1 | %2 | %3 |").arg(login, -20).arg(password, -20).arg(status, -20) << "\n";
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // Указываем явный путь к базе данных (как в сервере)
    const QString dbPath = QCoreApplication::applicationDirPath() + "/Test.db";
    qDebug() << "Using database:" << dbPath;

    DatabaseSingleton* db = DatabaseSingleton::getInstance();

    // Выводим текущие данные
    printUsersTable(db);

    QCoreApplication::quit();
    return 0;
}
