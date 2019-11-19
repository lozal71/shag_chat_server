#include "reciprocitydb.h"

reciprocityDB::reciprocityDB()
{
}

QVariantMap reciprocityDB::mapResponseAuth(QString login, QString pass)
{
    int userID = 0;
    QString userName;
    QVariantMap mapResponseDB;
    queryPull query;
   // запрашиваем в БД id, name по логину и паролю
    QSqlQuery qAuth = query.auth(login,pass);
    // фиксируем id, name
    while (qAuth.next()){
        userID = qAuth.value(0).toInt();
        userName = qAuth.value(1).toString();
         // меняем статус клиента на он-лайн и записываем текущее время
        query.userOnLine(userID);
        // определяем комнаты, в которых участвует клиент
        mapResponseDB["rooms"] = setMapRoomsID(userID);
        mapResponseDB["userName"] = userName;
        mapResponseDB["userID"] = userID;
        //qDebug() << "mapResponseDB" << mapResponseDB;
    }
    return mapResponseDB;
}


QList<int> reciprocityDB::insertMessage(int roomID, int senderID, QString text)
{
    QVariantMap mapResponseDB;
    queryPull query;
    // вставляем текст сообщения в БД
    query.insertMessage(roomID, senderID, text);
    int userID = 0;
    int userStatus = 0;
    QList<int> listUserOnline;
     // делаем запрос в БД о других (кроме пославшего сообщение)
    // пользователях в режиме онлайн,
    // которым будем делать рассылку поступившего сообщения
    QSqlQuery qUserFromRoom = query.selectUserFromRoom(roomID, senderID);
    // собираем итоговый ответ в MAP
    while (qUserFromRoom.next()){
        userID = qUserFromRoom.value(0).toInt();
        userStatus = qUserFromRoom.value(2).toInt();
        //castRoomID = qUserFromRoom.value(3).toInt();
        if (userStatus == 1) {
            listUserOnline.append(userID);
        }
     }
    return listUserOnline;
}

QVariantMap reciprocityDB::insertNewRoom(int userID, QString roomName)
{
    QVariantMap mapResponseDB;
    queryPull query;
    mapResponseDB["newRoomID"] = query.getNewRoomID(userID, roomName);
    mapResponseDB["newRoomName"] = roomName;
    return mapResponseDB;
}

QList<int> reciprocityDB::delRoom(int roomID, int adminID)
{
    int userID = 0;
    int userStatus;
    QString delRoomName;
    QList<int> listUserOnline;
    queryPull query;
    QSqlQuery qUserFromDelRoom = query.selectUserFromRoom(roomID, adminID);
    while (qUserFromDelRoom.next()){
        userID = qUserFromDelRoom.value(0).toInt();
        delRoomName = qUserFromDelRoom.value(1).toString();
        userStatus = qUserFromDelRoom.value(2).toInt();
        if (userStatus == 1) {
            listUserOnline.append(userID);
        }
        else{
            query.insertMessage(1,adminID,"Room " + delRoomName + " is moving away");
        }
    }
    query.delRoom(roomID);
    return listUserOnline;
}

QString reciprocityDB::getRoomName(int roomID)
{
    queryPull query;
    QSqlQuery qRoomName = query.selectRoomName(roomID);
    QString roomName;
    while(qRoomName.next()){
        roomName = qRoomName.value(0).toString();
    }
    //qDebug() << "getRoomName roomName" << roomName;
    return roomName;
}

int reciprocityDB::checktInvitedUser(QString userName, int roomID,
                                     QString textInvite, int senderID)
{
    queryPull query;
    QSqlQuery qUserStatusID = query.selectUserStatusID(userName);
    int invitedUserID = 0;
    //int invitedUserStatusID = 0;
    while (qUserStatusID.next()){
        invitedUserID = qUserStatusID.value(0).toInt();
        query.insertNewInvite(textInvite, roomID, senderID, invitedUserID);
        //invitedUserStatusID = qUserStatusID.value(1).toInt();
    }
    return invitedUserID;
}

