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

void chatServer::seachSession(QMap<int,QString> mapUserOnline, int delRoomID)
{
    if (!mapUserOnline.isEmpty()){
        for (const int id: mapUserOnline.keys()){
            QMutableListIterator<session*> i(sessionList);
            session* currentSession;
            while (i.hasNext()){
                currentSession = i.next();
                if (currentSession->getIdClient() == id){
                    break;
                }
            }
            currentSession->broadCast(mapUserOnline[id], delRoomID);
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
    //connect(sessionPntr, SIGNAL(logQueryReaded(QString)), mainWindow, logSlot);
    sessionList.append(sessionPntr);
    // связь прервалась - удаляем сессию
    connect(sessionPntr, &session::connectClosed,
            this, &chatServer::removeSession);
    connect(sessionPntr, &session::notifyRoomRemoval,
            this, &chatServer::seachSession);
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



