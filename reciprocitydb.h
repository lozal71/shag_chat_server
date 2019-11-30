#ifndef RECIPROCITYDB_H
#define RECIPROCITYDB_H
#include <QDir>
#include <QFile>
#include <QVariantMap>
#include "querypull.h"
#include <QDateTime>

class reciprocityDB: public QObject
{
    Q_OBJECT
public:
    reciprocityDB();
    ~reciprocityDB();
    queryPull  *queryP;
    QSqlDatabase chatDB;

    void connectChatToDB();

    QMap<int, QString> getUserIdUserName(QString login, QString pass);
    QString getRoomName(int roomID);
    QString getUserName(int userID);
    int getUserID(QString userName);
    QString getRole(int roleID);
    int getInvitedUserID(QString userName, int roomID, int senderID);
    int getIdSenderInvite(int inviteID);
    QString getInvitedName(int inviteID);
    QMap<int, QString> getInvitedRoom(int inviteID);
    QVariantMap getMembers(int roomID, int userID);
    QVariantMap getInvitations(int userID);
    QVariantMap getMapRoomsID(int userID);
    QList<int> getMembersIdOnline(int roomID, int senderID);
    int getNewRoomID(int userID, QString roomName);
    int getUserStatus(int userID, int roomID);

    void setOffLine(int userID);
    void setOnLine(int userID);

    void insertNewMess(int roomID, int senderID, QString text);

    void deleteRoom(int roomID);
    void deleteUser(int userID, int roomID);

    void insertNewInvite(QString text, int roomID, int senderID, int receiverID);
    QVariantMap acceptInvite(int inviteID, int roomID, int userID);
    void rejectInvite(int inviteID);

private:
    QVariantMap getMapMess(int roomID);
    QVariantMap collectMapMessID(QSqlQuery qMessage);
};

#endif // RECIPROCITYDB_H
