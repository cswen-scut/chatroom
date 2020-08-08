#ifndef CLIENT_H
#define CLIENT_H

#include<QTcpSocket>
#include<QtAlgorithms>
#include<string>
#include<fstream>
#include <QMessageBox>
#include<QDebug>
#include <QTextCodec>
#include<QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include"dataparser.h"
#include"dataencoder.h"
#include"configreader.h"
#define IMAGE_PATH "./image/"
#define TCP_BUFSIZ 8192
using namespace std;

class Client
{
public:
    QString hostName;
    quint16 port;
    QTcpSocket tcpClient;
    char buffer[TCP_BUFSIZ];
    unsigned int protocolId;
    unsigned int account;
    unsigned int dataType;
    unsigned int dataLength;
    const string ACK_PACKET = DataEncoder().encode(ACK,0,0,0);

public:
    Client();

    void readHeadData();

    QJsonObject readServerMsg();

    void writeText(unsigned int account ,string text, unsigned int protocolId = SEND);

    void closeTcpSocket();

    QTcpSocket * getTcpClient();

    ~Client();
};

#endif // CLIENT_H
