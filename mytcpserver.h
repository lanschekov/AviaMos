#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QDateTime>
#include "databasesingleton.h"
#include <QtNetwork>
#include <QByteArray>
#include <QDebug>

class MyTcpServer : public QObject {
    Q_OBJECT
public:
    void startServer();
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();

private slots:
    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();

private:
    QTcpServer *mTcpServer;
    QMap<int, QTcpSocket*> mClients;
    QMap<QTcpSocket*, QByteArray> m_buffers;

    void processCommand(QTcpSocket* client, const QString& command);
    void handleRegistration(QTcpSocket* client, const QString& login, const QString& password);
    void handleLogin(QTcpSocket* client, const QString& login, const QString& password);
    void log(const QString& message);
};

#endif // MYTCPSERVER_H
