#include "protocol_out.h"

protocolOut::protocolOut(){

}

protocolOut::protocolOut(QByteArray &str)
{
//    QDataStream sendStream(&block, QIODevice::ReadWrite);
//    //резервируем в начале блока память для будующего размера и  пишем в блок остальные данные,
//    sendStream << quint32(0) << str;
//    // устанавливаем указатель записи на начало
//    sendStream.device()->seek(0);
//    // помещаем в это начало получившийся размер блока.
//    sendStream << (quint32)(block.size()) - sizeof(quint32);
    quint32 packageSize = (quint32)str.length();
    QDataStream stream(&block, QIODevice::ReadWrite);
    stream << packageSize;
    stream << str;
}

QByteArray protocolOut::getMessageToClient()
{
    return block;
}
