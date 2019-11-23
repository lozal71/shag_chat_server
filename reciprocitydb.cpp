#include "reciprocitydb.h"

reciprocityDB::reciprocityDB()
{
    query = new queryPull();
}

reciprocityDB::~reciprocityDB()
{
    delete query;
}

QVariantMap reciprocityDB::mapResponseAuth(QString login, QString pass)
{
    int userID = 0;
    QString userName;
    QVariantMap mapResponseDB;
    //qDebug() << login << " " << pass;
    //queryPull query;

   // запрашиваем в БД id, name по логину и паролю
    QSqlQuery qAuth = query->getUserIdUserName(login,pass);
    // фиксируем id, name
    while (qAuth.next()){
        userID = qAuth.value(0).toInt();
        userName = qAuth.value(1).toString();
        mapResponseDB["userName"] = userName;
        mapResponseDB["userID"] = userID;
        // добавляем данные о поступивших приглашениях от других пользователей
        mapResponseDB["invite"] = getInvitations(userID);
        // определяем комнаты, в которых участвует клиент
        mapResponseDB["rooms"] = getMapRoomsID(userID);
         // меняем статус клиента на он-лайн и записываем текущее время
        query->setOnLine(userID);
        //qDebug() << "mapResponseDB" << mapResponseDB;
    }
    return mapResponseDB;
}

QMap<int, QString> reciprocityDB::getUserIdUserName(QString login, QString pass)
{
    qDebug() << "getUserIdUserName";
     int userID = 0;
     QString userName;
     QMap<int, QString> temp;
     QSqlQuery qAuth = query->getUserIdUserName(login,pass);
    while (qAuth.next()){
        userID = qAuth.value(0).toInt();
        userName = qAuth.value(1).toString();
    }
    qDebug() << userID << " " << userName;
    temp.insert(userID, userName);
    return temp;
}

QList<int> reciprocityDB::insertNewMessage(int roomID, int senderID, QString text)
{
    QVariantMap mapResponseDB;
    //queryPull query;

    // вставляем текст сообщения в БД
    query->insertMessage(roomID, senderID, text);
    int userID = 0;
    int userStatus = 0;
    QList<int> listUserOnline;
     // делаем запрос в БД о других (кроме пославшего сообщение)
    // участниках комнаты в режиме онлайн
    QSqlQuery qUserFromRoom = query->selectUserFromRoom(roomID, senderID);
    // собираем итоговый ответ в MAP
    while (qUserFromRoom.next()){
        userID = qUserFromRoom.value(0).toInt();
        userStatus = qUserFromRoom.value(2).toInt();
        if (userStatus == 1) {
            listUserOnline.append(userID);
        }
     }
    return listUserOnline;
}

QVariantMap reciprocityDB::insertNewRoom(int userID, QString roomName)
{
    QVariantMap mapResponseDB;
    //queryPull query;
    mapResponseDB["newRoomID"] = query->getNewRoomID(userID, roomName);
    mapResponseDB["newRoomName"] = roomName;
    return mapResponseDB;
}

QList<int> reciprocityDB::delRoom(int roomID, int adminID)
{
    int userID = 0;
    int userStatus;
    QString delRoomName;
    QList<int> listUserOnline;
    //queryPull query;
    QSqlQuery qUserFromDelRoom = query->selectUserFromRoom(roomID, adminID);
    while (qUserFromDelRoom.next()){
        userID = qUserFromDelRoom.value(0).toInt();
        delRoomName = qUserFromDelRoom.value(1).toString();
        userStatus = qUserFromDelRoom.value(2).toInt();
        if (userStatus == 1) {
            listUserOnline.append(userID);
        }
        else{
            query->insertMessage(1,adminID,"Room " + delRoomName + " is moving away");
        }
    }
    query->delRoom(roomID);
    return listUserOnline;
}

QString reciprocityDB::getRoomName(int roomID)
{
    //queryPull query;
    QSqlQuery qRoomName = query->getRoomName(roomID);
    QString roomName;
    while(qRoomName.next()){
        roomName = qRoomName.value(0).toString();
    }
    //qDebug() << "getRoomName roomName" << roomName;
    return roomName;
}

QString reciprocityDB::getUserName(int userID)
{
    QString userName;
    QSqlQuery qUserName = query->getUserName(userID);
    while (qUserName.next()){
        userName = qUserName.value(0).toString();
    }
    return userName;
}

QString reciprocityDB::getRole(int roleID)
{
    qDebug() << "getRole";
    QString role;
    QSqlQuery qRole = query->getRole(roleID);
    while (qRole.next()){
        role = qRole.value(0).toString();
    }
    return role;
}

int reciprocityDB::getInvitedUserID(QString userName, int roomID)
{
    //queryPull query;
    QSqlQuery qUserID = query->selectUserID(userName);
    int invitedUserID = 0;
    while (qUserID.next()){
        invitedUserID = qUserID.value(0).toInt();
    }
    if (invitedUserID != 0) {
        QSqlQuery qUserIdFromRoom = query->selectUserIdFromRoom(roomID);
        while (qUserIdFromRoom.next()){
            if (invitedUserID == qUserIdFromRoom.value(0).toInt()){
                invitedUserID = -1;
                break;
            }
        }
    }
    return invitedUserID;
}

