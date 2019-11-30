#include "reciprocitydb.h"

reciprocityDB::reciprocityDB()
{
    queryP = new queryPull();
    //query = queryPull::getQuery();
}

reciprocityDB::~reciprocityDB()
{
    delete queryP;
}

QMap<int, QString> reciprocityDB::getUserIdUserName(QString login, QString pass)
{
    //qDebug() << "getUserIdUserName";
     int userID = 0;
     QString userName;
     QMap<int, QString> temp;
//     QSqlQuery qAuth = query->getUserIdUserName(login,pass);
     QVariantList lst;
     lst << login << pass;
     qDebug() << lst;
     QSqlQuery qAuth = queryP->makeQuery(setQuery::getUserIdUserName, lst);
    while (qAuth.next()){
        userID = qAuth.value(0).toInt();
        userName = qAuth.value(1).toString();
    }
    //qDebug() << userID << " " << userName;
    temp.insert(userID, userName);
    return temp;
}

void reciprocityDB::insertNewMess(int roomID, int senderID, QString text)
{
   // query->insertNewMess(roomID, senderID, text);
    QVariantList lst;
    QDateTime td;
    lst << roomID << senderID << text << td.currentDateTime();
    queryP->makeQuery(setQuery::insertNewMess,lst);
}

void reciprocityDB::deleteRoom(int roomID)
{
    QVariantList lst;
    lst << roomID;
    queryP->makeQuery(setQuery::deleteRoom, lst);
    //query->deleteRoom(roomID);
}

int reciprocityDB::getNewRoomID(int userID, QString roomName)
{
    QVariantList lst;
    lst << roomName;
    queryP->makeQuery(setQuery::insertNewRoom,lst);
    //query->insertNewRoom(roomName);
    lst.clear();
    lst << userID << roomName;
//    QSqlQuery qRoom = query->getRoomID(roomName);
    QSqlQuery qRoom = queryP->makeQuery(setQuery::getRoomID,lst);
    int roomID = 0;
    while (qRoom.next()){
        roomID = qRoom.value(0).toInt();
    }
    lst.clear();
    lst << roomID << userID;
    //query->insertAdminToRoom(userID,roomID);
    queryP->makeQuery(setQuery::insertAdminToRoom, lst);
    return roomID;
}

int reciprocityDB::getUserStatus(int userID, int roomID)
{
//    QSqlQuery qUser = query->getUserStatus(userID,roomID);
    QVariantList lst;
    lst << userID << roomID;
    QSqlQuery qUser = queryP->makeQuery(setQuery::getUserStatus,lst);
    int statusID = 0;
    while (qUser.next()){
        statusID = qUser.value(0).toInt();
    }
    return statusID;
}


QString reciprocityDB::getRoomName(int roomID)
{
//    QSqlQuery qRoomName = query->getRoomName(roomID);
    QVariantList lst;
    lst  << roomID;
    QSqlQuery qRoomName = queryP->makeQuery(setQuery::getRoomName,lst);
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
    //QSqlQuery qUserName = query->getUserName(userID);
    QVariantList lst;
    lst << userID;
    QSqlQuery qUserName = queryP->makeQuery(setQuery::getUserName,lst);
    while (qUserName.next()){
        userName = qUserName.value(0).toString();
    }
    return userName;
}

int reciprocityDB::getUserID(QString userName)
{
    int userID = 0;
    //QSqlQuery qUser = query->getUserID(userName);
    QVariantList lst;
    lst << userName;
    QSqlQuery qUser = queryP->makeQuery(setQuery::getUserID,lst);
    while (qUser.next()){
        userID = qUser.value(0).toInt();
    }
    return userID;
}

QString reciprocityDB::getRole(int roleID)
{
    //qDebug() << "getRole";
    QString role;
    QVariantList lst;
    lst << roleID;
    QSqlQuery qRole = queryP->makeQuery(setQuery::getRole,lst);
    //QSqlQuery qRole = query->getRole(roleID);
    while (qRole.next()){
        role = qRole.value(0).toString();
    }
    return role;
}

int reciprocityDB::getInvitedUserID(QString userName, int roomID, int senderID)
{
    //queryPull query;
    //QSqlQuery qUserID = query->getUserID(userName);
    QVariantList lst;
    lst << userName;
    QSqlQuery qUserID= queryP->makeQuery(setQuery::getUserID,lst);
    int invitedUserID = 0;
    while (qUserID.next()){
        invitedUserID = qUserID.value(0).toInt();
    }
    //qDebug() << "invitedUserID" << invitedUserID;
    lst.clear();
    lst << roomID << senderID;
    if (invitedUserID != 0) {
//        QSqlQuery qMembers = query->getMembers(roomID, senderID);
        QSqlQuery qMembers = queryP->makeQuery(setQuery::getMembers,lst);
        while (qMembers.next()){
            if (invitedUserID == qMembers.value(0).toInt()){
                invitedUserID = -1;
                break;
            }
        }
    }
    //qDebug() << "invitedUserID" << invitedUserID;
    return invitedUserID;
}

