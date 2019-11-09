#include "reciprocitydb.h"

reciprocityDB::reciprocityDB()
{
}

QVariantMap reciprocityDB::readAuth(QString login, QString pass)
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
        mapResponseDB["rooms"] = setRooms(id);
    }
    return mapResponseDB;
}


QVariantMap reciprocityDB::readMessage(int roomID, int userID, QString text)
{
    QVariantMap mapResponseDB;
    queryPull query;
    mapResponseDB["sendResult"] = query.insertMessage(roomID,userID,text);
   // qDebug() << "td" << td;
    //qDebug() << "userID" << userID;

//    QSqlQuery qInsertNewMessage;
//    QString qsInsertNewmessage = "insert into messages (room_id, sender_id, time_sent, text, status) "
//                              "values (%1, %2, '%3', '%4', 1)";
//    //qDebug() << "queryInsert" << qsInsertNewmessage.arg(roomID).arg(userID).
//                //arg(td.toString("hh:mm:ss")).arg(text);
//    if (!qInsertNewMessage.exec(qsInsertNewmessage.arg(roomID).arg(userID).
//                                arg(td.toString("hh:mm:ss")).arg(text))){
//        qDebug() << "query insert into messages " << qInsertNewMessage.lastError();
//        mapResponseDB["sendResult"] = 0;
//    }
//    else{
//        mapResponseDB["sendResult"] = 1;
//    }
    return mapResponseDB;
}

//void reciprocityDB::setStatusRead(int id)
//{
//    QSqlQuery qSetStatusRead;
//    QString qsSetStatusRead = "UPDATE messages SET status = 2  WHERE id = %1";
//    if (!qSetStatusRead.exec(qsSetStatusRead.arg(id))){
//         qDebug() << "query UPDATE error" << qSetStatusRead.lastError();
//    }
//}

//void reciprocityDB::setCurrentTime(int id)
//{
//    QDateTime td;
//    td = td.currentDateTime();
//    //qDebug() << "td" << td;
//    QSqlQuery qSetTime;
//    QString qsSetTime = "UPDATE users SET time_session = '%1' WHERE id = %2";
//    //qDebug() << "qsSetTime.arg(td.toString(\"hh:mm:ss\")).arg(id) "
//      //       << qsSetTime.arg(td.toString("hh:mm:ss")).arg(id);
//    if (!qSetTime.exec(qsSetTime.arg(td.toString("hh:mm:ss")).arg(id))){
//         qDebug() << "query UPDATE users SET time_session " << qSetTime.lastError();
//    }
//}

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
        emit dbConnected(sTemp);
    }
    qDebug() << sTemp;
}

void reciprocityDB::setStatusOFFline(int id)
{
    queryPull query;
    query.userOffLine(id);
}

QVariantMap reciprocityDB::setRooms(int id)
{

    QVariantMap mapRoomsID;
    QVariantMap mapRoomName;
    QVariantMap mapRoomMess;
    QVariantMap mapMess;

    queryPull query;
    QSqlQuery qRooms = query.selectRooms(id);
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
    //qDebug() << "mapRoomsID" <<  mapRoomsID;
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
    //QDateTime td;
    //td = td.currentDateTime();
    //int messID;
//    QString qsMessage = "select messages.id, messages.time_sent, users.name, messages.text "
//            "from messages "
//            "inner join users on users.id = messages.sender_id "
//            "where "
//            "messages.room_id = %1 "
//            "and "
//            "CAST(strftime('%s', messages.time_sent)  AS  integer) >= CAST(strftime('%s',  users.time_last_session)  AS  integer)";
////            "select messages.id, messages.time_sent, users.name, messages.text "
////            "from messages "
////            "inner join users on users.id = messages.sender_id "
////            "where messages.room_id = %1 "
////            "and datetime(messages.time_sent) >= datetime(users.time_last_session)";
//    //qDebug() << "qsMessage" << qsMessage.arg(roomID);
//    if (!qMessage.exec(qsMessage.arg(roomID)))
//    {
//        qDebug() << "query select messages error" << qMessage.lastError();
//    }
//    else{
    queryPull query;
    qMessage = query.selectMessages(roomID);
    mapSenderMessage.clear();
    while (qMessage.next()){

        //messID = qMessage.value(0).toInt();
        messTime = qMessage.value(1).toString();
        senderName = qMessage.value(2).toString();
        textMess = qMessage.value(3).toString();
        mapSenderMessage[senderName] = textMess;
        mapMessage[messTime] = mapSenderMessage;
        mapSenderMessage.clear();
        //setStatusRead(messID);

    }
    //qDebug() << "mapMessage" <<  mapMessage;
    return mapMessage;
}
