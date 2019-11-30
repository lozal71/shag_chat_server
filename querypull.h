#ifndef QUERYPULL_H
#define QUERYPULL_H
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMap>
#include <QVariantList>

enum  setQuery {getUserIdUserName,
                getWaitInvitation,
                getRoomName,
                getUserName,
                getRole,
                getUnreadMess,
                getReadMess,
                getMembers,
                getRooms,
                getMembersIdOnline,
                getRoomID,
                getUserID,
                getUserStatus,
                getIdSenderInvite,
                getInvitedUserName, getInvitedRoomName,
                setOnLine, setOffLine,
                insertNewMess, insertNewRoom,
                insertAdminToRoom,
                insertNewInvite, insertInvitedUsers,
                deleteRoom, deleteUser,
                updateInvite};


class queryPull: public QSqlQuery
{
public:
    queryPull();
    QSqlQuery makeQuery(setQuery qQuery, QVariantList lstParam);
private:
    QSqlQuery query;
    static QMap<setQuery, QString> mapQ;
    static QMap<int,QString> numberParam;
};
#endif // QUERYPULL_H
