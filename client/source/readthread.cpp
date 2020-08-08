#include "readthread.h"
#include "readclient.h"
#include<QTcpSocket>
ReadThread::ReadThread()
{

}

void ReadThread::run(){
//    Client * client = Client::getTcpSocketClient();
//    QTcpSocket * tcpSocket = client->getTcpClient();
    ReadClient * readClient = ReadClient::getTcpSocketClient();
    QTcpSocket * tcpSocket = readClient->getTcpClient();
    while(true){
        if(tcpSocket->waitForReadyRead(-1)){
               readClient->readData();
        }
    }
}
