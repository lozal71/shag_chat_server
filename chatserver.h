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
    QList<session*> sessionList;
    reciprocityDB *db;
    void newClient();
    void removeSession();
    void setConnectServer();

signals:
    void serverError(const QString& text);
    void serverStarted(const QString& text);
    void sessionClosedForUI(const QString& text);
    void sessionClosedForDB(int id);
//    void dbConnected();
};

#endif // CHATSERVER_H
