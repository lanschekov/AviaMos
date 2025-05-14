#include "ticket.h"

Ticket::Ticket() :
    from(""), to(""), date(QDate::currentDate()), returnDate(QDate::currentDate()), flightClass("Эконом"), price(0)
{
}

Ticket::Ticket(QString from, QString to, QDate date, QDate returnDate, QString flightClass, double price) :
    from(from), to(to), date(date), returnDate(returnDate), flightClass(flightClass), price(price)
{
}

QString Ticket::getFrom() const
{
    return from;
}

QString Ticket::getTo() const
{
    return to;
}

QDate Ticket::getDate() const
{
    return date;
}

QDate Ticket::getReturnDate() const
{
    return returnDate;
}

QString Ticket::getFlightClass() const
{
    return flightClass;
}

double Ticket::getPrice() const
{
    return price;
}
