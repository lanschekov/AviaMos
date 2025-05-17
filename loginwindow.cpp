#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
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

    ClientAPI::getInstance()->connectToServer("localhost", 33333);

    // Подключаем сигналы ClientAPI к слотам LoginWindow
    connect(ClientAPI::getInstance(), &ClientAPI::authSuccess, this, &LoginWindow::onAuthSuccess);
    connect(ClientAPI::getInstance(), &ClientAPI::authFailed, this, &LoginWindow::onAuthFailed);
    connect(ClientAPI::getInstance(), &ClientAPI::regSuccess, this, &LoginWindow::onRegSuccess);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::onAuthSuccess() {
    // Создаем новое окно приложения
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();

    // Очищаем поля для следующего входа
    ui->leLogin->clear();
    ui->lePassword->clear();

    emit loginSucceeded();
}

void LoginWindow::onRegSuccess() {
    QMessageBox::information(this, "Успех", "Регистрация прошла успешно!\nТеперь вы можете авторизоваться");
    isRegistrationMode = false;
    ui->btnAuth->setText("Authorization");
    ui->leEmail->setVisible(false);
    ui->label_3->setVisible(false);

    // Очищаем поля
    ui->leLogin->clear();
    ui->lePassword->clear();
}

void LoginWindow::onAuthFailed() {
    QMessageBox::warning(this, "Ошибка",
                         isRegistrationMode ? "Регистрация не удалась: такой логин уже существует"
                                            : "Неверный логин или пароль");
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
}
