#include "dataparser.h"

DataParser::DataParser(char * buffer)
{
    bp = buffer;
}

bool DataParser:: baseParse() {
    this->protocolId = parseInt(PROTOCOL_ID_SIZE);
    this->account = parseInt(ACCOUNT_SIZE);
    this->dataType = parseInt(DATA_TYPE_SIZE);
    this->dataLength = parseInt(DATA_SIZE);
}

unsigned int DataParser::parseInt(int len) {
    unsigned int sum = 0;
    unsigned int i = 0;
    for (char* end = bp + len - 1; bp <= end; end--) {
        sum = sum + (((unsigned int)((unsigned char)(*end))) << i);
        i += 8;
    }
    bp = bp + len;
    return sum;
}

    unsigned int DataParser::getProtocolId() {
        return this->protocolId;
    }
    unsigned int DataParser::getAccount() {
        return this->account;
    }
    unsigned int DataParser::getDataType() {
        return this->dataType;
    }
    unsigned int DataParser::getDataLength() {
        return this->dataLength;
    }

    DataParser::~DataParser() {

    }
