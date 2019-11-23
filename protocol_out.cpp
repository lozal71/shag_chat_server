#include "protocol_out.h"

protocolOut::protocolOut()
{

}

protocolOut::protocolOut(QTcpSocket *socket)
{
    this->socket = socket;
}


//QByteArray protocolOut::getPackage()
//{
//    //qDebug() << "getPackage baPackage" <<baPackage ;
//    return baPackage;
//}

//void protocolOut::setPackage(QJsonDocument jdParam)
//{
//    baPackage.clear();
//    //qDebug() << "jdParam" << jdParam;
//    QByteArray baTemp = jdParam.toJson(QJsonDocument::Compact);
//    //quint32 packageSize = str.length();
//    //qDebug() << "baTemp" << baTemp;
//    quint32 packageSize = quint32(baTemp.size());
//    //qDebug() << "packageSize" << packageSize;
//    QDataStream stream(&baPackage, QIODevice::ReadWrite);
//    stream << packageSize;
//    stream << baTemp;
//    //qDebug() << "setPackage baPackage" <<baPackage ;
//}

void protocolOut::writeSocket(QVariantMap mapSocket)
{
    jsonDoc = QJsonDocument::fromVariant(mapSocket);
    transferJSONtoBA();
    //out->setPackage(jdResponse);
   // qDebug() << client.id; // << out->getPackage();
    //qDebug() << jsonDoc;
    this->socket->write(baPackage);
}

QVariantMap protocolOut::readSocket()
{
    QVariantMap mapCommand;
    transferBAtoJSON();
    QJsonObject joTemp = jsonDoc.object();
    return mapCommand =joTemp.toVariantMap();
}

void protocolOut::transferBAtoJSON()
{
    //qDebug() << "receiveJSONdoc";
    //QJsonDocument jdTemp;
    flag_error = false;
    // если в сокете меньше, чем 4 байта
    if (socket->bytesAvailable() < 4){
        flag_error = true;
     }
    else
    {
        quint32 packageSize = 0; // размер пришедшего сообщения
        quint32 baBufferLength = 0; // размер буфера
        qint64 bytesInSocket = 0;  // число байтов в сокете
        QByteArray baBuffer;
        baBuffer.clear();
        baBuffer = socket->read(4); // считываем 4 байта
        //qDebug() << "baBuffer" << baBuffer;
        QDataStream stream(baBuffer); // назначаем baBuffer получателем данных стрима
        stream >> packageSize; // считываем размер пришедшего сообщения
        //qDebug() << "packageSize " << packageSize;
        packageSize+=4; // прибавляем 4 байта, чтобы отсечь длину сообщения от данных
        baBuffer.clear();
//        baBufferLength = quint32(baBuffer.length());
        // узнаем сколько сейчас доступно байт в сокете
        bytesInSocket = socket->bytesAvailable();
        // пока не получены все байты сообщения
        while (true) {
            // если число доступных байт в сокете меньше или равно, чем ожидаемый остаток
            if (bytesInSocket <= packageSize - baBufferLength){
                baBuffer.append(socket->readAll()); // читаем из буфера все, что есть
        //        qDebug() << "buffer в цикле  " << baBuffer;
            }
            else  {
                    // читаем только остаток
                    baBuffer.append(socket->read(packageSize - baBufferLength));
                }
            baBufferLength += quint32(baBuffer.length());
            //qDebug() << "baBufferLength" << baBufferLength;
            bytesInSocket = socket->bytesAvailable();
            if (baBufferLength == packageSize) break;
        }
        baBuffer.remove(0, 4); // отсекаем длину сообщения
        //qDebug() << "buffer в конце " << baBuffer;
        QString sBuffer(baBuffer);
        jsonDoc = QJsonDocument::fromJson(sBuffer.toUtf8());
//         qDebug() << "jdTemp " <<jdTemp;
    }
    //return jdTemp;
}

bool protocolOut::isError()
{
    return flag_error;
}

void protocolOut::transferJSONtoBA()
{
    baPackage.clear();
    //qDebug() << "jdParam" << jdParam;
    QByteArray baTemp = jsonDoc.toJson(QJsonDocument::Compact);
    //quint32 packageSize = str.length();
    //qDebug() << "baTemp" << baTemp;
    quint32 packageSize = quint32(baTemp.size());
    //qDebug() << "packageSize" << packageSize;
    QDataStream stream(&baPackage, QIODevice::ReadWrite);
    stream << packageSize;
    stream << baTemp;
    //qDebug() << "setPackage baPackage" <<baPackage ;
}
