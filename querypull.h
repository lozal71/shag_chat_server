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
    QSqlQuery selectUserFromRoom(int roomID, int senderID);
    QSqlQuery selectCastMessage(int roomID);
    QSqlQuery selectUserName(int userID);
    QSqlQuery selectRole(int roleID);
    QSqlQuery selectRoomName(int roomID);
    QSqlQuery selectUserStatusID(QString userName);
    QSqlQuery insertInvitedUsers(int roomID, int userID, int statusID);
    QSqlQuery insertNewInvite(QString text, int roomID, int senderID,
                              int receiverID);
    QSqlQuery updateInviteAccept(int inviteID);
    QSqlQuery selectInvite (int receiverID);
    bool delRoom(int roomID);
    bool insertMessage(int roomID, int userID, QString text);
    int getNewRoomID(int userID, QString roomName);
private:
    QSqlQuery query;
    QMap<QString,QString> mapSetQuery;
};

#endif // QUERYPULL_H
