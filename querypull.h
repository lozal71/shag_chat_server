#ifndef QUERYPULL_H
#define QUERYPULL_H
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMap>
#include <QDateTime>
#include <QVariantList>

enum  setQuery {getUserIdUserName, getWaitInvitation, getRoomName, getUserName,
              getRole, getUnreadMess, getReadMess, getMembers, getRooms, getMembersIdOnline,
              getRoomID, getUserID, getUserStatus, getIdSenderInvite,
              getInvitedUserName, getInvitedRoomName,
              setOnLine, setOffLine,
              insertNewMess, insertNewRoom, insertAdminToRoom,insertNewInvite, insertInvitedUsers,
              deleteRoom, deleteUser,
              updateInvite};


class queryPull: public QSqlQuery
{
public:
//    static QMap<setQuery, QString> mapQ;
//    static queryPull *getQuery();
    queryPull();
//    QSqlQuery getUserIdUserName(QString login, QString pass);
//    QSqlQuery getWaitInvitation (int userID);
//    QSqlQuery getRoomName(int roomID);
//    QSqlQuery getUserName(int userID);
//    QSqlQuery getRole(int roleID);
//    QSqlQuery getUnreadMess(int roomID);
//    QSqlQuery getReadMess(int roomID);
//    QSqlQuery getMembers(int roomID, int userID);
//    QSqlQuery getRooms(int userID);
//    QSqlQuery getMembersIdOnline(int roomID, int senderID);
//    QSqlQuery getRoomID(QString roomName);
//    QSqlQuery getUserID(QString userName);
//    QSqlQuery getUserStatus(int userID, int roomID);

//    QSqlQuery setOnLine(int userID);
//    QSqlQuery setOffLine(int userID);

//    bool insertNewMess(int roomID, int userID, QString text);
//    bool insertNewRoom(QString roomName);
//    bool insertAdminToRoom(int userID, int roomID);
//    QSqlQuery insertNewInvite(QString text, int roomID, int senderID, int receiverID);
//    QSqlQuery insertInvitedUsers(int roomID, int userID, int statusID);

//    bool deleteRoom(int roomID);
//    bool deleteUser(int userID, int roomID);

//    QSqlQuery selectUserFromRoom(int roomID, int senderID);
//    QSqlQuery selectUserIdFromRoom(int roomID);
//    QSqlQuery selectCastMessage(int roomID);

//    QSqlQuery updateInviteAccept(int inviteID);
//    QSqlQuery updateInvite(int inviteID, int statusID);

//    QSqlQuery selectWaitInvitations();
//    QSqlQuery selectIdSenderInvite(int inviteID);
//    QSqlQuery selectInvitedUserName(int inviteID);
//    QSqlQuery selectInvitedRoomName(int inviteID);

    QSqlQuery makeQuery(setQuery qQuery, QVariantList lstParam);

//    int getNewRoomID(int userID, QString roomName);
private:
    QSqlQuery query;
    //queryPull();
   // QMap<QString,QString> mapSetQuery;
    //QMap<setQuery, QString> mapQueries;
    static QMap<setQuery, QString> mapQ;
    static QMap<int,QString> numberParam;
};
#endif // QUERYPULL_H
//    QMap<setQuery, QString> queryPull::mapQ =  {
//     { setQuery::getInvitedRoomName, "SELECT rooms.name, rooms.id "
//                                     "FROM rooms "
//                                     "INNER JOIN invitations on invitations.room_id = rooms.id "
//                                     "WHERE invitations.id = :1"},
//     { setQuery::getInvitedUserName, "SELECT users.name "
//                                     "FROM users "
//                                     "INNER JOIN invitations on invitations.receiver_id = users.id "
//                                     "WHERE invitations.id = :1"},
//     { setQuery::getIdSenderInvite, "SELECT sender_id "
//                                     "FROM invitations "
//                                     "WHERE id =:1 "},
//     {setQuery::getUserStatus,"SELECT user_status_id "
//                              "FROM rooms_users "
//                              "WHERE user_id = :userID "
//                              "AND room_id = :roomID "},
//     {setQuery::getMembersIdOnline, "SELECT user_id "
//                                    "FROM rooms_users "
//                                    "WHERE user_status_id = 1 "
//                                    "AND room_id = :1 "
//                                    "AND user_id != :1"},
//     {setQuery::getUserIdUserName,"SELECT id,name "
//                                                    "FROM users "
//                                                    "WHERE login=:1 AND password=:2 "},

