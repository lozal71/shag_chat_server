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
        emit dbConnected(sTemp);
    }
    qDebug() << sTemp;
}

QVariantMap reciprocityDB::setRooms(int id)
{
    QSqlQuery qRooms;
    QVariantMap mapRooms;
    QString qsRooms = "select rooms_users.room_id, rooms.name "
                      "from rooms_users inner join rooms on rooms.id = rooms_users.room_id "
                      "where rooms_users.user_id = %1";
    qDebug() << "qsRooms" << qsRooms.arg(id);
    if (!qRooms.exec(qsRooms.arg(id))){
        qDebug() << "query select room_id error" << qRooms.lastError();
    }
    else{
        qDebug() << "success query select rooms_users.room_id, rooms.name ";
        while (qRooms.next()){
            mapRooms[qRooms.value(0).toString()] = qRooms.value(1).toString();
        }
    }
    return mapRooms;
}
