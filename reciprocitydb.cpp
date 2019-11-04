#include "reciprocitydb.h"

reciprocityDB::reciprocityDB()
{
}

QVariantMap reciprocityDB::readAuth(QString login, QString pass)
{
    int id = 0;
    QVariantMap mapResponseDB;
    QSqlQuery qAuth;
    // запрашиваем id, name по логину и паролю
    QString qsAuth = "select id,name from users where users.login='%1' and users.password='%2'";
    qDebug() << "qsAuth" << qsAuth.arg(login).arg(pass);
    if (!qAuth.exec(qsAuth.arg(login).arg(pass)))
    {
            qDebug() << "query SELECT error" << qAuth.lastError();
    }
    else
    {
        qDebug() << "query qAuth success" ;
        while (qAuth.next()){
            // если что-то нашлось
            if (qAuth.numRowsAffected() != -1)
            {
                // фиксируем id, name
                id = qAuth.value(0).toInt();
                mapResponseDB["id"] = id;
                mapResponseDB["name"] = qAuth.value(1).toString();
                // меняем статус клиента на он-лайн
                setStatusONline(id);
                // определяем комнаты, в которых участвует клиент
                mapResponseDB["rooms"] = setRooms(id);
             }
            else{
                mapResponseDB["id"] = 0;
            }
        }
    }
    return mapResponseDB;
}

void reciprocityDB::setStatusONline(int id)
{

    QSqlQuery qSetStatus;
    QString qsSetStatus;
    qsSetStatus = "UPDATE users SET status = 1  WHERE id = %1";
    if (!qSetStatus.exec(qsSetStatus.arg(id)))
    {
        qDebug() << "query UPDATE error" << qSetStatus.lastError();
    }
}

void reciprocityDB::setStatusOFFline(int id)
{
    QSqlQuery qSetStatus;
    QString qsSetStatus;
    qsSetStatus = "UPDATE users SET status = 2  WHERE id = %1";
    if (!qSetStatus.exec(qsSetStatus.arg(id)))
    {
        qDebug() << "query UPDATE error" << qSetStatus.lastError();
    }
}

void reciprocityDB::setStatusRead(int id)
{
    QSqlQuery qSetStatusRead;
    QString qsSetStatusRead = "UPDATE messages SET status = 2  WHERE id = %1";
    if (!qSetStatusRead.exec(qsSetStatusRead.arg(id))){
         qDebug() << "query UPDATE error" << qSetStatusRead.lastError();
    }
}

void reciprocityDB::connectChatToDB()
{
    emit test();
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

        qDebug() << "emit dbConnected(sTemp);";
        emit dbConnected(sTemp);
    }
    qDebug() << sTemp;
}

QVariantMap reciprocityDB::setRooms(int id)
{
    QSqlQuery qRooms;
    QVariantMap mapRoomsID;
    QVariantMap mapRoomName;
    QVariantMap mapRoomMess;
    QVariantMap mapMess;
    QString qsRooms = "select rooms_users.room_id, rooms.name "
                      "from rooms_users inner join rooms on rooms.id = rooms_users.room_id "
                      "where rooms_users.user_id = %1";
    qDebug() << "qsRooms" << qsRooms.arg(id);
    if (!qRooms.exec(qsRooms.arg(id))){
        qDebug() << "query select room_id error" << qRooms.lastError();
    }
    else{
        qDebug() << "success query select rooms_users.room_id, rooms.name ";
        int roomID;
        QString roomName;
        mapRoomName.clear();
        while (qRooms.next()){
            roomID = qRooms.value(0).toInt();
            roomName = qRooms.value(1).toString();
            mapRoomName[roomName] = setMessages(roomID);
            mapRoomsID[QString::number(roomID)] = mapRoomName;
            mapRoomName.clear();
        }
    }
    qDebug() << "mapRoomsID" <<  mapRoomsID;
    return mapRoomsID;
}

QVariantMap reciprocityDB::setMessages(int roomID)
{
    QSqlQuery qMessage;
    QVariantMap mapMessage;
    QVariantMap mapSenderMessage;
    QString messTime;
    QString senderName;
    QString textMess;
    int messID;
    QString qsMessage = "select messages.id, messages.time_sent, users.name, messages.text "
                        "from messages "
                        "inner join users on users.id = messages.sender_id "
                        "where messages.room_id = %1 and messages.status = 1 ";
    if (!qMessage.exec(qsMessage.arg(roomID))){
        qDebug() << "query select messages error" << qMessage.lastError();
    }
    else{
        mapSenderMessage.clear();
        while (qMessage.next()){
            messID = qMessage.value(0).toInt();
            messTime = qMessage.value(1).toString();
            senderName = qMessage.value(2).toString();
            textMess = qMessage.value(3).toString();
            mapSenderMessage[senderName] = textMess;
            mapMessage[messTime] = mapSenderMessage;
            mapSenderMessage.clear();
            setStatusRead(messID);
        }
    }
    qDebug() << "mapMessage" <<  mapMessage;
    return mapMessage;
}
