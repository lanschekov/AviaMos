#include "clientapi.h"

// Инициализация статических членов
ClientAPI* ClientAPI::p_instance = nullptr;
SingletonDestroyer ClientAPI::destroyer;

void SingletonDestroyer::initialize(ClientAPI* p) {
    p_instance = p;
}

SingletonDestroyer::~SingletonDestroyer() {
    delete p_instance;
}

ClientAPI::ClientAPI(QObject *parent) : QObject(parent) {
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &ClientAPI::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientAPI::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientAPI::onReadyRead);
}

ClientAPI::~ClientAPI() {
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
    delete m_socket;
}

ClientAPI* ClientAPI::getInstance() {
    if (!p_instance) {
        p_instance = new ClientAPI();
        destroyer.initialize(p_instance);
    }
    return p_instance;
}

void ClientAPI::connectToServer(const QString& host, quint16 port) {
    m_socket->connectToHost(host, port);
}

void ClientAPI::sendCommand(const QString& command) {
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray data = command.toUtf8() + "\r\n";
        m_socket->write(data);
    }
}

bool ClientAPI::isConnected() const {
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void ClientAPI::onConnected() {
    emit connected();
    qDebug() << "Connected to server";
}

void ClientAPI::onDisconnected() {
    emit disconnected();
    qDebug() << "Disconnected from server";
}

QString ClientAPI::getCurrentUser() const {
    return currentUser;
}

void ClientAPI::onReadyRead() {
    QByteArray data = m_socket->readAll();
    QString response = QString::fromUtf8(data).trimmed();

    emit dataReceived(response);

    if(response.startsWith("auth+")) {
        currentUser = response.split(' ')[1]; // Сохраняем логин пользователя
        emit authSuccess();
    }
    else if(response.startsWith("REG+")) {
        emit regSuccess();  // Успешная регистрация
    }
    else if(response.startsWith("auth-") || response.startsWith("REG-")) {
        emit authFailed();  // Неудачная попытка
    }
}
