#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent) {
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    if (!mTcpServer->listen(QHostAddress::Any, 33333)) {
        log("Server failed to start: " + mTcpServer->errorString());
    } else {
        log("Server started on port 33333");
    }
}

void MyTcpServer::slotNewConnection() {
    QTcpSocket* clientSocket = mTcpServer->nextPendingConnection();
    int clientId = clientSocket->socketDescriptor();
    mClients[clientId] = clientSocket;

    connect(clientSocket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);

    log("New connection from: " + clientSocket->peerAddress().toString());
    clientSocket->write("Connected to server. Use: register <user> <pass> or login <user> <pass>\r\n> ");
}

void MyTcpServer::slotServerRead() {
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    QByteArray rawData = clientSocket->readAll();

    QByteArray& buffer = m_buffers[clientSocket];
    buffer += rawData;

    while (buffer.contains('\n')) {
        int pos = buffer.indexOf('\n');
        QByteArray commandData = buffer.left(pos).trimmed();
        buffer = buffer.mid(pos + 1);

        QString command = QString::fromUtf8(commandData);
        log("Обработка команды: " + command);
        processCommand(clientSocket, command);
    }
}

void MyTcpServer::processCommand(QTcpSocket* client, const QString& command) {
    QStringList parts = command.split(' ', Qt::SkipEmptyParts);
    if (parts.isEmpty()) {
        client->write("ERROR: Empty command\r\n> ");
        return;
    }

    QString cmd = parts[0].toLower();
    if (cmd == "register") {
        if (parts.size() == 3) {
            handleRegistration(client, parts[1], parts[2]);
        } else {
            client->write("ERROR: Usage: register <username> <password>\r\n> ");
        }
    } else if (cmd == "login") {
        if (parts.size() == 3) {
            handleLogin(client, parts[1], parts[2]);
        } else {
            client->write("ERROR: Usage: login <username> <password>\r\n> ");
        }
    } else {
        client->write("ERROR: Unknown command. Available commands: register, login\r\n> ");
    }
}

void MyTcpServer::handleRegistration(QTcpSocket* client, const QString& login, const QString& password) {
    DatabaseSingleton* db = DatabaseSingleton::getInstance();

    if (!db->getDatabase().isOpen()) {
        client->write("REG- Ошибка подключения к базе данных\r\n");
        return;
    }

    bool success = db->reg(login, password);
    QString response = success ? "REG+ " + login + "\r\n" : "REG- " + (db->getDatabase().lastError().isValid() ? db->getDatabase().lastError().text() : "Логин уже существует") + "\r\n";
    client->write(response.toUtf8());
}

void MyTcpServer::handleLogin(QTcpSocket* client, const QString& login, const QString& password) {
    QStringList result = DatabaseSingleton::getInstance()->auth(login, password, 0);
    client->write(result.join(" ").toUtf8() + "\r\n");
    log("Попытка входа: " + login + " - " + result.join(" "));
}

void MyTcpServer::slotClientDisconnected() {
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    log("Client disconnected: " + clientSocket->peerAddress().toString());
    mClients.remove(clientSocket->socketDescriptor());
    clientSocket->deleteLater();
}

void MyTcpServer::log(const QString& message) {
    qDebug() << "[" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << "]" << message;
}

MyTcpServer::~MyTcpServer() {
    foreach (QTcpSocket* socket, mClients.values()) {
        socket->close();
    }
    mTcpServer->close();
}

void MyTcpServer::startServer()
{
    if (!mTcpServer->listen(QHostAddress::Any, 33333)) {
        log("Server failed to start: " + mTcpServer->errorString());
    } else {
        log("Server started on port 33333");
    }
}
