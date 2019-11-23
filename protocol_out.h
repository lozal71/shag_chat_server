#ifndef PROTOCOLOUT_H
#define PROTOCOLOUT_H
#include <QDataStream>
#include <QDebug>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QJsonObject>

class protocolOut: public QObject
{
    Q_OBJECT
public:
    protocolOut();
    protocolOut(QTcpSocket *socket);
    //QByteArray getPackage();
    //void setPackage(QJsonDocument jdParam);
    void writeSocket(QVariantMap mapSocket);
    QVariantMap readSocket();
    bool isError();
private:
    QTcpSocket *socket;
    QByteArray baPackage;
    QJsonDocument jsonDoc;
    void transferJSONtoBA();
    void transferBAtoJSON();
    bool flag_error;
};

#endif // PROTOCOLOUT_H
