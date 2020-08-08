#ifndef READCLIENT_H
#define READCLIENT_H
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
class ReadClient : public QObject, public Client
{
   Q_OBJECT
private:
    ReadClient();
    static ReadClient *readClient;
    string imagePath;
    string content = "";
    string notice = "";
public:
    static ReadClient * getTcpSocketClient();

    bool readData();

    void readTextContent();

    void readImageContent();

    string getTextContent();

    string getNoticeContent();

    string getImagePath();

    ~ReadClient();
signals:
    void textDisplayAble();
    void imageDisplayAble();
    void noticeDisplayAble();
    void onlineDisplayAble();
};

#endif // READCLIENT_H
