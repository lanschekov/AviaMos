#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("АВИАМОС");

    // Настройка валидатора даты (формат дд.мм.гггг)
    QRegularExpressionValidator* dateValidator = new QRegularExpressionValidator(
        QRegularExpression("\\d{2}\\.\\d{2}\\.\\d{4}"), this);
    ui->dateEditBack->setValidator(dateValidator);
    ui->dateEditBack->setPlaceholderText("дд.мм.гггг");

    // Установка начальных дат
    ui->dateEditWhen->setDate(QDate::currentDate());
    ui->dateEditBirth->setDate(QDate::currentDate().addYears(-30));

    // Настройка таблицы билетов
    ui->tableWidgetTickets->setColumnCount(6);
    ui->tableWidgetTickets->setHorizontalHeaderLabels(
        QStringList() << "Откуда" << "Куда" << "Дата" << "Обратно" << "Класс" << "Цена");
    ui->tableWidgetTickets->horizontalHeader()->setStretchLastSection(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonSearch_clicked()
{
    QString from = ui->lineEditFrom->text();
    QString to = ui->lineEditTo->text();
    QDate date = ui->dateEditWhen->date();
    QString dateBackStr = ui->dateEditBack->text();

    // Проверка формата даты
    QDate dateBack = QDate::fromString(dateBackStr, "dd.MM.yyyy");
    if (!dateBack.isValid()) {
        QMessageBox::warning(this, "Ошибка", "Некорректная дата обратного вылета!");
        return;
    }

    // Очистка и заполнение таблицы (заглушка)
    ui->tableWidgetTickets->setRowCount(0);
    for (int i = 0; i < 3; ++i) {
        int row = ui->tableWidgetTickets->rowCount();
        ui->tableWidgetTickets->insertRow(row);

        ui->tableWidgetTickets->setItem(row, 0, new QTableWidgetItem(from));
        ui->tableWidgetTickets->setItem(row, 1, new QTableWidgetItem(to));
        ui->tableWidgetTickets->setItem(row, 2, new QTableWidgetItem(date.toString("dd.MM.yyyy")));
        ui->tableWidgetTickets->setItem(row, 3, new QTableWidgetItem(dateBackStr));
        ui->tableWidgetTickets->setItem(row, 4, new QTableWidgetItem(ui->comboBoxClass->currentText()));
        ui->tableWidgetTickets->setItem(row, 5, new QTableWidgetItem(QString::number(5000 + i*2000) + " ₽"));
    }
}

void MainWindow::on_pushButtonBook_clicked()
{
    if (ui->lineEditLastName->text().isEmpty() ||
        ui->lineEditFirstName->text().isEmpty() ||
        ui->lineEditDocNumber->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля пассажира");
        return;
    }

    if (ui->tableWidgetTickets->currentRow() == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите рейс для бронирования");
        return;
    }

    QMessageBox::information(this, "Успех", "Билет успешно оформлен!");
}
