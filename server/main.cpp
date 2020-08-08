#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<arpa/inet.h>
#include<cstdlib>
#include<unistd.h>
#include<sys/epoll.h>
#include<json/json.h>
#include"ProtocolHead/HeadData.h"
#include"Service/DataProcesser.h"
#include "Service/UserService.h"
#include "Service/Online.h"
#include "config/server_config.h"

using namespace std;

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{}, clientAddr{};
    int opt = 1;
    if (-1 == setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        cout << "setsockopt fail" << endl;
        exit(-1);
    }//设置端口复用
    int epfd = epoll_create(MAX_CONNECTIONS);
    epoll_event ev{}, events[MAX_CONNECTIONS];
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev)) {
        cout << "epoll_ctl fail" << endl;
        exit(-1);
    }
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, HOST, &serverAddr.sin_addr);
    if (-1 == bind(lfd, (sockaddr *) &serverAddr, sizeof(serverAddr))) {
        cout << "bind fail" << endl;
        exit(-1);
    }
    if (-1 == listen(lfd, MAX_CONNECTIONS)) {
        cout << "listen fail" << endl;
        exit(-1);
    }
    cout << "listening..." << endl;
    char ipAddress[BUFSIZ];
    UserService us;
    Online online;
    while (true) {
        int nready = epoll_wait(epfd, events, MAX_CONNECTIONS, -1);
        if (nready < 0) {
            cout << "epoll_wait error" << endl;
            exit(-1);
        }
        cout << "收到" << nready << "个请求" << endl;
        for (int i = 0; i < nready; i++) {
            int fd = events[i].data.fd;
            if (fd == lfd) {
                socklen_t len = sizeof(clientAddr);
                int cfd = accept(lfd, (sockaddr *) &clientAddr, &len);
                ev.data.fd = cfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(clientAddr));
                //设置超时read
                struct timeval timeout = {1, 0};
                setsockopt(cfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(struct timeval));
            } else if (events[i].events & EPOLLIN) {
                HeadData hd(fd);
                unsigned int protocolId = hd.getProtocolId();
                unsigned int account = hd.getAccount();
                unsigned int dataType = hd.getDataType();
                unsigned int dataLength = hd.getDataLength();
                DataProcesser dp;
                switch (protocolId) {
                    case LOGIN: {
                        string loginMsg = dp.readTextContent(fd, dataLength);
                        Json::Reader jsonReader;
                        Json::Value msg;
                        jsonReader.parse(loginMsg, msg);
                        string account = msg["account"].asString();
                        string password = msg["password"].asString();
                        pair<int, string> user = us.checkLogin(account, password);
                        Json::Value loginResult;
                        //登录成功
                        if (user.first != 0) {
                            if (online.isLogin(user.first)) {
                                loginResult["status"] = LOGIN_EXIST;
                            } else {
                                online.appendUser(user);
                                online.appendWriteFd(user.first, fd);
                                loginResult["status"] = LOGIN_SUCCESS;
                                loginResult["username"] = user.second;
                            }
                        }
                        //失败
                        else {
                            loginResult["status"] = LOGIN_FAIL;
                        }
                        string loginResultStr = loginResult.toStyledString();
                        dp.writeMsg(fd, 0, loginResultStr);
                    }
                        break;
                    case REGISTER: {
                        string registerMsg = dp.readTextContent(fd, dataLength);
                        Json::Reader jsonReader;
                        Json::Value registerResult;
                        Json::Value msg;
                        jsonReader.parse(registerMsg, msg);
                        string account = msg["account"].asString();
                        string username = msg["username"].asString();
                        string password = msg["password"].asString();
                        if (us.isRegistered(account) || !us.registerUser(account, username, password)) {
                            registerResult["status"] = REGISTER_FAIL;
                        } else {
                            registerResult["status"] = REGISTER_SUCCESS;
                        }
                        dp.writeMsg(fd, 0, registerResult.toStyledString(), REGISTER);
                    }
                        break;

                    case SEND: {
                        string baseMsg = online.getUserName(account) + "(" + to_string(account) + ")说:";
                        if (dataType == TEXT) {
                            dp.writeTextToAllUser(online.getAllReadFd(), account, baseMsg);
                            string content = dp.readTextContent(fd, dataLength);
                            dp.writeTextToAllUser(online.getAllReadFd(), account, content);
                        } else if (dataType == IMAGE) {
                            string imagePath = dp.readImageContent(fd, dataLength);
                            if (dp.getFileLength(imagePath) == dataLength) {
                                dp.writeTextToAllUser(online.getAllReadFd(), account, baseMsg);
                                dp.writeImageToAllUser(online.getAllReadFd(), account, imagePath);
                            } else {
                                ev.data.fd = fd;
                                ev.events = EPOLLIN;
                                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                                close(fd);
                                close(online.getReadFd(fd));
                                string logoutMsg =
                                        online.getUserName(account) + "(" + to_string(account) + ")" + "离开了聊天室!";
                                online.removeUser(account);
                                vector<int> fds = online.getAllReadFd();
                                if (!fds.empty()) {
                                    dp.writeTextToAllUser(fds, account, logoutMsg, NOTICE);
                                    dp.writeTextToAllUser(fds, 0, online.getOnlineListStr(), ONLINELIST);
                                }
                            }
                        }
                    }
                        break;
                    case READ: {
                        ev.data.fd = fd;
                        ev.events = EPOLLIN;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                        online.appendReadFd(account, fd);
                        string loginMsg = online.getUserName(account) + "(" + to_string(account) + ")" + "走进了聊天室!";
                        dp.writeTextToAllUser(online.getAllReadFd(), account, loginMsg, NOTICE);
                        dp.writeTextToAllUser(online.getAllReadFd(), account, online.getOnlineListStr(), ONLINELIST);
                    }
                        break;
                    case LOGOUT: {
                        sleep(1);
                        ev.data.fd = fd;
                        ev.events = EPOLLIN;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                        close(fd);
                        close(online.getReadFd(fd));
                        string logoutMsg = online.getUserName(account) + "(" + to_string(account) + ")" + "离开了聊天室!";
                        online.removeUser(account);
                        vector<int> fds = online.getAllReadFd();
                        cout << "当前在线人数:" << fds.size() << endl;
                        if (!fds.empty()) {
                            dp.writeTextToAllUser(fds, account, logoutMsg, NOTICE);
                            dp.writeTextToAllUser(fds, 0, online.getOnlineListStr(), ONLINELIST);
                        }
                    }
                        break;
                    case CLOSE: {
                        sleep(1);
                        ev.data.fd = fd;
                        ev.events = EPOLLIN;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                        close(fd);
                    }
                        break;
                }

            }
        }
    }

    close(lfd);
    return 0;
}
