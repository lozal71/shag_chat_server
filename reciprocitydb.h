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
    QVariantMap  mapResponseAuth(QString login, QString pass);
    void connectChatToDB();
    void setStatusOFFline(int id);
    QList<int> insertNewMessage(int roomID, int senderID, QString text);
    QVariantMap insertNewRoom(int userID, QString roomName);
    QList<int> delRoom(int roomID, int adminID);
    QString getRoomName(int roomID);
    QString getUserName(int userID);
    int getInvitedUserID(QString userName, int roomID);
    int getIdSenderInvite(int inviteID);
    QString getInvitedName(int inviteID);
    QMap<int, QString> getInvitedRoom(int inviteID);
    void insertNewInvite(QString text, int roomID, int senderID,
                      int receiverID);
    QVariantMap getUserIdNameFromRoom(int roomID, int adminID);
    QVariantMap getInvitations(int userID);
    QVariantMap acceptInvite(int inviteID, int roomID, int userID);
    void rejectInvite(int inviteID);
    void deleteUser(int userID, int roomID);
    QVariantMap setMapRoomsID(int userID);
private:

    QVariantMap setMapStatusMess(int roomID);
    QVariantMap collectMapMessID(QSqlQuery qMessage);
};

#endif // RECIPROCITYDB_H
