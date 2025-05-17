#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "ticket.h"
#include <stdexcept>
#include <QStatusBar>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonSearch_clicked();
    void on_pushButtonBook_clicked();

private:
    bool validateDate(const QString& date);
    Ui::MainWindow *ui;
    QVector<Ticket> foundTickets;
    void setupUI();
    void updateTotalPrice();
};

#endif // MAINWINDOW_H
