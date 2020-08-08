//
// Created by cswen on 2020/8/3.
//

#include "DataProcesser.h"

DataProcesser::DataProcesser() {

}

int DataProcesser::checkSocketConnected(int sock) {
    if (sock <= 0)
        return 0;
    tcp_info info{};
    int len = sizeof(info);
    getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) &len);
    if (info.tcpi_state == TCP_ESTABLISHED) {
        return 1;
    } else {
        return 0;
    }
}

string DataProcesser::readTextContent(int fd, unsigned int dataLength) {
    unsigned int count = 0;
    //注意size一定不能是unsigned型, 因为后面read可能会返回-1，从而造成错误
    int size = 0;
    unsigned int buffSize = TCP_BUFSIZ;
    string content;
    while (true) {
        if (checkSocketConnected(fd) == 0) {
            break;
        }
        if((size = read(fd, buffer, min(buffSize, dataLength - count))) <= 0){
            break;
        }
        if (size > 0) {
            count += size;
            content += string(buffer, size);
        }
        if (count == dataLength) {
            break;
        }
    }
    return content;
}

string DataProcesser::readImageContent(int fd, unsigned int dataLength) {
    //获取当前时间戳作为文件名前缀
    string imagePath = IMAGE_PATH + MyTime::getTimeStampStr() + ".png";
    ofstream os(imagePath, ios::out | ios::binary);
    unsigned int count = 0;
    int size = 0;
    unsigned int buffSize = TCP_BUFSIZ;
    while (true) {
        if (checkSocketConnected(fd) == 0) {
            break;
        }
        if((size = recv(fd, buffer, min(buffSize, dataLength - count), MSG_WAITALL)) <= 0){
            break;
        }
        count += size;
        os.write(buffer, size);
        if (count == dataLength) {
            break;
        }
    }
    os.close();
    return imagePath;
}

void DataProcesser::writeText(int fd, unsigned int account, string text, unsigned int protocolId) {
    DataEncoder de;
    string headStr = de.encode(protocolId, account, TEXT, text.length());
    if (checkSocketConnected(fd) == 0) {
        return;
    }
    send(fd, headStr.data(), headStr.length(), MSG_NOSIGNAL);
    read(fd, buffer, BASE_BUFFER_SIZE);
    int count = 0;
    unsigned int dataLength = text.length();
    const char *data = text.data();
    unsigned int buffSize = TCP_BUFSIZ;
    HeadData hd;
    while (true) {
        if (checkSocketConnected(fd) == 0) {
            break;
        }
        ssize_t size = send(fd, data, min(buffSize, dataLength - count), MSG_NOSIGNAL);
        count += size;
        data = data + size;
        //接收客户端的确认信息
        unsigned int splitDataLength = 0;
        while (true) {
            if (checkSocketConnected(fd) == 0) {
                break;
            }
            read(fd, buffer, BASE_BUFFER_SIZE);
            hd.parse(buffer);
            splitDataLength += hd.getDataLength();
            if (splitDataLength == size) {
                break;
            }
        }
        if (count == dataLength) {
            break;
        }
    }
}

void DataProcesser::writeImage(int fd, unsigned int account, const string &imagePath) {
    ifstream in(imagePath, ios::in | ios::binary);
    if (!in.is_open()) {
        cout << "文件打开失败" << endl;
        return;
    }
    int imageSize = getFileLength(imagePath);
    DataEncoder de;
    string headStr = de.encode(SEND, account, IMAGE, imageSize);
    if (imageSize == 0) {
        in.close();
        return;
    }
    in.seekg(0);
    if (checkSocketConnected(fd) == 0) {
        return;
    }
    send(fd, headStr.data(), headStr.length(), MSG_NOSIGNAL);
    read(fd, buffer, BASE_BUFFER_SIZE);
    int count = 0;
    HeadData hd;
    while (in.tellg() != -1) {
        if (checkSocketConnected(fd) == 0) {
            break;
        }
        in.read(buffer, TCP_BUFSIZ);
        send(fd, buffer, in.gcount(), MSG_NOSIGNAL);
        //接收客户端的确认信息
        unsigned int splitDataLength = 0;
        while (true) {
            if (checkSocketConnected(fd) == 0) {
                break;
            }
            read(fd, buffer, BASE_BUFFER_SIZE);
            hd.parse(buffer);
            splitDataLength += hd.getDataLength();
            if (splitDataLength == in.gcount()) {
                break;
            }
        }
        count += in.gcount();
    }
    in.close();
}

void DataProcesser::writeMsg(int fd, unsigned int account, string text, unsigned int protocolId) {
    DataEncoder de;
    string headStr = de.encode(protocolId, account, TEXT, text.length());
    text = headStr + text;
    send(fd, text.data(), text.length(), MSG_NOSIGNAL);
}

void
DataProcesser::writeTextToAllUser(const vector<int> &fds, int account, const string &text, unsigned int protocolId) {
    for (auto &fd : fds) {
        writeText(fd, account, text, protocolId);
    }
}

void DataProcesser::writeImageToAllUser(const vector<int> &fds, int account, const string &imagePath) {
    for (auto &fd : fds) {
        writeImage(fd, account, imagePath);
    }
}

int DataProcesser::getFileLength(const string &fileName) {
    ifstream in(fileName, ios::in | ios::binary);
    if (!in.is_open()) {
        cout << "文件打开失败" << endl;
        return 0;
    }
    in.seekg(0, ios::end); //设置文件指针到文件流的尾部
    int fileLength = in.tellg();
    in.close();
    return fileLength;
}

