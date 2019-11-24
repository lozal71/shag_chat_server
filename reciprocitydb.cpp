#include "reciprocitydb.h"

reciprocityDB::reciprocityDB()
{
    query = new queryPull();
}

reciprocityDB::~reciprocityDB()
{
    delete query;
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

void reciprocityDB::insertNewMess(int roomID, int senderID, QString text)
{
    query->insertNewMess(roomID, senderID, text);
}

void reciprocityDB::deleteRoom(int roomID)
{
    query->deleteRoom(roomID);
}

int reciprocityDB::getNewRoomID(int userID, QString roomName)
{
    query->insertNewRoom(roomName);
    QSqlQuery qRoom = query->getRoomID(roomName);
    int roomID = 0;
    while (qRoom.next()){
        roomID = qRoom.value(0).toInt();
    }
    query->insertAdminToRoom(userID,roomID);
    return roomID;
}


QString reciprocityDB::getRoomName(int roomID)
{
    QSqlQuery qRoomName = query->getRoomName(roomID);
    QString roomName;
    while(qRoomName.next()){
        roomName = qRoomName.value(0).toString();
    }
    qDebug() << "getRoomName roomName" << roomName;
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

int reciprocityDB::getUserID(QString userName)
{
    int userID = 0;
    QSqlQuery qUser = query->getUserID(userName);
    while (qUser.next()){
        userID = qUser.value(0).toInt();
    }
    return userID;
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

int reciprocityDB::getInvitedUserID(QString userName, int roomID, int senderID)
{
    //queryPull query;
    QSqlQuery qUserID = query->getUserID(userName);
    int invitedUserID = 0;
    while (qUserID.next()){
        invitedUserID = qUserID.value(0).toInt();
    }
    qDebug() << "invitedUserID" << invitedUserID;
    if (invitedUserID != 0) {
        QSqlQuery qMembers = query->getMembers(roomID, senderID);
        while (qMembers.next()){
            if (invitedUserID == qMembers.value(0).toInt()){
                invitedUserID = -1;
                break;
            }
        }
    }
    qDebug() << "invitedUserID" << invitedUserID;
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

QList<int> reciprocityDB::getMembersIdOnline(int roomID, int senderID)
{
    QList<int> listUserOnline;
     // делаем запрос в БД о других (кроме пославшего сообщение)
    // участниках комнаты в режиме онлайн
    QSqlQuery qMembersOnLine = query->getMembersIdOnline(roomID, senderID);
    while (qMembersOnLine.next()){
        listUserOnline.append(qMembersOnLine.value(0).toInt());
     }
    return listUserOnline;
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




