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
#include <QDateTime>

class reciprocityDB: public QObject
{
    Q_OBJECT
public:
    reciprocityDB();
    QSqlDatabase chatDB;
    QVariantMap readAuth(QString login, QString pass);
    void connectChatToDB();
    void setStatusOFFline(int id);
    QVariantMap readMessage(int roomID, int userID, QDateTime td, QString text);
private:

    void setStatusONline(int id);
    void setStatusRead(int id);
    void setCurrentTime(int id);
    QVariantMap setRooms(int id);
    QVariantMap setMessages(int roomID);
signals:
     void dbConnected(const QString &text);
     void test();
};

#endif // RECIPROCITYDB_H
