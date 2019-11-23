#ifndef RECIPROCITYDB_H
#define RECIPROCITYDB_H
#include <QDir>
#include <QFile>
#include <QVariantMap>
#include "querypull.h"


class reciprocityDB: public QObject
{
    Q_OBJECT
public:
    reciprocityDB();
    ~reciprocityDB();
    queryPull *query;
    QSqlDatabase chatDB;

    void connectChatToDB();
    QVariantMap  mapResponseAuth(QString login, QString pass);

    QMap<int, QString> getUserIdUserName(QString login, QString pass);
    QString getRoomName(int roomID);
    QString getUserName(int userID);
    QString getRole(int roleID);
    int getInvitedUserID(QString userName, int roomID);
    int getIdSenderInvite(int inviteID);
    QString getInvitedName(int inviteID);
    QMap<int, QString> getInvitedRoom(int inviteID);
    QVariantMap getMembers(int roomID, int userID);
    QVariantMap getInvitations(int userID);
    QVariantMap getMapRoomsID(int userID);

    void setOffLine(int userID);
    void setOnLine(int userID);

    QList<int> insertNewMessage(int roomID, int senderID, QString text);
    QVariantMap insertNewRoom(int userID, QString roomName);
    QList<int> delRoom(int roomID, int adminID);
    void insertNewInvite(QString text, int roomID, int senderID, int receiverID);
    QVariantMap acceptInvite(int inviteID, int roomID, int userID);
    void rejectInvite(int inviteID);
    void deleteUser(int userID, int roomID);
private:
    QVariantMap getMapMess(int roomID);
    QVariantMap collectMapMessID(QSqlQuery qMessage);
};

#endif // RECIPROCITYDB_H
