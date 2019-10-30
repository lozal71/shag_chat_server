#ifndef PROTOCOLIN_H
#define POROTOCOLIN_H
#include <QDataStream>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

enum setCodeCommand {ErrorMessage, Auth, SessionClosed,  NoConnect } ;

class protocolIn: public QObject
{
public:
    protocolIn(QTcpSocket *socket);
    setCodeCommand getCode();
    QString getLoginClient();
    QString getPassClient();
private:
    setCodeCommand codeCommand;
    QString loginClient;
    QString passClient;
    QJsonObject jsonDataClient;
    QByteArray getMessage(QTcpSocket *socket);
};

#endif // POROTOCOLIN_H
