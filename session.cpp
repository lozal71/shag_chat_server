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
    QDateTime td;
    td =td.currentDateTime();
    QVariantMap mapMess;
    mapMess["timeMess"] = td.toString();
    mapMess["senderName"] = "you";
    mapMess["textMess"] = text;
    mapMess["roomID"] = roomID;
    return mapMess;
}

void session::broadCastDelRoom(QString roomName, int roomID)
{
    QVariantMap mapCommand;
    QVariantMap mapData;
    mapCommand["codeCommand"] = setCodeCommand::CastDelRoom;
    QDateTime td;
    td = td.currentDateTime();
    mapData["timeMess"] = td;
    mapData["textMess"] = "Room " + roomName + " is moving away";
    mapData["senderName"] = "admin of room";
    mapData["roomName"] = roomName;
    mapData["roomID"] = roomID;
    mapCommand["joDataInput"] = mapData;
    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
    qDebug() << "broadCastDelRoom jdResponse" << jdResponse;
    out->setPackage(jdResponse);
    socketSession->write(out->getPackage());
}

void session::broadCast(QString text, QString senderName, int roomID)
{
    QVariantMap mapCommand;
    QVariantMap mapData;
    mapCommand["codeCommand"] = setCodeCommand::CastMess;
    QDateTime td;
    td = td.currentDateTime();
    mapData["timeMess"] = td;
    mapData["textMess"] = text;
    mapData["senderName"] = senderName;
    mapData["roomID"] = roomID;
    mapCommand["joDataInput"] = mapData;
    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
   // qDebug() << "client.id"  << client.id;
    qDebug() << " broadcast jdResponse" << jdResponse;
    out->setPackage(jdResponse);
    socketSession->write(out->getPackage());
}

//void session::sendInvite(QString senderName, QString textInvite, QString roomName, int roomID)
void session::sendInvite()
{
    QVariantMap mapCommand;
    QVariantMap mapData;
    mapData["invite"] = db->getInvitations(this->client.id);
    //qDebug() << "mapData[\"invite\"]" << mapData["invite"];
    mapCommand["codeCommand"] = setCodeCommand::questInvite;
//    mapData["textInvite"] = textInvite;
//    mapData["senderName"] = senderName;
//    mapData["roomName"] = roomName;
//    mapData["roomID"] = roomID;
    mapCommand["joDataInput"] = mapData;
    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
   // qDebug() << "client.id"  << client.id;
    //qDebug() << " sendInvite" << jdResponse;
    out->setPackage(jdResponse);
    socketSession->write(out->getPackage());
}

