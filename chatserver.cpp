#include "chatserver.h"


chatServer::chatServer()
{
   serverChat = new QTcpServer();
   db = new reciprocityDB();
   connect(serverChat, &QTcpServer::newConnection,
           this, &chatServer::newClient);
   connect(this,&chatServer::serverStarted,
           db, &reciprocityDB::connectChatToDB);
   connect(this, &chatServer::oneSessionClosed,
           db,&reciprocityDB::setStatusOFFline);

}

chatServer::~chatServer(){
    delete db;
    delete serverChat;
}

void chatServer::serverStart()
{
    qDebug() << "serverStart";
    if (serverChat->listen (QHostAddress::Any, 6000)){
        emit serverStarted();
    }
    else{
        qDebug() << "not connect server" << serverChat->errorString();
        emit serverError(serverChat->errorString());
    }
}

void chatServer::newClient()
{
    session *sessionPntr = new session(serverChat->nextPendingConnection());
    sessionPull.append(sessionPntr);
    connect(sessionPntr, &session::connectClosed, this, &chatServer::removeSession);
}

void chatServer::removeSession()
{
    session *sessionPntr = static_cast<session*>(sender());
    int id = sessionPntr->getIdClient();
    emit oneSessionClosed(id);
    QMutableListIterator<session*> i(sessionPull);
    while (i.hasNext()){
        session* currentSession = i.next();
        if (currentSession == sessionPntr){
            i.remove();
            break;
        }
    }
    delete sessionPntr;
}