int reciprocityDB::getIdSenderInvite(int inviteID)
{
    //queryPull query;
   // QSqlQuery qIdSenderInvite = query->selectIdSenderInvite(inviteID);
    QVariantList lst;
    lst << inviteID;
    QSqlQuery qIdSenderInvite= queryP->makeQuery(setQuery::getIdSenderInvite,lst);
    int idSenderInvite = 0;
    while (qIdSenderInvite.next()){
        idSenderInvite = qIdSenderInvite.value(0).toInt();
    }
    return idSenderInvite;
}

QString reciprocityDB::getInvitedName(int inviteID)
{
    //queryPull query;
    //QSqlQuery qInvitedName = query->selectInvitedUserName(inviteID);
    QVariantList lst;
    lst << inviteID;
    QSqlQuery qInvitedName= queryP->makeQuery(setQuery::getInvitedUserName,lst);
    QString userName;
    while (qInvitedName.next()){
        userName = qInvitedName.value(0).toString();
    }
    return userName;
}

QMap<int, QString> reciprocityDB::getInvitedRoom(int inviteID)
{
    //queryPull query;
    //QSqlQuery qRoom = query->selectInvitedRoomName(inviteID);
    QVariantList lst;
    lst << inviteID;
    QSqlQuery qRoom= queryP->makeQuery(setQuery::getInvitedRoomName,lst);
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
    QVariantList lst;
    lst << text << roomID << senderID << receiverID;
    QSqlQuery qinsertNewInvite= queryP->makeQuery(setQuery::insertNewInvite,lst);
    //QSqlQuery qinsertNewInvite = query->insertNewInvite(text,roomID,senderID,receiverID);
}

QVariantMap reciprocityDB::getMembers(int roomID, int userID)
{
    QVariantMap temp;
   // QSqlQuery qUser = query->getMembers(roomID, userID);
    QVariantList lst;
    lst << roomID << userID;
    QSqlQuery qUser= queryP->makeQuery(setQuery::getMembers,lst);
    while (qUser.next()){
        temp.insert(qUser.value(0).toString(), qUser.value(1).toString());
    }
    return temp;
}

QVariantMap reciprocityDB::getInvitations(int userID)
{
    //qDebug() << "getInvitations";
    QVariantMap mapInvitations;
    QVariantMap mapInvite;
    int inviteID =0;
    int roomID = 0;
    int senderID = 0;
    QString roomName;
    QString senderName;
    QString text;
    //QSqlQuery qInvtations = query->getWaitInvitation(userID);
    QVariantList lst;
    lst << userID;
    QSqlQuery qInvtations= queryP->makeQuery(setQuery::getWaitInvitation,lst);
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
    QVariantList lst;
    lst << 1 << inviteID;
    queryP->makeQuery(setQuery::updateInvite,lst);
    //query->updateInvite(inviteID,1);
    lst.clear();
    lst << roomID << userID << 1;
    queryP->makeQuery(setQuery::insertInvitedUsers,lst);
    //query->insertInvitedUsers(roomID, userID, 1);
    mapMess = getMapMess(roomID);
    return mapMess;
}

void reciprocityDB::rejectInvite(int inviteID)
{
    //queryPull query;
    //query->updateInvite(inviteID,2);
    QVariantList lst;
    lst << 2 << inviteID;
    queryP->makeQuery(setQuery::updateInvite,lst);
}

void reciprocityDB::deleteUser(int userID, int roomID)
{
    QVariantList lst;
    lst << userID << roomID;
    queryP->makeQuery(setQuery::deleteUser,lst);
    //query->deleteUser(userID,roomID);
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
    QVariantList lst;
    lst << userID;
    queryP->makeQuery(setQuery::setOffLine,lst);
    //query->setOffLine(userID);
}

void reciprocityDB::setOnLine(int userID)
{
    QVariantList lst;
    lst << userID;
    queryP->makeQuery(setQuery::setOnLine,lst);
    //query->setOnLine(userID);
}

QVariantMap reciprocityDB::getMapRoomsID(int userID)
{
    QVariantList lst;
    lst << userID;
    QSqlQuery qRooms = queryP->makeQuery(setQuery::getRooms,lst);
    //QSqlQuery qRooms = query->getRooms(userID);
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
    QVariantList lst;
    lst << roomID << senderID;
    QSqlQuery qMembersOnLine = queryP->makeQuery(setQuery::getMembersIdOnline,lst);
    //QSqlQuery qMembersOnLine = query->getMembersIdOnline(roomID, senderID);
    while (qMembersOnLine.next()){
        listUserOnline.append(qMembersOnLine.value(0).toInt());
     }
    return listUserOnline;
}


QVariantMap reciprocityDB::getMapMess(int roomID)
{
    QVariantMap mapAllMess;
    QVariantList lst;
    lst << roomID;
    mapAllMess["unread"] = collectMapMessID(queryP->makeQuery(setQuery::getUnreadMess,lst));
    //mapAllMess["unread"] = collectMapMessID(query->getUnreadMess(roomID));
    mapAllMess["read"] = collectMapMessID(queryP->makeQuery(setQuery::getReadMess,lst));
    //mapAllMess["read"] = collectMapMessID(query->getReadMess(roomID));
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




