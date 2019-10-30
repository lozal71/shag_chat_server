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
#include <QList>
#include <QJsonArray>

class session: public QObject
{
    Q_OBJECT
public:
    session(QTcpSocket *socket);
    int getID();
private:
    QTcpSocket *socketSession;
    setCodeCommand codeCommand;
    QString login;
    QString pass;
    int id;
    void commandHandler();
    QJsonObject readFromDB();
    QJsonArray toJson(const QList<int> & list);

signals:
    void connectClosed();
    void sessionClosed();
};

#endif // SESSION_H
