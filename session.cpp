#include "session.h"


session::session()
{

}

session::session(QTcpSocket *socket)
{
    socketSession = socket;
    protocol = new protocolOut(socket);
    db = new reciprocityDB();
    setConnectSession();
}

session::~session(){
    delete protocol;
    delete db;
}

void session::setConnectSession()
{
    // в сокете появились новые данные -
    //читаем пришедший запрос и пишем ответ
    connect (socketSession, &QTcpSocket::readyRead,
             this,&session::readQuery);
    connect(socketSession, &QTcpSocket::disconnected,
            this, &session::connectClosed);
    connect(this, &session::readyWrite,
            protocol, &protocolOut::writeSocket);
}

// обратный ответ пославшему новое сообщение
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

void session::BackAuth(setCodeCommand code)
{
    // обратный ответ из БД запросившему авторизацию
    QVariantMap mapData = mapQuery["joData"].toMap();
    qDebug() <<mapData;
    QMap<int, QString> IdName = db->getUserIdUserName(mapData["login"].toString(),
                                                      mapData["pass"].toString());
    mapData.clear();
    qDebug() << IdName;
    int userID = IdName.firstKey();
    mapData.insert("userID", userID);
    mapData.insert("userName", IdName.first());
     qDebug() <<mapData;
    // добавляем данные о поступивших приглашениях от других пользователей
    QVariantMap mapInvitations = db->getInvitations(userID);
    mapData.insert("invite", mapInvitations);
     qDebug() <<mapData;
    // определяем комнаты, в которых участвует клиент
    // в комнатах определяются сообщения и список участников
    QVariantMap mapRooms = db->getMapRoomsID(userID);
    mapData.insert("rooms", mapRooms);
    qDebug() <<mapData;
    mapBack.insert("joData", mapData);
    mapBack.insert("codeCommand", code);
    qDebug() << mapBack;
    // меняем статус клиента на он-лайн и записываем текущее время
    db->setOnLine(userID);
    emit readyWrite(mapBack);
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
}

void session::messInvite()
{
    QVariantMap mapCommand;
    QVariantMap mapData;
    mapData["invite"] = db->getInvitations(this->client.id);
    mapCommand["codeCommand"] = setCodeCommand::questInvite;
    mapCommand["joDataInput"] = mapData;
    emit readyWrite(mapCommand);
}

int session::getClientID()
{
    return client.id;
}

QString session::getClientName()
{
    return client.name;
}


void session::readQuery()
{
    mapQuery = protocol->readSocket();
    // определяем команду запроса
    setCodeCommand code = setCodeCommand(mapQuery["codeCommand"].toInt());
    // определяем данные для дальнейшей обработки
    QVariantMap mapData =  mapQuery["joData"].toMap();
    if (protocol->isError()){
          qDebug() << "Problem: error massage";
    }
    else {
        switch (code) {
        case setCodeCommand::Auth:
        {
            qDebug() <<  "query auth ";
            BackAuth(code);
//            mapBack = db->mapResponseAuth(mapData["login"].toString(),
//                                               mapData["pass"].toString());
            // фиксируем в сессии id и имя клиента
            this->client.id = mapBack["joData"].toMap()["userID"].toInt();
            this->client.name = mapBack["joData"].toMap()["userName"].toString();
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
            mapBack = backNewMess(roomID, text);
            break;
        }
        case setCodeCommand::NewRoom:
        {
            qDebug() <<  "query new room ";
            // обратный ответ инициатору создания новой комнаты из БД: newRoomID, newRoomName -
            mapBack = db->insertNewRoom(client.id, mapData["roomNew"].toString());
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
            mapBack["delRoomID"] = delRoomID;
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
            mapBack["invitedUserID"] = invitedUserID;
            mapBack["invitedUserName"] = mapData["userName"].toString();
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
            mapBack["mess"] = db->acceptInvite(inviteID,roomID, client.id);
            mapBack["roomID"] = roomID;
            mapBack["roomName"] = mapData["roomName"].toString();
            mapBack["inviteID"] = inviteID;
            QVariantMap mapUsers = db->getMembers(roomID, client.id);
            mapBack["users"] = mapUsers;
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
            mapBack["inviteID"] = inviteID;
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
            mapBack["updateParam"] = setUpdateUsers::removeUser;
            mapBack["timeMess"] = td;
            mapBack["textMess"] = "you is removed from room " + roomName;
            mapBack["senderName"] = "you";
            mapBack["userID"] = userID;
            mapBack["roomID"] = roomID;
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
    mapQuery["joDataInput"]=mapBack;
    //qDebug() << mapCommand;
    emit readyWrite(mapQuery);
}





