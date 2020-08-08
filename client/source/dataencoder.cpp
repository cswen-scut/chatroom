#include "dataencoder.h"
#include<QDebug>
DataEncoder::DataEncoder() {

}

string DataEncoder::encode(unsigned int protocolId, unsigned int account, unsigned int dataType, unsigned int dataLength) {
    hp = head;
    encodeElement(protocolId, PROTOCOL_ID_SIZE);
    encodeElement(account, ACCOUNT_SIZE);
    encodeElement(dataType, DATA_TYPE_SIZE);
    encodeElement(dataLength, DATA_SIZE);
    return string(head,sizeof(head));
}

void DataEncoder::encodeElement(unsigned int data, unsigned int len) {
    char* c = hp + len - 1;
    for (int i = len; i > 0; i--) {
        *c = (char)(data & 0xff);
        c--;
        data >>= 8;
    }
    hp = hp + len;
}
