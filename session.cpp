#include "session.h"


session::session(QTcpSocket *socket)
{
    socketSession = socket;
    connect (socketSession, &QTcpSocket::readyRead,
             this,&session::commandHandler);
    connect(socketSession, &QTcpSocket::disconnected,
            this, &session::connectClosed);
}

void session::commandHandler()
{
    //QThread::sleep(10000);

    protocolIn InputMessage(socketSession);
    codeCommand = InputMessage.getCode();
    QByteArray baRreplayToClient;
    QJsonObject joTemp;
    switch (codeCommand) {
            case setCodeCommand::ErrorMessage:
            {
                joTemp.insert("codeCommand", setCodeCommand::ErrorMessage);
                joTemp.insert("joDataInput", "your have send message-error");
            }
            break;
            case setCodeCommand::Auth:
            {
                joTemp.insert("codeCommand", setCodeCommand::Auth);
                login =InputMessage.getLoginClient();
                pass = InputMessage.getPassClient();
                joTemp.insert("joDataInput", readFromDB());
            }
            break;
            case setCodeCommand::SessionClosed:
            {}
            break;
            case setCodeCommand::NoConnect:
            {}
            break;
        }
    qDebug() << "joTemp " << joTemp;
    QJsonDocument doc(joTemp);
    baRreplayToClient = doc.toJson(QJsonDocument::Compact);
    protocolOut OutputMessage(baRreplayToClient);
    socketSession->write(OutputMessage.getMessageToClient());
}

QJsonObject session::readFromDB()
{
    QSqlQuery qAuth;
    QJsonObject joReplayDB;
    switch (codeCommand)
    {
        case setCodeCommand::ErrorMessage: {} break;
        case setCodeCommand::Auth:
        {
//            QString qsAuth = "select id,name from users where users.login='"+ login +
//                    "' and users.password='" + pass +"'";
           // qDebug() << "queryString" << queryString;
            QString qsAuth = "select id,name from users where users.login='%1' and users.password='%2'";
            qDebug() << "qsAuth" << qsAuth.arg(login).arg(pass);
            if (!qAuth.exec(qsAuth.arg(login).arg(pass))){
                    qDebug() << "query SELECT error" << qAuth.lastError();
            }
            else {
                qDebug() << "query success" ;
                while (qAuth.next()){

                    //qDebug() << "query.size()" << query.numRowsAffected();
                    if (qAuth.numRowsAffected() != -1){
                        id = qAuth.value(0).toInt();
                        joReplayDB.insert("id",qAuth.value(0).toInt());
                        joReplayDB.insert("name",qAuth.value(1).toString());
                        QSqlQuery qSetUserOnline;
                        QString qsSetUserOnline;
                        //qsSetUserOnline = "UPDATE users SET status = 1  WHERE id = \"";
//                        qsSetUserOnline.append(QString::number(qSetUserOnline.value(0).toInt()));
//                        qsSetUserOnline.append("\"");
                        qsSetUserOnline = "UPDATE users SET status = 1  WHERE id = %1";
                        if (!qSetUserOnline.exec(qsSetUserOnline.arg(qSetUserOnline.value(0).toInt()))){
                            qDebug() << "query UPDATE error" << qSetUserOnline.lastError();
                        }
                        QSqlQuery qRoomsID;
                        QList<int> lRoomsID;
                        QString qsRoomsID = "select room_id from rooms_users where rooms_users.user_id = %1";
//                        QSqlQuery qRoomsName;
//                        QList<int> lRoomsName;
//                        QString qsRooms = "select rooms_users.room_id, rooms.name"
//                                          "from rooms_users inner join rooms on rooms.id = rooms_users.room_id"
//                                          "where rooms_users.user_id = %1";
                        if (!qRoomsID.exec(qsRoomsID.arg(id))){
                            qDebug() << "query select room_id error" << qRoomsID.lastError();
                        }
                        else{
                            qDebug() << "query select room_id ";
                            while (qRoomsID.next()){
                                lRoomsID.append(qRoomsID.value(0).toInt());
                            }
                            //auto doc = QJsonDocument(toJson(lRoomsID));
                            joReplayDB.insert("list_rooms_id", toJson(lRoomsID));
//                            QJsonArray arr = joReplayDB.value("list_rooms_id").toArray();
//                            arr.toVariantList();
                        }
                     }
                    else{
                        joReplayDB.insert("id",0);
                        joReplayDB.insert("name","null");
                    }
                }
            }
        }
         break;
        case setCodeCommand::NoConnect:
            {} break;
        case setCodeCommand::SessionClosed:
            {} break;
    }
    qDebug() << "joReplayDB " << joReplayDB;
    return joReplayDB;
}

int session::getID()
{
    return id;
}

QJsonArray session::toJson(const QList<int> & list) {
   QJsonArray array;
   for (auto & i : list)
      array.append(i);
   return array;
}
