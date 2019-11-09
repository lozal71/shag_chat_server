#ifndef QUERYPULL_H
#define QUERYPULL_H
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMap>
#include <QDateTime>

class queryPull: public QSqlQuery
{
public:
    queryPull();
    QSqlQuery auth(QString login, QString pass);
    QSqlQuery userOnLine(int id);
    QSqlQuery userOffLine(int id);
    QSqlQuery selectRooms(int id);
    QSqlQuery selectUnreadMessages(int roomID);
    QSqlQuery selectReadMessages(int roomID);
    bool insertMessage(int roomID, int userID, QString text);
private:
    QSqlQuery query;
    QMap<QString,QString> mapSetQuery;
};

#endif // QUERYPULL_H
