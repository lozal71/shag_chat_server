#include "session.h"


session::session()
{

}

session::session(QTcpSocket *socket)
{
    socketSession = socket;
    in = new protocolIn();
    out = new protocolOut(socket);
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
    connect(this, &session::readyWrite,
            out, &protocolOut::writeSocket);
}

// эхо-ответ на новое сообщение
QVariantMap session::backNewMess(int roomID, QString text)
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

void session::messDelRoom(QString roomName, int roomID)
{
    QVariantMap mapCommand;
    QVariantMap mapData;
    mapCommand["codeCommand"] = setCodeCommand::MessDelRoom;
    QDateTime td;
    td = td.currentDateTime();
    mapData["timeMess"] = td;
    mapData["textMess"] = "Room " + roomName + " is removed";
    mapData["senderName"] = "admin of room";
    mapData["roomName"] = roomName;
    mapData["roomID"] = roomID;
    mapCommand["joDataInput"] = mapData;
    emit readyWrite(mapCommand);
    //writeSocket(mapCommand);
//    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
//    //qDebug() << "broadCastDelRoom jdResponse" << jdResponse;
//    out->setPackage(jdResponse);
//    qDebug() << client.id << out->getPackage();
//    socketSession->write(out->getPackage());
}

void session::newMess(QString text, QString senderName, int roomID)
{
    QVariantMap mapCommand;
    QVariantMap mapData;
    mapCommand["codeCommand"] = setCodeCommand::SendMess;
    QDateTime td;
    td = td.currentDateTime();
    mapData["timeMess"] = td;
    mapData["textMess"] = text;
    mapData["senderName"] = senderName;
    mapData["roomID"] = roomID;
    mapCommand["joDataInput"] = mapData;
    emit readyWrite(mapCommand);
    //writeSocket(mapCommand);
//    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
//   // qDebug() << "client.id"  << client.id;
//    //qDebug() << " broadcast jdResponse" << jdResponse;
//    out->setPackage(jdResponse);
//    //qDebug() << client.id << out->getPackage();
//    socketSession->write(out->getPackage());
}


void session::messUpdateUsers(int userID, QString userName,
                                   int roomID, QString roomName, setUpdateUsers param)
{
    QVariantMap mapCommand;
    QVariantMap mapData;
    QDateTime td;
    td = td.currentDateTime();
    mapData["updateParam"] = param;
    mapData["timeMess"] = td;
    if (param == setUpdateUsers::addUser){
        mapData["textMess"] = userName + " is added to room " + roomName;
    }
    else {
        mapData["textMess"] = userName + " is removed from room " + roomName;
    }
    mapData["senderName"] = userName;
    mapCommand["codeCommand"] = setCodeCommand::updateUsers;
    mapData["userID"] = userID;
    mapData["roomID"] = roomID;
    mapCommand["joDataInput"] = mapData;
    emit readyWrite(mapCommand);
    //writeSocket(mapCommand);
//    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
//   // qDebug() << "client.id"  << client.id;
//    //qDebug() << " broadcast jdResponse" << jdResponse;
//    out->setPackage(jdResponse);
//    //qDebug() << client.id << out->getPackage();
//    socketSession->write(out->getPackage());
}

void session::sendInvite()
{
    QVariantMap mapCommand;
    QVariantMap mapData;
    mapData["invite"] = db->getInvitations(this->client.id);
    mapCommand["codeCommand"] = setCodeCommand::questInvite;
    mapCommand["joDataInput"] = mapData;
    emit readyWrite(mapCommand);
    //writeSocket(mapCommand);
//    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
//    out->setPackage(jdResponse);
//    //qDebug() << client.id; // << out->getPackage();
//    socketSession->write(out->getPackage());
}

int session::getIdClient()
{
    return client.id;
}


