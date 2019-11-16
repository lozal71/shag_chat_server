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
    void broadCastDelRoom(QString roomName, int roomID);
    void broadCast(QString text, QString senderName, int roomID);
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
};

#endif // SESSION_H
