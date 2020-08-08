#ifndef WRITECLIENT_H
#define WRITECLIENT_H
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
#include "client.h"

using namespace std;
class WriteClient : public QObject, public Client
{
   Q_OBJECT
private:
    WriteClient();
    static WriteClient *writeClient;

public:
    static WriteClient * getTcpSocketClient();

    void writeImage(unsigned int account, string imagePath);

    ~WriteClient();

};
#endif // WRITECLIENT_H
