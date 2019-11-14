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
    QSqlQuery selectRooms(int id, int userRole);
    QSqlQuery selectUnreadMessages(int roomID);
    QSqlQuery selectReadMessages(int roomID);
    QSqlQuery selectUserFromRoom(int roomID, int senderID);
    QSqlQuery selectCastMessage(int roomID);
    QSqlQuery selectUserName(int userID);
    bool delRoom(int roomID);
    bool insertMessage(int roomID, int userID, QString text);
    int getNewRoomID(int userID, QString roomName);
private:
    QSqlQuery query;
    QMap<QString,QString> mapSetQuery;
};

#endif // QUERYPULL_H
