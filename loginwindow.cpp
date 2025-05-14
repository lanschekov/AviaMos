#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow),
    isRegistrationMode(false)
{
    ui->setupUi(this);
    setWindowTitle("AviaMos Login");
    ui->leEmail->setVisible(false);
    ui->label_3->setVisible(false);

    // Подключаемся к серверу
    ClientAPI::getInstance()->connectToServer("localhost", 33333);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_btnChange_clicked()
{
    isRegistrationMode = !isRegistrationMode;
    ui->leEmail->setVisible(isRegistrationMode);
    ui->label_3->setVisible(isRegistrationMode);

    if(isRegistrationMode) {
        ui->btnAuth->setText("Registration");
    } else {
        ui->btnAuth->setText("Authorization");
    }
}

void LoginWindow::on_btnAuth_clicked()
{
    QString login = ui->leLogin->text();
    QString password = ui->lePassword->text();

    if(login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields");
        return;
    }

    if(isRegistrationMode) {
        ClientAPI::getInstance()->sendCommand("register " + login + " " + password);
    } else {
        ClientAPI::getInstance()->sendCommand("login " + login + " " + password);
    }

    connect(ClientAPI::getInstance(), &ClientAPI::dataReceived,
            this, &LoginWindow::handleServerResponse);
}

void LoginWindow::handleServerResponse(const QString &response) {
    disconnect(ClientAPI::getInstance(), &ClientAPI::dataReceived,
               this, &LoginWindow::handleServerResponse);

    if(response.startsWith("auth+")) {
        // Успешная авторизация
        QMessageBox::information(this, "Успех", "Авторизация прошла успешно!");
        emit loginSucceeded();
    }
    else if(response.startsWith("REG+")) {
        // Успешная регистрация
        QMessageBox::information(this, "Успех", "Регистрация прошла успешно!\nТеперь вы можете авторизоваться");
        isRegistrationMode = false;
        ui->btnAuth->setText("Authorization");
        ui->leEmail->setVisible(false);
        ui->label_3->setVisible(false);
    }
    else {
        // Ошибка
        QMessageBox::warning(this, "Ошибка",
                             isRegistrationMode ? "Регистрация не удалась: такой логин уже существует"
                                                : "Неверный логин или пароль");
    }
}
