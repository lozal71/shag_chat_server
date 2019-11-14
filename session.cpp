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

void session::broadCastDelRoom(QString text, int roomID)
{
    QVariantMap mapCommand;
    QVariantMap mapData;
    mapCommand["codeCommand"] = setCodeCommand::CastDelRoom;
    mapData["cast"] = text;
    mapData["roomID"] = roomID;
    mapCommand["joDataInput"] = mapData;
    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
    //qDebug() << "44 jdResponse" << jdResponse;
    out->setPackage(jdResponse);
    socketSession->write(out->getPackage());
}

void session::broadCast(QVariantMap mapTimeSenderMess, int roomID, int senderID)
{
    QVariantMap mapCommand;
    QVariantMap mapData; // {"cast":{mapCastMessage},
    mapCommand["codeCommand"] = setCodeCommand::CastMess;
//    QVariantMap mapCastMessage; //  {messTime:{mapSenderMessage}}
//    QVariantMap mapReadMessage; //  {messTime:{mapSenderMessage}}
//    QVariantMap mapSenderMessage; // {senderName:{textMess}}
//    QString messTime;
//    QString senderName;
//    QString textMess;
//    QDateTime td;
//    td = td.currentDateTime();
//    messTime =  td.toString();
//    senderName = QString::number(senderID);
//    textMess = text;
//    mapSenderMessage[senderName] = textMess;
//    mapCastMessage[messTime] = mapSenderMessage;
    mapData["cast"] = mapTimeSenderMess;
    mapData["roomID"] = roomID;
    mapCommand["joDataInput"] = mapData;
    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
    qDebug() << "client.id"  << client.id;
    qDebug() << " 71 jdResponse" << jdResponse;

    out->setPackage(jdResponse);
    socketSession->write(out->getPackage());
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
    //qDebug() << "94 mapCommand" << mapCommand;

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
                int roomID = 0;
                sLogText = "query send message ";
                QVariantMap mapData =  mapCommand["joDataInput"].toMap();
                // получаем ответ из БД
                //qDebug() << "mapData" << mapData;
                QVariantMap mapUserOnline;
                roomID = mapData["roomID"].toInt();
                mapUserOnline = sessionDB->insertMessage(roomID, client.id,
                                                        mapData["text"].toString());
                //qDebug() << "126 mapUserOnline" <<mapUserOnline ;
                QVariantMap mapTime;
                QDateTime td;
                td =td.currentDateTime();
                QVariantMap mapMess;
                mapMess["you"] = mapData["text"].toString();
                mapTime[td.toString()] = mapMess;
                mapResponse["cast"] = mapTime;
                mapResponse["roomID"] = roomID;
                emit notifyNewMessage(mapUserOnline,roomID,client.id);
            }
            break;
            case setCodeCommand::NewRoom:{
                sLogText = "query new room ";
                QVariantMap mapData =  mapCommand["joDataInput"].toMap();
                // получаем ответ из БД
                mapResponse = sessionDB->insertNewRoom(client.id, mapData["roomNew"].toString());
                break;
            }
            case setCodeCommand::DelRoom:{
                sLogText = "query del room ";
                QVariantMap mapData =  mapCommand["joDataInput"].toMap();
                // удаляем комнату из БД
                QMap<int,QString> mapUserOnline;
                int delRoomID = mapData["delRoomID"].toInt();
                mapUserOnline = sessionDB->delRoom(delRoomID, client.id);
                mapResponse["delRoomID"] = delRoomID;
                emit notifyRoomRemoval(mapUserOnline, delRoomID);
                break;
            }
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



