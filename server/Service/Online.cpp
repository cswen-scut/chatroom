//
// Created by cswen on 2020/8/3.
//

#include "Online.h"

string Online::getOnlineListStr() {
    Json::Value onlineList;
    for (auto &item : userMap) {
        Json::Value userJson;
        userJson["account"] = item.second.account;
        userJson["username"] = item.second.username;
        userJson["loginTime"] = item.second.loginTime;
        onlineList.append(userJson);
    }
    return onlineList.toStyledString();
}

bool Online::appendUser(int account, string username) {
    user u = {account, move(username), MyTime::getCurrentFormatTimeStr()};
    userMap[account] = u;
    return true;
}

bool Online::removeUser(int account) {
    userMap.erase(account);
    writeFdToReadFd.erase(accountToFd[account]);
    accountToFd.erase(account);
    return true;
}

bool Online::appendUser(pair<int, string> &user) {
    return appendUser(user.first, user.second);
}

int Online::getReadFd(int writeFd) {
    return writeFdToReadFd[writeFd];
}

vector<int> Online::getAllReadFd() {
    vector<int> v;
    for (auto &item : writeFdToReadFd) {
        v.push_back(item.second);
    }
    return v;
}

bool Online::appendWriteFd(int account, int fd) {
    accountToFd[account] = fd;
    return true;
}

bool Online::appendReadFd(int account, int fd) {
    writeFdToReadFd[accountToFd[account]] = fd;
    return true;
}

string Online::getUserJsonStr(int account) {
    Json::Value jsonUser;
    jsonUser["account"] = account;
    jsonUser["username"] = userMap[account].username;
    return jsonUser.toStyledString();
}

string Online::getUserName(int account) {
    return userMap[account].username;
}

bool Online::isLogin(int account) {
    return userMap.count(account) != 0;
}
