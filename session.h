#ifndef SESSION_H
#define SESSION_H

#include <QTcpSocket>
#include <QThread>
#include <QJsonObject>
#include "protocol_in.h"
#include "protocol_out.h"
#include "reciprocitydb.h"

enum setCodeCommand {Auth = 1};

class session: public QObject
{
    Q_OBJECT
public:
    session();
    ~session();
    session(QTcpSocket *socket);
    int getIdClient();
private:
    QTcpSocket *socketSession;
    setCodeCommand codeCommand;

    QString login;
    QString pass;
    protocolIn *in;
    protocolOut *out;
    reciprocityDB *sessionDB;
    QVariantMap mapRooms;
    int idClient;
    void readQuery();
    void writeResponse(QVariantMap mapParam);
signals:
    void connectClosed();
    void sessionClosed(int id);
    void logQueryReaded(QString sParam);
    void queryReaded(QVariantMap mapParam);
};

#endif // SESSION_H
