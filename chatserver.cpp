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
    // закрытие сессии - изменение статуса клиента в БД на офф-лайн
    connect(this, &chatServer::sessionClosedForDB,
            db,&reciprocityDB::setStatusOFFline);
}

void chatServer::sendMessDelRoom(QList<int> listUserOnline, int roomID, QString roomName)
{
    if (!listUserOnline.isEmpty()){
        // просматриваем список онлайн-участников комнаты
        QListIterator<int> id(listUserOnline);
        int currentID;
        while (id.hasNext()){
            currentID = id.next();
            // просматриваем список сессий
            QListIterator<session*> i(sessionList);
            session* currentSession;
            while (i.hasNext()){
                currentSession = i.next();
                // если найдена сессия для онлайн-участника комнаты
                if (currentSession->getIdClient() == currentID){
                    // посылаем сообщение клиенту об удалении комнаты
                    currentSession->messDelRoom(roomName, roomID);
                    break;
                }
            }
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
            // просматриваем список сессий
            QListIterator<session*> i(sessionList);
            session* currentSession;
            while (i.hasNext()){
                currentSession = i.next();
                // если найдена сессия для онлайн-участника комнаты
                if (currentSession->getIdClient() == currentID){
                    // посылаем клиенту сообщение о появлении/удалении пользователя
                    // и команду изменить список участников комнаты
                    currentSession->messUpdateUsers(userID, userName,
                                                    roomID, roomName, param);
                    break;
                }
            }
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
            // просматриваем список сессий
            QListIterator<session*> iSession(sessionList);
            session* currentSession;
            while (iSession.hasNext()){
                currentSession = iSession.next();
                // если найдена сессия для онлайн-участника комнаты
                if (currentSession->getIdClient() == currentID){
                    // посылаем клиенту новое сообщение в комнату
                    currentSession->newMess(text, senderName, roomID);
                    break;
                }
            }

        }
    }
}


void chatServer::sendInvite(int invitedUserID)
{
    // просматриваем список сессий
    QListIterator<session*> iSession(sessionList);
    session* currentSession;
    while (iSession.hasNext()){
        currentSession = iSession.next();
        // если найдена сессия для приглашаемого участника
        if (currentSession->getIdClient() ==invitedUserID){
            // посылаем приглашение
            currentSession->sendInvite();
            break;
        }
    }
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
    connect(sessionPntr, &session::sendMessDelRoom,
            this, &chatServer::sendMessDelRoom);
    connect(sessionPntr, &session::sendNewMessage,
            this, &chatServer::sendNewMess);
    connect(sessionPntr, &session::sendInviteUser,
            this, &chatServer::sendInvite);
    connect(sessionPntr, &session::sendUpdateUsers,
            this, &chatServer::sendMessUpdateUsesrs);
}

void chatServer::removeSession()
{
    qDebug() << "removeSession";
    // определяем сессию(клиента),который послал сигнал
    session *sessionPntr = static_cast<session*>(sender());
    // определяем id клиента
    int id = sessionPntr->getIdClient();
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



