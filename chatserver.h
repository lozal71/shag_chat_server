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
private:
    QTcpServer *serverChat;
    QList<session*> sessionList;
    reciprocityDB *db;
    void newClient();
    void removeSession();
    void setConnectServer();
    void seachSessionForDelRoom(QList<int> listUserOnline, int roomID, QString roomName);
    void seachSession(QList<int> listUserOnline, QString text,
                      QString senderName, int roomID);
    void seachSessionForInvite(int invitedUserID);
    void seachSessionForRejectInvite(int idSenderInvite, QString invitedName, QString roomName);
    QObject *mainWindow;
    const char* logSlot;

signals:
    void serverStarted();
    void logToMainWindow(const QString& massage);
    void sessionClosedForDB(int id);

};

#endif // CHATSERVER_H
