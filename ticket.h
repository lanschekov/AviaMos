#ifndef TICKET_H
#define TICKET_H

#include <QString>
#include <QDate>

class Ticket
{
public:
    Ticket();
    Ticket(QString from, QString to, QDate date, QDate returnDate, QString flightClass, double price);

    QString getFrom() const;
    QString getTo() const;
    QDate getDate() const;
    QDate getReturnDate() const;
    QString getFlightClass() const;
    double getPrice() const;

private:
    QString from;
    QString to;
    QDate date;
    QDate returnDate;
    QString flightClass;
    double price;
};

#endif // TICKET_H
