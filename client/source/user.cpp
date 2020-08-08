#include "user.h"

User::User(unsigned int userId, QString username)
{
    this->userId = userId;
    this->username = username;
}

QString User::getUsername(){
    return username;
}
unsigned int User::getUserId(){
    return userId;
}
