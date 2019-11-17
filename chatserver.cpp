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

void chatServer::seachSessionForDelRoom(QList<int> listUserOnline, int roomID, QString roomName)
{
    if (!listUserOnline.isEmpty()){
        if (!listUserOnline.isEmpty()){
            QListIterator<int> id(listUserOnline);
            int currentID;
            while (id.hasNext()){
                currentID = id.next();
                QListIterator<session*> i(sessionList);
                session* currentSession;
                while (i.hasNext()){
                    currentSession = i.next();
                    if (currentSession->getIdClient() == currentID){
                        currentSession->broadCastDelRoom(roomName, roomID);
                        break;
                    }
                }
            }

        }
    }
}

void chatServer::seachSession(QList<int> listUserOnline, QString text,
                              QString senderName, int roomID)
{
    if (!listUserOnline.isEmpty()){
        QListIterator<int> id(listUserOnline);
        int currentID;
        while (id.hasNext()){
            currentID = id.next();
            QListIterator<session*> iSession(sessionList);
            session* currentSession;
            while (iSession.hasNext()){
                currentSession = iSession.next();
                if (currentSession->getIdClient() == currentID){
                    currentSession->broadCast(text, senderName, roomID);
                    break;
                }
            }

        }
    }
}

void chatServer::seachSessioForInvite(int invitedUserID, QString senderName,
                                      QString roomName, QString textInvite, int roomID)
{
    QListIterator<session*> iSession(sessionList);
    session* currentSession;
    while (iSession.hasNext()){
        currentSession = iSession.next();
        if (currentSession->getIdClient() ==invitedUserID){
            currentSession->sendInvite(senderName, textInvite, roomName, roomID);
            break;
        }
    }
}

void chatServer::serverStart()
{
    if (serverChat->listen (QHostAddress::Any, 6000)){
        qDebug() << "serverStart";
        emit serverStarted();
       // emit logToMainWindow("serverStart \n");
    }
    else{
        qDebug() << "not connect server" << serverChat->errorString();
        //emit logToMainWindow(serverChat->errorString());
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
    connect(sessionPntr, &session::notifyRoomRemoval,
            this, &chatServer::seachSessionForDelRoom);
    connect(sessionPntr, &session::notifyNewMessage,
            this, &chatServer::seachSession);
    connect(sessionPntr, &session::sendInviteUser,
            this, &chatServer::seachSessioForInvite);
}

void chatServer::removeSession()
{
    qDebug() << "removeSession";
    // определяем сессию(клиента),который послал сигнал
    session *sessionPntr = static_cast<session*>(sender());
    // определяем id клиента
    int id = sessionPntr->getIdClient();
    // посылаем сигнал в UI для логирования окончания данной сессии
    //emit logToMainWindow("session "+QString::number(id) +" closed\n");
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



