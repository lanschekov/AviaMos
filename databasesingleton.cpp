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

QMutex DatabaseSingleton::m_mutex; // Добавляем в начало файла

DatabaseSingleton::DatabaseSingleton() {
    QMutexLocker locker(&m_mutex); // Защищаем инициализацию

    db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = QCoreApplication::applicationDirPath() + "/" + DatabaseName;
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Database connection error:" << db.lastError().text();
        return;
    }

    // Оптимизации SQLite
    QSqlQuery query(db);
    query.exec("PRAGMA journal_mode=WAL");
    query.exec("PRAGMA synchronous=NORMAL");
    query.exec("PRAGMA foreign_keys=ON");
}


DatabaseSingleton::~DatabaseSingleton()
{
    QString connectionName = db.connectionName();

    if (db.isOpen()) {
        db.close();
    }

    QSqlDatabase::removeDatabase(connectionName);
}

DatabaseSingleton* DatabaseSingleton::getInstance() {
    if (!p_instance) {
        p_instance = new DatabaseSingleton();
        destroyer.initialize(p_instance);
    }
    return p_instance;
}

bool DatabaseSingleton::reg(const QString& login, const QString& password, const QString& status) {
    QMutexLocker locker(&m_mutex); // Защищаем метод

    if (!db.isOpen()) {
        qDebug() << "Database not open! Reopening...";
        if (!db.open()) {
            qDebug() << "Failed to reopen database:" << db.lastError().text();
            return false;
        }
    }

    QSqlQuery query(db);
    query.prepare("SELECT login FROM User WHERE login = ?");
    query.addBindValue(login);

    if (!query.exec()) {
        qDebug() << "Check user error:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        return false; // Пользователь уже существует
    }

    query.prepare("INSERT INTO User (login, password, status) VALUES (?, ?, ?)");
    query.addBindValue(login);
    query.addBindValue(password);
    query.addBindValue(status);

    return query.exec();
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

bool DatabaseSingleton::createTicketsTable() {
    QSqlQuery query(db);
    QString createTableQuery =
        "CREATE TABLE IF NOT EXISTS Tickets ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "departure VARCHAR(50) NOT NULL, "
        "destination VARCHAR(50) NOT NULL, "
        "departure_date DATE NOT NULL, "
        "return_date DATE, "
        "flight_class VARCHAR(20) NOT NULL, "
        "price REAL NOT NULL, "
        "booked_by VARCHAR(20) DEFAULT NULL, "
        "FOREIGN KEY(booked_by) REFERENCES User(login))";

    return query.exec(createTableQuery);
}

QVector<Ticket> DatabaseSingleton::getAvailableTickets() {
    QVector<Ticket> tickets;
    QSqlQuery query(db);
    query.exec("SELECT * FROM Tickets WHERE booked_by IS NULL");

    while (query.next()) {
        // Преобразуем дату из SQLite формата (YYYY-MM-DD)
        QDate depDate = QDate::fromString(query.value("departure_date").toString(), Qt::ISODate);

        Ticket ticket(
            query.value("departure").toString(),
            query.value("destination").toString(),
            depDate,
            query.value("return_date").isNull() ?
                QDate() : QDate::fromString(query.value("return_date").toString(), Qt::ISODate),
            query.value("flight_class").toString(),
            query.value("price").toDouble()
            );
        tickets.append(ticket);
    }
    return tickets;
}

bool DatabaseSingleton::bookTicket(int ticketId, const QString& username) {
    QSqlQuery query(db);
    query.prepare("UPDATE Tickets SET booked_by = :username WHERE id = :id AND booked_by IS NULL");
    query.bindValue(":username", username);
    query.bindValue(":id", ticketId);

    return query.exec() && query.numRowsAffected() > 0;
}

bool DatabaseSingleton::addTicket(const Ticket& ticket) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO Tickets (departure, destination, departure_date, return_date, flight_class, price) "
                  "VALUES (:departure, :destination, :departure_date, :return_date, :flight_class, :price)");
    query.bindValue(":departure", ticket.getFrom());
    query.bindValue(":destination", ticket.getTo());
    query.bindValue(":departure_date", ticket.getDate());
    query.bindValue(":return_date", ticket.getReturnDate());
    query.bindValue(":flight_class", ticket.getFlightClass());
    query.bindValue(":price", ticket.getPrice());

    return query.exec();
}

DatabaseSingleton* DatabaseSingleton::p_instance = nullptr;
DatabaseSingletonDestroyer DatabaseSingleton::destroyer;
