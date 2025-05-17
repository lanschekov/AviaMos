#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QDate>
#include "databasesingleton.h"
#include "clientapi.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("АВИАМОС");
    // Настройка дат
    ui->dateEditWhen->setDisplayFormat("dd.MM.yyyy");

    // Инициализация таблицы
    ui->tableWidgetTickets->setColumnCount(7);
    QStringList headers = {"Id", "Откуда", "Куда", "Дата вылета", "Дата возврата", "Класс", "Цена"};
    ui->tableWidgetTickets->setHorizontalHeaderLabels(headers);

    // Настройка даты
    ui->dateEditWhen->setDate(QDate::currentDate());
    ui->dateEditWhen->setMinimumDate(QDate::currentDate());
    ui->dateEditBack->setDate(QDate::currentDate());
    ui->dateEditBack->setMinimumDate(QDate::currentDate());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonSearch_clicked()
{
    // Очищаем таблицу
    ui->tableWidgetTickets->setRowCount(0);

    // Получаем параметры поиска
    QString from = ui->lineEditFrom->text().trimmed();
    QString to = ui->lineEditTo->text().trimmed();
    QDate departureDate = ui->dateEditWhen->date();
    QString flightClass = ui->comboBoxClass->currentText();
    QString returnDateStr = ui->dateEditBack->text().trimmed();

    // Формируем SQL-запрос (используем простые имена полей)
    QString sql =
        "SELECT id, departure, destination, "
        "strftime('%d.%m.%Y', departure_date) as dep_date, "
        "CASE WHEN return_date IS NULL THEN '-' "
        "ELSE strftime('%d.%m.%Y', return_date) END as ret_date, "
        "flight_class, price "
        "FROM Tickets WHERE booked_by IS NULL ";

    // Добавляем условия
    QStringList conditions;
    if (!from.isEmpty()) conditions.append("departure LIKE '%" + from + "%'");
    if (!to.isEmpty()) conditions.append("destination LIKE '%" + to + "%'");
    if (flightClass != "Все") conditions.append("flight_class = '" + flightClass + "'");
    if (departureDate.isValid()) conditions.append("date(departure_date) = date('" + departureDate.toString("yyyy-MM-dd") + "')");

    if (!returnDateStr.isEmpty()) {
        QDate returnDate = QDate::fromString(returnDateStr, "dd.MM.yyyy");
        if (returnDate.isValid()) {
            conditions.append("(return_date IS NULL OR date(return_date) = date('" + returnDate.toString("yyyy-MM-dd") + "'))");
        }
    }

    if (!conditions.isEmpty()) {
        sql += " AND " + conditions.join(" AND ");
    }

    qDebug() << "SQL Query:" << sql;

    // Выполняем запрос
    QSqlQuery query;
    if (!query.exec(sql)) {
        QMessageBox::warning(this, "Ошибка", "Ошибка запроса: " + query.lastError().text());
        return;
    }

    // Заполняем таблицу
    while (query.next()) {
        int row = ui->tableWidgetTickets->rowCount();
        ui->tableWidgetTickets->insertRow(row);

        QTableWidgetItem *idItem = new QTableWidgetItem(query.value("id").toString());
        ui->tableWidgetTickets->setItem(row, 0, idItem);

        // Проверяем и форматируем даты
        QString depDate = query.value(3).toString(); // 4-й столбец (индекс 3)
        QString retDate = query.value(4).toString();  // 5-й столбец (индекс 4)

        // Если дата в неправильном формате, пробуем преобразовать
        if (depDate.contains("%")) {
            QDate date = QDate::fromString(query.value("departure_date").toString(), "yyyy-MM-dd");
            depDate = date.toString("dd.MM.yyyy");
        }

        if (retDate.contains("%") && retDate != "-") {
            QDate date = QDate::fromString(query.value("return_date").toString(), "yyyy-MM-dd");
            retDate = date.toString("dd.MM.yyyy");
        }

        // Заполняем строку таблицы
        ui->tableWidgetTickets->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        ui->tableWidgetTickets->setItem(row, 1, new QTableWidgetItem(query.value("departure").toString()));
        ui->tableWidgetTickets->setItem(row, 2, new QTableWidgetItem(query.value("destination").toString()));
        ui->tableWidgetTickets->setItem(row, 3, new QTableWidgetItem(depDate));
        ui->tableWidgetTickets->setItem(row, 4, new QTableWidgetItem(retDate));
        ui->tableWidgetTickets->setItem(row, 5, new QTableWidgetItem(query.value("flight_class").toString()));
        ui->tableWidgetTickets->setItem(row, 6, new QTableWidgetItem(QString::number(query.value("price").toDouble(), 'f', 2) + " ₽"));
    }

    statusBar()->showMessage(QString("Найдено билетов: %1").arg(ui->tableWidgetTickets->rowCount()), 3000);
}

void MainWindow::on_pushButtonBook_clicked()
{
    // 1. Проверка авторизации
    QString username = ClientAPI::getInstance()->getCurrentUser();
    if (username.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Требуется авторизация!");
        return;
    }

    // 2. Проверка выбора билета
    int currentRow = ui->tableWidgetTickets->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите билет из списка");
        return;
    }

    // 3. Получаем ID билета из скрытого столбца
    QTableWidgetItem *idItem = ui->tableWidgetTickets->item(currentRow, 0);
    if (!idItem) {
        QMessageBox::critical(this, "Ошибка", "Не удалось получить ID билета");
        return;
    }

    // 4. Проверяем и преобразуем ID
    bool ok;
    int ticketId = idItem->text().toInt(&ok);
    if (!ok || ticketId <= 0) {

        qDebug() << "Invalid ticket ID:" << idItem->text()
                 << "Row:" << currentRow
                 << "Column:" << 0
                 << "All row data:";

        for (int col = 0; col < ui->tableWidgetTickets->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidgetTickets->item(currentRow, col);
            qDebug() << "Column" << col << ":" << (item ? item->text() : "NULL");
        }

        QMessageBox::critical(this, "Ошибка",
                              QString("Неверный ID билета: %1\nПопробуйте обновить список")
                                  .arg(idItem->text()));
        return;
    }

    // 5. Выполняем бронирование
    DatabaseSingleton* db = DatabaseSingleton::getInstance();
    if (!db->bookTicket(ticketId, username)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось забронировать билет");
        return;
    }

    QMessageBox::information(this, "Успех", "Билет успешно забронирован!");
    on_pushButtonSearch_clicked(); // Обновляем список
}
