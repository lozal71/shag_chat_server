#include "session.h"


session::session()
{

}

session::session(QTcpSocket *socket)
{
    socketSession = socket;
    in = new protocolIn();
    out = new protocolOut();
    sessionDB = new reciprocityDB();
    setConnectSession();

}

session::~session(){
    delete in;
    delete out;
    delete sessionDB;
}

void session::setConnectSession()
{
    // в сокете появились новые данные -
    //читаем пришедший запрос и пишем ответ
    connect (socketSession, &QTcpSocket::readyRead,
             this,&session::readQueryWriteResponse);
    connect(socketSession, &QTcpSocket::disconnected,
            this, &session::connectClosed);
}

int session::getIdClient()
{
    return client.id;
}


void session::readQueryWriteResponse()
{
    QString sLogText;
    setCodeCommand codeCommand;
    // получаем JSON-документ из сокета и преобразуем его в QVariantMap
    QJsonDocument jdTemp = in->receiveJSONdoc(socketSession);
    QJsonObject joTemp = jdTemp.object();
    //qDebug() << "joTemp" << joTemp;
    QVariantMap mapCommand =joTemp.toVariantMap();
    QVariantMap mapResponse;
    //qDebug() << "mapCommand" << mapCommand;

    // определяем команду запроса
    codeCommand = setCodeCommand(mapCommand["codeCommand"].toInt());

    if (in->isError()){
          sLogText = "Problem: error massage";
    }
    else {

        switch (codeCommand) {
            case setCodeCommand::Auth:
            {
                sLogText = "query auth for ";
                QVariantMap mapData =  mapCommand["joDataInput"].toMap();
                QString login = mapData["login"].toString();
                QString pass = mapData["pass"].toString();
                sLogText += login + "," + pass + "\n";
                // получаем ответ из БД
                mapResponse = sessionDB->mapResponseAuth(login, pass);
                client.id = mapResponse["id"].toInt();
                //client.mapRooms = mapResponse["rooms"].toMap();
            }
            break;
            case setCodeCommand::Send:
            {
                sLogText = "query send message ";
                QVariantMap mapData =  mapCommand["joDataInput"].toMap();
                // получаем ответ из БД
                //qDebug() << "mapData" << mapData;
                mapResponse = sessionDB->readMessage(mapData["roomID"].toInt(),
                                                        client.id,
                                                        mapData["text"].toString());
            }
            break;
        }
    }
    //emit logQueryReaded(sLogText);
    mapCommand["joDataInput"]=mapResponse;
    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
    //qDebug() << "jdResponse" << jdResponse;
    out->setPackage(jdResponse);
    socketSession->write(out->getPackage());
    qDebug() << sLogText;
}