//          {setQuery::getWaitInvitation, "SELECT id, text, room_id, sender_id "
//                                                      "FROM invitations "
//                                                      "WHERE receiver_id =:userID "
//                                                      "AND status_id = 3"},

//          {setQuery::getRoomName, "SELECT name "
//                                              "FROM rooms "
//                                              "WHERE rooms.id = :1 "},

//          {setQuery::getUserName, "SELECT name "
//                                               "FROM users "
//                                               "WHERE id = :1"},

//          {setQuery::getRole,"SELECT role "
//                                          "FROM users_role "
//                                          "WHERE id =:1"},

//          {setQuery::getUnreadMess, "SELECT "
//                                    "messages.id, messages.time_sent, users.name, messages.text "
//                                    "FROM messages "
//                                    "INNER JOIN users on users.id = messages.sender_id "
//                                    "WHERE messages.room_id = :1 "
//                                    "AND "
//                                    "CAST(strftime('%s', messages.time_sent)  AS  integer) "
//                                     "> CAST(strftime('%s',  users.time_last_session)  AS  integer) "},

//          {setQuery::getReadMess, "SELECT "
//                                              "messages.id, messages.time_sent, users.name, messages.text "
//                                              "FROM messages "
//                                              "INNER JOIN users on users.id = messages.sender_id "
//                                              "WHERE messages.room_id = :1 "
//                                              "AND "
//                                              "CAST(strftime('%s', messages.time_sent)  AS  integer) "
//                                              "<= CAST(strftime('%s',  users.time_last_session)  AS  integer) "},

//          {setQuery::getMembers, "SELECT users.id, users.name "
//                                             "FROM rooms_users "
//                                             "INNER JOIN users ON users.id = rooms_users.user_id "
//                                             "WHERE rooms_users.room_id = :1 "
//                                             "AND users.id != :2"},



//          {setQuery::getRooms, "SELECT "
//                               "rooms_users.room_id, rooms.name, rooms_users.user_role_id "
//                               "FROM rooms_users "
//                               "INNER JOIN rooms ON rooms.id = rooms_users.room_id "
//                               "WHERE rooms_users.user_id = :1"},

//          {setQuery::setOnLine, "UPDATE users "
//                                            "SET status = 1,  time_session =:td "
//                                            "WHERE id = :1"},

//          {setQuery::setOffLine, "UPDATE users "
//                                             "SET status = 2, time_last_session = time_session "
//                                             "WHERE id = :1"},

//          {setQuery::insertNewMess, "INSERT INTO messages "
//                                                "(room_id, sender_id, text, time_sent) "
//                                                "VALUES (:1, :2, :3, :4) "},

//          {setQuery::insertNewRoom, "INSERT INTO rooms "
//                                                "(name, users_count) "
//                                                "VALUES (:roomName, 1) "},

//           {setQuery::getRoomID, "SELECT id "
//                                            "FROM rooms "
//                                            "WHERE name = :1"},

//          {setQuery::insertAdminToRoom, " INSERT INTO rooms_users "
//                                                    "(room_id, user_id, user_status_id, user_role_id) "
//                                                    "VALUES  (:roomID, :userID, 1, 1) "},
//          {setQuery::deleteRoom, "DELETE FROM rooms "
//                                          "WHERE rooms.id = :1"},
//          {setQuery::getUserID, "SELECT id "
//                                            "FROM users "
//                                            "WHERE name = :1"},
//          {setQuery::insertNewInvite, "INSERT INTO invitations "
//                                                  "(text, room_id, sender_id, receiver_id, status_id) "
//                                                  "VALUES (:1, :2, :3, :4, 3)"},
//          {setQuery::updateInvite, "UPDATE invitations "
//                                               "SET status_id = :1"
//                                                "WHERE id = :1"},
//          {setQuery::insertInvitedUsers, "INSERT INTO rooms_users "
//                                                      "(room_id, user_id, user_status_id, user_role_id) "
//                                                      "VALUES  (:1, :2, :3, 2) "},
//          {setQuery::deleteUser, "DELETE FROM rooms_users "
//                                             "WHERE rooms_users.user_id = :1 "
//                                             "AND rooms_users.room_id = :2"},
// };


