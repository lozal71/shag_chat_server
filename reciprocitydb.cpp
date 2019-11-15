#include "reciprocitydb.h"

reciprocityDB::reciprocityDB()
{
}

QVariantMap reciprocityDB::mapResponseAuth(QString login, QString pass)
{
    int userID = 0;
    QString userName;
    QVariantMap mapResponseDB;
    QVariantMap mapUserName;
    queryPull query;
   // запрашиваем id, name по логину и паролю
    QSqlQuery qAuth = query.auth(login,pass);
    while (qAuth.next()){
        // фиксируем id, name
        userID = qAuth.value(0).toInt();
        userName = qAuth.value(1).toString();
         // меняем статус клиента на он-лайн и записываем текущее время
        query.userOnLine(userID);

        // определяем комнаты, в которых участвует клиент
        mapUserName.insert(userName,setMapUserRole(userID));
        mapResponseDB.insert(QString::number(userID),mapUserName);
    }
    return mapResponseDB;
}


QVariantMap reciprocityDB::insertMessage(int roomID, int userID, QString text)
{
    QVariantMap mapResponseDB;
    queryPull query;
    // вставляем текст сообщения в БД
    query.insertMessage(roomID,userID,text);
    // запрашиваем имя отправителя сообщения
    QString senderName;
    QSqlQuery qSenderName = query.selectUserName(userID);
    while (qSenderName.next()){
        senderName = qSenderName.value(0).toString();
    }
    // формируем MAP
    QVariantMap mapMess;
    QDateTime td;
    td = td.currentDateTime();
    mapMess["timeMess"] = td.toString();
    mapMess["senderName"] = senderName;
    mapMess["textMess"] = text;
    int id = 0;
    int status = 0;
    //int castRoomID = 0;

    // делаем запрос в БД о пользователях в режиме онлайн,
    // которым будем делать рассылку поступившего сообщения
    QSqlQuery qUserFromRoom = query.selectUserFromRoom(roomID, userID);
    // собираем итоговый ответ в MAP
    while (qUserFromRoom.next()){
        id = qUserFromRoom.value(0).toInt();
        status = qUserFromRoom.value(2).toInt();
        //castRoomID = qUserFromRoom.value(3).toInt();
        if (status == 1) {
            QVariantMap mapRoomID;
            mapRoomID.insert(QString::number(roomID), mapMess);
            mapResponseDB.insert(QString::number(id),mapRoomID);
        }
        else {
            query.insertMessage(roomID, userID, text);
        }
     }
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

QMap<int,QString> reciprocityDB::delRoom(int roomID, int adminID)
{
    int id = 0;
    int status;
    QString delRoomName;
    QMap<int,QString> mapUserOnline;
    queryPull query;
    QSqlQuery qUserFromDelRoom = query.selectUserFromRoom(roomID, adminID);
    while (qUserFromDelRoom.next()){
        id = qUserFromDelRoom.value(0).toInt();
        delRoomName = qUserFromDelRoom.value(1).toString();
        status = qUserFromDelRoom.value(2).toInt();
        if (status == 1) {
            mapUserOnline[id] = "Room " + delRoomName + " is moving away";
            //query.insertMessage(1,adminID,"Room " + delRoomName + " is moving away");
        }
        else{
            query.insertMessage(1,adminID,"Room " + delRoomName + " is moving away");
        }
    }
    query.delRoom(roomID);

    //qDebug() << "mapUserOnline" << mapUserOnline;
    return mapUserOnline;
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

QVariantMap reciprocityDB::setMapUserRole(int id)
{
    queryPull query;
    //выбираем из БД комнаты, в которых клиент - admin
    QSqlQuery qRoomsAdminRole = query.selectRooms(id,1);
    //выбираем из БД комнаты, в которых клиент - user
    QSqlQuery qRoomsUserRole = query.selectRooms(id,2);
    // собираем информацию в MAP
    QVariantMap mapUserRole;
    // собираем комнаты, в которых клиент - admin
    mapUserRole.insert("admin",collectMapRoomsID(qRoomsAdminRole));
    // собираем комнаты, в которых клиент - user
    mapUserRole.insert("user",collectMapRoomsID(qRoomsUserRole));
    qDebug() << "149 mapUserRole" <<mapUserRole ;
    return mapUserRole;
}


QVariantMap reciprocityDB::setMapStatusMess(int roomID)
{
     queryPull query;
    // собираем информацию в MAP
    QVariantMap mapAllMess;
    mapAllMess["unread"] = collectMapMessID(query.selectUnreadMessages(roomID));
    qDebug() <<"mapAllMess[\"unread\"]"<< mapAllMess["unread"];
    mapAllMess["read"] = collectMapMessID(query.selectReadMessages(roomID));
    qDebug() <<"mapAllMess[\"read\"]"<< mapAllMess["read"];
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
    }
    return mapMessID;
}

QVariantMap reciprocityDB::collectMapRoomsID(QSqlQuery qUserRole)
{
    QVariantMap mapRoomsID;
    QVariantMap mapRoomName;
    int roomID = 0;
    QString roomName;
    while (qUserRole.next()){
        roomID = qUserRole.value(0).toInt();
        roomName = qUserRole.value(1).toString();
        //qDebug() << "roomID" << roomID;
        //qDebug() << "roomName" << roomName;

        // определяем сообщения в комнате
        mapRoomName.insert(roomName, setMapStatusMess(roomID));
        //qDebug() << "mapRoomName" << mapRoomName;
        mapRoomsID.insert(QString::number(roomID),mapRoomName);
        mapRoomName.clear();
    }
    return mapRoomsID;
}


