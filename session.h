#ifndef SESSION_H
#define SESSION_H
#include <QDebug>
#include <QTcpSocket>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include "protocol_in.h"
#include "protocol_out.h"
#include <QThread>

class session: public QObject
{
    Q_OBJECT
public:
    session(QTcpSocket *socket);

private:
    QTcpSocket *socketSession;
    setCodeCommand codeCommand;
    QString login;
    QString pass;
    void commandHandler();
    QJsonObject readFromDB();

signals:
    void connectClosed();
    void sessionClosed();
};

#endif // SESSION_H
