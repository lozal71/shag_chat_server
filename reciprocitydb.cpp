#include "reciprocitydb.h"

reciprocityDB::reciprocityDB()
{
}

QVariantMap reciprocityDB::mapResponseAuth(QString login, QString pass)
{
    int id = 0;
    QVariantMap mapResponseDB;
    mapResponseDB["id"] = id;
    queryPull query;
   // запрашиваем id, name по логину и паролю
    QSqlQuery qAuth = query.auth(login,pass);
    while (qAuth.next()){
        // фиксируем id, name
        id = qAuth.value(0).toInt();
        mapResponseDB["id"] = qAuth.value(0).toInt();
        mapResponseDB["name"] = qAuth.value(1).toString();
        // меняем статус клиента на он-лайн и записываем текущее время
        query.userOnLine(id);
        // определяем комнаты, в которых участвует клиент
        mapResponseDB["rooms"] = setMapRooms(id);
    }
    return mapResponseDB;
}


QVariantMap reciprocityDB::readMessage(int roomID, int userID, QString text)
{
    QVariantMap mapResponseDB;
    queryPull query;
    mapResponseDB["sendResult"] = query.insertMessage(roomID,userID,text);
    return mapResponseDB;
}

QVariantMap reciprocityDB::insertNewRoom(int userID, QString roomName)
{
    QVariantMap mapResponseDB;
    queryPull query;
    mapResponseDB["newRoomID"] = query.getNewRoomID(userID, roomName);
    mapResponseDB["newRoomName"] = roomName;
    return mapResponseDB;
}

QVariantMap reciprocityDB::delRoom(int roomID)
{

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

QVariantMap reciprocityDB::setMapRooms(int id)
{
    //выбираем комнаты для клиента
    queryPull query;
    QSqlQuery qRooms = query.selectRooms(id);
    // собираем информацию в MAP
    int roomID;
    QString roomName;
    int userRoleID;
    QVariantMap mapRoomsID; // {roomID:{mapRoomName}}
    QVariantMap mapRoomName; // {roomName:{mapMapMessages}}
    mapRoomName.clear();
    while (qRooms.next()){
        roomID = qRooms.value(0).toInt();
        roomName = qRooms.value(1).toString();
        userRoleID = qRooms.value(2).toInt();
        // определяем сообщения в комнате
        mapRoomName[roomName] = setMapMessages(roomID);
        mapRoomsID[QString::number(roomID)] = mapRoomName;
        mapRoomName.clear();
    }
    //qDebug() << "mapRoomsID" <<  mapRoomsID;
    return mapRoomsID;
}

QVariantMap reciprocityDB::setMapMessages(int roomID)
{
    // выбираем непрочитанные сообщения в комнате
    queryPull query;
    QSqlQuery qMessage = query.selectUnreadMessages(roomID);
    // собираем информацию в MAP
    QVariantMap mapAllMessage; // {"unread":{mapUnreadMessage}},
                               //  "read":{mapReadMessage}}}
    QVariantMap mapUnreadMessage; //  {messTime:{mapSenderMessage}}
    QVariantMap mapReadMessage; //  {messTime:{mapSenderMessage}}
    QVariantMap mapSenderMessage; // {senderName:{textMess}}
    QString messTime;
    QString senderName;
    QString textMess;
    mapSenderMessage.clear();
    while (qMessage.next()){
        messTime = qMessage.value(0).toString();
        senderName = qMessage.value(1).toString();
        textMess = qMessage.value(2).toString();
        mapSenderMessage[senderName] = textMess;
        mapUnreadMessage[messTime] = mapSenderMessage;
        mapSenderMessage.clear();
    }
    mapAllMessage["unread"]=mapUnreadMessage;
    //qDebug() << "mapUnreadMessage" <<  mapUnreadMessage;

    // выбираем прочитанные сообщения в комнате
    qMessage = query.selectReadMessages(roomID);
    while (qMessage.next()){
        messTime = qMessage.value(0).toString();
        senderName = qMessage.value(1).toString();
        textMess = qMessage.value(2).toString();
        mapSenderMessage[senderName] = textMess;
        mapReadMessage[messTime] = mapSenderMessage;
        mapSenderMessage.clear();
    }
    //qDebug() << "mapreadMessage" <<  mapReadMessage;
    mapAllMessage["read"]=mapReadMessage;
    //qDebug() << "mapAllMessage" << mapAllMessage;
    return mapAllMessage;
}
