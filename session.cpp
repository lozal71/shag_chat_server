#include "session.h"

session::session()
{

}

session::session(QTcpSocket *socket)
{
    socketSession = socket;
    protocol = new net(socket);
    db = new reciprocityDB();
    client.id = 0;
    client.name = "";
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
            protocol, &net::writeSocket);
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
     mapBack.insert("codeCommand", setCodeCommand::invite);
     emit readyWrite(mapBack);
}

void session::backDelRoom()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    int delRoomID = mapData["delRoomID"].toInt();
    mapData.clear();

    mapData.insert("delRoomID", delRoomID);

    mapBack.insert("joData", mapData);
    mapBack.insert("codeCommand", setCodeCommand::delRoom);
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
    mapBack.insert("codeCommand", setCodeCommand::newRoom);
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
    mapBack.insert("codeCommand", setCodeCommand::auth);
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
    mapData.clear();
    // делаем отметку в БД об отклонении приглашения
    db->rejectInvite(inviteID);

    mapData.insert("inviteID",inviteID);
    mapBack.insert("joData", mapData);
    mapBack.insert("codeCommand", setCodeCommand::rejectInvite);
    emit readyWrite(mapBack);
}

// обратный ответ администратору комнаты, который удаляет пользователя
void session::backDelUser()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    int roomID = mapData["roomID"].toInt();
    int userID =  mapData["userID"].toInt();
    QString roomName = db->getRoomName(roomID);
    QString userName = db->getUserName(userID);
    mapData.clear();

    QDateTime td;
    td = td.currentDateTime();
    mapData["updateParam"] = setUpdateUsers::removeUser;
    mapData["timeMess"] = td;
    mapData["textMess"] = userName + " is removed the room " + roomName;
    mapData["senderName"] = client.name;
    mapData["userID"] = userID;
    mapData["roomID"] = roomID;

    mapBack.insert("joData", mapData);
    mapBack.insert("codeCommand", setCodeCommand::delUser);
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
    if (invitedUserID !=0 && invitedUserID != -1)
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
    int roomID =  mapData["roomID"].toInt();
    int statusID = db->getUserStatus(senderID, roomID);
    //QString senderName = mapData["senderName"].toString();
    QString roomName = mapData["roomName"].toString();
    QString invitedName = mapData["invitedName"].toString();
    QString text = client.name + " rejected an invitation ";
    db->insertNewMess(roomID, client.id, text);
    QList<int> listUserOnline;
    if (statusID == 1) {
        listUserOnline.append(senderID);
        // испускаем сигнал: послать приглашавшему сообщение об отказе
        emit distribNewMess(listUserOnline, text, client.name, roomID);
    }
}

void session::prepareDistribAcceptInvite()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    QString userName = mapData["userName"].toString();
    QString roomName = mapData["roomName"].toString();
    int roomID = mapData["roomID"].toInt();
    //получаем из БД список онлайн-участников комнаты
    QList<int> listUserOnline = db->getMembersIdOnline(roomID, client.id);
     //записываем в БД сообщение о принятии приглашения
    db->insertNewMess(roomID, client.id, userName + " accepted invite");
     //испускаем сигнал: послать онлайн-участникам комнаты
     //сообщение о новом пользователе и команду обновить список пользователей в комнате
    emit sendUpdateUsers(listUserOnline, client.id, client.name,
                         roomID, roomName, setUpdateUsers::addUser);
}

void session::prepareDistribDelUser()
{
    QVariantMap mapData = mapQuery["joData"].toMap();
    int roomID = mapData["roomID"].toInt();
    int userID =  mapData["userID"].toInt();
    QString roomName = db->getRoomName(roomID);
    QString userName = db->getUserName(userID);
    QString textMess = client.name + " deleted user " + userName  + " from " + roomName;
    QString textDel =  textMess + " text" + mapData["text"].toString();
    mapData.clear();

    // получаем из БД список онлайн-участников комнаты
    QList<int> listUserOnline = db->getMembersIdOnline(roomID, client.id);
    // оставляем сообщение
    db->insertNewMess(roomID, client.id, textMess);
    // если удаляемый пользователь - не в сети , оставляем сообщение
    // для него в комнате default (id=1)
    if (!listUserOnline.contains(userID)){
        db->insertNewMess(1, client.id, textDel);
    }
    else {
        emit sendToDelUser(userID, roomID, textDel);
    }
    // удаляем пользователя из комнаты в БД
    db->deleteUser(userID, roomID);
    // снова получаем из БД список онлайн-участников комнаты (уже без удаляемого)
    listUserOnline = db->getMembersIdOnline(roomID, client.id);
    //испускаем сигнал: послать онлайн-участникам комнаты
    //сообщение об удалении пользователя и команду обновить список пользователей в комнате
    emit sendUpdateUsers(listUserOnline, userID, userName,
                        roomID, roomName, setUpdateUsers::removeUser);
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
    mapServerCommand.insert("codeCommand", setCodeCommand::messDelRoom);
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
    mapServerCommand.insert("codeCommand", setCodeCommand::sendMess);
    emit readyWrite(mapServerCommand);
}

void session::sendMessUpdateUsers(int userID, QString userName,
                                   int roomID, QString roomName, setUpdateUsers param)
{
    QVariantMap mapData;
//    QDateTime td;
//    td = td.currentDateTime();
    mapData.insert("timeMess", QDateTime::currentDateTime());
    mapData.insert("updateParam", param);
    if (param == setUpdateUsers::addUser){
        mapData.insert("textMess",userName + " is added to room " + roomName);
    }
    else {
        if (client.id == userID){
                mapData.insert("updateParam", setUpdateUsers::delIsYou);
        }
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

void session::sendMessToDelUser(int roomID, QString textDel)
{
    QVariantMap mapData;
    QDateTime td;
    td = td.currentDateTime();
    mapData.insert("timeMess", td);
    mapData.insert("updateParam", setUpdateUsers::delIsYou);
    mapData.insert("roomID", roomID);
    mapData.insert("textDel", textDel);
    mapData.insert("senderName", "admin");
    mapData.insert("userID", 0);
    mapData.insert("textMess", textDel);
    mapServerCommand.insert("joData", mapData);
    mapServerCommand.insert("codeCommand", setCodeCommand::updateUsers);
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
        case setCodeCommand::auth:
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
        case setCodeCommand::newRoom:
        {
            qDebug() <<  "query new room ";
            // обратный ответ инициатору создания комнаты
            backNewRoom();
            break;
        }
        case setCodeCommand::delRoom:
        {
            qDebug() <<  "query del room ";
            backDelRoom();
            prepareDistribMessDelRoom();
            break;
        }
        case setCodeCommand::invite:
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
            prepareMessRejectInvite();
            break;
        }
        case setCodeCommand::delUser:
        {
            qDebug() <<  "query delete user";
            backDelUser();
            prepareDistribDelUser();
            break;
        }
        case setCodeCommand::messDelRoom:
        {
            // эта команда не приходит от клиента, а инициируется на сервере,
            // поэтому здесь не рассматривается
            break;
        }
        case setCodeCommand::sendMess:
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





