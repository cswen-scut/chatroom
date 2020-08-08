#ifndef USER_H
#define USER_H
#include <QString>
#include<string>
using namespace std;

class User
{
private:
    QString username;
    unsigned int userId;
public:
    User(unsigned int userId, QString username);

    QString getUsername();

    unsigned int getUserId();
};

#endif // USER_H
