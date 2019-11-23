#include "querypull.h"

queryPull::queryPull()
{
    mapQueries[setQuery::getUserIdUserName] = "SELECT id,name "
                                              "FROM users "
                                              "WHERE login=:login AND password=:pass ";
    mapSetQuery["auth"] = "SELECT id,name "
                          "FROM users "
                          "WHERE users.login=:login AND users.password=:pass";
    mapQueries[setQuery::getWaitInvitation] = "SELECT id, text, room_id, sender_id "
                                                "FROM invitations "
                                                "WHERE receiver_id =:userID "
                                                "AND status_id = 3";
    mapSetQuery["selectInvitations"] = "SELECT id, text, room_id, sender_id "
                                  "FROM invitations "
                                  "WHERE invitations.receiver_id =:receiverID "
                                  "AND invitations.status_id = 3";
    mapQueries[setQuery::getRoomName] = "SELECT name "
                                        "FROM rooms "
                                        "WHERE rooms.id = :roomID ";
    mapSetQuery["selectRoomName"] = "SELECT name "
                                    "FROM rooms "
                                    "WHERE rooms.id = :roomID ";
    mapQueries[setQuery::getUserName] =  "SELECT name "
                                         "FROM users "
                                         "WHERE id = :userID ";
    mapSetQuery["selectUserName"] = "SELECT name "
                                    "FROM users "
                                    "WHERE id = :userID ";
    mapQueries[setQuery::getRole] = "SELECT role "
                                    "FROM users_role "
                                    "WHERE id =:roleID";
    mapSetQuery["selectRole"] = "SELECT users_role.role "
                                "FROM users_role "
                                "WHERE users_role.id =:roleID";
    mapQueries[setQuery::getUnreadMess] = "SELECT "
                                          "messages.id, messages.time_sent, users.name, messages.text "
                                          "FROM messages "
                                          "INNER JOIN users on users.id = messages.sender_id "
                                          "WHERE messages.room_id = :roomID "
                                          "AND "
                                          "CAST(strftime('%s', messages.time_sent)  AS  integer) "
                                          "> CAST(strftime('%s',  users.time_last_session)  AS  integer) ";
    mapSetQuery["selectUnreadMessages"] = "SELECT "
                                          "messages.id, messages.time_sent, users.name, messages.text "
                                          "FROM messages "
                                          "INNER JOIN users on users.id = messages.sender_id "
                                          "WHERE messages.room_id = :roomID "
                                          "AND "
                                          "CAST(strftime('%s', messages.time_sent)  AS  integer) "
                                          "> CAST(strftime('%s',  users.time_last_session)  AS  integer) ";
    mapQueries[setQuery::getReadMess] = "SELECT "
                                        "messages.id, messages.time_sent, users.name, messages.text "
                                        "FROM messages "
                                        "INNER JOIN users on users.id = messages.sender_id "
                                        "WHERE messages.room_id = :roomID "
                                        "AND "
                                        "CAST(strftime('%s', messages.time_sent)  AS  integer) "
                                        "<= CAST(strftime('%s',  users.time_last_session)  AS  integer) ";
    mapSetQuery["selectReadMessages"] ="SELECT "
                                       "messages.id, messages.time_sent, users.name, messages.text "
                                "FROM messages "
                                "INNER JOIN users on users.id = messages.sender_id "
                                "WHERE messages.room_id = :roomID "
                                "AND "
                                "CAST(strftime('%s', messages.time_sent)  AS  integer) "
                                "<= CAST(strftime('%s',  users.time_last_session)  AS  integer) ";
    mapQueries[setQuery::getMembers] = "SELECT users.id, users.name "
                                       "FROM rooms_users "
                                       "INNER JOIN users ON users.id = rooms_users.user_id "
                                       "WHERE rooms_users.room_id = :roomID "
                                       "AND users.id != :userID";
    mapSetQuery["selectUserIdNameFromRoom"] = "SELECT users.id, users.name "
                                              "FROM rooms_users "
                                              "INNER JOIN users on users.id = rooms_users.user_id "
                                              "WHERE rooms_users.room_id = :roomID "
                                              "AND users.id != :adminID";
    mapQueries[setQuery::getRooms] = "SELECT "
                                     "rooms_users.room_id, rooms.name, rooms_users.user_role_id "
                                     "FROM rooms_users "
                                     "INNER JOIN rooms ON rooms.id = rooms_users.room_id "
                                     "WHERE rooms_users.user_id = :userID ";
    mapSetQuery["selectRooms"]="SELECT rooms_users.room_id, rooms.name, rooms_users.user_role_id "
                                "FROM rooms_users "
                                "INNER JOIN rooms on rooms.id = rooms_users.room_id "
                                "WHERE rooms_users.user_id = :id ";
    mapQueries[setQuery::setOnLine] = "UPDATE users "
                                      "SET status = 1,  time_session =:td "
                                      "WHERE id = :userID";
    mapSetQuery["userOnLine"] = "UPDATE users "
                                "SET status = 1,  time_session =:td "
                                "WHERE id = :id";
    mapQueries[setQuery::setOffLine] = "UPDATE users "
                                       "SET status = 2, time_last_session = time_session "
                                       "WHERE id = :userID";
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

    mapSetQuery["insertNewMessage"] ="INSERT INTO messages "
                                     "(room_id, sender_id, text, time_sent) "
                                     "VALUES (:roomID, :senderID, :text, :td)";
    mapSetQuery["insertNewInvite"] ="INSERT INTO invitations "
                                     "(text, room_id, sender_id, receiver_id, status_id) "
                                     "VALUES (:text, :roomID, :senderID, :receiverID, 3)";

    mapSetQuery["selectIdSenderInvite"] = "SELECT sender_id "
                                          "FROM invitations "
                                          "WHERE id =:inviteID ";
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
    mapSetQuery["updateInvite"] = "UPDATE invitations "
                                  "SET status_id = :statusID "
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
    mapSetQuery["selectUserIdFromRoom"] = "SELECT user_id "
                                          "FROM rooms_users "
                                          "WHERE room_id = :roomID ";
    mapSetQuery["delRoom"] = "DELETE FROM rooms "
                             "WHERE rooms.id = :roomID ";

    mapSetQuery["delRoomFromRoomsUsers"] = "DELETE FROM rooms_users "
                                           "WHERE rooms_users.room_id = :roomID ";

    mapSetQuery["selectUserID"] = "SELECT id "
                                   "FROM users "
                                   "WHERE name = :userName";


    mapSetQuery["selectInvitedUserName"] = "SELECT users.name "
                                           "FROM users "
                                           "INNER JOIN invitations on invitations.receiver_id = users.id "
                                           "WHERE invitations.id = :inviteID ";
    mapSetQuery["selectInvitedRoomName"] = "SELECT rooms.name, rooms.id "
                                           "FROM rooms "
                                           "INNER JOIN invitations on invitations.room_id = rooms.id "
                                           "WHERE invitations.id = :inviteID ";

    mapSetQuery["deleteUser"] = "DELETE FROM rooms_users "
                                "WHERE rooms_users.user_id = :userID "
                                "AND rooms_users.room_id = :roomID";
}

