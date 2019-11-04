#include "chatserver.h"


chatServer::chatServer(QObject *mainWindow, const char* logSlot)
    //mainWindow(mainWindow), logSlot(logSlot)
{
   this->mainWindow = mainWindow;
   this->logSlot = logSlot;
   serverChat = new QTcpServer();
   db = new reciprocityDB();
   connect(this, SIGNAL(logToMainWindow(QString)), mainWindow, logSlot);
   connect(db, SIGNAL(dbConnected(QString)), mainWindow, logSlot);
   //connect(serverChat, SIGNAL(logQueryReaded(QString)), mainWindow, logSlot);
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

void chatServer::serverStart()
{

    qDebug() << "serverStart";
    if (serverChat->listen (QHostAddress::Any, 6000)){
        emit serverStarted();
        emit logToMainWindow("serverStart \n");
    }
    else{
        qDebug() << "not connect server" << serverChat->errorString();
        emit logToMainWindow(serverChat->errorString());
    }
}

void chatServer::newClient()
{
    //получаем сокет для дальнейшей связи с клиентом
    session *sessionPntr = new session(serverChat->nextPendingConnection());
    // формируем лист указателей на сессии
    connect(sessionPntr, SIGNAL(logQueryReaded(QString)), mainWindow, logSlot);
    sessionList.append(sessionPntr);
    // связь прервалась - удаляем сессию
    connect(sessionPntr, &session::connectClosed, this, &chatServer::removeSession);
}

void chatServer::removeSession()
{
    // определяем сессию(клиента),который послал сигнал
    session *sessionPntr = static_cast<session*>(sender());
    // определяем его id клиента
    int id = sessionPntr->getIdClient();
    // посылаем сигнал в UI для логирования окончания данной сессии
    emit logToMainWindow("session "+QString::number(id) +" closed\n");
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



