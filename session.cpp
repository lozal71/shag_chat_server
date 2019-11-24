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

// обратный ответ пославшему приглашение
void session::backInvite()
{
     QVariantMap mapData = mapQuery["joData"].toMap();
     // узнаем id приглашаемого участника комнаты по его имени
     QString userName = mapData["userName"].toString();
     int roomID = mapData["roomID"].toInt();
     int invitedUserID = db->getInvitedUserID(userName, roomID, client.id);
     mapData.clear();

     mapData.insert("invitedUserID", invitedUserID);
     mapData.insert("invitedUserName",userName);

     mapBack.insert("joData", mapData);
     mapBack.insert("codeCommand", setCodeCommand::Invite);
     emit readyWrite(mapBack);
}

void session::backDelRoom()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    int delRoomID = mapData["delRoomID"].toInt();
    mapData.clear();

    mapData.insert("delRoomID", delRoomID);

    mapBack.insert("joData", mapData);
    mapBack.insert("codeCommand", setCodeCommand::DelRoom);
    emit readyWrite(mapBack);
}

void session::backNewRoom()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    QString roomName = mapData["roomNew"].toString();
    int roomID = db->getNewRoomID(client.id,roomName);
    mapData.clear();
    mapData.insert("newRoomID", roomID);
    mapData.insert("newRoomName",roomName);

    mapBack.insert("joData", mapData);
    mapBack.insert("codeCommand", setCodeCommand::NewRoom);
    emit readyWrite(mapBack);
}

// обратный ответ пославшему новое сообщение
void session::backNewMess()
{
    QVariantMap mapMess = mapQuery["joData"].toMap();
    int roomID = mapMess["roomID"].toInt();
    QString text = mapMess["text"].toString();
    mapMess.clear();

    QDateTime td;
    td =td.currentDateTime();
    mapMess["timeMess"] = td.toString();
    mapMess["senderName"] = client.name;
    mapMess["textMess"] = text;
    mapMess["roomID"] = roomID;

    mapBack.insert("joData", mapMess);
    mapBack.insert("codeCommand", setCodeCommand::newMess);
    emit readyWrite(mapBack);
}

 // обратный ответ запросившему авторизацию
void session::backAuth()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    QMap<int, QString> IdName = db->getUserIdUserName(mapData["login"].toString(),
                                                      mapData["pass"].toString());
    mapData.clear();
    int userID = IdName.firstKey();
    mapData.insert("userID", userID);
    mapData.insert("userName", IdName.first());
    // добавляем данные о поступивших приглашениях от других пользователей
    QVariantMap mapInvitations = db->getInvitations(userID);
    mapData.insert("invite", mapInvitations);
    // определяем комнаты, в которых участвует клиент
    // в комнатах определяются сообщения и список участников
    QVariantMap mapRooms = db->getMapRoomsID(userID);
    mapData.insert("rooms", mapRooms);

    mapBack.insert("joData", mapData);
    mapBack.insert("codeCommand", setCodeCommand::Auth);
    // меняем статус клиента на онлайн и записываем текущее время сессии
    db->setOnLine(userID);
    emit readyWrite(mapBack);
}

// обратный ответ принявшему приглашение
void session::backAcceptInvite()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    QString userName = mapData["userName"].toString();
    QString roomName = mapData["roomName"].toString();
    int roomID = mapData["roomID"].toInt();
    int inviteID = mapData["inviteID"].toInt();
    mapData.clear();

    QVariantMap mapMess = db->acceptInvite(inviteID,roomID, client.id);
    mapData.insert("mess", mapMess);
    mapData.insert("roomID",roomID);
    mapData.insert("roomName", roomName);
    mapData.insert("inviteID", inviteID);
    QVariantMap mapUsers = db->getMembers(roomID, client.id);
    mapData.insert("members", mapUsers);

    mapBack.insert("joData", mapData);
    mapBack.insert("codeCommand", setCodeCommand::acceptInvite);
    emit readyWrite(mapBack);
}

