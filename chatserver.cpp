#include "chatserver.h"

chatServer::chatServer()
{
   serverChat = new QTcpServer();
   db = new reciprocityDB();
   setConnectServer();
}

chatServer::~chatServer(){
    delete db;
    delete serverChat;
}


void chatServer::setConnectServer()
{
    // присоединился новый клиент
    connect(serverChat, &QTcpServer::newConnection,
            this, &chatServer::newClient);
    // старт сервера - соединение с БД
    connect(this,&chatServer::serverStarted,
            db, &reciprocityDB::connectChatToDB);
}

void chatServer::sendMessDelRoom(QList<int> listUserOnline, int roomID,
                                 QString roomName, QString adminName)
{
    if (!listUserOnline.isEmpty()){
        // просматриваем список онлайн-участников комнаты
        QListIterator<int> id(listUserOnline);
        int currentID;
        while (id.hasNext()){
            currentID = id.next();
            session* currentSession = getSession(currentID);
            currentSession->sendMessDelRoom(roomName, roomID, adminName);
        }
    }
}

void chatServer::sendMessUpdateUsesrs(QList<int> listUserOnline, int userID,
                                      QString userName, int roomID,
                                      QString roomName, setUpdateUsers param)
{
    if (!listUserOnline.isEmpty()){
        // просматриваем список онлайн-участников комнаты
        QListIterator<int> id(listUserOnline);
        int currentID;
        while (id.hasNext()){
            currentID = id.next();
            session* currentSession = getSession(currentID);
            currentSession->sendMessUpdateUsers(userID, userName,
                                            roomID, roomName, param);
        }
    }
}

void chatServer::sendNewMess(QList<int> listUserOnline, QString text,
                              QString senderName, int roomID)
{
    if (!listUserOnline.isEmpty()){
        // просматриваем список онлайн-участников комнаты
        QListIterator<int> id(listUserOnline);
        int currentID;
        while (id.hasNext()){
            currentID = id.next();
            session* currentSession = getSession(currentID);
            currentSession->sendMess(text, senderName, roomID);
        }
    }
}


void chatServer::sendInvite(int invitedUserID)
{
    session* currentSession = getSession(invitedUserID);
    currentSession->sendMessInvite();
}


session *chatServer::getSession(int sessionID)
{
    QListIterator<session*> iSession(sessionList);
    session* currentSession = sessionList.first();
    while (iSession.hasNext()){
        currentSession = iSession.next();
        if (currentSession->getClientID() == sessionID){
            break;
        }
    }
    return currentSession;
}


void chatServer::serverStart()
{
    if (serverChat->listen (QHostAddress::Any, 6000)){
        qDebug() << "serverStart";
        emit serverStarted();
    }
    else{
        qDebug() << "not connect server" << serverChat->errorString();
    }
}

void chatServer::newClient()
{
    qDebug() << "newClient";
    //получаем сокет для дальнейшей связи с клиентом
    session *sessionPntr = new session(serverChat->nextPendingConnection());
    // формируем лист указателей на сессии
    sessionList.append(sessionPntr);
    // связь прервалась - удаляем сессию
    connect(sessionPntr, &session::connectClosed,
            this, &chatServer::removeSession);
    connect(sessionPntr, &session::distribMessDelRoom,
            this, &chatServer::sendMessDelRoom);
    connect(sessionPntr, &session::distribNewMess,
            this, &chatServer::sendNewMess);
    connect(sessionPntr, &session::sendInvite,
            this, &chatServer::sendInvite);
    connect(sessionPntr, &session::sendUpdateUsers,
            this, &chatServer::sendMessUpdateUsesrs);
    connect(this, &chatServer::sessionClosedForDB,
            sessionPntr,&session::setOffLine);
}

void chatServer::removeSession()
{
    qDebug() << "removeSession";
    // определяем сессию(клиента),который послал сигнал
    session *sessionPntr = static_cast<session*>(sender());
    // определяем id клиента
    int id = sessionPntr->getClientID();
     qDebug() << "session "+QString::number(id) +" closed";
    // посылаем сигнал в БД для изменения статуса клиента на офф-лайн
    emit sessionClosedForDB(id);
    // находим указатель в списке сессий и удаляем из списка
    QMutableListIterator<session*> i(sessionList);
    while (i.hasNext()){
        session* currentSession = i.next();
        if (currentSession == sessionPntr){
            i.remove();
            break;
        }
    }
    // освобождаем память, выделенную для этой сессии
    delete sessionPntr;
}



