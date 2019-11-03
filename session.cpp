#include "session.h"


session::session()
{

}

session::session(QTcpSocket *socket)
{
    socketSession = socket;
    in = new protocolIn();
    out = new protocolOut();
    sessionDB = new reciprocityDB();
    connect (socketSession, &QTcpSocket::readyRead,
             this,&session::readQuery);

//    connect( this, &session::sessionClosed,
//             sessionDB, &reciprocityDB::setStatusOFFline);

    connect(socketSession, &QTcpSocket::disconnected,
            this, &session::connectClosed);
}

session::~session(){
    delete in;
    delete out;
    delete sessionDB;
}

int session::getIdClient()
{
    return idClient;
}


void session::readQuery()
{
    // получаем JSON-документ из сокета
    QJsonDocument jdTemp = in->receiveJSONdoc(socketSession);
    QString sTemp;
    QJsonObject joTemp = jdTemp.object();
    //qDebug() << "joTemp" << joTemp;
    QVariantMap mapCommand =joTemp.toVariantMap();
    QVariantMap mapResponse;
    //qDebug() << "mapCommand" << mapCommand;

    // определяем команду запроса
    codeCommand = setCodeCommand(mapCommand["codeCommand"].toInt());

    if (in->isError()){
         sTemp = "Problem: error massage";
    }
    else {
        switch (codeCommand) {
            case setCodeCommand::Auth:
            {
                sTemp = "query auth for ";
                QVariantMap mapData =  mapCommand["joDataInput"].toMap();
//                joTemp.insert("codeCommand", setCodeCommand::Auth);
                login =mapData["login"].toString();
                pass = mapData["pass"].toString();
                //joTemp.insert("joDataInput", readFromDB());
                sTemp += login + "," + pass + "\n";
                mapResponse = sessionDB->readAuth(login, pass);
                idClient = mapResponse["id"].toInt();
                mapRooms = mapResponse["rooms"].toMap();
            }
            break;
        }
    }
    emit logQueryReaded(sTemp);
    mapCommand["joDataInput"]=mapResponse;
    writeResponse(mapCommand);
}

void session::writeResponse(QVariantMap mapParam)
{
    QJsonDocument jdResponse = QJsonDocument::fromVariant(mapParam);
    out->setPackage(jdResponse);
    socketSession->write(out->getPackage());
}