QVariantMap reciprocityDB::getInvitations(int userID)
{
    QVariantMap mapInvitations;
    QVariantMap mapInvite;
    int inviteID =0;
    int roomID = 0;
    int senderID = 0;
    QString roomName;
    QString senderName;
    queryPull query;
    QSqlQuery qInvtations = query.selectInvite(userID);
    while (qInvtations.next()){
        inviteID = qInvtations.value(0).toInt();
        mapInvite.insert("text", qInvtations.value(1).toString());
        roomID = qInvtations.value(2).toInt();
        mapInvite["roomID"] = roomID;
        QSqlQuery qRoomName = query.selectRoomName(roomID);
        while(qRoomName.next()){
            roomName = qRoomName.value(0).toString();
        }
        mapInvite.insert("roomName", roomName);
        senderID = qInvtations.value(3).toInt();
        mapInvite["senderID"] = senderID;
        QSqlQuery qUserName = query.selectUserName(senderID);
        while (qUserName.next()){
            senderName = qUserName.value(0).toString();
        }
        mapInvite.insert("senderName",senderName);
        mapInvitations.insert(QString::number(inviteID), mapInvite);
    }
    return  mapInvitations;
}

QVariantMap reciprocityDB::acceptInvite(int inviteID, int roomID, int userID)
{
    QVariantMap mapMess;
    queryPull query;
    query.updateInviteAccept(inviteID);
    query.insertInvitedUsers(roomID, userID, 1);
    mapMess = setMapStatusMess(roomID);
    return mapMess;
}


void reciprocityDB::connectChatToDB()
{
    chatDB = QSqlDatabase::addDatabase("QSQLITE");
    QString pathBIN = QDir::currentPath();
    QString pathDB = QFileInfo(pathBIN).path();
    qDebug() <<"pathDB" << pathDB;
    QString sTemp;
    //dbChat.setDatabaseName("D:/itstep/Qt/Projects/server_221019/chat_server-master/chat_server-master/chat.db3");
    chatDB.setDatabaseName(pathDB+"/DBchat/chat.db3");
    if (!chatDB.open())
    {
        sTemp = "error connect \n";
    }
    else
    {
        sTemp = "connect to db \n";

        //qDebug() << "emit dbConnected(sTemp);";
        //emit dbConnected(sTemp);
    }
    qDebug() << sTemp;
}

void reciprocityDB::setStatusOFFline(int id)
{
    queryPull query;
    query.userOffLine(id);
}

QVariantMap reciprocityDB::setMapRoomsID(int id)
{
    queryPull query;
    // выбираем из БД roomID, roomName, roleID (роль - администратор/пользователь)
    QSqlQuery qRooms = query.selectRooms(id);
    // собираем информацию в MAP
    QVariantMap mapRoomsID;
    QVariantMap mapRooms;
    QVariantMap mapMess;
    int roomID = 0;
    QString roomName;
    int roleID;
    QSqlQuery qRole;
    QString role;
    while (qRooms.next()){
        roomID = qRooms.value(0).toInt();
        roomName = qRooms.value(1).toString();
        roleID = qRooms.value(2).toInt();
        //qDebug() << "roleID" << roleID;
        mapRooms.insert("roomID", roomID);
        mapRooms.insert("roomName", roomName);
        // определяем название роли admin или user
        qRole = query.selectRole(roleID);
        while (qRole.next()) {
            role = qRole.value(0).toString();
            mapRooms.insert("role", role);

        }
        // определяем сообщения в комнате
        mapMess = setMapStatusMess(roomID);
        mapRooms.insert("mess", mapMess);
        mapRoomsID.insert(qRooms.value(0).toString(),mapRooms);
        mapRooms.clear();
    }
    return mapRoomsID;
}


QVariantMap reciprocityDB::setMapStatusMess(int roomID)
{
    queryPull query;
    // собираем информацию в MAP
    QVariantMap mapAllMess;
    mapAllMess["unread"] = collectMapMessID(query.selectUnreadMessages(roomID));
    //qDebug() <<"mapAllMess[\"unread\"]"<< mapAllMess["unread"];
    mapAllMess["read"] = collectMapMessID(query.selectReadMessages(roomID));
    //qDebug() <<"mapAllMess[\"read\"]"<< mapAllMess["read"];
    return mapAllMess;
}

QVariantMap reciprocityDB::collectMapMessID(QSqlQuery qMessage)
{
    QVariantMap mapMessID;
    QVariantMap mapMess;
    while (qMessage.next()){
        mapMess.insert("timeMess", qMessage.value(1).toString());
        mapMess.insert("senderName", qMessage.value(2).toString());
        mapMess.insert("textMess", qMessage.value(3).toString());
        mapMessID.insert(qMessage.value(0).toString(),mapMess);
        mapMess.clear();
    }
    return mapMessID;
}




