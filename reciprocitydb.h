#ifndef RECIPROCITYDB_H
#define RECIPROCITYDB_H
#include <QDir>
#include <QFile>
//#include <QSqlError>
//#include <QSqlQuery>
//#include <QSqlDatabase>
//#include <QDebug>
#include <QVariantMap>
#include <QList>
#include "querypull.h"

class reciprocityDB: public QObject
{
    Q_OBJECT
public:
    reciprocityDB();
    QSqlDatabase chatDB;
    QVariantMap mapResponseAuth(QString login, QString pass);
    void connectChatToDB();
    void setStatusOFFline(int id);
    QMap<int,QString> readMessage(int roomID, int userID, QString text);
    QVariantMap insertNewRoom(int userID, QString roomName);
    QMap<int,QString> delRoom(int roomID, int adminID);
    //QVariantMap setMapCastMessages(int roomID);
private:
    QVariantMap setMapRooms(int id);
    QVariantMap setMapMessages(int roomID);

};

#endif // RECIPROCITYDB_H