void session::sendRejectInvite(QString invitedName, QString roomName)
{
    QVariantMap mapCommand;
    QVariantMap mapData;
    mapCommand["codeCommand"] = setCodeCommand::notifyRejectInvite;
    mapData["textReject"] = invitedName + " reject invite in room " + roomName;
    mapCommand["joDataInput"] = mapData;
    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
   // qDebug() << "client.id"  << client.id;
    //qDebug() << " sendInvite" << jdResponse;
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

            // фиксируем id и имя клиента
            this->client.id = mapResponse["userID"].toInt();
            this->client.name = mapResponse["userName"].toString();
            mapResponse["invite"] = db->getInvitations(this->client.id);
            break;
        }
        case setCodeCommand::Send:
        {
            sLogText = "query send message ";
            QVariantMap mapData =  mapCommand["joDataInput"].toMap();
            int roomID = mapData["roomID"].toInt();
            QString text = mapData["text"].toString();
            // получаем ответ из БД о пользователях - онлайн
            QList<int> listUserOnline;
            listUserOnline = db->insertMessage(roomID, client.id, text);
            // испускаем сигнал о необходимости уведомления
            // пользователей-онлайн о новом сообщении
            emit notifyNewMessage(listUserOnline, text, client.name, roomID);
            // готовим ответ пославшему сообщение
            mapResponse = prepareResponseToSender(roomID, text);
            break;
        }
        case setCodeCommand::NewRoom:
        {
            sLogText = "query new room ";
            QVariantMap mapData =  mapCommand["joDataInput"].toMap();
            // получаем ответ из БД
            mapResponse = db->insertNewRoom(client.id, mapData["roomNew"].toString());
            break;
        }
        case setCodeCommand::DelRoom:
        {
            sLogText = "query del room ";
            QVariantMap mapData =  mapCommand["joDataInput"].toMap();
            // удаляем комнату из БД
            QList<int> listUserOnline;
            int delRoomID = mapData["delRoomID"].toInt();
            QString roomName = db->getRoomName(delRoomID);
            listUserOnline = db->delRoom(delRoomID, client.id);
            mapResponse["delRoomID"] = delRoomID;
            emit notifyRoomRemoval(listUserOnline, delRoomID, roomName);
            break;
        }
        case setCodeCommand::Invite:
        {
            sLogText = "query invite ";
            QVariantMap mapData =  mapCommand["joDataInput"].toMap();
            int invitedUserID = 0;
            invitedUserID = db->getInvitedUserID(mapData["userName"].toString(),
                                            mapData["roomID"].toInt());
            mapResponse["invitedUserID"] = invitedUserID;
            mapResponse["invitedUserName"] = mapData["userName"].toString();
            if (invitedUserID !=0 and invitedUserID != -1)
            {
                QString roomName = db->getRoomName(mapData["roomID"].toInt());
                db->insertNewInvite(mapData["textInvite"].toString(),
                                    mapData["roomID"].toInt(),
                                    client.id, (invitedUserID));
                emit sendInviteUser(invitedUserID, client.name, roomName,
                                        mapData["textInvite"].toString(),
                                        mapData["roomID"].toInt());
            }
            break;
        }
        case setCodeCommand::acceptInvite:
        {
            sLogText = "query acceptInvite ";
            QVariantMap mapData =  mapCommand["joDataInput"].toMap();
            int roomID = mapData["roomID"].toInt();
            int inviteID = mapData["inviteID"].toInt();
            QString userName = mapData["userName"].toString();
            mapResponse["mess"] = db->acceptInvite(inviteID,roomID, client.id);
            mapResponse["roomID"] = roomID;
            mapResponse["roomName"] = mapData["roomName"].toString();
            mapResponse["inviteID"] = inviteID;
            // получаем ответ из БД о пользователях - онлайн
            QList<int> listUserOnline;
            listUserOnline = db->insertMessage(roomID,
                                               client.id,
                                               userName + " accept invite");
            // испускаем сигнал о необходимости уведомления
            // пользователей-онлайн о новом сообщении
            emit notifyNewMessage(listUserOnline,
                                  userName + " accept invite",
                                  client.name, roomID);
            break;
        }
        case setCodeCommand::rejectInvite:
        {
            sLogText = "query rejectInvite ";
            QVariantMap mapData =  mapCommand["joDataInput"].toMap();
            int inviteID = mapData["inviteID"].toInt();
            db->rejectInvite(inviteID);
            //int idSenderInvite = db->getIdSenderInvite(inviteID);
            //QString invitedName = db->getInvitedName(inviteID);
            QMap<int, QString> room = db->getInvitedRoom(inviteID);
            //emit notifyRejectInvite(idSenderInvite, invitedName, roomName);

            QList<int> listUserOnline;
            QString text = client.name + " reject invite in " + room.first();
            listUserOnline = db->insertMessage(room.firstKey(), client.id, text);
            // испускаем сигнал о необходимости уведомления
            // пользователей-онлайн о новом сообщении
            emit notifyNewMessage(listUserOnline, text, client.name, room.firstKey());



            mapResponse["inviteID"] = inviteID;
            break;
        }
        }
    }
    // преобразуем в JSON-формат
    mapCommand["joDataInput"]=mapResponse;
    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
    qDebug() << "jdResponse" << jdResponse;

    // отправляем в сокет
    out->setPackage(jdResponse);
    socketSession->write(out->getPackage());

    // логируем в консоль
    qDebug() << sLogText;
}



