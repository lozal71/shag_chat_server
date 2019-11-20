#ifndef SESSION_H
#define SESSION_H

#include <QTcpSocket>
#include <QThread>
#include <QJsonObject>
#include "protocol_in.h"
#include "protocol_out.h"
#include "reciprocitydb.h"

enum setCodeCommand {Auth = 1, Send = 2, NewRoom = 3, DelRoom = 4,
                     CastDelRoom = 5, CastMess = 6, Invite = 7, questInvite = 8,
                     acceptInvite = 9, rejectInvite = 10, delUser = 11,
                     userInRoom = 12};


class session: public QObject
{
    Q_OBJECT
public:
    session();
    ~session();
    session(QTcpSocket *socket);
    int getIdClient();
    void broadCastDelRoom(QString roomName, int roomID);
    void broadCast(QString text, QString senderName, int roomID);
    //void sendInvite(QString senderName, QString textInvite, QString roomName, int roomID);
    void sendInvite();
    void sendRejectInvite(QString invitedName, QString roomName);
private:
    QTcpSocket *socketSession;
    protocolIn *in;
    protocolOut *out;
    reciprocityDB *db;
    struct dataClient{
        int id;
        QString name;
    } client;
    void readQueryWriteResponse();
    void setConnectSession();
    QVariantMap prepareResponseToSender(int roomID, QString text);
signals:
    void connectClosed();
    void sessionClosed(int id);
    void logQueryReaded(const QString &text);
    void queryReaded(QVariantMap mapParam);
    void notifyRoomRemoval(QList<int> listUserOnline, int delRoomID, QString roomName);
    void notifyNewMessage(QList<int> listUserOnline, QString text,
                          QString senderName, int roomID);
    void sendInviteUser(int invitedUserID, QString senderName,
                        QString roomName, QString textInvite, int roomID);
    void notifyRejectInvite(int idSenderInvite, QString invitedName, QString roomName);
};

#endif // SESSION_H
