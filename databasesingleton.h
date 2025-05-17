#ifndef DATABASESINGLETON_H
#define DATABASESINGLETON_H

#ifdef DATABASE_LIB
#define DATABASE_EXPORT Q_DECL_EXPORT
#else
#define DATABASE_EXPORT Q_DECL_IMPORT
#endif

#include <QMutex>
#include <QThread>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <ticket.h>

#define DatabaseName "Test.db"

class DatabaseSingleton;
class DatabaseSingletonDestroyer;

class DatabaseSingletonDestroyer {
private:
    DatabaseSingleton* p_instance;
public:
    ~DatabaseSingletonDestroyer();
    void initialize(DatabaseSingleton* p);
};

class DatabaseSingleton {
private:
    static QMutex m_mutex;
    static DatabaseSingleton* p_instance;
    static DatabaseSingletonDestroyer destroyer;
    QSqlDatabase db;

protected:
    DatabaseSingleton();
    DatabaseSingleton(const DatabaseSingleton&) = delete;
    DatabaseSingleton& operator=(const DatabaseSingleton&) = delete;
    ~DatabaseSingleton();
    friend class DatabaseSingletonDestroyer;

public:
    // Метод для доступа к базе данных
    QSqlDatabase getDatabase() const { return db; }

    static DatabaseSingleton* getInstance();
    bool reg(const QString& login, const QString& password, const QString& status = "user");
    QStringList auth(const QString& login, const QString& password, int id_connection);
    void printUsersTable() const;
    bool createTicketsTable();
    QVector<Ticket> getAvailableTickets();
    bool bookTicket(int ticketId, const QString& username);
    bool addTicket(const Ticket& ticket);
};

#endif // DATABASESINGLETON_H
