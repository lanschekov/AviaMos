#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include "clientapi.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void loginSucceeded();

private slots:
    void on_btnChange_clicked();
    void on_btnAuth_clicked();
    void handleServerResponse(const QString &response);

private:
    Ui::LoginWindow *ui;
    bool isRegistrationMode;
};

#endif // LOGINWINDOW_H