// обратный ответ отклонившему приглашение
void session::backRejectInvite()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    int inviteID = mapData["inviteID"].toInt();
    // делаем отметку в БД об отклонении приглашения
    db->rejectInvite(inviteID);

    mapBack.insert("joData", mapData);
    mapBack.insert("codeCommand", setCodeCommand::acceptInvite);
    emit readyWrite(mapBack);
}

void session::prepareDistribNewMess()
{
    //готовим рассылку сообщения другим участникам комнаты
    int roomID = mapBack["joData"].toMap()["roomID"].toInt();
    QString text = mapBack["joData"].toMap()["textMess"].toString();
    // получаем из БД список онлайн-участников комнаты
    QList<int> listUserOnline = db->getMembersIdOnline(roomID, client.id);
     // записываем новое сообщение в БД
    db->insertNewMess(roomID, client.id, text);
    // испускаем сигнал: послать всем онлайн-участникам комнаты новое сообщение
    emit distribNewMess(listUserOnline, text, client.name, roomID);
}

void session::prepareDistribMessDelRoom()
{
    int delRoomID = mapBack["joData"].toMap()["delRoomID"].toInt();
    QString roomName = db->getRoomName(delRoomID);
    // получаем из БД список онлайн-участников комнаты
    QList<int> listUserOnline = db->getMembersIdOnline(delRoomID, client.id);
    // в комнате -default (id=1) - оставляем сообщение
    db->insertNewMess(1,client.id,"Room " + roomName + " is moving away");
    // удаляем комнату
    db->deleteRoom(delRoomID);
    // испускаем сигнал: послать всем онлайн-участникам комнаты сообщение о ее удалении
    emit distribMessDelRoom(listUserOnline, delRoomID, roomName, client.name);
}

void session::prepareMessInvite()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    int invitedUserID = mapBack["joData"].toMap()["invitedUserID"].toInt();
    // если приглашаемый участник  - допустим
    if (invitedUserID !=0 and invitedUserID != -1)
    {
        QString textInvite = mapData["textInvite"].toString();
        int roomID = mapData["roomID"].toInt();
        QString roomName = db->getRoomName(roomID);
        // записываем новое приглашение в БД
        db->insertNewInvite(textInvite, roomID, client.id, invitedUserID);
        // испускаем сигнал: послать приглашение
        emit sendInvite(invitedUserID, client.name, roomName,textInvite,roomID);
    }
}

void session::prepareMessRejectInvite()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    int senderID = mapData["senderID"].toInt();
    QString senderName = mapData["senderName"].toString();
    QString roomName = mapData["roomName"].toString();
    int roomID =  mapData["roomID"].toInt();
    QString invitedName = mapData["invitedName"].toString();
    QString text = client.name + " reject invite in " + roomName;
    emit sendRejectInvite(senderID,invitedName,roomID,roomName);
}

void session::prepareDistribAcceptInvite()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    QString userName = mapData["userName"].toString();
    QString roomName = mapData["roomName"].toString();
    int roomID = mapData["roomID"].toInt();
    //получаем из БД список онлайн-участников комнаты
    QList<int> listUserOnline = db->getMembersIdOnline(roomID, client.id);
     //записываем в БД сообщение о принятии приглашения и
    db->insertNewMess(roomID, client.id, userName + " accepted invite");
     //испускаем сигнал: послать онлайн-участникам комнаты
     //сообщение о новом пользователе и команду обновить список пользователей в комнате
    emit sendUpdateUsers(listUserOnline, client.id, client.name,
                           roomID, roomName, setUpdateUsers::addUser);
}

void session::sendMessDelRoom(QString roomName, int roomID, QString adminName)
{
    QVariantMap mapData;
    QDateTime td;
    td = td.currentDateTime();
    mapData.insert("timeMess",td);
    mapData.insert("textMess", "Room " + roomName + " is removed");
    mapData.insert("senderName", adminName);
    mapData.insert("roomName", roomName);
    mapData.insert("roomID", roomID);

    mapServerCommand.insert("joData", mapData);
    mapServerCommand.insert("codeCommand", setCodeCommand::MessDelRoom);
    emit readyWrite(mapServerCommand);
}

