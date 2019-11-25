#ifndef SESSION_H
#define SESSION_H

#include <QTcpSocket>
//#include <QJsonObject>
//#include "protocol_in.h"
#include "protocol_out.h"
#include "reciprocitydb.h"

enum setUpdateUsers {addUser, removeUser};
enum setCodeCommand {auth = 1, newMess = 2, newRoom = 3, delRoom = 4,
                     messDelRoom = 5, sendMess = 6, invite = 7, questInvite = 8,
                     acceptInvite = 9, rejectInvite = 10, delUser = 11, updateUsers = 12};


class session: public QObject
{
    Q_OBJECT
public:
    session();
    ~session();
    session(QTcpSocket *socket);
    int getClientID();
    QString getClientName();
    void sendMessDelRoom(QString roomName, int roomID, QString adminName);
    void sendMess(QString text, QString senderName, int roomID);
    void sendMessUpdateUsers(int userID, QString userName,
                               int roomID, QString roomName, setUpdateUsers param);
    void sendMessInvite();
    //void connectDB();
    void setOffLine(int userID);
private:
    QTcpSocket *socketSession;
    protocolOut *protocol;
    reciprocityDB *db;
    QVariantMap mapQuery;
    QVariantMap mapBack;
    QVariantMap mapServerCommand;
    struct dataClient{
        int id;
        QString name;
    } client;
    void readQuery();
    void setConnectSession();

    void backInvite();
    void backDelRoom();
    void backNewRoom();
    void backNewMess();
    void backAuth();
    void backAcceptInvite();
    void backRejectInvite();
    void backDelUser();

    void prepareDistribNewMess();
    void prepareDistribMessDelRoom();
    void prepareMessInvite();
    void prepareMessRejectInvite();
    void prepareDistribAcceptInvite();
    void prepareDistribDelUser();
signals:
    void connectClosed();
    void sessionClosed(int id);
    void distribMessDelRoom(QList<int> listUserOnline, int delRoomID,
                            QString roomName, QString adminName);
    void distribNewMess(QList<int> listUserOnline, QString text,
                          QString senderName, int roomID);
    void sendUpdateUsers(QList<int> listUserOnline,
                           int userID, QString userName,
                           int roomID, QString roomName, setUpdateUsers param);
    void sendInvite(int invitedUserID, QString senderName,
                        QString roomName, QString textInvite, int roomID);
    void readyWrite(QVariantMap mapParam);
};

#endif // SESSION_H
