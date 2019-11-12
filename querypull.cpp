#include "querypull.h"

queryPull::queryPull()
{
    mapSetQuery["auth"] = "SELECT id,name "
                          "FROM users "
                          "WHERE users.login=:login AND users.password=:pass";
    mapSetQuery["userOnLine"] = "UPDATE users "
                                "SET status = 1,  time_session =:td "
                                "WHERE id = :id";
    mapSetQuery["userOffLine"] = "UPDATE users "
                                 "SET status = 2, time_last_session = time_session "
                                 "WHERE id = :id";
    mapSetQuery["selectRooms"]="SELECT rooms_users.room_id, rooms.name, rooms_users.user_role_id "
                            "FROM rooms_users "
                            "INNER JOIN rooms on rooms.id = rooms_users.room_id "
                            "WHERE rooms_users.user_id = :id";
    mapSetQuery["selectUnreadMessages"] ="SELECT messages.time_sent, users.name, messages.text "
                                "FROM messages "
                                "INNER JOIN users on users.id = messages.sender_id "
                                "WHERE messages.room_id = :roomID "
                                "AND "
                                "CAST(strftime('%s', messages.time_sent)  AS  integer) "
                                "> CAST(strftime('%s',  users.time_last_session)  AS  integer) ";
    mapSetQuery["selectReadMessages"] ="SELECT messages.time_sent, users.name, messages.text "
                                "FROM messages "
                                "INNER JOIN users on users.id = messages.sender_id "
                                "WHERE messages.room_id = :roomID "
                                "AND "
                                "CAST(strftime('%s', messages.time_sent)  AS  integer) "
                                "<= CAST(strftime('%s',  users.time_last_session)  AS  integer) "
                                "LIMIT 5 OFFSET 5";
    mapSetQuery["insertNewMessage"] ="INSERT INTO messages "
                                     "(room_id, sender_id, text, time_sent) "
                                     "VALUES (:roomID, :senderID, :text, :td)";
    mapSetQuery["insertNewRoom"] ="INSERT INTO rooms "
                                  "(name, users_count) "
                                  "VALUES (:roomName, 1)";
    mapSetQuery["selectIDNewRoom"] = "SELECT id "
                                     "FROM rooms "
                                     "WHERE name = :roomName";
    mapSetQuery["insertNewRoomUsers"] = " INSERT INTO rooms_users "
                                        "(room_id, user_id, user_status_id, user_role_id) "
                                        "VALUES  (:roomID, :userID, 1, 1)";
}

QSqlQuery queryPull::auth(QString login, QString pass)
{
    // запрашиваем id, name по логину и паролю
    query.prepare(mapSetQuery["auth"]);
    query.bindValue(":login", login);
    query.bindValue(":pass", pass);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["auth"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::userOnLine(int id)
{
    QDateTime td;
    query.prepare(mapSetQuery["userOnLine"]);
    query.bindValue(":id", id);
    query.bindValue(":td", td.currentDateTime());
    if (!query.exec())
    {
        qDebug() << mapSetQuery["userOnLine"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::userOffLine(int id)
{
    query.prepare(mapSetQuery["userOffLine"]);
    query.bindValue(":id", id);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["userOffLine"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectRooms(int id)
{
    query.prepare(mapSetQuery["selectRooms"]);
    query.bindValue(":id", id);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectRooms"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectUnreadMessages(int roomID)
{
    query.prepare(mapSetQuery["selectUnreadMessages"]);
    query.bindValue(":roomID", roomID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectUnreadMessages"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectReadMessages(int roomID)
{
    query.prepare(mapSetQuery["selectReadMessages"]);
    query.bindValue(":roomID", roomID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectReadMessages"] << query.lastError();
    }
    return query;
}

bool queryPull::insertMessage(int roomID, int userID, QString text)
{
    query.prepare(mapSetQuery["insertNewMessage"]);
    query.bindValue(":senderID", userID);
    query.bindValue(":roomID", roomID);
    query.bindValue(":text", text);
    QDateTime td;
    query.bindValue(":td", td.currentDateTime());
    if (!query.exec())
    {
        qDebug() << mapSetQuery["insertNewMessage"] << query.lastError();
        return false;
    }
    else return true;
}

int queryPull::getNewRoomID(int userID, QString roomName)
{
    int roomID = 0;
    query.prepare(mapSetQuery["insertNewRoom"]);
    query.bindValue(":roomName", roomName);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["insertNewRoom"] << query.lastError();
    }
    else
    {

        query.prepare(mapSetQuery["selectIDNewRoom"]);
        query.bindValue(":roomName", roomName);
        if (!query.exec())
        {
            qDebug() << mapSetQuery["selectIDNewRoom"] << query.lastError();
        }
        while (query.next()){
            roomID = query.value(0).toInt();
            qDebug() << "roomID" << roomID;
        }
        query.prepare(mapSetQuery["insertNewRoomUsers"]);
        query.bindValue(":roomID", roomID);
        query.bindValue(":userID", userID);
        if (!query.exec())
        {
            qDebug() << mapSetQuery["insertNewRoomUsers"] << query.lastError();
        }
    }

    return roomID;
}




