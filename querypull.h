#ifndef QUERYPULL_H
#define QUERYPULL_H
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMap>
#include <QDateTime>

enum setQuery {getUserIdUserName, getWaitInvitation, getRoomName, getUserName,
              getRole, getUnreadMess, getReadMess, getMembers, getRooms,
              setOnLine, setOffLine};

class queryPull: public QSqlQuery
{
public:
    queryPull();
    QSqlQuery getUserIdUserName(QString login, QString pass);
    QSqlQuery getWaitInvitation (int userID);
    QSqlQuery getRoomName(int roomID);
    QSqlQuery getUserName(int userID);
    QSqlQuery getRole(int roleID);
    QSqlQuery getUnreadMess(int roomID);
    QSqlQuery getReadMess(int roomID);
    QSqlQuery getMembers(int roomID, int userID);
    QSqlQuery getRooms(int userID);

    QSqlQuery setOnLine(int userID);
    QSqlQuery setOffLine(int userID);


    QSqlQuery selectUserFromRoom(int roomID, int senderID);
    QSqlQuery selectUserIdFromRoom(int roomID);
    QSqlQuery selectCastMessage(int roomID);



    QSqlQuery selectUserID(QString userName);
    QSqlQuery insertInvitedUsers(int roomID, int userID, int statusID);
    QSqlQuery insertNewInvite(QString text, int roomID, int senderID,
                              int receiverID);
    QSqlQuery updateInviteAccept(int inviteID);
    QSqlQuery updateInvite(int inviteID, int statusID);

    QSqlQuery selectWaitInvitations();
    QSqlQuery selectIdSenderInvite(int inviteID);
    QSqlQuery selectInvitedUserName(int inviteID);
    QSqlQuery selectInvitedRoomName(int inviteID);

    QSqlQuery deleteUser(int userID, int roomID);
    bool delRoom(int roomID);
    bool insertMessage(int roomID, int userID, QString text);
    int getNewRoomID(int userID, QString roomName);
private:
    QSqlQuery query;
    QMap<QString,QString> mapSetQuery;
    QMap<setQuery, QString> mapQueries;
};

#endif // QUERYPULL_H
