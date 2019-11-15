#include "session.h"


session::session()
{

}

session::session(QTcpSocket *socket)
{
    socketSession = socket;
    in = new protocolIn();
    out = new protocolOut();
    db = new reciprocityDB();
    setConnectSession();

}

session::~session(){
    delete in;
    delete out;
    delete db;
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

// готовим ответ пославшему сообщение
QVariantMap session::prepareResponseToSender(int roomID, QString text)
{
    QVariantMap mapResponse;
    QDateTime td;
    td =td.currentDateTime();
    QVariantMap mapMess;
    mapMess["timeMess"] = td.toString();
    mapMess["senderName"] = "you";
    mapMess["textMess"] = text;
//    QVariantMap mapMessID;
//    mapMessID.insert("0",mapMess);
//    QVariantMap mapStatusMess;
//    mapStatusMess.insert("cast",mapMess);
 /*   QVariantMap mapRoomName;
    mapRoomName.insert("undefined", mapStatusMess);
    QVariantMap mapRoomID; */
    mapResponse.insert(QString::number(roomID),mapMess);
//    QVariantMap mapUserRole;
//    mapUserRole.insert("undefined",mapRoomID);
//    QVariantMap mapUserName;
//    mapUserName.insert("undefined", mapUserRole);
//    mapResponse.insert(QString::number(userID), mapUserName);
    return mapResponse;
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

void session::broadCast(QVariantMap mapRoomID)
{
    QVariantMap mapCommand;
    QVariantMap mapData; // {"cast":{mapCastMessage},
    mapCommand["codeCommand"] = setCodeCommand::CastMess;
    mapCommand["joDataInput"] = mapRoomID;
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
                mapResponse = db->mapResponseAuth(login, pass);
                client.id = mapResponse.firstKey().toInt();
                //client.mapRooms = mapResponse["rooms"].toMap();
            }
            break;
            case setCodeCommand::Send:
            {

                sLogText = "query send message ";
                QVariantMap mapData =  mapCommand["joDataInput"].toMap();
                int roomID = mapData["roomID"].toInt();
                QString text = mapData["text"].toString();
                // получаем ответ из БД о пользователях - онлайн
                QVariantMap mapUserOnline;
                mapUserOnline = db->insertMessage(roomID, client.id, text);
                // испускаем сигнал о необходимости уведомления
                // пользователей-онлайн о новом сообщении
                emit notifyNewMessage(mapUserOnline);
                // готовим ответ пославшему сообщение

                mapResponse = prepareResponseToSender(roomID, text);
            }
            break;
            case setCodeCommand::NewRoom:{
                sLogText = "query new room ";
                QVariantMap mapData =  mapCommand["joDataInput"].toMap();
                // получаем ответ из БД
                mapResponse = db->insertNewRoom(client.id, mapData["roomNew"].toString());
                break;
            }
            case setCodeCommand::DelRoom:{
                sLogText = "query del room ";
                QVariantMap mapData =  mapCommand["joDataInput"].toMap();
                // удаляем комнату из БД
                QMap<int,QString> mapUserOnline;
                int delRoomID = mapData["delRoomID"].toInt();
                mapUserOnline = db->delRoom(delRoomID, client.id);
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



