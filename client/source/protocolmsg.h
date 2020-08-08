#ifndef PROTOCOLMSG_H
#define PROTOCOLMSG_H
#define BASE_BUFFER_SIZE 8
#define PROTOCOL_ID_SIZE 1
#define ACCOUNT_SIZE 2
#define DATA_TYPE_SIZE 1
#define DATA_SIZE 4
#define LOGIN 1
#define SEND 2
#define READ 3
#define NOTICE 4
#define ACK 5
#define LOGOUT 6
#define REGISTER 7
#define ONLINELIST 8
#define CLOSE 9
#define TEXT 1
#define IMAGE 2
#define LOGIN_SUCCESS 0
#define LOGIN_FAIL 1
#define LOGIN_EXIST 2
#define REGISTER_SUCCESS 0
#define REGISTER_FAIL 1
#endif // PROTOCOLMSG_H
