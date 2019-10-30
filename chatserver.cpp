#include "chatserver.h"


chatServer::chatServer()
{
   serverChat = new QTcpServer();
   connect(serverChat, &QTcpServer::newConnection,
           this, &chatServer::newClient);
}

void chatServer::serverStart()
{
    if (serverChat->listen (QHostAddress::Any, 6000)){
        emit serverStarted();
        connectChatToDB();
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
    emit oneSessionClosed();
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

void chatServer::connectChatToDB()
{
    dbChat = QSqlDatabase::addDatabase("QSQLITE");
    QString pathBIN = QDir::currentPath();
    QString pathDB = QFileInfo(pathBIN).path();
    qDebug() <<"pathDB" << pathDB;
    //dbChat.setDatabaseName("D:/itstep/Qt/Projects/server_221019/chat_server-master/chat_server-master/chat.db3");
    dbChat.setDatabaseName(pathDB+"/DB/chat.db3");
    if (!dbChat.open())
    {
        qDebug() << "error connect";
    }
    else
    {
        emit dbConnected();
        qDebug() << "connect to db";

    }
}