int reciprocityDB::getIdSenderInvite(int inviteID)
{
    //queryPull query;
    QSqlQuery qIdSenderInvite = query->selectIdSenderInvite(inviteID);
    int idSenderInvite = 0;
    while (qIdSenderInvite.next()){
        idSenderInvite = qIdSenderInvite.value(0).toInt();
    }
    return idSenderInvite;
}

QString reciprocityDB::getInvitedName(int inviteID)
{
    //queryPull query;
    QSqlQuery qInvitedName = query->selectInvitedUserName(inviteID);
    QString userName;
    while (qInvitedName.next()){
        userName = qInvitedName.value(0).toString();
    }
    return userName;
}

QMap<int, QString> reciprocityDB::getInvitedRoom(int inviteID)
{
    //queryPull query;
    QSqlQuery qRoom = query->selectInvitedRoomName(inviteID);
    QString roomName;
    int roomID;
    QMap<int, QString> temp;
    while (qRoom.next()){
        roomName = qRoom.value(0).toString();
        roomID = qRoom.value(1).toInt();
    }
    temp.insert(roomID,roomName);
    return temp;
}

void reciprocityDB::insertNewInvite(QString text, int roomID, int senderID, int receiverID)
{
    //queryPull query;
    QSqlQuery qinsertNewInvite = query->insertNewInvite(text,roomID,senderID,receiverID);
}

QVariantMap reciprocityDB::getMembers(int roomID, int userID)
{
    QVariantMap temp;
    QSqlQuery qUser = query->getMembers(roomID, userID);
    while (qUser.next()){
        temp.insert(qUser.value(0).toString(), qUser.value(1).toString());
    }
    return temp;
}

QVariantMap reciprocityDB::getInvitations(int userID)
{
    qDebug() << "getInvitations";
    QVariantMap mapInvitations;
    QVariantMap mapInvite;
    int inviteID =0;
    int roomID = 0;
    int senderID = 0;
    QString roomName;
    QString senderName;
    QString text;
    QSqlQuery qInvtations = query->getWaitInvitation(userID);
    while (qInvtations.next()){
        inviteID = qInvtations.value(0).toInt();
        text = qInvtations.value(1).toString();
        roomID = qInvtations.value(2).toInt();
        senderID = qInvtations.value(3).toInt();
        roomName = getRoomName(roomID);
        senderName=getUserName(senderID);
        mapInvite.insert("text", text);
        mapInvite.insert("roomID", roomID);
        mapInvite.insert("roomName", roomName);
        mapInvite.insert("senderID", senderID);
        mapInvite.insert("senderName",senderName);
        mapInvitations.insert(QString::number(inviteID), mapInvite);
        mapInvite.clear();
    }
    return  mapInvitations;
}

QVariantMap reciprocityDB::acceptInvite(int inviteID, int roomID, int userID)
{
    QVariantMap mapMess;
    query->updateInvite(inviteID,1);
    query->insertInvitedUsers(roomID, userID, 1);
    mapMess = getMapMess(roomID);
    return mapMess;
}

void reciprocityDB::rejectInvite(int inviteID)
{
    //queryPull query;
    query->updateInvite(inviteID,2);
}

void reciprocityDB::deleteUser(int userID, int roomID)
{
    //queryPull query;
    QSqlQuery qDel=query->deleteUser(userID,roomID);
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
    }
    qDebug() << sTemp;
}

void reciprocityDB::setOffLine(int userID)
{
    query->setOffLine(userID);
}

void reciprocityDB::setOnLine(int userID)
{
    query->setOnLine(userID);
}

QVariantMap reciprocityDB::getMapRoomsID(int userID)
{
    qDebug() << "getMapRoomsID";
    QSqlQuery qRooms = query->getRooms(userID);
    QVariantMap mapRoomsID;
    QVariantMap mapRooms;
    QVariantMap mapMess;
    QVariantMap mapMembers;
    int roomID = 0;
    QString roomName;
    int roleID;
    QString role;
    while (qRooms.next()){
        roomID = qRooms.value(0).toInt();
        roomName = qRooms.value(1).toString();
        roleID = qRooms.value(2).toInt();
        role =getRole(roleID);
        mapRooms.insert("roomID", roomID);
        mapRooms.insert("roomName", roomName);
        mapRooms.insert("role", role);
        // определяем сообщения в комнате с разделением на прочитанные/непрочитанные
        mapMess = getMapMess(roomID);
        mapRooms.insert("mess", mapMess);
        // определяем участников комнаты, кроме userID
        mapMembers = getMembers(roomID, userID);
        mapRooms.insert("members", mapMembers);
        mapRoomsID.insert(qRooms.value(0).toString(),mapRooms);
        mapRooms.clear();
        mapMess.clear();
        mapMembers.clear();
    }
    return mapRoomsID;
}


QVariantMap reciprocityDB::getMapMess(int roomID)
{
    QVariantMap mapAllMess;
    mapAllMess["unread"] = collectMapMessID(query->getUnreadMess(roomID));
    mapAllMess["read"] = collectMapMessID(query->getReadMess(roomID));
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




