#include "databasesingleton.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>

DatabaseSingletonDestroyer::~DatabaseSingletonDestroyer() {
    delete p_instance;
}

void DatabaseSingletonDestroyer::initialize(DatabaseSingleton* p) {
    p_instance = p;
}

DatabaseSingleton::DatabaseSingleton() {
    db = QSqlDatabase::addDatabase("QSQLITE");

    // Получаем путь к папке с исполняемым файлом
    QString dbPath = QCoreApplication::applicationDirPath() + "/" + DatabaseName;

    // Для отладки выведем путь
    qDebug() << "Database path:" << dbPath;

    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Database connection error:" << db.lastError().text();
        return;
    }

    qDebug() << "Database opened successfully at:" << db.databaseName();

    QSqlQuery query(db);
    QString createTableQuery =
        "CREATE TABLE IF NOT EXISTS User ("
        "login VARCHAR(20) NOT NULL UNIQUE, "
        "password VARCHAR(20) NOT NULL, "
        "status VARCHAR(20) NOT NULL)";

    if (!query.exec(createTableQuery)) {
        qDebug() << "Table creation error:" << query.lastError().text();
    } else {
        qDebug() << "Table User created or already exists";
    }
}

DatabaseSingleton::~DatabaseSingleton() {
    db.close();
}

DatabaseSingleton* DatabaseSingleton::getInstance() {
    if (!p_instance) {
        p_instance = new DatabaseSingleton();
        destroyer.initialize(p_instance);
    }
    return p_instance;
}

bool DatabaseSingleton::reg(const QString& login, const QString& password, const QString& status) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO User (login, password, status) VALUES (:login, :password, :status)");
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    query.bindValue(":status", status);

    if(!query.exec()) {
        qDebug() << "Ошибка регистрации:" << query.lastError().text();
        return false;
    }
    return true;
}

QStringList DatabaseSingleton::auth(const QString& login, const QString& password, int id_connection) {
    QSqlQuery query(db);
    query.prepare("SELECT status FROM User WHERE login = :login AND password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", password);

    if(query.exec() && query.next()) {
        return {"auth+", login, query.value("status").toString()};
    }
    return {"auth-"};
}


void DatabaseSingleton::printUsersTable() const {
    QSqlQuery query(db);

    if (!query.exec("SELECT * FROM User")) {
        qDebug() << "Error reading table:" << query.lastError().text();
        return;
    }

    qDebug() << "\nUsers Table:";
    qDebug() << "| Login               | Password            | Status             |";
    qDebug() << "--------------------------------------------------------------------------------";

    while (query.next()) {
        QString login = query.value("login").toString();
        QString password = query.value("password").toString();
        QString status = query.value("status").toString();
        qDebug() << "|" << login.leftJustified(20, ' ')
                 << "|" << password.leftJustified(20, ' ')
                 << "|" << status.leftJustified(20, ' ') << "|";
    }
}

DatabaseSingleton* DatabaseSingleton::p_instance = nullptr;
DatabaseSingletonDestroyer DatabaseSingleton::destroyer;
