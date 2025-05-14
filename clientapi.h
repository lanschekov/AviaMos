#ifndef CLIENTAPI_H
#define CLIENTAPI_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>

class ClientAPI;

class SingletonDestroyer
{
private:
    ClientAPI* p_instance;
public:
    ~SingletonDestroyer();
    void initialize(ClientAPI* p);
};

class ClientAPI: public QObject
{
    Q_OBJECT

private:
    static ClientAPI* p_instance;
    static SingletonDestroyer destroyer;
    QTcpSocket* m_socket;

protected:
    ClientAPI(QObject *parent = nullptr);
    ~ClientAPI();
    ClientAPI(const ClientAPI&) = delete;
    ClientAPI& operator=(ClientAPI&) = delete;
    friend class SingletonDestroyer;

public:
    static ClientAPI* getInstance();
    void connectToServer(const QString& host, quint16 port);
    void sendCommand(const QString& command);
    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void dataReceived(const QString& data);
    void authSuccess();
    void authFailed();
    void regSuccess();

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
};

#endif // CLIENTAPI_H
