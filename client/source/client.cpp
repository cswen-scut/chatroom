#include "client.h"

Client::Client()
{

}

QJsonObject Client::readServerMsg(){
    readHeadData();
    tcpClient.read(buffer, dataLength);
    QString serverMsg = QString::fromStdString(string(buffer, dataLength));
    QJsonObject serverInfoObj = QJsonDocument::fromJson(serverMsg.toUtf8()).object();
    return serverInfoObj;
}

void Client:: readHeadData(){
    int size = tcpClient.read(buffer, BASE_BUFFER_SIZE);
    DataParser parser(buffer);
    parser.baseParse();
    protocolId = parser.getProtocolId();
    account = parser.getAccount();
    dataType = parser.getDataType();
    dataLength = parser.getDataLength();
}

void Client:: writeText(unsigned int account ,string text, unsigned int protocolId){
    DataEncoder encoder;
    qDebug() << "------------输入的字节数-------------" << text.length();
    string headStr = encoder.encode(protocolId, account, TEXT, text.length());
    tcpClient.write(headStr.data(), headStr.length());
    qDebug() << "------------头部大小-------------" << headStr.length();
    //text = headStr + text;
    if(text.length() != 0){
        tcpClient.write(text.data(), text.length());
    }
    qDebug() << "------------发送成功,总数据大小-------------" << text.length();
}

QTcpSocket * Client::getTcpClient(){
    return &tcpClient;
}

void Client:: closeTcpSocket(){
    tcpClient.disconnectFromHost();
    tcpClient.close();
}

Client::~Client(){

}
