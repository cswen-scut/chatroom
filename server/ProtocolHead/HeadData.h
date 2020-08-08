//
// Created by cswen on 2020/8/3.
//

#pragma once
#include"protocolmsg.h"
#include <unistd.h>
#include<arpa/inet.h>
#include<iostream>

using namespace std;

class HeadData {
    //协议号(1B)-账号(2B)-数据类型(1B)-数据长度(4B)-数据
private:
    char buffer[BASE_BUFFER_SIZE];
    const char *bp;
    unsigned int protocolId;
    unsigned int account;
    unsigned int dataType;
    unsigned int dataLength;

    bool baseParse();

    unsigned int parseInt(int len);

public:
    HeadData(int fd);

    HeadData();

    bool parse(const char *buffer);

    unsigned int getProtocolId();

    unsigned int getAccount();

    unsigned int getDataType();

    unsigned int getDataLength();

};

