#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <QTcpServer>
#include <QDebug>
#include <QList>
#include <QDir>
#include <QFile>
#include "session.h"

class chatServer: public QObject
{
    Q_OBJECT
public:
    chatServer();
    void serverStart();
    QSqlDatabase dbChat;
private:
    QTcpServer *serverChat;
    QList<session*> sessionPull;
    QSqlDatabase dbServer;
    void newClient();
    void removeSession();
    void connectChatToDB();

signals:
    void serverError(const QString& text);
    void serverStarted();
    void oneSessionClosed();
    void dbConnected();
};

#endif // CHATSERVER_H