void session::sendMess(QString text, QString senderName, int roomID)
{
    QVariantMap mapData;
    QDateTime td;
    td = td.currentDateTime();
    mapData["timeMess"] = td;
    mapData["textMess"] = text;
    mapData["senderName"] = senderName;
    mapData["roomID"] = roomID;

    mapServerCommand.insert("joData", mapData);
    mapServerCommand.insert("codeCommand", setCodeCommand::SendMess);
    emit readyWrite(mapServerCommand);
}

void session::sendMessUpdateUsers(int userID, QString userName,
                                   int roomID, QString roomName, setUpdateUsers param)
{
    QVariantMap mapData;
    QDateTime td;
    td = td.currentDateTime();
    mapData.insert("timeMess", td);
    mapData.insert("updateParam", param);
    if (param == setUpdateUsers::addUser){
        mapData.insert("textMess",userName + " is added to room " + roomName);
    }
    else {
        mapData.insert("textMess",userName + " is removed from room " + roomName);
    }
    mapData.insert("senderName", userName);
    mapData.insert("userID", userID);
    mapData.insert("roomID", roomID);

    mapServerCommand.insert("joData", mapData);
    mapServerCommand.insert("codeCommand", setCodeCommand::updateUsers);
    emit readyWrite(mapServerCommand);
}

void session::sendMessInvite()
{
    QVariantMap mapData;
    mapData["invite"] = db->getInvitations(this->client.id);
    mapServerCommand.insert("joData", mapData);
    mapServerCommand.insert("codeCommand", setCodeCommand::questInvite);
    emit readyWrite(mapServerCommand);
}

void session::setOffLine(int userID)
{
    db->setOffLine(userID);
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
             // обратный ответ запросившему авторизацию
            backAuth();
            // фиксируем в сессии id и имя клиента
            this->client.id = mapBack["joData"].toMap()["userID"].toInt();
            this->client.name = mapBack["joData"].toMap()["userName"].toString();
            break;
        }
        case setCodeCommand::newMess:
        {
            qDebug() <<  "query new message ";
            // обратный ответ автору нового сообщения
            backNewMess();
            // готовим рассылку сообщения другим участникам комнаты
            prepareDistribNewMess();
            break;
        }
        case setCodeCommand::NewRoom:
        {
            qDebug() <<  "query new room ";
            // обратный ответ инициатору создания комнаты
            backNewRoom();
            break;
        }
        case setCodeCommand::DelRoom:
        {
            qDebug() <<  "query del room ";
            backDelRoom();
            prepareDistribMessDelRoom();
            break;
        }
        case setCodeCommand::Invite:
        {
            qDebug() <<  "query invite ";
            backInvite();
            prepareMessInvite();
            break;
        }
        case setCodeCommand::acceptInvite:
        {
            qDebug() <<  "query acceptInvite ";
            backAcceptInvite();
            prepareDistribAcceptInvite();
            break;
        }
        case setCodeCommand::rejectInvite:
        {
            qDebug() <<  "query rejectInvite ";
            backRejectInvite();
//            int inviteID = mapData["inviteID"].toInt();
//            // делаем отметку в БД об отклонении приглашения
//            db->rejectInvite(inviteID);
            // получаем данные о комнате, приглашение в которую было отказано
            QMap<int, QString> room = db->getInvitedRoom(inviteID);
            QString text = client.name + " reject invite in " + room.first();
            // записываем в БД сообщение об отклонении приглашения и
            // получаем из БД список онлайн-участников комнаты
           // QList<int> listUserOnline = db->insertNewMess(room.firstKey(), client.id, text);
            // испускаем сигнал: послать онлайн-участникам комнаты
            // сообщение об отклонении приглашения
           // emit distribNewMess(listUserOnline, text, client.name, room.firstKey());
            // обратный ответ пославшениму отклонение приглашения
            //mapBack["inviteID"] = inviteID;
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
           // QList<int> listUserOnline = db->insertNewMess(roomID, client.id, text);
            // испускаем сигнал: послать онлайн-участникам комнаты
            // сообщение об удалении пользователя и команду обновить список пользователей в комнате
           // emit sendUpdateUsers(listUserOnline, userID, userName,
           //                        roomID, roomName, setUpdateUsers::removeUser);
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
}





