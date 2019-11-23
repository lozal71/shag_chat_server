#ifndef SESSION_H
#define SESSION_H

#include <QTcpSocket>
//#include <QJsonObject>
//#include "protocol_in.h"
#include "protocol_out.h"
#include "reciprocitydb.h"

enum setUpdateUsers {addUser, removeUser};
enum setCodeCommand {Auth = 1, newMess = 2, NewRoom = 3, DelRoom = 4,
                     MessDelRoom = 5, SendMess = 6, Invite = 7, questInvite = 8,
                     acceptInvite = 9, rejectInvite = 10, delUser = 11, updateUsers = 12 };


class session: public QObject
{
    Q_OBJECT
public:
    session();
    ~session();
    session(QTcpSocket *socket);
    int getClientID();
    QString getClientName();
    void messDelRoom(QString roomName, int roomID);
    void newMess(QString text, QString senderName, int roomID);
    void messUpdateUsers(int userID, QString userName,
                               int roomID, QString roomName, setUpdateUsers param);
    void messInvite();
    void messRejectInvite(QString invitedName, QString roomName);
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
    QVariantMap backNewMess(int roomID, QString text);
    void BackAuth(setCodeCommand code);
signals:
    void connectClosed();
    void sessionClosed(int id);
    void sendMessDelRoom(QList<int> listUserOnline, int delRoomID, QString roomName);
    void sendNewMessage(QList<int> listUserOnline, QString text,
                          QString senderName, int roomID);
    void sendUpdateUsers(QList<int> listUserOnline,
                           int userID, QString userName,
                           int roomID, QString roomName, setUpdateUsers param);
    void sendInviteUser(int invitedUserID, QString senderName,
                        QString roomName, QString textInvite, int roomID);
    void sendRejectInvite(int idSenderInvite, QString invitedName, QString roomName);
    void readyWrite(QVariantMap mapParam);
};

#endif // SESSION_H
