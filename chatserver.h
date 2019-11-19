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
//    chatServer(QObject* mainWindow, const char *logSlot);
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
    void seachSessionForDelRoom(QList<int> listUserOnline, int roomID, QString roomName);
    void seachSession(QList<int> listUserOnline, QString text,
                      QString senderName, int roomID);
//    void seachSessioForInvite(int invitedUserID, QString senderName,
//                              QString roomName, QString textInvite, int roomID);
    void seachSessioForInvite(int invitedUserID);
    QObject *mainWindow;
    const char* logSlot;

signals:
//    void serverError(const QString& text);
    void serverStarted();
//    void sessionClosedForUI(const QString& text);
    void logToMainWindow(const QString& massage);
    void sessionClosedForDB(int id);
//    void dbConnected();
};

#endif // CHATSERVER_H
