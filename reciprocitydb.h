#ifndef RECIPROCITYDB_H
#define RECIPROCITYDB_H
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QDebug>
#include <QVariantMap>
#include <QList>

class reciprocityDB: public QObject
{
    Q_OBJECT
public:
    QSqlDatabase chatDB;
    reciprocityDB();
    QVariantMap readAuth(QString login, QString pass);
    void setStatusONline(int id);
    void setStatusOFFline(int id);
    void connectChatToDB();
    QVariantMap setRooms(int id);
    QVariantMap setMessages(int roomID);
signals:
     void dbConnected(const QString &text);
};

#endif // RECIPROCITYDB_H
