#include "writeclient.h"

WriteClient::WriteClient()
{

}
//初始化静态成员变量
WriteClient* WriteClient::writeClient = nullptr;
WriteClient * WriteClient::getTcpSocketClient(){
    if(writeClient == nullptr){
        writeClient = new WriteClient;
        unordered_map<string, string> configMap = ConfigReader::readServerConfig();
        qDebug() << "从配置文件读取的" << QString::fromStdString(configMap["HOST"]) << "->" << QString::fromStdString(configMap["PORT"]);
        writeClient->hostName = QString::fromStdString(configMap["HOST"]);
        writeClient->port = stoi(configMap["PORT"]);
        qDebug() << "------------正在连接服务器--------------" << writeClient->hostName;
        long long start;
        time(&start);
        writeClient->tcpClient.connectToHost(writeClient->hostName, writeClient->port);
        bool connResult = writeClient->tcpClient.waitForConnected(10000);
        long long end;
        time(&end);
        qDebug() << "连接时长" << (end - start);
        if(end - start < 8){
            qDebug() << "------------连接服务器成功--------------";
        }
        else {
            writeClient = nullptr;
        }
    }
    return writeClient;
}

void WriteClient:: writeImage(unsigned int account, string imagePath){
    ifstream in(imagePath, ios::in | ios::binary);
    in.seekg(0, ios::end); //设置文件指针到文件流的尾部
    streampos imageSize = in.tellg();
    DataEncoder encoder;
    string headStr = encoder.encode(SEND, account, IMAGE, imageSize);
    in.seekg(0);
    qDebug() << "需要发送的图片的字节数-----" << imageSize;
    int count = 0;
    tcpClient.write(headStr.data(), headStr.length());
    while(in.tellg() != -1){
        in.read(buffer, TCP_BUFSIZ);
        int size = tcpClient.write(buffer, in.gcount());
        count += size;
    }
    qDebug() << "实际发送的字节数---------" << count;
    in.close();
}

WriteClient::~WriteClient(){
    delete writeClient;
    writeClient = nullptr;
}
