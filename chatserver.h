#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <QTcpServer>
#include <QDebug>
#include <QList>

#include "session.h"

class chatServer: public QObject
{
    Q_OBJECT
public:
    chatServer();
    ~chatServer();
    void serverStart();
    //QSqlDatabase chatDB;
private:
    QTcpServer *serverChat;
    QList<session*> sessionPull;
    reciprocityDB *db;
    void newClient();
    void removeSession();
    //void connectChatToDB();

signals:
    void serverError(const QString& text);
    void serverStarted();
    void oneSessionClosed(int id);
//    void dbConnected();
};

#endif // CHATSERVER_H
