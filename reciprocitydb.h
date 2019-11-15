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
    QVariantMap insertMessage(int roomID, int userID, QString text);
    QVariantMap insertNewRoom(int userID, QString roomName);
    QMap<int,QString> delRoom(int roomID, int adminID);
private:
    QVariantMap setMapUserRole(int id);
    QVariantMap setMapStatusMess(int roomID);
    QVariantMap collectMapMessID(QSqlQuery qMessage);
    QVariantMap collectMapRoomsID(QSqlQuery qUserRole);
};

#endif // RECIPROCITYDB_H
