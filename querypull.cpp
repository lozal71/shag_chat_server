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
    mapSetQuery["selectRoomsAdminRole"]="SELECT rooms_users.room_id, rooms.name "
                            "FROM rooms_users "
                            "INNER JOIN rooms on rooms.id = rooms_users.room_id "
                            "WHERE rooms_users.user_id = :id "
                             "AND rooms_users.user_role_id = 1 ";
    mapSetQuery["selectRoomsUserRole"]="SELECT rooms_users.room_id, rooms.name, rooms_users.user_role_id "
                            "FROM rooms_users "
                            "INNER JOIN rooms on rooms.id = rooms_users.room_id "
                            "WHERE rooms_users.user_id = :id "
                            "AND rooms_users.user_role_id = 2 ";
    mapSetQuery["selectUnreadMessages"] = "SELECT "
                                          "messages.id, messages.time_sent, users.name, messages.text "
                                          "FROM messages "
                                          "INNER JOIN users on users.id = messages.sender_id "
                                          "WHERE messages.room_id = :roomID "
                                          "AND "
                                          "CAST(strftime('%s', messages.time_sent)  AS  integer) "
                                          "> CAST(strftime('%s',  users.time_last_session)  AS  integer) ";
    mapSetQuery["selectReadMessages"] ="SELECT messages.id, messages.time_sent, users.name, messages.text "
                                "FROM messages "
                                "INNER JOIN users on users.id = messages.sender_id "
                                "WHERE messages.room_id = :roomID "
                                "AND "
                                "CAST(strftime('%s', messages.time_sent)  AS  integer) "
                                "<= CAST(strftime('%s',  users.time_last_session)  AS  integer) ";
    mapSetQuery["insertNewMessage"] ="INSERT INTO messages "
                                     "(room_id, sender_id, text, time_sent) "
                                     "VALUES (:roomID, :senderID, :text, :td)";
    mapSetQuery["insertNewInvite"] ="INSERT INTO invitations "
                                     "(text, room_id, sender_id, receiver_id, status_id) "
                                     "VALUES (:text, :roomID, :senderID, :receiverID, 3)";
    mapSetQuery["selectInvitations"] = "SELECT id, text, room_id, sender_id "
                                  "FROM invitations "
                                  "WHERE invitations.receiver_id =:receiverID "
                                  "AND invitations.status_id = 3";

    mapSetQuery["insertNewRoom"] ="INSERT INTO rooms "
                                  "(name, users_count) "
                                  "VALUES (:roomName, 1) ";
    mapSetQuery["selectIDNewRoom"] = "SELECT id "
                                     "FROM rooms "
                                     "WHERE name = :roomName ";
    mapSetQuery["insertNewRoomUsers"] = " INSERT INTO rooms_users "
                                        "(room_id, user_id, user_status_id, user_role_id) "
                                        "VALUES  (:roomID, :userID, 1, 1) ";
    mapSetQuery["insertInvitedUsers"] = " INSERT INTO rooms_users "
                                        "(room_id, user_id, user_status_id, user_role_id) "
                                        "VALUES  (:roomID, :userID, :statusID, 2) ";
    mapSetQuery["updateInviteAccept"] = "UPDATE invitations "
                                        "SET status_id = 1 "
                                        "WHERE id = :inviteID";
    mapSetQuery["selectWaitInvitations"] = "SELECT id "
                                           "FROM invitations "
                                           "WHERE status_id = 3";
    mapSetQuery["selectUserFromRoom"] = "SELECT "
                                      "rooms_users.user_id, rooms.name, "
                                      "rooms_users.user_status_id, rooms_users.room_id "
                                      "FROM rooms_users "
                                      "INNER JOIN rooms on rooms_users.room_id = rooms.id "
                                      "WHERE rooms_users.room_id = :roomID "
                                      "AND rooms_users.user_id != :senderID ";
    mapSetQuery["delRoom"] = "DELETE FROM rooms "
                             "WHERE rooms.id = :roomID ";
    mapSetQuery["selectRoomName"] = "SELECT name "
                                    "FROM rooms "
                                    "WHERE rooms.id = :roomID ";
    mapSetQuery["delRoomFromRoomsUsers"] = "DELETE FROM rooms_users "
                                           "WHERE rooms_users.room_id = :roomID ";
    mapSetQuery["selectUserName"] = "SELECT name "
                                    "FROM users "
                                    "WHERE id = :userID ";
    mapSetQuery["selectUserID"] = "SELECT id "
                                   "FROM users "
                                   "WHERE name = :userName";
    mapSetQuery["selectRooms"]="SELECT rooms_users.room_id, rooms.name, rooms_users.user_role_id "
                                "FROM rooms_users "
                                "INNER JOIN rooms on rooms.id = rooms_users.room_id "
                                "WHERE rooms_users.user_id = :id ";
    mapSetQuery["selectRole"] = "SELECT users_role.role "
                                "FROM users_role "
                                "WHERE users_role.id =:roleID";

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

QSqlQuery queryPull::selectUserFromRoom(int roomID, int senderID)
{
    query.prepare(mapSetQuery["selectUserFromRoom"]);
    query.bindValue(":roomID", roomID);
    query.bindValue(":senderID", senderID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectUserFromRoom"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectCastMessage(int roomID)
{
    query.prepare(mapSetQuery["selectCastMessage"]);
    query.bindValue(":roomID", roomID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectCastMessage"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectUserName(int userID)
{
    query.prepare(mapSetQuery["selectUserName"]);
    query.bindValue(":userID", userID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectUserName"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectRole(int roleID)
{
    query.prepare(mapSetQuery["selectRole"]);
    query.bindValue(":roleID", roleID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectRole"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectRoomName(int roomID)
{
    query.prepare(mapSetQuery["selectRoomName"]);
    query.bindValue(":roomID", roomID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectRoomName"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectUserID(QString userName)
{
    query.prepare(mapSetQuery["selectUserID"]);
    query.bindValue(":userName", userName);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectUserID"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::insertInvitedUsers(int roomID, int userID, int statusID)
{
    query.prepare(mapSetQuery["insertInvitedUsers"]);
    query.bindValue(":roomID", roomID);
    query.bindValue(":userID", userID);
    query.bindValue(":statusID", statusID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["insertInvitedUsers"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::insertNewInvite(QString text, int roomID,
                                     int senderID, int receiverID)
{
    query.prepare(mapSetQuery["insertNewInvite"]);
    query.bindValue(":text", text);
    query.bindValue(":roomID", roomID);
    query.bindValue(":senderID", senderID);
    query.bindValue(":receiverID", receiverID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["insertNewInvite"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::updateInviteAccept(int inviteID)
{
    query.prepare(mapSetQuery["updateInviteAccept"]);
    query.bindValue(":inviteID", inviteID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["updateInviteAccept"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectInvitations(int receiverID)
{
    query.prepare(mapSetQuery["selectInvitations"]);
    query.bindValue(":receiverID", receiverID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectInvitations"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectWaitInvitations()
{
    query.prepare(mapSetQuery["selectWaitInvitations"]);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectWaitInvitations"] << query.lastError();
    }
    return query;
}

bool queryPull::delRoom(int roomID)
{
    query.prepare(mapSetQuery["delRoom"]);
    query.bindValue(":roomID", roomID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["delRoom"] << query.lastError();
        return false;
    }
    else
    {
        query.prepare(mapSetQuery["delRoomFromRoomsUsers"]);
        query.bindValue(":roomID", roomID);
        if (!query.exec())
        {
            qDebug() << mapSetQuery["delRoomFromRoomsUsers"] << query.lastError();
            return false;
        }
        return true;
    }
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




