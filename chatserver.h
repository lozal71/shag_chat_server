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
    void sendMessDelRoom(QList<int> listUserOnline, int roomID,
                         QString roomName, QString adminName);
    void sendMessUpdateUsesrs(QList<int> listUserOnline,
                                     int userID, QString userName,
                                     int roomID, QString roomName, setUpdateUsers param);
    void sendNewMess(QList<int> listUserOnline, QString text,
                      QString senderName, int roomID);
    void sendInvite(int invitedUserID);
    void sendMessToDelUser(int delUserID, int roomID, QString textDel);
    QObject *mainWindow;
    session* getSession(int sessionID);
signals:
    void serverStarted();
    void sessionClosedForDB(int id);

};

#endif // CHATSERVER_H