void session::readQueryWriteResponse()
{
    setCodeCommand codeCommand;
//    // получаем JSON-документ из сокета
//    QJsonDocument jdTemp = in->receiveJSONdoc(socketSession);
//    QJsonObject joTemp = jdTemp.object();
//    //qDebug() << "joTemp" << joTemp;
//    QVariantMap mapCommand =joTemp.toVariantMap();

    QVariantMap mapCommand = out->readSocket();
    // определяем команду запроса
    codeCommand = setCodeCommand(mapCommand["codeCommand"].toInt());
    // определяем данные для дальнейшей обработки
    QVariantMap mapData =  mapCommand["joDataInput"].toMap();
    QVariantMap mapResponse; // ответ от сервера
    if (in->isError()){
          qDebug() << "Problem: error massage";
    }
    else {
        switch (codeCommand) {
        case setCodeCommand::Auth:
        {
            qDebug() <<  "query auth ";
            // обратный ответ запросившему авторизацию из БД в виде:
                    // {{"userID" : userID},
                    //  {"userName": userName}
                    //  {"invite": mapInvite}
                    //  {"rooms": mapRoomsID{...mapMess}}}
            // и переключаем пользователя в режим он-лайн
             mapResponse = db->mapResponseAuth(mapData["login"].toString(),
                                               mapData["pass"].toString());
            // фиксируем в сессии id и имя клиента
            this->client.id = mapResponse["userID"].toInt();
            this->client.name = mapResponse["userName"].toString();
            break;
        }
        case setCodeCommand::newMess:
        {
            qDebug() <<  "query new message ";
            int roomID = mapData["roomID"].toInt();
            QString text = mapData["text"].toString();
            // записываем новое сообщение в БД и
            // получаем из БД список онлайн-участников комнаты
            QList<int> listUserOnline = db->insertNewMessage(roomID, client.id, text);
            // испускаем сигнал: послать всем онлайн-участникам комнаты новое сообщение
            emit sendNewMessage(listUserOnline, text, client.name, roomID);
            // обратный ответ автору нового сообщения
            mapResponse = backNewMess(roomID, text);
            break;
        }
        case setCodeCommand::NewRoom:
        {
            qDebug() <<  "query new room ";
            // обратный ответ инициатору создания новой комнаты из БД: newRoomID, newRoomName -
            mapResponse = db->insertNewRoom(client.id, mapData["roomNew"].toString());
            break;
        }
        case setCodeCommand::DelRoom:
        {
            qDebug() <<  "query del room ";
            int delRoomID = mapData["delRoomID"].toInt();
            // удаляем комнату из БД
            // получаем из БД список онлайн-участников комнаты
            // всем остальным участникам оставляем сообщение об удалении комнаты
            QList<int> listUserOnline = db->delRoom(delRoomID, client.id);
            // испускаем сигнал: послать всем онлайн-участникам комнаты сообщение о ее удалении
            QString roomName = db->getRoomName(delRoomID);
            emit sendMessDelRoom(listUserOnline, delRoomID, roomName);
            // обратный ответ инициатору удаления комнаты
            mapResponse["delRoomID"] = delRoomID;
            break;
        }
        case setCodeCommand::Invite:
        {
            qDebug() <<  "query invite ";
            // узнаем id приглашаемого участника комнаты по его имени
            int invitedUserID = db->getInvitedUserID(mapData["userName"].toString(),
                                                     mapData["roomID"].toInt());
            // если приглашаемый участник  - допустим
            if (invitedUserID !=0 and invitedUserID != -1)
            {
                QString roomName = db->getRoomName(mapData["roomID"].toInt());
                // записываем новое приглашение в БД
                db->insertNewInvite(mapData["textInvite"].toString(),
                                    mapData["roomID"].toInt(),
                                    client.id, invitedUserID);
                // испускаем сигнал: послать приглашение
                emit sendInviteUser(invitedUserID, client.name, roomName,
                                        mapData["textInvite"].toString(),
                                        mapData["roomID"].toInt());
            }
            // обратный ответ пославшему приглашение
            mapResponse["invitedUserID"] = invitedUserID;
            mapResponse["invitedUserName"] = mapData["userName"].toString();
            break;
        }
        case setCodeCommand::acceptInvite:
        {
            qDebug() <<  "query acceptInvite ";

            int roomID = mapData["roomID"].toInt();
            int inviteID = mapData["inviteID"].toInt();
            QString userName = mapData["userName"].toString();
            // записываем в БД сообщение о принятии приглашения и
            // получаем из БД список онлайн-участников комнаты
            QList<int> listUserOnline = db->insertNewMessage(roomID, client.id,
                                                             userName + " accept invite");
            // испускаем сигнал: послать онлайн-участникам комнаты
            // сообщение о новом пользователе и команду обновить список пользователей в комнате
            emit sendUpdateUsers(listUserOnline, client.id, client.name,
                                   roomID, mapData["roomName"].toString(), setUpdateUsers::addUser);
            // обратный ответ послашему согласие на приглашение: сбор всех данных о комнате
            mapResponse["mess"] = db->acceptInvite(inviteID,roomID, client.id);
            mapResponse["roomID"] = roomID;
            mapResponse["roomName"] = mapData["roomName"].toString();
            mapResponse["inviteID"] = inviteID;
            QVariantMap mapUsers = db->getUserIdNameFromRoom(roomID, client.id);
            mapResponse["users"] = mapUsers;
            break;
        }
        case setCodeCommand::rejectInvite:
        {
            qDebug() <<  "query rejectInvite ";
            int inviteID = mapData["inviteID"].toInt();
            // делаем отметку в БД об отклонении приглашения
            db->rejectInvite(inviteID);
            // получаем данные о комнате, приглашение в которую было отказано
            QMap<int, QString> room = db->getInvitedRoom(inviteID);
            QString text = client.name + " reject invite in " + room.first();
            // записываем в БД сообщение об отклонении приглашения и
            // получаем из БД список онлайн-участников комнаты
            QList<int> listUserOnline = db->insertNewMessage(room.firstKey(), client.id, text);
            // испускаем сигнал: послать онлайн-участникам комнаты
            // сообщение об отклонении приглашения
            emit sendNewMessage(listUserOnline, text, client.name, room.firstKey());
            // обратный ответ пославшениму отклонение приглашения
            mapResponse["inviteID"] = inviteID;
            break;
        }
        case setCodeCommand::delUser:
        {
            qDebug() <<  "query delete user";
            int roomID = mapData["roomID"].toInt();
            int userID =  mapData["userID"].toInt();
            QString roomName = db->getRoomName(roomID);
            QString userName = db->getUserName(userID);
            QString text = client.name + " deleted user " + userName
                            + " from " + roomName + " text: " + mapData["text"].toString();
            // записываем в БД сообщение об удалении пользователя из комнаты и
            // получаем из БД список онлайн-участников комнаты
            QList<int> listUserOnline = db->insertNewMessage(roomID, client.id, text);
            // испускаем сигнал: послать онлайн-участникам комнаты
            // сообщение об удалении пользователя и команду обновить список пользователей в комнате
            emit sendUpdateUsers(listUserOnline, userID, userName,
                                   roomID, roomName, setUpdateUsers::removeUser);
            // удаляем пользователя из комнаты в БД
            db->deleteUser(userID, roomID);
            // обратный ответ инициатору удаления пользователя
            QDateTime td;
            td = td.currentDateTime();
            mapResponse["updateParam"] = setUpdateUsers::removeUser;
            mapResponse["timeMess"] = td;
            mapResponse["textMess"] = "you is removed from room " + roomName;
            mapResponse["senderName"] = "you";
            mapResponse["userID"] = userID;
            mapResponse["roomID"] = roomID;
            break;
        }
        case setCodeCommand::MessDelRoom:
        {
            // эта команда не приходит от клиента, а инициируется на сервере,
            // поэтому здесь не рассматривается
            break;
        }
        case setCodeCommand::SendMess:
        {
            // эта команда не приходит от клиента, а инициируется на сервере,
            // поэтому здесь не рассматривается
            break;
        }
        case setCodeCommand::questInvite:
        {
            // эта команда не приходит от клиента, а инициируется на сервере,
            // поэтому здесь не рассматривается
            break;
        }
        case setCodeCommand::updateUsers:
        {
            // эта команда не приходит от клиента, а инициируется на сервере,
            // поэтому здесь не рассматривается
            break;
        }
        }
    }
    // преобразуем в JSON-формат
    mapCommand["joDataInput"]=mapResponse;
    qDebug() << mapCommand;
    emit readyWrite(mapCommand);
    //writeSocket(mapCommand);
//    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapCommand);
//    //qDebug() << "295 jdResponse" << jdResponse;

//    // отправляем в сокет
//    out->setPackage(jdResponse);
//    qDebug() << client.id; // << out->getPackage();
//    socketSession->write(out->getPackage());
}

void session::writeSocket(QVariantMap mapSocket)
{
    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapSocket);
    out->setPackage(jdResponse);
   // qDebug() << client.id; // << out->getPackage();
    socketSession->write(out->getPackage());
}



