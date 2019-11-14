#ifndef SESSION_H
#define SESSION_H

#include <QTcpSocket>
#include <QThread>
#include <QJsonObject>
#include "protocol_in.h"
#include "protocol_out.h"
#include "reciprocitydb.h"


enum setCodeCommand {Auth = 1, Send = 2, NewRoom = 3, DelRoom = 4, CastDelRoom = 5, CastMess = 6};

class session: public QObject
{
    Q_OBJECT
public:
    session();
    ~session();
    session(QTcpSocket *socket);
    int getIdClient();
    void broadCastDelRoom(QString text, int roomID);
    void broadCast(QVariantMap mapTimeSenderMess, int roomID, int senderID);
private:
    QTcpSocket *socketSession;
    protocolIn *in;
    protocolOut *out;
    reciprocityDB *sessionDB;
    struct dataClient{
        int id;
        //QVariantMap mapRooms;
    } client;
    void readQueryWriteResponse();
    void setConnectSession();

signals:
    void connectClosed();
    void sessionClosed(int id);
    void logQueryReaded(const QString &text);
    void queryReaded(QVariantMap mapParam);
    void notifyRoomRemoval(QMap<int,QString> mapUserOnline, int delRoomID);
    void notifyNewMessage(QVariantMap mapUserOnline, int roomID, int senderID);
};

#endif // SESSION_H
