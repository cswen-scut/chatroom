//
// Created by cswen on 2020/8/3.
//

#pragma once
#include"../Dao/MySQLConnector.h"

using namespace std;

class UserService {
private:
    MySQLConnector *connector;

    bool checkAccount(string &account);

    bool checkPassword(string &password);

public:
    UserService();

    pair<int, string> checkLogin(string account, string password);

    bool isRegistered(string account);

    bool registerUser(string account, string username, string password);
};

