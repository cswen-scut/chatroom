#include "mytime.h"
#include "readclient.h"

ReadClient::ReadClient()
{

}
ReadClient * ReadClient::readClient = nullptr;
ReadClient * ReadClient::getTcpSocketClient(){
    if(readClient == nullptr){
        readClient = new ReadClient;
        unordered_map<string, string> configMap = ConfigReader::readServerConfig();
        qDebug() << "从配置文件读取的" << QString::fromStdString(configMap["HOST"]) << "->" << QString::fromStdString(configMap["PORT"]);
        readClient->hostName = QString::fromStdString(configMap["HOST"]);
        readClient->port = stoi(configMap["PORT"]);
        qDebug() << "------------正在连接服务器--------------";
        readClient->tcpClient.connectToHost(readClient->hostName, readClient->port);
        qDebug() << "------------连接服务器成功--------------";
    }
    return readClient;
}

bool ReadClient::readData(){
    //读取数据头
    readHeadData();
    //确认信息
    tcpClient.write(ACK_PACKET.data(), ACK_PACKET.length());
    content = "";
    qDebug() << "----------数据长度-----------" << dataLength;
    if(dataType == TEXT){
        readTextContent();
        return true;
    }
    else if(dataType == IMAGE){
        readImageContent();
        return true;
    }
    return false;
}

void ReadClient::readTextContent(){
    while(tcpClient.waitForReadyRead(-1)){
        unsigned int size = 0;
        qDebug() << "-------readTextContent需要读取的字节数---------" << dataLength;
        unsigned int splitDataLength = tcpClient.bytesAvailable();
        unsigned int bufsize = TCP_BUFSIZ;
        while(dataLength != 0 && tcpClient.bytesAvailable()){
            qDebug() << "-----------------可读数据------------" << tcpClient.bytesAvailable();
            size = tcpClient.read(buffer, min(bufsize, dataLength));
            dataLength -= size;
            qDebug() << "-------readTextContent剩余读取的字节数---------" << dataLength;
            content = content + string(buffer, size);
        }
//        tcpClient.write(ACK_PACKET.data(), ACK_PACKET.length());
        string SPLIT_ACK_PACKET = DataEncoder().encode(ACK,account,TEXT,splitDataLength);
        tcpClient.write(SPLIT_ACK_PACKET.data(), SPLIT_ACK_PACKET.length());
        if(dataLength == 0){
            switch(protocolId){
                case SEND: emit textDisplayAble();break;
            case NOTICE:notice = content; emit noticeDisplayAble();break;
            case ONLINELIST: emit onlineDisplayAble();break;
            }
            break;
        }
        qDebug() << "-----------readTextContent剩余读取的字节数------" << dataLength;
    }
    qDebug() << "读取完毕.......";
}

void ReadClient::readImageContent(){
    //获取当前时间
    imagePath = IMAGE_PATH + MyTime::getTimeStampStr() + ".png";
    //保存图片
    ofstream os(imagePath, ios::out | ios::binary | ios::app);
    while(tcpClient.waitForReadyRead(-1)){
        unsigned int size = 0;
        qDebug() << "-------readImageContent需要读取的字节数---------" << dataLength;
        unsigned int splitDataLength = tcpClient.bytesAvailable();
        unsigned int bufsize = TCP_BUFSIZ;
        while(dataLength != 0 && tcpClient.bytesAvailable()){
            qDebug() << "-----------------可读数据------------" << tcpClient.bytesAvailable();
            size = tcpClient.read(buffer, min(bufsize, dataLength));
            dataLength -= size;
            qDebug() << "-------readImageContent剩余读取的字节数---------" << dataLength;
            os.write(buffer,size);
        }
        string SPLIT_ACK_PACKET = DataEncoder().encode(ACK,0,0,splitDataLength);
        tcpClient.write(SPLIT_ACK_PACKET.data(), SPLIT_ACK_PACKET.length());
        if(dataLength == 0){
            break;
        }
        qDebug() << "-----------readTextContent一轮读取后剩余读取的字节数------" << dataLength;
    }
    os.close();
    emit imageDisplayAble();
    qDebug() << "图片读取完毕.......";
}

string ReadClient::getTextContent(){
    return content;
}

string ReadClient::getNoticeContent(){
    return notice;
}

string ReadClient::getImagePath(){
    return imagePath;
}

ReadClient::~ReadClient(){
    delete readClient;
    readClient = nullptr;
}