QSqlQuery queryPull::getUserIdUserName(QString login, QString pass)
{
    // запрашиваем id, name по логину и паролю
    //query.prepare(mapSetQuery["auth"]);
    query.prepare(mapQueries[setQuery::getUserIdUserName]);
    query.bindValue(":login", login);
    query.bindValue(":pass", pass);
    if (!query.exec())
    {
        qDebug() <<mapQueries[setQuery::getUserIdUserName] << query.lastError();
    }
    return query;
}


QSqlQuery queryPull::setOnLine(int userID)
{
    QDateTime td;
    query.prepare(mapQueries[setQuery::setOnLine]);
    query.bindValue(":userID", userID);
    query.bindValue(":td", td.currentDateTime());
    if (!query.exec())
    {
        qDebug() << mapQueries[setQuery::setOnLine] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::setOffLine(int userID)
{
    query.prepare(mapQueries[setQuery::setOffLine]);
    query.bindValue(":userID", userID);
    if (!query.exec())
    {
        qDebug() << mapQueries[setQuery::setOffLine] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::getRooms(int userID)
{
    qDebug() << "querypull getRooms";
    query.prepare(mapQueries[setQuery::getRooms]);
    query.bindValue(":userID", userID);
    if (!query.exec())
    {
        qDebug() << "querypull getRooms";
        qDebug() << mapQueries[setQuery::getRooms] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::getUnreadMess(int roomID)
{
    qDebug() << "getUnreadMess";
    query.prepare(mapQueries[setQuery::getUnreadMess]);
    query.bindValue(":roomID", roomID);
    if (!query.exec())
    {
        qDebug() << mapQueries[setQuery::getUnreadMess] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::getReadMess(int roomID)
{
    qDebug() << "getReadMess";
    query.prepare(mapQueries[setQuery::getReadMess]);
    query.bindValue(":roomID", roomID);
    if (!query.exec())
    {
        qDebug() << mapQueries[setQuery::getReadMess] << query.lastError();
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

QSqlQuery queryPull::selectUserIdFromRoom(int roomID)
{
    query.prepare(mapSetQuery["selectUserIdFromRoom"]);
    query.bindValue(":roomID", roomID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectUserIdFromRoom"] << query.lastError();
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

QSqlQuery queryPull::getUserName(int userID)
{
    query.prepare(mapQueries[setQuery::getUserName]);
    query.bindValue(":userID", userID);
    if (!query.exec())
    {
        qDebug() << mapQueries[setQuery::getUserName] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::getRole(int roleID)
{
    query.prepare(mapQueries[setQuery::getRole]);
    query.bindValue(":roleID", roleID);
    if (!query.exec())
    {
        qDebug() << mapQueries[setQuery::getRole] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::getRoomName(int roomID)
{
    query.prepare(mapQueries[setQuery::getRoomName]);
    query.bindValue(":roomID", roomID);
    if (!query.exec())
    {
        qDebug() << mapQueries[setQuery::getRoomName] << query.lastError();
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

QSqlQuery queryPull::updateInvite(int inviteID, int statusID)
{
    query.prepare(mapSetQuery["updateInvite"]);
    query.bindValue(":inviteID", inviteID);
    query.bindValue(":statusID", statusID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["updateInvite"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::getWaitInvitation(int userID)
{
    query.prepare(mapQueries[setQuery::getWaitInvitation]);
    query.bindValue(":userID", userID);
    if (!query.exec())
    {
        qDebug() << mapQueries[setQuery::getWaitInvitation] << query.lastError();
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

QSqlQuery queryPull::selectIdSenderInvite(int inviteID)
{
    query.prepare(mapSetQuery["selectIdSenderInvite"]);
    query.bindValue(":inviteID", inviteID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectIdSenderInvite"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectInvitedUserName(int inviteID)
{
    query.prepare(mapSetQuery["selectInvitedUserName"]);
    query.bindValue(":inviteID", inviteID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectInvitedUserName"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::selectInvitedRoomName(int inviteID)
{
    query.prepare(mapSetQuery["selectInvitedRoomName"]);
    query.bindValue(":inviteID", inviteID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["selectInvitedRoomName"] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::getMembers(int roomID, int userID)
{
    query.prepare(mapQueries[setQuery::getMembers]);
    query.bindValue(":roomID", roomID);
    query.bindValue(":userID", userID);
    if (!query.exec())
    {
        qDebug() << mapQueries[setQuery::getMembers] << query.lastError();
    }
    return query;
}

QSqlQuery queryPull::deleteUser(int userID, int roomID)
{
    query.prepare(mapSetQuery["deleteUser"]);
    query.bindValue(":userID", userID);
    query.bindValue(":roomID", roomID);
    if (!query.exec())
    {
        qDebug() << mapSetQuery["deleteUser"] << query.lastError();
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




