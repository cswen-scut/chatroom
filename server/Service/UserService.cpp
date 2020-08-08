//
// Created by cswen on 2020/8/3.
//

#include "UserService.h"

bool UserService::checkAccount(string &account) {
    //账号长度在3-6位
    if (account.length() > 6 || account.length() < 3) {
        cout << "账号长度有误" << endl;
        return false;
    }
    //账号只能是数字
    for (char &c : account) {
        if (!(c >= '0' && c <= '9')) {
            cout << "账号只能是数字" << endl;
            return false;
        }
    }
    //账号不能超过65535
    if (stoi(account) > 65535) {
        cout << "账号不能超过65535" << endl;
        return false;
    }
    return true;
}

bool UserService::checkPassword(string &password) {
    //密码长度为6-16位
    if (password.length() < 6 || password.length() > 16) {
        cout << "密码长度有误" << endl;
        return false;
    }
    //密码由字母,数字组成
    for (char &c : password) {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
            cout << "密码有误" << endl;
            return false;
        }
    }
    return true;
}

UserService::UserService() {
    connector = MySQLConnector::getMySQLConnector();
}

pair<int, string> UserService::checkLogin(string account, string password) {
    pair<int, string> user;
    if (checkAccount(account) && checkPassword(password)) {
        user = connector->queryUser(account, password);
    }
    return user;
}

bool UserService::isRegistered(string account) {
    if (!checkAccount(account)) {
        return true;
    }
    return connector->queryUser(account);
}

bool UserService::registerUser(string account, string username, string password) {
    return checkAccount(account) && checkPassword(password) && connector->insertUser(account, username, password);
}
