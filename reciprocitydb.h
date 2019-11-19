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
    QSqlDatabase chatDB;
    QVariantMap  mapResponseAuth(QString login, QString pass);
    void connectChatToDB();
    void setStatusOFFline(int id);
    QList<int> insertMessage(int roomID, int senderID, QString text);
    QVariantMap insertNewRoom(int userID, QString roomName);
    QList<int> delRoom(int roomID, int adminID);
    QString getRoomName(int roomID);
    int getUserID(QString userName);
    void insertNewInvite(QString text, int roomID, int senderID,
                      int receiverID);
    QVariantMap getInvitations(int userID);
    QVariantMap acceptInvite(int inviteID, int roomID, int userID);

private:
    QVariantMap setMapRoomsID(int id);
    QVariantMap setMapStatusMess(int roomID);
    QVariantMap collectMapMessID(QSqlQuery qMessage);
};

#endif // RECIPROCITYDB_H
